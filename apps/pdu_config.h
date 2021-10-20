

/*================================================================
 *   
 *   
 *   文件名称：pdu_config.h
 *   创 建 者：肖飞
 *   创建日期：2020年06月30日 星期二 13时06分02秒
 *   修改日期：2021年10月20日 星期三 17时27分00秒
 *   描    述：
 *
 *================================================================*/
#ifndef _PDU_CONFIG_H
#define _PDU_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

//最大支持的pdu组数
#define PDU_GROUP_NUMBER_MAX 2

#pragma pack(push, 1)

typedef enum {
	POWER_MODULE_POLICY_AVERAGE = 0,
	POWER_MODULE_POLICY_PRIORITY,
} power_module_policy_t;

typedef struct {
	//当前组连接的枪数
	uint8_t channel_number;

	//一个模块组内的模块数
	uint8_t power_module_number_per_power_module_group;
} pdu_group_config_t;

typedef struct {
	uint8_t pdu_group_number;

	pdu_group_config_t pdu_group_config[PDU_GROUP_NUMBER_MAX];
	uint8_t policy;
} pdu_config_t;

#pragma pack(pop)

#endif //_PDU_CONFIG_H
