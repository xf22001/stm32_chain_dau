

/*================================================================
 *
 *
 *   文件名称：display_cache.h
 *   创 建 者：肖飞
 *   创建日期：2021年07月17日 星期六 09时42分47秒
 *   修改日期：2022年06月02日 星期四 08时57分40秒
 *   描    述：
 *
 *================================================================*/
#ifndef _DISPLAY_CACHE_H
#define _DISPLAY_CACHE_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

typedef enum {
	DISPLAY_POWER_MODULE_TYPE_PSEUDO = 0,
	DISPLAY_POWER_MODULE_TYPE_HUAWEI,
	DISPLAY_POWER_MODULE_TYPE_INCREASE,
	DISPLAY_POWER_MODULE_TYPE_INFY,
	DISPLAY_POWER_MODULE_TYPE_STATEGRID,
	DISPLAY_POWER_MODULE_TYPE_YYLN,
	DISPLAY_POWER_MODULE_TYPE_WINLINE,
	DISPLAY_POWER_MODULE_TYPE_ZTE,
} display_power_module_type_t;

#pragma pack(push, 1)

typedef struct {
	uint8_t file[64];
	uint8_t func[48];
	uint16_t line;
} exception_location_cache_t;

typedef struct {
	uint8_t sync;
	exception_location_cache_t deadlock_location;
	exception_location_cache_t assert_fail_location;
} display_cache_app_t;

typedef struct {
	uint8_t sync;
	uint8_t power_module_type_sync;
	uint8_t power_module_type;//display_power_module_type_t
} display_cache_channels_t;

typedef struct {
	uint8_t sync;
} display_cache_channel_t;

#pragma pack(pop)

#endif //_DISPLAY_CACHE_H
