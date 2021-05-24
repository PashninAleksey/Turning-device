#include "my_mibs.h"
#include "lwip/snmp.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_scalar.h"
#include <string.h>
#include "lwip/apps/snmp_mib2.h"
#include "lwip/apps/snmp_table.h"
#include "lwip/sys.h"

extern uint32_t y_pos;
extern uint32_t x_pos;
extern uint32_t y_max;
extern uint32_t x_max;

extern float T_current;

static s16_t get_temperature_value(struct snmp_node_instance* instance,
		void* value);
static s16_t get_x_max(struct snmp_node_instance* instance, void* value);
static s16_t get_y_max(struct snmp_node_instance* instance, void* value);
static s16_t get_x_pos(struct snmp_node_instance* instance, void* value);
static s16_t get_y_pos(struct snmp_node_instance* instance, void* value);

static const struct snmp_scalar_node switches_temperature =
SNMP_SCALAR_CREATE_NODE_READONLY(1, SNMP_ASN1_TYPE_INTEGER,
		get_temperature_value);

static const struct snmp_scalar_node x_max_node =
SNMP_SCALAR_CREATE_NODE_READONLY(2, SNMP_ASN1_TYPE_INTEGER, get_x_max);
static const struct snmp_scalar_node y_max_node =
SNMP_SCALAR_CREATE_NODE_READONLY(3, SNMP_ASN1_TYPE_INTEGER, get_y_max);
static const struct snmp_scalar_node x_pos_node =
SNMP_SCALAR_CREATE_NODE_READONLY(4, SNMP_ASN1_TYPE_INTEGER, get_x_pos);
static const struct snmp_scalar_node y_pos_node =
SNMP_SCALAR_CREATE_NODE_READONLY(5, SNMP_ASN1_TYPE_INTEGER, get_y_pos);

static const struct snmp_node* const pashnin_nodes[] = {
		&switches_temperature.node.node, &x_max_node.node, &y_max_node.node,
		&x_pos_node.node, &y_pos_node.node };

static const struct snmp_tree_node pashnin_tree_node =
SNMP_CREATE_TREE_NODE(1, pashnin_nodes);

static const u32_t my_base_oid[] = { 1, 3, 6, 1, 4, 1, 26381, 1 };
const struct snmp_mib pashnin_mib =
SNMP_MIB_CREATE(my_base_oid, &pashnin_tree_node.node);

/*----------------------------------------------------------------------------*/

/* leds value .1.3.6.1.4.1.26381.1.1.0 */
static s16_t get_temperature_value(struct snmp_node_instance* instance,
		void* value) {

	int *int_ptr = (int*) value;
	*int_ptr = (int) (T_current * 100);
	return sizeof(*int_ptr);
}

/* the get maximum value on the x-axis .1.3.6.1.4.1.26381.1.2.0 */
static s16_t get_x_max(struct snmp_node_instance* instance, void* value) {
	int *int_ptr = (int*) value;
	*int_ptr = (int) (x_max);
	return sizeof(*int_ptr);
}

/* the get maximum value on the y-axis .1.3.6.1.4.1.26381.1.3.0 */
static s16_t get_y_max(struct snmp_node_instance* instance, void* value) {
	int *int_ptr = (int*) value;
	*int_ptr = (int) (y_max);
	return sizeof(*int_ptr);
}

//current value on the x-axis .1.3.6.1.4.1.26381.1.4.0
static s16_t get_x_pos(struct snmp_node_instance* instance, void* value) {
	int *int_ptr = (int*) value;
	*int_ptr = (int) (x_pos);
	return sizeof(*int_ptr);
}

//current value on the y-axis .1.3.6.1.4.1.26381.1.5.0
static s16_t get_y_pos(struct snmp_node_instance* instance, void* value) {
	int *int_ptr = (int*) value;
	*int_ptr = (int) (y_pos);
	return sizeof(*int_ptr);
}

