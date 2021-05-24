#include "tcp_server.h"
#include "math.h"
#include <string.h>

extern uint32_t y_max, x_max;
//current value on the x-axis
uint32_t x_pos = 0;
//current value on the y-axis
uint32_t y_pos = 0;

extern uint8_t relay_mode;

extern float T_current;
extern uint8_t MACAddr[6];
extern uint8_t IP_ADDRESS[4];
extern uint8_t NETMASK_ADDRESS[4];
extern uint8_t GATEWAY_ADDRESS[4];
extern uint16_t tcp_port, t_heat1, t_heat2, t_cold1, t_cold2;

extern uint32_t PORT_and_TEMPERATURE_FLASH_ADDRESS[5];
extern osMailQId TemperatureSetHandle;
extern IWDG_HandleTypeDef hiwdg;

void generate_zero(uint8_t *buf, uint8_t size, uint8_t len) {

	for (int j = 0; j < len; j++) {
		buf[size - 1 - j] = buf[len - 1 - j];
	}
	for (int i = 0; i < size - len; i++) {
		buf[i] = '0';
	}
}
void RotateV(int step) {
	float usDelay = 70;

	if (step < 0) {
		HAL_GPIO_WritePin(GPIOE, DIR2_Pin, GPIO_PIN_SET);
	}
	if (step >= 0) {
		HAL_GPIO_WritePin(GPIOE, DIR2_Pin, GPIO_PIN_RESET);
	}

	for (int i = 0; i < abs(step); i++) {
		HAL_IWDG_Refresh(&hiwdg);
		HAL_GPIO_WritePin(GPIOE, STEP2_Pin, GPIO_PIN_SET);
		HAL_Delay(usDelay / 1000);
		HAL_GPIO_WritePin(GPIOE, STEP2_Pin, GPIO_PIN_RESET);
		HAL_Delay(usDelay / 1000);
	}
}

void RotateH(int step) {
	float usDelay = 70;

	if (step < 0) {
		HAL_GPIO_WritePin(GPIOD, DIR1_Pin, GPIO_PIN_SET);
	}
	if (step >= 0) {
		HAL_GPIO_WritePin(GPIOD, DIR1_Pin, GPIO_PIN_RESET);
	}

	for (int i = 0; i < abs(step); i++) {
		HAL_IWDG_Refresh(&hiwdg);
		HAL_GPIO_WritePin(GPIOD, STEP1_Pin, GPIO_PIN_SET);
		HAL_Delay(usDelay / 1000);
		HAL_GPIO_WritePin(GPIOD, STEP1_Pin, GPIO_PIN_RESET);
		HAL_Delay(usDelay / 1000);
	}
}

void tcp_thread(void *arg) {

	err_t err, recv_err;
	struct netconn *conn;
	struct netbuf *inbuf;
	struct netconn *newconn;
	struct_sock *arg_sock;
	arg_sock = (struct_sock*) arg;
	conn = arg_sock->conn;
	u16_t buflen;
	uint8_t* buf;
	uint8_t data_answer[100] = { '\0' };
	uint8_t data_parameter[80] = { '\0' };
	uint8_t OK_answer[] = "A00000";
	uint8_t Error_answer[] = "A01000";
	uint8_t EndStop_answer[] = "A02000";
	uint8_t Max_y_answer[] = "A03000";
	uint8_t Max_x_answer[] = "A04000";
	uint8_t Max_xy_answer[] = "A05000";

	uint8_t signature_command[1];
	uint8_t id_command[2];
	uint8_t len_command[3];
	uint8_t parameter_command[30];
	uint8_t int_id_com, int_len_com;

	uint8_t tmp_data[3];
	uint8_t str_x_max[20];
	uint8_t str_y_max[20];
	uint8_t strV[20];
	uint8_t strH[20];
	uint8_t strD[20];
	uint8_t strU[20];
	uint8_t strR[20];
	uint8_t strL[20];

	int step_L, step_R, step_D, step_U, step_H, step_V, indexH, indexV, indexR,
			indexL, indexD, indexU;
	uint8_t t_cold_str1[3];
	uint8_t t_cold_str2[3];
	uint8_t t_heat_str1[3];
	uint8_t t_heat_str2[3];
	uint8_t t_read[7];

	//temperature measurement results

	volatile set_T T_s;
	volatile set_T *T_s_p;

	T_s_p = &T_s;

	GPIO_PinState currentButtonStateH = HAL_GPIO_ReadPin(GPIOC, End_Stop1_Pin);
	GPIO_PinState currentButtonStateV = HAL_GPIO_ReadPin(GPIOC, End_Stop2_Pin);
	for (;;) {

		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) {
			for (;;) {

				recv_err = netconn_recv(newconn, &inbuf);
				if (recv_err == ERR_OK) {
					netbuf_data(inbuf, (void**) &buf, &buflen);
					if ((buf[0] == 0x0D) || (buf[0] == 0x0A)) {
						netbuf_delete(inbuf);
						continue;
					}
					memset(tmp_data, '\0', sizeof(tmp_data));
					memset(data_answer, '\0', sizeof(data_answer));
					memset(data_parameter, '\0', sizeof(data_parameter));
					memset(signature_command, '\0', sizeof(signature_command));
					memset(id_command, '\0', sizeof(id_command));
					memset(len_command, '\0', sizeof(len_command));
					memset(parameter_command, '\0', sizeof(parameter_command));

					for (int i = 0; i < sizeof(signature_command); i++) {
						signature_command[i] = buf[0];
					}
					for (int i = 0; i < sizeof(id_command); i++) {
						id_command[i] = buf[i + sizeof(signature_command)];
					}
					int_id_com = atoi(id_command);
					for (int i = 0; i < sizeof(len_command); i++) {
						len_command[i] = buf[i + sizeof(signature_command)
								+ sizeof(id_command)];
					}
					int_len_com = atoi(len_command);

					for (int i = 0; i < int_len_com; i++) {
						parameter_command[i] = buf[i + sizeof(signature_command)
								+ sizeof(id_command) + sizeof(len_command)];
					}

					if (signature_command[0] == 'S') {
						switch (int_id_com) {
						//reset system
						case 1:
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//set temperature t_heat1,t_heat2,t_cold1,t_cold2
						case 2:
							T_s_p = osMailAlloc(TemperatureSetHandle,
							osWaitForever);
							memset(t_cold_str1, '\0', sizeof(t_cold_str1));
							memset(t_cold_str2, '\0', sizeof(t_cold_str2));
							memset(t_heat_str1, '\0', sizeof(t_heat_str1));
							memset(t_heat_str2, '\0', sizeof(t_heat_str2));

							for (int i = 0; i < 3; i++) {
								t_heat_str1[i] = parameter_command[i];
								t_heat_str2[i] = parameter_command[i + 3];
								t_cold_str1[i] = parameter_command[i + 6];
								t_cold_str2[i] = parameter_command[i + 9];
							}
							t_heat1 = atoi(t_heat_str1);
							t_heat2 = atoi(t_heat_str2);
							t_cold1 = atoi(t_cold_str1);
							t_cold2 = atoi(t_cold_str2);

							T_s.T_heat1 = atoi(t_heat_str1);
							T_s.T_heat2 = atoi(t_heat_str2);
							T_s.T_cold1 = atoi(t_cold_str1);
							T_s.T_cold2 = atoi(t_cold_str2);

							osMailPut(TemperatureSetHandle, &T_s);
							Set_var_for_flash(t_heat1,
									PORT_and_TEMPERATURE_FLASH_ADDRESS[1]);
							Set_var_for_flash(t_heat2,
									PORT_and_TEMPERATURE_FLASH_ADDRESS[2]);
							Set_var_for_flash(t_cold1,
									PORT_and_TEMPERATURE_FLASH_ADDRESS[3]);
							Set_var_for_flash(t_cold2,
									PORT_and_TEMPERATURE_FLASH_ADDRESS[4]);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);

							break;

							//read set and current temperatures
						case 3:

							memset(t_heat_str1, '\0', sizeof(t_heat_str1));
							memset(t_heat_str2, '\0', sizeof(t_heat_str2));
							memset(t_cold_str1, '\0', sizeof(t_cold_str1));
							memset(t_cold_str2, '\0', sizeof(t_cold_str2));
							memset(t_read, '\0', sizeof(t_read));

							itoa(t_heat1, t_heat_str1, 10);
							itoa(t_heat2, t_heat_str2, 10);
							itoa(t_cold1, t_cold_str1, 10);
							itoa(t_cold2, t_cold_str2, 10);
							sprintf(t_read, "%d.%02d", (int) T_current,
									(uint16_t) abs(
											(fabs(T_current)
													- (uint16_t) fabs(T_current))
													* 100));

							data_answer[0] = 'A';
							data_answer[1] = '0';
							data_answer[2] = '0';
							data_answer[3] = '0';
							data_answer[4] = '1';
							data_answer[5] = '9';

							generate_zero(t_heat_str1, sizeof(t_heat_str1),
									strlen(t_heat_str1));
							generate_zero(t_heat_str2, sizeof(t_heat_str2),
									strlen(t_heat_str2));
							generate_zero(t_cold_str1, sizeof(t_cold_str1),
									strlen(t_cold_str1));
							generate_zero(t_cold_str2, sizeof(t_cold_str2),
									strlen(t_cold_str2));
							generate_zero(t_read, sizeof(t_read),
									strlen(t_read));

							//if the current temperature is negative then move the minus sign
							if (t_read[1] == '-') {
								t_read[0] = '-';
								t_read[1] = '0';
							}
							if (t_read[2] == '-') {
								t_read[0] = '-';
								t_read[1] = '0';
								t_read[2] = '0';
							}

							strcat(data_answer, t_read);
							strcat(data_answer, t_heat_str1);
							strcat(data_answer, t_heat_str2);
							strcat(data_answer, t_cold_str1);
							strcat(data_answer, t_cold_str2);

							netconn_write(newconn, data_answer,
									strlen(data_answer), NETCONN_COPY);
							break;
							//set ip-address
						case 4:
							for (int j = 0; j < 4; j++) {
								for (int i = 0; i < 3; i++) {
									tmp_data[i] = parameter_command[i + 3 * j];
								}
								IP_ADDRESS[j] = atoi(tmp_data);
							}
							Set_IP_ADDR(IP_ADDRESS);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//set netmask-address
						case 5:
							for (int j = 0; j < 4; j++) {
								for (int i = 0; i < 3; i++) {
									tmp_data[i] = parameter_command[i + 3 * j];
								}
								NETMASK_ADDRESS[j] = atoi(tmp_data);
							}
							Set_MASK_ADDR(NETMASK_ADDRESS);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//set gateway-address
						case 6:
							for (int j = 0; j < 4; j++) {
								for (int i = 0; i < 3; i++) {
									tmp_data[i] = parameter_command[i + 3 * j];
								}
								GATEWAY_ADDRESS[j] = atoi(tmp_data);
							}
							Set_GTW_ADDR(GATEWAY_ADDRESS);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//set mac-address
						case 7:
							for (int j = 0; j < 6; j++) {
								for (int i = 0; i < 3; i++) {
									tmp_data[i] = parameter_command[i + 3 * j];
								}
								MACAddr[j] = atoi(tmp_data);
							}
							Set_MAC_ADDR(MACAddr);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//set port tcp
						case 8:

							Set_var_for_flash(atoi(parameter_command),
									PORT_and_TEMPERATURE_FLASH_ADDRESS[0]);
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							netconn_close(newconn);
							netbuf_delete(inbuf);
							NVIC_SystemReset();
							break;

							//relay on/off
						case 9:

							if (atoi(parameter_command) == 0) {
								HAL_GPIO_WritePin(GPIOC, RELAY_Pin,
										GPIO_PIN_RESET);

								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else if (atoi(parameter_command) == 1) {
								HAL_GPIO_WritePin(GPIOC, RELAY_Pin,
										GPIO_PIN_SET);

								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							break;

							//read net parameters
						case 10:
							data_answer[0] = 'A';
							data_answer[1] = '0';
							data_answer[2] = '0';

							for (int i = 0; i < 4; i++) {
								memset(tmp_data, '\0', sizeof(tmp_data));
								itoa(IP_ADDRESS[i], tmp_data, 10);
								generate_zero(tmp_data, sizeof(tmp_data),
										strlen(tmp_data));
								strcat(data_parameter, tmp_data);
							}
							for (int i = 0; i < 4; i++) {
								memset(tmp_data, '\0', sizeof(tmp_data));
								itoa(NETMASK_ADDRESS[i], tmp_data, 10);
								generate_zero(tmp_data, sizeof(tmp_data),
										strlen(tmp_data));
								strcat(data_parameter, tmp_data);
							}
							for (int i = 0; i < 4; i++) {
								memset(tmp_data, '\0', sizeof(tmp_data));
								itoa(GATEWAY_ADDRESS[i], tmp_data, 10);
								generate_zero(tmp_data, sizeof(tmp_data),
										strlen(tmp_data));
								strcat(data_parameter, tmp_data);
							}
							for (int i = 0; i < 6; i++) {
								memset(tmp_data, '\0', sizeof(tmp_data));
								itoa(MACAddr[i], tmp_data, 10);
								generate_zero(tmp_data, sizeof(tmp_data),
										strlen(tmp_data));
								strcat(data_parameter, tmp_data);
							}

							memset(tmp_data, '\0', sizeof(tmp_data));
							itoa(tcp_port, tmp_data, 10);
							generate_zero(tmp_data, sizeof(tmp_data),
									strlen(tmp_data));
							strcat(data_parameter, tmp_data);

							memset(len_command, '\0', sizeof(len_command));
							itoa(strlen(data_parameter), len_command, 10);
							generate_zero(len_command, sizeof(len_command),
									strlen(len_command));
							strcat(data_answer, len_command);
							strcat(data_answer, data_parameter);
							netconn_write(newconn, data_answer,
									strlen(data_answer), NETCONN_COPY);
							break;
							//set relay mode
						case 11:
							relay_mode = atoi(parameter_command);
							if (relay_mode == 0 || relay_mode == 1) {

								Set_var_for_flash(relay_mode,
										PORT_and_TEMPERATURE_FLASH_ADDRESS[5]);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							break;
						default:
							netconn_write(newconn, Error_answer,
									strlen(Error_answer), NETCONN_COPY);
							break;
						}

					}

					else if (signature_command[0] == 'E') {
						switch (int_id_com) {

						//enable driver for step motors
						case 1:
							//disable stepper motor for horizontal axis
							if (atoi(parameter_command) == 10) {
								HAL_GPIO_WritePin(GPIOE, ENABLE1_Pin,
										GPIO_PIN_SET);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
								//enable stepper motor for horizontal axis
							} else if (atoi(parameter_command) == 11) {
								HAL_GPIO_WritePin(GPIOE, ENABLE1_Pin,
										GPIO_PIN_RESET);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
								//disable stepper motor for vertical axis
							} else if (atoi(parameter_command) == 20) {
								HAL_GPIO_WritePin(GPIOA, ENABLE2_Pin,
										GPIO_PIN_SET);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
								//enable stepper motor for vertical axis
							} else if (atoi(parameter_command) == 21) {
								HAL_GPIO_WritePin(GPIOA, ENABLE2_Pin,
										GPIO_PIN_RESET);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							break;

							//set to the starting position
						case 2:
							memset(strV, '\0', sizeof(strV));
							memset(strH, '\0', sizeof(strH));

							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'V') {
									indexV = i;
									i = strlen(parameter_command);
								}
							}

							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'H') {
									indexH = i;
									i = strlen(parameter_command);
								}
							}

							if (parameter_command[indexH + 1] != 'N') {

								for (int i = 0; i < indexV - indexH - 1; i++) {
									strH[i] = parameter_command[indexH + 1 + i];
								}
								step_H = atoi(strH);

								currentButtonStateH = HAL_GPIO_ReadPin(
								GPIOC,
								End_Stop1_Pin);
								while (currentButtonStateH == GPIO_PIN_RESET) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateH(-1);
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
								}

								currentButtonStateH = HAL_GPIO_ReadPin(
								GPIOC,
								End_Stop1_Pin);
								while (currentButtonStateH == GPIO_PIN_SET) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateH(1);
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
								}
								x_pos = 0;

								if (step_H <= x_max) {
									RotateH(step_H);
									x_pos = x_pos + step_H;
								}
							}

							if (parameter_command[indexV + 1] != 'N') {

								for (int i = 0;
										i
												< strlen(parameter_command) - 1
														- indexV; i++) {
									strV[i] = parameter_command[indexV + 1 + i];
								}
								step_V = atoi(strV);

								currentButtonStateV = HAL_GPIO_ReadPin(
								GPIOC,
								End_Stop2_Pin);
								while (currentButtonStateV == GPIO_PIN_RESET) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateV(-1);
									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
								}

								currentButtonStateV = HAL_GPIO_ReadPin(
								GPIOC,
								End_Stop2_Pin);
								while (currentButtonStateV == GPIO_PIN_SET) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateV(1);
									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
								}

								y_pos = 0;
								if (step_V <= y_max) {
									RotateV(step_V);
									y_pos = y_pos + step_V;
								}
							}

							if (step_V > y_max && step_H < x_max) {
								netconn_write(newconn, Max_y_answer,
										strlen(Max_y_answer), NETCONN_COPY);
							} else if (step_H > x_max && step_V < y_max) {
								netconn_write(newconn, Max_x_answer,
										strlen(Max_x_answer), NETCONN_COPY);
							}
							if (step_H > x_max && step_V > y_max) {
								netconn_write(newconn, Max_xy_answer,
										strlen(Max_xy_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							}

							break;

							//move right left down up
						case 3:
							memset(strR, '\0', sizeof(strR));
							memset(strL, '\0', sizeof(strL));
							memset(strD, '\0', sizeof(strD));
							memset(strU, '\0', sizeof(strU));

							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'R') {
									indexR = i;
									i = strlen(parameter_command);
								}
							}
							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'L') {
									indexL = i;
									i = strlen(parameter_command);
								}
							}
							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'U') {
									indexU = i;
									i = strlen(parameter_command);
								}
							}
							for (int i = 0; i < strlen(parameter_command);
									i++) {
								if (parameter_command[i] == 'D') {
									indexD = i;
									i = strlen(parameter_command);
								}
							}

							//move to the right
							if (parameter_command[indexR + 1] > 0) {

								for (int i = 0; i < indexD - indexR - 1; i++) {
									strR[i] = parameter_command[indexR + 1 + i];
								}
								step_R = atoi(strR);
								for (int i = 0; i < abs(step_R); i++) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateH(1);
									x_pos++;
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
									if (currentButtonStateH == GPIO_PIN_SET) {
										i = step_R;
										netconn_write(newconn, EndStop_answer,
												strlen(EndStop_answer),
												NETCONN_COPY);
									}
									if (x_pos == x_max) {
										i = step_R;
										netconn_write(newconn, Max_x_answer,
												strlen(Max_x_answer),
												NETCONN_COPY);
									}
								}
								if (x_pos == x_max
										|| currentButtonStateH
												== GPIO_PIN_SET) {
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
									while (currentButtonStateH == GPIO_PIN_SET) {
										HAL_IWDG_Refresh(&hiwdg);
										RotateH(-1);
										currentButtonStateH = HAL_GPIO_ReadPin(
										GPIOC,
										End_Stop1_Pin);
									}

									break;
								}
							}

							//move to the down
							if (parameter_command[indexD + 1] > 0) {

								for (int i = 0; i < indexL - indexD - 1; i++) {
									strD[i] = parameter_command[indexD + 1 + i];
								}
								step_D = atoi(strD);

								for (int i = 0; i < abs(step_D); i++) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateV(1);
									y_pos++;

									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
									if (currentButtonStateV == GPIO_PIN_SET) {
										i = step_D;
										netconn_write(newconn, EndStop_answer,
												strlen(EndStop_answer),
												NETCONN_COPY);
									}

									if (y_pos == y_max) {
										i = step_D;
										netconn_write(newconn, Max_y_answer,
												strlen(Max_y_answer),
												NETCONN_COPY);

									}
								}
								if (y_pos == y_max
										|| currentButtonStateV
												== GPIO_PIN_SET) {
									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
									while (currentButtonStateV == GPIO_PIN_SET) {
										HAL_IWDG_Refresh(&hiwdg);
										RotateV(-1);
										currentButtonStateV = HAL_GPIO_ReadPin(
										GPIOC,
										End_Stop2_Pin);
									}
									break;
								}
							}

							//move to the left
							if (parameter_command[indexL + 1] > 0) {

								for (int i = 0; i < indexU - indexL - 1; i++) {
									strL[i] = parameter_command[indexL + 1 + i];
								}
								step_L = atoi(strL);

								for (int i = 0; i < abs(step_L); i++) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateH(-1);
									x_pos--;
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
									if (currentButtonStateH == GPIO_PIN_SET) {
										i = step_L;
										netconn_write(newconn, EndStop_answer,
												strlen(EndStop_answer),
												NETCONN_COPY);
									}
								}
								if (currentButtonStateH == GPIO_PIN_SET) {
									currentButtonStateH = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop1_Pin);
									while (currentButtonStateH == GPIO_PIN_SET) {
										HAL_IWDG_Refresh(&hiwdg);
										RotateH(1);
										currentButtonStateH = HAL_GPIO_ReadPin(
										GPIOC,
										End_Stop1_Pin);
									}
									break;
								}

							}
							//move to the up
							if (parameter_command[indexU + 1] > 0) {
								for (int i = 0;
										i
												< strlen(parameter_command)
														- indexU - 1; i++) {
									strU[i] = parameter_command[indexU + 1 + i];
								}
								step_U = atoi(strU);

								for (int i = 0; i < step_U; i++) {
									HAL_IWDG_Refresh(&hiwdg);
									RotateV(-1);
									y_pos--;
									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
									if (currentButtonStateV == GPIO_PIN_SET) {
										i = step_U;
										netconn_write(newconn, EndStop_answer,
												strlen(EndStop_answer),
												NETCONN_COPY);

									}
								}
								if (currentButtonStateV == GPIO_PIN_SET) {
									currentButtonStateV = HAL_GPIO_ReadPin(
									GPIOC,
									End_Stop2_Pin);
									while (currentButtonStateV == GPIO_PIN_SET) {
										HAL_IWDG_Refresh(&hiwdg);
										RotateV(1);
										currentButtonStateV = HAL_GPIO_ReadPin(
										GPIOC,
										End_Stop2_Pin);
									}
									break;
								}
							}

							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);
							break;

							//setting the microstep
						case 4:
							if (atoi(parameter_command) == 11) {
								HAL_GPIO_WritePin(GPIOE, MS11_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOE, MS21_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOE, MS31_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 12) {
								HAL_GPIO_WritePin(GPIOE, MS11_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS21_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOE, MS31_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 13) {
								HAL_GPIO_WritePin(GPIOE, MS11_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOE, MS21_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS31_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 14) {
								HAL_GPIO_WritePin(GPIOE, MS11_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS21_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS31_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 15) {
								HAL_GPIO_WritePin(GPIOE, MS11_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS21_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOE, MS31_Pin,
										GPIO_PIN_SET);
							}

							else if (atoi(parameter_command) == 21) {
								HAL_GPIO_WritePin(GPIOA, MS12_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOA, MS22_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOA, MS32_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 22) {
								HAL_GPIO_WritePin(GPIOA, MS12_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS22_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOA, MS32_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 23) {
								HAL_GPIO_WritePin(GPIOA, MS12_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOA, MS22_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS32_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 24) {
								HAL_GPIO_WritePin(GPIOA, MS12_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS22_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS32_Pin,
										GPIO_PIN_RESET);
							}

							else if (atoi(parameter_command) == 25) {
								HAL_GPIO_WritePin(GPIOA, MS12_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS22_Pin,
										GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, MS32_Pin,
										GPIO_PIN_SET);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							netconn_write(newconn, OK_answer, strlen(OK_answer),
									NETCONN_COPY);

							break;

							//setting the maximum value of x
						case 5:
							x_max = atoi(parameter_command);
							if (x_max > 0) {

								Set_var_for_flash(x_max,
										PORT_and_TEMPERATURE_FLASH_ADDRESS[6]);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							break;

							//setting the maximum value of y
						case 6:
							y_max = atoi(parameter_command);
							if (y_max > 0) {

								Set_var_for_flash(y_max,
										PORT_and_TEMPERATURE_FLASH_ADDRESS[7]);
								netconn_write(newconn, OK_answer,
										strlen(OK_answer), NETCONN_COPY);
							} else {
								netconn_write(newconn, Error_answer,
										strlen(Error_answer), NETCONN_COPY);
							}
							break;

							//get x_pos and y_pos, xmax and ymax
						case 7:
							memset(str_x_max, '\0', sizeof(str_x_max));
							memset(str_y_max, '\0', sizeof(str_y_max));
							memset(strV, '\0', sizeof(strV));
							memset(strH, '\0', sizeof(strH));
							memset(len_command, '\0', sizeof(len_command));

							itoa(y_pos, strV, 10);
							itoa(x_pos, strH, 10);
							itoa(x_max, str_x_max, 10);
							itoa(y_max, str_y_max, 10);
							int_len_com = strlen(strV) + strlen(strH)
									+ strlen(str_x_max) + strlen(str_y_max) + 6;
							itoa(int_len_com, len_command, 10);
							generate_zero(len_command, sizeof(len_command),
									strlen(len_command));
							data_answer[0] = 'A';
							data_answer[1] = '0';
							data_answer[2] = '0';
							strcat(data_answer, len_command);
							strcat(data_answer, "X");
							strcat(data_answer, strH);
							strcat(data_answer, "Y");
							strcat(data_answer, strV);
							strcat(data_answer, "Xm");
							strcat(data_answer, str_x_max);
							strcat(data_answer, "Ym");
							strcat(data_answer, str_y_max);
							netconn_write(newconn, data_answer,
									strlen(data_answer), NETCONN_COPY);
							break;

						default:
							netconn_write(newconn, Error_answer,
									strlen(Error_answer), NETCONN_COPY);
							break;
						}
					}

					else {
						netconn_write(newconn, Error_answer,
								strlen(Error_answer), NETCONN_COPY);
					}

					netbuf_delete(inbuf);

				} else {
					netbuf_delete(inbuf);
					netconn_close(newconn);
					netconn_delete(newconn);
					break;
				}

			}

		} else {
			osDelay(1);
		}
	}
}
