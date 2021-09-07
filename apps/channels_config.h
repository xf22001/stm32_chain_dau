

/*================================================================
 *   
 *   
 *   文件名称：channels_config.h
 *   创 建 者：肖飞
 *   创建日期：2020年06月18日 星期四 09时15分08秒
 *   修改日期：2021年09月07日 星期二 11时49分52秒
 *   描    述：
 *
 *================================================================*/
#ifndef _CHANNELS_CONFIG_H
#define _CHANNELS_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"
#include "can_txrx.h"
#include "usart_txrx.h"

#ifdef __cplusplus
}
#endif

typedef enum {
	CHANNELS_POWER_MODULE_TYPE_NONE = 0,
	CHANNELS_POWER_MODULE_TYPE_NATIVE,
	CHANNELS_POWER_MODULE_TYPE_PROXY,
	CHANNELS_POWER_MODULE_TYPE_PROXY_AND_NATIVE,
} channels_power_module_type_t;

typedef struct {
	uint8_t channels_power_module_number;
	CAN_HandleTypeDef *hcan;
	channels_power_module_type_t channels_power_module_type;
} power_module_config_t;

typedef struct {
	void *huart;
	void *con_gpio;
	uint16_t con_pin;
} display_config_t;

typedef struct {
	uint8_t relay_id;
	uint8_t channel_id[2];
	GPIO_TypeDef *gpio_port;
	uint16_t gpio_pin;
} relay_node_info_t;

typedef struct {
	uint8_t pdu_group_id;
	uint8_t relay_node_info_size;
	relay_node_info_t **relay_node_info;
} pdu_group_relay_info_t;

typedef struct {
	uint8_t pdu_group_size;
	pdu_group_relay_info_t **pdu_group_relay_info;
} relay_info_t;

typedef struct {
	uint8_t id;
	CAN_HandleTypeDef *hcan_com;
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *gpio_port_spi_cs;
	uint16_t gpio_pin_spi_cs;
	GPIO_TypeDef *gpio_port_spi_wp;
	uint16_t gpio_pin_spi_wp;
	GPIO_TypeDef *gpio_port_force_stop;
	uint16_t gpio_pin_force_stop;
	GPIO_TypeDef *gpio_port_fan;
	uint16_t gpio_pin_fan;
	GPIO_TypeDef *gpio_port_door;
	uint16_t gpio_pin_door;

	power_module_config_t power_module_config;

	uint8_t channel_number;
	uint8_t relay_board_number;

	display_config_t display_config;
	relay_info_t relay_info;
} channels_config_t;

channels_config_t *get_channels_config(uint8_t id);
#endif //_CHANNELS_CONFIG_H
