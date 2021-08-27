

/*================================================================
 *   
 *   
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2020年06月18日 星期四 09时17分57秒
 *   修改日期：2021年08月27日 星期五 10时56分24秒
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

channels_config_t channels_config = {
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
