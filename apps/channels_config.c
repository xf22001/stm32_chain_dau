

/*================================================================
 *
 *
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2020年06月18日 星期四 09时17分57秒
 *   修改日期：2021年09月18日 星期六 11时35分30秒
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
extern ADC_HandleTypeDef hadc1;

static relay_node_info_t relay_node_info_0_0 = {
	.relay_id = 0,
	.channel_id = {0, 1},
	.gpio_port = kg1_GPIO_Port,//km1-kg1
	.gpio_pin = kg1_Pin,
	.gpio_port_fb = fb1_GPIO_Port,
	.gpio_pin_fb = fb1_Pin,
};

static relay_node_info_t relay_node_info_0_1 = {
	.relay_id = 1,
	.channel_id = {1, 2},
	.gpio_port = kg2_GPIO_Port,//km2-kg2
	.gpio_pin = kg2_Pin,
	.gpio_port_fb = fb2_GPIO_Port,
	.gpio_pin_fb = fb2_Pin,
};

static relay_node_info_t relay_node_info_0_2 = {
	.relay_id = 2,
	.channel_id = {2, 3},
	.gpio_port = kg3_GPIO_Port,//km3-kg3
	.gpio_pin = kg3_Pin,
	.gpio_port_fb = fb3_GPIO_Port,
	.gpio_pin_fb = fb3_Pin,
};

static relay_node_info_t relay_node_info_0_3 = {
	.relay_id = 3,
	.channel_id = {3, 4},
	.gpio_port = kg4_GPIO_Port,//km4-kg4
	.gpio_pin = kg4_Pin,
	.gpio_port_fb = fb4_GPIO_Port,
	.gpio_pin_fb = fb4_Pin,
};

static relay_node_info_t relay_node_info_0_4 = {
	.relay_id = 4,
	.channel_id = {4, 0},
	.gpio_port = kg5_GPIO_Port,//km5-kg5
	.gpio_pin = kg5_Pin,
	.gpio_port_fb = fb5_GPIO_Port,
	.gpio_pin_fb = fb5_Pin,
};

static relay_node_info_t *relay_node_info_0_sz[] = {
	&relay_node_info_0_0,
	&relay_node_info_0_1,
	&relay_node_info_0_2,
	&relay_node_info_0_3,
	&relay_node_info_0_4,
};

static pdu_group_relay_info_t pdu_group_relay_info_0 = {
	.pdu_group_id = 0,
	.relay_node_info = relay_node_info_0_sz,
	.size = ARRAY_SIZE(relay_node_info_0_sz),
};

static pdu_group_relay_info_t *pdu_group_relay_info_sz[] = {
	&pdu_group_relay_info_0,
};

static channel_relay_fb_node_info_t channel_relay_fb_node_info_0 = {
	.channel_id = 0,
	.gpio_port_fb = fb6_GPIO_Port,
	.gpio_pin_fb = fb6_Pin,
	.hadc = &hadc1,
	.rank = 0,
};

static channel_relay_fb_node_info_t channel_relay_fb_node_info_1 = {
	.channel_id = 1,
	.gpio_port_fb = fb7_GPIO_Port,
	.gpio_pin_fb = fb7_Pin,
	.hadc = &hadc1,
	.rank = 1,
};

static channel_relay_fb_node_info_t channel_relay_fb_node_info_2 = {
	.channel_id = 2,
	.gpio_port_fb = fb8_GPIO_Port,
	.gpio_pin_fb = fb8_Pin,
	.hadc = &hadc1,
	.rank = 2,
};

static channel_relay_fb_node_info_t channel_relay_fb_node_info_3 = {
	.channel_id = 3,
	.gpio_port_fb = fb9_GPIO_Port,
	.gpio_pin_fb = fb9_Pin,
	.hadc = &hadc1,
	.rank = 3,
};

static channel_relay_fb_node_info_t channel_relay_fb_node_info_4 = {
	.channel_id = 4,
	.gpio_port_fb = fb10_GPIO_Port,
	.gpio_pin_fb = fb10_Pin,
	.hadc = &hadc1,
	.rank = 4,
};

static channel_relay_fb_node_info_t *channel_relay_fb_node_info_sz[] = {
	&channel_relay_fb_node_info_0,
	&channel_relay_fb_node_info_1,
	&channel_relay_fb_node_info_2,
	&channel_relay_fb_node_info_3,
	&channel_relay_fb_node_info_4,
};

static pdu_group_channel_relay_fb_info_t pdu_group_channel_relay_fb_info_0 = {
	.pdu_group_id = 0,
	.size = ARRAY_SIZE(channel_relay_fb_node_info_sz),
	.channel_relay_fb_node_info = channel_relay_fb_node_info_sz,
};

static pdu_group_channel_relay_fb_info_t *pdu_group_channel_relay_fb_info_sz[] = {
	&pdu_group_channel_relay_fb_info_0,
};

static channels_config_t channels_config = {
	.id = 0,
	.hcan_com = &hcan1,
	.gpio_port_force_stop = in2_GPIO_Port,
	.gpio_pin_force_stop = in2_Pin,
	.gpio_port_fan = rey5_GPIO_Port,
	.gpio_pin_fan = rey5_Pin,
	.gpio_port_door = in1_GPIO_Port,
	.gpio_pin_door = in1_Pin,

	.channel_number = 0,
	.relay_board_number = 0,

	.display_config = {
		.huart = &huart2,
		.con_gpio = con485_GPIO_Port,
		.con_pin = con485_Pin,
		.led_gpio = led485_GPIO_Port,
		.led_pin = led485_Pin,
	},

	.power_module_config = {
		.channels_power_module_number = 0,
		.hcan = &hcan2,
		.channels_power_module_type = CHANNELS_POWER_MODULE_TYPE_NATIVE,
	},
	.relay_info = {
		.pdu_group_size = ARRAY_SIZE(pdu_group_relay_info_sz),
		.pdu_group_relay_info = pdu_group_relay_info_sz,
	},
	.channel_relay_fb_info = {
		.pdu_group_size = ARRAY_SIZE(pdu_group_channel_relay_fb_info_sz),
		.pdu_group_channel_relay_fb_info = pdu_group_channel_relay_fb_info_sz,
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
