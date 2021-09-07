

/*================================================================
 *
 *
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2020年06月18日 星期四 09时17分57秒
 *   修改日期：2021年09月07日 星期二 13时08分10秒
 *   描    述：
 *
 *================================================================*/
#include "channels_config.h"

#include "main.h"
#include "os_utils.h"
#include "log.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern UART_HandleTypeDef huart2;

static relay_node_info_t relay_node_info_0_0 = {
	.relay_id = 0,
	.channel_id = {0, 1},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_0_1 = {
	.relay_id = 1,
	.channel_id = {1, 2},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_0_2 = {
	.relay_id = 2,
	.channel_id = {2, 3},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_0_3 = {
	.relay_id = 3,
	.channel_id = {3, 4},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_0_4 = {
	.relay_id = 4,
	.channel_id = {4, 0},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_1_0 = {
	.relay_id = 0,
	.channel_id = {5, 6},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_1_1 = {
	.relay_id = 1,
	.channel_id = {6, 7},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_1_2 = {
	.relay_id = 2,
	.channel_id = {7, 8},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_1_3 = {
	.relay_id = 3,
	.channel_id = {8, 9},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t relay_node_info_1_4 = {
	.relay_id = 4,
	.channel_id = {9, 5},
	.gpio_port = NULL,
	.gpio_pin = 0,
};

static relay_node_info_t *relay_node_info_0_sz[] = {
	&relay_node_info_0_0,
	&relay_node_info_0_1,
	&relay_node_info_0_2,
	&relay_node_info_0_3,
	&relay_node_info_0_4,
};

static relay_node_info_t *relay_node_info_1_sz[] = {
	&relay_node_info_1_0,
	&relay_node_info_1_1,
	&relay_node_info_1_2,
	&relay_node_info_1_3,
	&relay_node_info_1_4,
};

static pdu_group_relay_info_t pdu_group_relay_info_0 = {
	.pdu_group_id = 0,
	.relay_node_info = relay_node_info_0_sz,
	.relay_node_info_size = ARRAY_SIZE(relay_node_info_0_sz),
};

static pdu_group_relay_info_t pdu_group_relay_info_1 = {
	.pdu_group_id = 1,
	.relay_node_info = relay_node_info_1_sz,
	.relay_node_info_size = ARRAY_SIZE(relay_node_info_1_sz),
};

static pdu_group_relay_info_t *pdu_group_relay_info_sz[] = {
	&pdu_group_relay_info_0,
	&pdu_group_relay_info_1,
};

static channels_config_t channels_config = {
	.id = 0,
	.hcan_com = &hcan2,
	.gpio_port_force_stop = in4_GPIO_Port,
	.gpio_pin_force_stop = in4_Pin,
	.gpio_port_fan = rey1_GPIO_Port,
	.gpio_pin_fan = rey1_Pin,
	.gpio_port_door = in2_GPIO_Port,
	.gpio_pin_door = in2_Pin,

	.channel_number = 0,
	.relay_board_number = 0,

	.display_config = {
		.huart = &huart2,
	},

	.power_module_config = {
		.channels_power_module_number = 0,
		.hcan = &hcan1,
		.channels_power_module_type = CHANNELS_POWER_MODULE_TYPE_NATIVE,
	},
	.relay_info = {
		.pdu_group_size = ARRAY_SIZE(pdu_group_relay_info_sz),
		.pdu_group_relay_info = pdu_group_relay_info_sz,
	},
};

static channels_config_t *channels_config_sz[] = {
	&channels_config,
};

channels_config_t *get_channels_config(uint8_t id)
{
	int i;
	channels_config_t *channels_config = NULL;
	channels_config_t *channels_config_item = NULL;

	for(i = 0; i < ARRAY_SIZE(channels_config_sz); i++) {
		channels_config_item = channels_config_sz[i];

		if(channels_config_item->id == id) {
			channels_config = channels_config_item;
			break;
		}
	}

	return channels_config;
}
