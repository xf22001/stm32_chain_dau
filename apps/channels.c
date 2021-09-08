

/*================================================================
 *
 *
 *   文件名称：channels.c
 *   创 建 者：肖飞
 *   创建日期：2020年06月18日 星期四 09时23分30秒
 *   修改日期：2021年09月08日 星期三 16时50分54秒
 *   描    述：
 *
 *================================================================*/
#include "channels.h"

#include <string.h>
#include <stdlib.h>

#include "os_utils.h"
#include "object_class.h"
#include "power_modules.h"
#include "channels_communication.h"
#include "channel_command.h"
#include "modbus_data_value.h"
#include "modbus_addr_handler.h"
#include "config_layout.h"
#include "display.h"

#include "log.h"

typedef union {
	power_module_status_t s;
	uint16_t v;
} u_power_module_status_t;

static object_class_t *channels_class = NULL;

static uint8_t list_size(struct list_head *head)
{
	uint8_t size = 0;
	struct list_head *pos;

	list_for_each(pos, head) {
		size++;
	}

	return size;
}

static int list_contain(struct list_head *item, struct list_head *head)
{
	int ret = -1;
	struct list_head *pos;

	list_for_each(pos, head) {
		if(pos == item) {
			ret = 0;
			break;
		}
	}

	return ret;
}

char *get_channel_state_des(channel_state_t state)
{
	char *des = "unknow";

	switch(state) {
			add_des_case(CHANNEL_STATE_IDLE);
			add_des_case(CHANNEL_STATE_PREPARE_START);
			add_des_case(CHANNEL_STATE_START);
			add_des_case(CHANNEL_STATE_RUNNING);
			add_des_case(CHANNEL_STATE_PREPARE_STOP);
			add_des_case(CHANNEL_STATE_STOP);
			add_des_case(CHANNEL_STATE_RELAY_CHECK);

		default: {
		}
		break;
	}

	return des;
}

char *get_channel_event_type_des(channel_event_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_EVENT_TYPE_UNKNOW);
			add_des_case(CHANNEL_EVENT_TYPE_START_CHANNEL);
			add_des_case(CHANNEL_EVENT_TYPE_STOP_CHANNEL);
			add_des_case(CHANNEL_EVENT_TYPE_RELAY_CHECK);

		default: {
		}
		break;
	}

	return des;
}

char *get_channels_event_type_des(channels_event_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNELS_EVENT_CHANNEL_UNKNOW);
			add_des_case(CHANNELS_EVENT_CHANNEL_EVENT);

		default: {
		}
		break;
	}

	return des;
}

char *get_channels_change_state_des(channels_change_state_t state)
{
	char *des = "unknow";

	switch(state) {
			add_des_case(CHANNELS_CHANGE_STATE_IDLE);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_FREE);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG_SYNC);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_ASSIGN);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_READY_SYNC);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG);
			add_des_case(CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG_SYNC);

		default: {
		}
		break;
	}

	return des;
}

char *get_power_module_policy_des(power_module_policy_t policy)
{
	char *des = "unknow";

	switch(policy) {
			add_des_case(POWER_MODULE_POLICY_PRIORITY);

		default: {
		}
		break;
	}

	return des;
}

int set_fault(bitmap_t *faults, int fault, uint8_t v)
{
	return set_bitmap_value(faults, fault, v);
}

int get_fault(bitmap_t *faults, int fault)
{
	return get_bitmap_value(faults, fault);
}

int get_first_fault(bitmap_t *faults)
{
	int fault = get_first_value_index(faults, 1);

	if(fault >= faults->size) {
		fault = -1;
	}

	return fault;
}

static void print_power_module_item_info(const uint8_t pad, power_module_item_info_t *power_module_item_info)
{
	//u_power_module_status_t u_power_module_status;
	char *_pad = (char *)os_calloc(1, pad + 1);

	OS_ASSERT(_pad != NULL);
	memset(_pad, '\t', pad);

	_printf("%smodule_id:%d\n", _pad, power_module_item_info->module_id);
	_printf("%s\tstate:%s\n", _pad, get_power_module_item_state_des(power_module_item_info->status.state));
	_printf("%s\trequire_output_voltage:%d\n", _pad, power_module_item_info->status.require_output_voltage);
	_printf("%s\trequire_output_current:%d\n", _pad, power_module_item_info->status.require_output_current);
	_printf("%s\tsetting_output_voltage:%d\n", _pad, power_module_item_info->status.setting_output_voltage);
	_printf("%s\tsetting_output_current:%d\n", _pad, power_module_item_info->status.setting_output_current);
	_printf("%s\tsmooth_setting_output_current:%d\n", _pad, power_module_item_info->status.smooth_setting_output_current);
	_printf("%s\tmodule_output_voltage:%d\n", _pad, power_module_item_info->status.module_output_voltage);
	_printf("%s\tmodule_output_current:%d\n", _pad, power_module_item_info->status.module_output_current);
	//_printf("%s\tconnect_state:%d\n", _pad, power_module_item_info->status.connect_state);
	//u_power_module_status.v = power_module_item_info->status.module_status;
	//_printf("%s\tpoweroff:%d\n", _pad, u_power_module_status.s.poweroff);
	//_printf("%s\tfault:%d\n", _pad, u_power_module_status.s.fault);
	//_printf("%s\toutput_state:%d\n", _pad, u_power_module_status.s.output_state);
	//_printf("%s\tfan_state:%d\n", _pad, u_power_module_status.s.fan_state);
	//_printf("%s\tinput_overvoltage:%d\n", _pad, u_power_module_status.s.input_overvoltage);
	//_printf("%s\tinput_lowvoltage:%d\n", _pad, u_power_module_status.s.input_lowvoltage);
	//_printf("%s\toutput_overvoltage:%d\n", _pad, u_power_module_status.s.output_overvoltage);
	//_printf("%s\toutput_lowvoltage:%d\n", _pad, u_power_module_status.s.output_lowvoltage);
	//_printf("%s\tprotect_overcurrent:%d\n", _pad, u_power_module_status.s.protect_overcurrent);
	//_printf("%s\tprotect_overtemperature:%d\n", _pad, u_power_module_status.s.protect_overtemperature);
	//_printf("%s\tsetting_poweroff:%d\n", _pad, u_power_module_status.s.setting_poweroff);

	os_free(_pad);
}

static void print_power_module_group_info(const uint8_t pad, power_module_group_info_t *power_module_group_info)
{
	char *_pad = (char *)os_calloc(1, pad + 1);
	struct list_head *head = &power_module_group_info->power_module_item_list;
	power_module_item_info_t *power_module_item_info;

	OS_ASSERT(_pad != NULL);
	memset(_pad, '\t', pad);

	_printf("%spdu_group_id:%d\n", _pad, power_module_group_info->pdu_group_info->pdu_group_id);
	_printf("%sgroup_id:%d\n", _pad, power_module_group_info->group_id);
	list_for_each_entry(power_module_item_info, head, power_module_item_info_t, list) {
		print_power_module_item_info(pad + 1, power_module_item_info);
	}

	os_free(_pad);
}

static void print_channel_info(const uint8_t pad, channel_info_t *channel_info)
{
	char *_pad = (char *)os_calloc(1, pad + 1);

	OS_ASSERT(_pad != NULL);
	memset(_pad, '\t', pad);

	_printf("%schannel_id:%d\n", _pad, channel_info->channel_id);
	_printf("%s\tstate:%s\n", _pad, get_channel_state_des(channel_info->status.state));
	_printf("%s\trequire_output_voltage:%d\n", _pad, channel_info->status.require_output_voltage);
	_printf("%s\trequire_output_current:%d\n", _pad, channel_info->status.require_output_current);
	_printf("%s\trequire_work_state:%d\n", _pad, channel_info->status.require_work_state);
	_printf("%s\treassign:%d\n", _pad, channel_info->status.reassign);
	_printf("%s\treassign_module_number:%d\n", _pad, channel_info->status.reassign_module_group_number);
	_printf("%s\tcharge_output_voltage:%d\n", _pad, channel_info->status.charge_output_voltage);
	_printf("%s\tcharge_output_current:%d\n", _pad, channel_info->status.charge_output_current);
	//_printf("%s\tconnect_state:%d\n", _pad, channel_info->status.connect_state);

	os_free(_pad);
}

static int try_to_start_channel(channel_info_t *channel_info)
{
	int ret = -1;

	channel_info->channel_request_state = CHANNEL_REQUEST_STATE_START;

	switch(channel_info->status.state) {
		case CHANNEL_STATE_IDLE: {
			debug("channel_id %d start", channel_info->channel_id);
			channel_info->status.module_ready_notify = 1;

			ret = 0;
		}
		break;

		case CHANNEL_STATE_PREPARE_STOP:
		case CHANNEL_STATE_STOP: {
			channels_info_t *channels_info = channel_info->channels_info;
			channels_com_info_t *channels_com_info = (channels_com_info_t *)channels_info->channels_com_info;

			debug("channel_id %d start with state %s, retry!",
			      channel_info->channel_id,
			      get_channel_state_des(channel_info->status.state));
			channels_com_channel_module_assign_info(channels_com_info, channel_info->channel_id, MODULE_ASSIGN_INFO_RETRY);
		}
		break;

		default: {
			debug("channel_id %d start with state %s",
			      channel_info->channel_id,
			      get_channel_state_des(channel_info->status.state));
		}
		break;
	}

	return ret;
}

static int try_to_stop_channel(channel_info_t *channel_info)
{
	int ret = -1;

	channel_info->channel_request_state = CHANNEL_REQUEST_STATE_STOP;

	if(channel_info->status.state != CHANNEL_STATE_IDLE) {
		debug("channel_id %d stop", channel_info->channel_id);

		ret = 0;
	} else {
		debug("channel_id %d stop with state %s",
		      channel_info->channel_id,
		      get_channel_state_des(channel_info->status.state));
	}

	return ret;
}

static void default_handle_channel_event(void *_channel_info, void *_channels_event)
{
	uint8_t match = 0;

	channel_info_t *channel_info = (channel_info_t *)_channel_info;
	channels_event_t *channels_event = (channels_event_t *)_channels_event;
	channel_event_t *channel_event = channels_event->event;

	if(channels_event->type != CHANNELS_EVENT_CHANNEL_EVENT) {
		return;
	}

	if(channel_event == NULL) {
		return;
	}

	if(channel_event->channel_id == channel_info->channel_id) {
		match = 1;
	} else if(channel_event->channel_id == 0xff) {
		match = 1;
	}

	if(match == 0) {
		return;
	}

	debug("channel_id %d process event %s!", channel_info->channel_id, get_channel_event_type_des(channel_event->type));

	switch(channel_event->type) {
		case CHANNEL_EVENT_TYPE_START_CHANNEL: {
			if(try_to_start_channel(channel_info) != 0) {
			}
		}
		break;

		case CHANNEL_EVENT_TYPE_STOP_CHANNEL: {
			if(try_to_stop_channel(channel_info) != 0) {
			}
		}
		break;

		case CHANNEL_EVENT_TYPE_RELAY_CHECK: {
		}

		default: {
		}
		break;
	}
}

static uint16_t get_single_module_max_output_current(channels_settings_t *channels_settings)
{
	if(channels_settings->module_max_output_current == 0) {
		return 60 * 10;
	}

	return channels_settings->module_max_output_current;
}

#define CHANNEL_INFO_PREPARE_START_TIMEOUT (30 * 1000)
#define CHANNEL_INFO_PREPARE_STOP_TIMEOUT (30 * 1000)

static void module_voltage_current_correction(channels_settings_t *channels_settings, uint16_t *voltage, uint16_t *current)
{
	if(*voltage == 0) {
		*current = 0;
		return;
	}

	if(*current == 0) {
		*voltage = 0;
		return;
	}

	if(*voltage > channels_settings->module_max_output_voltage) {
		*voltage = channels_settings->module_max_output_voltage;
	}

	if(*voltage < channels_settings->module_min_output_voltage) {
		*voltage = channels_settings->module_min_output_voltage;
	}

	if(*current > channels_settings->module_max_output_current) {
		*current = channels_settings->module_max_output_current;
	}

	if(*current < channels_settings->module_min_output_current) {
		*current = channels_settings->module_min_output_current;
	}
}

static void module_power_limit_correction(channels_settings_t *channels_settings, uint16_t *voltage, uint16_t *current)
{
	uint32_t power = *voltage * *current;

	if(channels_settings->module_max_output_power != 0) {
		uint32_t max_power = channels_settings->module_max_output_power * 100;

		if(power > max_power) {
			*current = max_power / *voltage;
		}
	}
}

static void handle_channel_state(channel_info_t *channel_info)
{
	pdu_group_info_t *pdu_group_info = channel_info->pdu_group_info;
	uint8_t channel_id = channel_info->channel_id;
	uint32_t ticks = osKernelSysTick();

	if(ticks_duration(ticks, channel_info->status.connect_state_stamp) >= 1 * 1000) {
		channels_info_t *channels_info = channel_info->channels_info;
		channels_com_info_t *channels_com_info = (channels_com_info_t *)channels_info->channels_com_info;
		channel_info->status.connect_state = channels_com_get_connect_state(channels_com_info, channel_id);
	}

	switch(channel_info->status.state) {
		case CHANNEL_STATE_IDLE: {
			if(channel_info->channel_request_state == CHANNEL_REQUEST_STATE_START) {//开机
				channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
				pdu_config_t *pdu_config = &channels_info->channels_settings.pdu_config;

				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;

				if(pdu_config->policy == POWER_MODULE_POLICY_PRIORITY) {
					channel_info->status.reassign = 1;//使能二次重分配
					channel_info->status.reassign_module_group_number = 1;
				} else {
					channel_info->status.reassign = 0;
					channel_info->status.reassign_module_group_number = 0;
				}

				list_move_tail(&channel_info->list, &pdu_group_info->channel_active_list);
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE;//模块组重分配

				channel_info->channel_state_change_stamp = ticks;
				channel_info->status.state = CHANNEL_STATE_PREPARE_START;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			} else if(channel_info->channel_request_state == CHANNEL_REQUEST_STATE_RELAY_CHECK) {
				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;
				channel_info->status.state = CHANNEL_STATE_RELAY_CHECK;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			}
		}
		break;

		case CHANNEL_STATE_PREPARE_START: {
			if(channel_info->channel_request_state == CHANNEL_REQUEST_STATE_STOP) {//关机
				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;

				list_move_tail(&channel_info->list, &pdu_group_info->channel_deactive_list);
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE;//重新分配模块组

				channel_info->channel_state_change_stamp = ticks;
				channel_info->status.state = CHANNEL_STATE_PREPARE_STOP;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			} else {
				if(list_contain(&channel_info->list, &pdu_group_info->channel_active_list) != 0) {//无法启动，停止启动过程
					channel_info->status.state = CHANNEL_STATE_IDLE;
					debug("channel_id %d start failed! channel_id %d to state %s failed!", channel_id, channel_id, get_channel_state_des(channel_info->status.state));
				} else {
					if(pdu_group_info->channels_change_state != CHANNELS_CHANGE_STATE_IDLE) {//配置没有同步完成
						if(ticks_duration(ticks, channel_info->channel_state_change_stamp) >= CHANNEL_INFO_PREPARE_START_TIMEOUT) {//超时,报警?
							debug("channels_change_state %s! channel_id %d in state %s!",
							      get_channels_change_state_des(pdu_group_info->channels_change_state),
							      channel_id,
							      get_channel_state_des(channel_info->status.state));

							list_move_tail(&channel_info->list, &pdu_group_info->channel_deactive_list);
							pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE;//重新分配模块组

							channel_info->channel_state_change_stamp = ticks;
							channel_info->status.state = CHANNEL_STATE_PREPARE_STOP;
							debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
						}
					} else {
						channel_info->status.state = CHANNEL_STATE_START;
						debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
					}
				}
			}
		}
		break;

		case CHANNEL_STATE_START: {
			channel_info->status.state = CHANNEL_STATE_RUNNING;
			debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
		}
		break;

		case CHANNEL_STATE_RUNNING: {
			if(channel_info->channel_request_state == CHANNEL_REQUEST_STATE_STOP) {//关机
				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;

				list_move_tail(&channel_info->list, &pdu_group_info->channel_deactive_list);
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE;//重新分配模块组

				channel_info->channel_state_change_stamp = ticks;
				channel_info->status.state = CHANNEL_STATE_PREPARE_STOP;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			} else {
				channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
				pdu_config_t *pdu_config = &channels_info->channels_settings.pdu_config;
				channels_settings_t *channels_settings = &channels_info->channels_settings;

				if(pdu_config->policy == POWER_MODULE_POLICY_PRIORITY) {
					if(channel_info->status.require_work_state == CHANNEL_WORK_STATE_CHARGE) {
						uint16_t single_module_max_output_current = get_single_module_max_output_current(channels_settings);
						uint16_t require_output_current = channel_info->status.require_output_current;
						uint16_t require_output_voltage = channel_info->status.require_output_voltage;

						module_voltage_current_correction(channels_settings, &require_output_voltage, &single_module_max_output_current);

						//debug("channel_id %d require_output_voltage:%d", channel_id, require_output_voltage);
						//debug("channel_id %d single_module_max_output_current:%d", channel_id, single_module_max_output_current);

						if(channel_info->status.charge_output_voltage != 0) {
							require_output_voltage = channel_info->status.charge_output_voltage;
							//debug("channel_id %d require_output_voltage:%d", channel_id, require_output_voltage);
						}

						module_power_limit_correction(channels_settings, &require_output_voltage, &single_module_max_output_current);

						//debug("channel_id %d single_module_max_output_current:%d", channel_id, single_module_max_output_current);

						if(single_module_max_output_current != 0) {
							pdu_group_info_t *pdu_group_info = channel_info->pdu_group_info;
							pdu_group_config_t *pdu_group_config = &pdu_config->pdu_group_config[pdu_group_info->pdu_group_id];
							uint16_t module_group_output_current = single_module_max_output_current * pdu_group_config->power_module_number_per_power_module_group;

							if(channel_info->status.reassign == 1) {
								channel_info->status.reassign = 0;
								channel_info->status.reassign_module_group_number = (require_output_current + (module_group_output_current - 1)) / module_group_output_current;
								debug("channel_id %d require_output_current:%d", channel_id, require_output_current);
								debug("channel_id %d module_group_output_current:%d", channel_id, module_group_output_current);


								debug("channel_id %d reassign_module_group_number:%d", channel_id, channel_info->status.reassign_module_group_number);
								pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE;//模块组重分配
							} else {
								if(pdu_group_info->channels_change_state == CHANNELS_CHANGE_STATE_IDLE) {
									if(channel_info->status.reassign_module_group_number != ((require_output_current + (module_group_output_current - 1)) / module_group_output_current)) {
										debug("request channel_id %d reassign!!!!!!!!!", channel_id);
										channel_info->status.reassign = 1;
									}
								}
							}

						} else {
							debug("channel_id %d require_output_current:%d, require_output_voltage:%d", channel_info->channel_id, require_output_current, require_output_voltage);
						}
					}
				}
			}
		}
		break;

		case CHANNEL_STATE_PREPARE_STOP: {
			if(pdu_group_info->channels_change_state != CHANNELS_CHANGE_STATE_IDLE) {//配置没有同步完成
				//if(ticks_duration(ticks, channel_info->channel_state_change_stamp) >= CHANNEL_INFO_PREPARE_STOP_TIMEOUT) {//超时,报警?
				//	debug("channels_change_state %s! channel_id %d in state %s!", get_channels_change_state_des(pdu_group_info->channels_change_state), channel_id, get_channel_state_des(channel_info->status.state));
				//}
			} else {
				channel_info->status.state = CHANNEL_STATE_STOP;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			}

		}
		break;

		case CHANNEL_STATE_STOP: {
			channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;

			list_move_tail(&channel_info->list, &pdu_group_info->channel_idle_list);

			channel_info->status.state = CHANNEL_STATE_IDLE;
			debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
		}
		break;

		case CHANNEL_STATE_RELAY_CHECK: {//自检中
			if(channel_info->channel_request_state == CHANNEL_REQUEST_STATE_STOP) {//关机
				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_NONE;
			} else {
				channel_info->status.state = CHANNEL_STATE_IDLE;
				debug("channel_id %d to state %s", channel_id, get_channel_state_des(channel_info->status.state));
			}
		}
		break;

		default: {
		}
		break;
	}
}

static void handle_power_module_group_info(channel_info_t *channel_info, uint16_t module_require_voltage, uint16_t module_group_require_current)
{
	power_module_group_info_t *power_module_group_info;
	channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
	channels_settings_t *channels_settings = &channels_info->channels_settings;
	struct list_head *head = &channel_info->power_module_group_list;
	list_for_each_entry(power_module_group_info, head, power_module_group_info_t, list) {
		struct list_head *head1 = &power_module_group_info->power_module_item_list;
		uint16_t module_require_current;
		power_module_item_info_t *power_module_item_info;
		uint8_t power_module_number_per_power_module_group = list_size(head1);

		if(power_module_number_per_power_module_group == 0) {
			debug("");
			continue;
		}

		module_require_current = module_group_require_current / power_module_number_per_power_module_group;

		if((module_require_current == 0) && (channel_info->status.require_output_current != 0)) {
			module_require_current = channels_settings->module_min_output_current;
			debug("module_require_current:%d", module_require_current);
		}

		module_voltage_current_correction(channels_settings, &module_require_voltage, &module_require_current);
		module_power_limit_correction(channels_settings, &channel_info->status.charge_output_voltage, &module_require_current);

		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			power_module_item_info->status.require_output_voltage = module_require_voltage;
			power_module_item_info->status.require_output_current = module_require_current;
			//debug("setting module_id:%d, require_output_voltage:%d, require_output_current:%d",
			//      power_module_item_info->module_id,
			//      power_module_item_info->status.require_output_voltage,
			//      power_module_item_info->status.require_output_current);
		}
	}
}

static void handle_channel_power_module_group_info(channel_info_t *channel_info)
{
	struct list_head *head;
	uint8_t module_group_size;
	uint16_t module_require_voltage;
	uint16_t module_group_require_current;

	head = &channel_info->power_module_group_list;

	module_group_size = list_size(head);

	if(module_group_size == 0) {
		return;
	}

	if(channel_info->status.require_output_stamp == channel_info->status.module_sync_stamp) {
		return;
	}

	channel_info->status.module_sync_stamp = channel_info->status.require_output_stamp;

	//debug("setting channel_id %d, require_output_voltage:%d, require_output_current:%d",
	//      channel_info->channel_id,
	//      channel_info->status.require_output_voltage,
	//      channel_info->status.require_output_current);

	module_require_voltage = channel_info->status.require_output_voltage;
	module_group_require_current = channel_info->status.require_output_current / module_group_size;

	handle_power_module_group_info(channel_info, module_require_voltage, module_group_require_current);
}

static void default_handle_channel_periodic(void *fn_ctx, void *chain_ctx)
{
	channel_info_t *channel_info = (channel_info_t *)fn_ctx;

	handle_channel_power_module_group_info(channel_info);
	handle_channel_state(channel_info);
}

void free_channels_info(channels_info_t *channels_info)
{
	app_panic();
}

static uint8_t get_power_module_group_number_per_pdu_group_config(pdu_group_config_t *pdu_group_config)
{
	return pdu_group_config->channel_number;
}

static uint8_t get_power_module_group_number(pdu_config_t *pdu_config)
{
	uint8_t power_module_group_number = 0;
	int i;

	for(i = 0; i < pdu_config->pdu_group_number; i++) {
		pdu_group_config_t *pdu_group_config = &pdu_config->pdu_group_config[i];
		power_module_group_number += get_power_module_group_number_per_pdu_group_config(pdu_group_config);
	}

	return power_module_group_number;
}

static uint8_t get_power_module_item_number_per_pdu_group_config(pdu_group_config_t *pdu_group_config)
{
	uint8_t power_module_number = 0;

	//一枪对应一个模块组
	power_module_number += pdu_group_config->channel_number * pdu_group_config->power_module_number_per_power_module_group;

	return power_module_number;
}

static uint8_t get_power_module_item_number(pdu_config_t *pdu_config)
{
	uint8_t power_module_number = 0;
	int i;

	for(i = 0; i < pdu_config->pdu_group_number; i++) {
		pdu_group_config_t *pdu_group_config = &pdu_config->pdu_group_config[i];
		power_module_number += get_power_module_item_number_per_pdu_group_config(pdu_group_config);
	}

	return power_module_number;
}

static uint8_t get_channel_number(pdu_config_t *pdu_config)
{
	uint8_t channel_number = 0;
	int i;

	for(i = 0; i < pdu_config->pdu_group_number; i++) {
		pdu_group_config_t *pdu_group_config = &pdu_config->pdu_group_config[i];
		channel_number += pdu_group_config->channel_number;
	}

	return channel_number;
}

static int channels_info_load_config(channels_info_t *channels_info)
{
	config_layout_t *config_layout = get_config_layout();
	size_t offset = (size_t)&config_layout->channels_settings_seg.storage_channels_settings.channels_settings;
	debug("offset:%d", offset);
	return load_config_item(channels_info->storage_info, "dau", (uint8_t *)&channels_info->channels_settings, sizeof(channels_settings_t), offset);
}

static int channels_info_save_config(channels_info_t *channels_info)
{
	config_layout_t *config_layout = get_config_layout();
	size_t offset = (size_t)&config_layout->channels_settings_seg.storage_channels_settings.channels_settings;
	debug("offset:%d", offset);
	return save_config_item(channels_info->storage_info, "dau", (uint8_t *)&channels_info->channels_settings, sizeof(channels_settings_t), offset);
}

static void restore_channels_settings(channels_settings_t *channels_settings)
{
	pdu_config_t *pdu_config = &channels_settings->pdu_config;
	pdu_group_config_t *pdu_group_config;

	memset(channels_settings, 0, sizeof(channels_settings_t));

	pdu_config->policy = POWER_MODULE_POLICY_PRIORITY;

	pdu_config->pdu_group_number = 1;

	pdu_group_config = &pdu_config->pdu_group_config[0];
	pdu_group_config->channel_number = 5;
	pdu_group_config->power_module_number_per_power_module_group = 3;

	//pdu_group_config = &pdu_config->pdu_group_config[1];
	//pdu_group_config->channel_number = 5;
	//pdu_group_config->power_module_number_per_power_module_group = 3;

	//channels_settings->power_module_type = POWER_MODULE_TYPE_WINLINE;
	channels_settings->power_module_type = POWER_MODULE_TYPE_PSEUDO;
	channels_settings->module_max_output_voltage = 10000;
	channels_settings->module_min_output_voltage = 2000;
	channels_settings->module_max_output_current = 10000;
	channels_settings->module_min_output_current = 1;
	channels_settings->module_max_output_power = 20000;
}


static void init_channels_settings(channels_info_t *channels_info)
{
	int ret = -1;
	channels_settings_t *channels_settings = &channels_info->channels_settings;

	ret = channels_info_load_config(channels_info);

	ret = -1;

	if(ret != 0) {
		debug("load config failed! restore config...");
		restore_channels_settings(channels_settings);
		channels_info_save_config(channels_info);
	} else {
		debug("load config successfully!");
	}

	load_channels_display_cache(channels_info);
}

static relay_node_info_t *get_relay_node_info_by_channel_id(channels_config_t *channels_config, uint8_t pdu_group_id, uint8_t channel_id_a, uint8_t channel_id_b)
{
	relay_node_info_t *relay_node_info = NULL;
	relay_info_t *relay_info = &channels_config->relay_info;
	pdu_group_relay_info_t *pdu_group_relay_info = NULL;
	int i;

	OS_ASSERT(channel_id_a != channel_id_b);

	for(i = 0; i < relay_info->pdu_group_size; i++) {
		pdu_group_relay_info_t *pdu_group_relay_info_item = relay_info->pdu_group_relay_info[i];

		if(pdu_group_relay_info_item->pdu_group_id == pdu_group_id) {
			pdu_group_relay_info = pdu_group_relay_info_item;
			break;
		}
	}

	if(pdu_group_relay_info == NULL) {
		return relay_node_info;
	}

	for(i = 0; i < pdu_group_relay_info->size; i++) {
		relay_node_info_t *relay_node_info_item = pdu_group_relay_info->relay_node_info[i];
		int j;
		uint8_t found = 1;

		for(j = 0; j < 2; j++) {
			if(channel_id_a == relay_node_info_item->channel_id[j]) {
				break;
			}
		}

		if(j == 2) {
			found = 0;
			continue;
		}

		for(j = 0; j < 2; j++) {
			if(channel_id_b == relay_node_info_item->channel_id[j]) {
				break;
			}
		}

		if(j == 2) {
			found = 0;
			continue;
		}

		if(found != 0) {
			relay_node_info = relay_node_info_item;
			break;
		}
	}

	return relay_node_info;
}

static relay_node_info_t *get_relay_node_info_relay_id(channels_config_t *channels_config, uint8_t pdu_group_id, uint8_t relay_id)
{
	relay_node_info_t *relay_node_info = NULL;
	relay_info_t *relay_info = &channels_config->relay_info;
	pdu_group_relay_info_t *pdu_group_relay_info = NULL;
	int i;

	for(i = 0; i < relay_info->pdu_group_size; i++) {
		pdu_group_relay_info_t *pdu_group_relay_info_item = relay_info->pdu_group_relay_info[i];

		if(pdu_group_relay_info_item->pdu_group_id == pdu_group_id) {
			pdu_group_relay_info = pdu_group_relay_info_item;
			break;
		}
	}

	if(pdu_group_relay_info == NULL) {
		return relay_node_info;
	}

	for(i = 0; i < pdu_group_relay_info->size; i++) {
		relay_node_info_t *relay_node_info_item = pdu_group_relay_info->relay_node_info[i];

		if(relay_id == relay_node_info_item->relay_id) {
			relay_node_info = relay_node_info_item;
			break;
		}
	}

	return relay_node_info;
}

static channel_relay_fb_node_info_t *get_channel_relay_fb_node_info(channels_config_t *channels_config, uint8_t pdu_group_id, uint8_t channel_id)
{
	channel_relay_fb_node_info_t *channel_relay_fb_node_info = NULL;
	channel_relay_fb_info_t *channel_relay_fb_info = &channels_config->channel_relay_fb_info;
	pdu_group_channel_relay_fb_info_t *pdu_group_channel_relay_fb_info = NULL;
	int i;

	for(i = 0; i < channel_relay_fb_info->pdu_group_size; i++) {
		pdu_group_channel_relay_fb_info_t *pdu_group_channel_relay_fb_info_item = channel_relay_fb_info->pdu_group_channel_relay_fb_info[i];

		if(pdu_group_channel_relay_fb_info_item->pdu_group_id == pdu_group_id) {
			pdu_group_channel_relay_fb_info = pdu_group_channel_relay_fb_info_item;
			break;
		}
	}

	if(pdu_group_channel_relay_fb_info == NULL) {
		return channel_relay_fb_node_info;
	}

	for(i = 0; i < pdu_group_channel_relay_fb_info->size; i++) {
		channel_relay_fb_node_info_t *channel_relay_fb_node_info_item = pdu_group_channel_relay_fb_info->channel_relay_fb_node_info[i];

		if(channel_relay_fb_node_info_item->channel_id == channel_id) {
			channel_relay_fb_node_info = channel_relay_fb_node_info_item;
			break;
		}
	}

	return channel_relay_fb_node_info;
}


static void channel_info_deactive_power_module_group(channel_info_t *channel_info)
{
	struct list_head *pos;
	struct list_head *n;
	struct list_head *head;
	pdu_group_info_t *pdu_group_info = channel_info->pdu_group_info;

	head = &channel_info->power_module_group_list;

	list_for_each_safe(pos, n, head) {
		power_module_group_info_t *power_module_group_info = list_entry(pos, power_module_group_info_t, list);
		power_module_item_info_t *power_module_item_info;
		struct list_head *head1 = &power_module_group_info->power_module_item_list;
		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE;
		}
		list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_deactive_list);
	}
}

static void free_power_module_group_for_stop_channel(pdu_group_info_t *pdu_group_info)
{
	channel_info_t *channel_info;
	struct list_head *head;

	head = &pdu_group_info->channel_deactive_list;

	list_for_each_entry(channel_info, head, channel_info_t, list) {
		channel_info_deactive_power_module_group(channel_info);
	}
}

static void channel_info_deactive_unneeded_power_module_group_priority(channel_info_t *channel_info)//POWER_MODULE_POLICY_PRIORITY
{
	struct list_head *pos;
	struct list_head *n;
	struct list_head *head;
	pdu_group_info_t *pdu_group_info = channel_info->pdu_group_info;
	channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
	channels_config_t *channels_config = channels_info->channels_config;
	struct list_head list_unneeded_power_module_group = LIST_HEAD_INIT(list_unneeded_power_module_group);
	channel_info_t *channel_info_item;
	channel_info_t *channel_info_item_prev;
	power_module_group_info_t *power_module_group_info_item;
	uint8_t assigned;
	channel_relay_fb_node_info_t *channel_relay_fb_node_info;

	head = &channel_info->power_module_group_list;

	list_for_each_safe(pos, n, head) {
		power_module_group_info_t *power_module_group_info = list_entry(pos, power_module_group_info_t, list);
		list_move_tail(&power_module_group_info->list, &list_unneeded_power_module_group);
	}

	power_module_group_info_item = channels_info->power_module_group_info + channel_info->channel_id;

	if(list_contain(&power_module_group_info_item->list, &list_unneeded_power_module_group) == 0) {
		//恢复模块归属
		list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);
	}

	assigned = list_size(&channel_info->power_module_group_list);

	if(assigned >= channel_info->status.reassign_module_group_number) {
		debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
		goto exit;
	}

	//left search
	channel_info_item_prev = channel_info;
	channel_info_item = channel_info;

	while(channel_info_item != NULL) {
		uint8_t next_channel_id;
		uint8_t next_power_module_group_id;
		relay_node_info_t *relay_node_info;

		assigned = list_size(&channel_info->power_module_group_list);

		if(assigned >= channel_info->status.reassign_module_group_number) {
			debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
			goto exit;
		}

		if(channel_info_item->channel_id == 0) {
			next_channel_id = channels_info->channel_number - 1;
		} else {
			next_channel_id = channel_info_item->channel_id - 1;
		}

		channel_info_item = channels_info->channel_info + next_channel_id;

		if(channel_info_item->pdu_group_info->pdu_group_id != pdu_group_info->pdu_group_id) {
			continue;
		}

		if(list_contain(&channel_info_item->list, &pdu_group_info->channel_active_list) == 0) {
			break;
		}

		channel_relay_fb_node_info = get_channel_relay_fb_node_info(channels_config, pdu_group_info->pdu_group_id, channel_info_item->channel_id);
		OS_ASSERT(channel_relay_fb_node_info != NULL);

		while(HAL_GPIO_ReadPin(channel_relay_fb_node_info->gpio_port_fb, channel_relay_fb_node_info->gpio_pin_fb) == GPIO_PIN_SET) {
			debug("");
		}

		next_power_module_group_id = next_channel_id;
		power_module_group_info_item = channels_info->power_module_group_info + next_power_module_group_id;

		if(list_contain(&power_module_group_info_item->list, &list_unneeded_power_module_group) != 0) {
			break;
		}

		//恢复模块归属
		list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);

		//set relay channel_info_item_prev---channel_info_item, by id
		//todo
		relay_node_info = get_relay_node_info_by_channel_id(channels_config,
		                  pdu_group_info->pdu_group_id,
		                  channel_info_item_prev->channel_id,
		                  channel_info_item->channel_id);
		OS_ASSERT(relay_node_info != NULL);
		set_bitmap_value(pdu_group_info->relay_map, relay_node_info->relay_id, 1);

		channel_info_item_prev = channel_info_item;
	}

	//right search
	channel_info_item_prev = channel_info;
	channel_info_item = channel_info;

	while(channel_info_item != NULL) {
		uint8_t next_channel_id;
		uint8_t next_power_module_group_id;
		relay_node_info_t *relay_node_info;

		assigned = list_size(&channel_info->power_module_group_list);

		if(assigned >= channel_info->status.reassign_module_group_number) {
			debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
			goto exit;
		}

		if(channel_info_item->channel_id == (channels_info->channel_number - 1)) {
			next_channel_id = 0;
		} else {
			next_channel_id = channel_info_item->channel_id + 1;
		}

		channel_info_item = channels_info->channel_info + next_channel_id;

		if(channel_info_item->pdu_group_info->pdu_group_id != pdu_group_info->pdu_group_id) {
			continue;
		}

		if(list_contain(&channel_info_item->list, &pdu_group_info->channel_active_list) == 0) {
			break;
		}

		channel_relay_fb_node_info = get_channel_relay_fb_node_info(channels_config, pdu_group_info->pdu_group_id, channel_info_item->channel_id);
		OS_ASSERT(channel_relay_fb_node_info != NULL);

		while(HAL_GPIO_ReadPin(channel_relay_fb_node_info->gpio_port_fb, channel_relay_fb_node_info->gpio_pin_fb) == GPIO_PIN_SET) {
			debug("");
		}

		next_power_module_group_id = next_channel_id;
		power_module_group_info_item = channels_info->power_module_group_info + next_power_module_group_id;

		if(list_contain(&power_module_group_info_item->list, &list_unneeded_power_module_group) != 0) {
			break;
		}

		//恢复模块归属
		list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);

		//set relay channel_info_item_prev---channel_info_item, by id
		//todo
		relay_node_info = get_relay_node_info_by_channel_id(channels_config,
		                  pdu_group_info->pdu_group_id,
		                  channel_info_item_prev->channel_id,
		                  channel_info_item->channel_id);
		OS_ASSERT(relay_node_info != NULL);
		set_bitmap_value(pdu_group_info->relay_map, relay_node_info->relay_id, 1);

		channel_info_item_prev = channel_info_item;
	}

exit:
	head = &list_unneeded_power_module_group;

	//清理需要关闭的模块
	list_for_each_safe(pos, n, head) {
		power_module_group_info_t *power_module_group_info = list_entry(pos, power_module_group_info_t, list);
		power_module_item_info_t *power_module_item_info;
		struct list_head *head1 = &power_module_group_info->power_module_item_list;
		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE;
		}
		list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_deactive_list);
		debug("remove module group_id %d from channel_id %d", power_module_group_info->group_id, channel_info->channel_id);
	}
}

static void free_power_module_group_for_active_channel_priority(pdu_group_info_t *pdu_group_info)//POWER_MODULE_POLICY_PRIORITY
{
	channel_info_t *channel_info;
	struct list_head *head;

	head = &pdu_group_info->channel_active_list;

	list_for_each_entry(channel_info, head, channel_info_t, list) {
		channel_info_deactive_unneeded_power_module_group_priority(channel_info);
	}
}

static void clean_up_relay_map(pdu_group_info_t *pdu_group_info)
{
	int i;

	for(i = 0; i < pdu_group_info->channel_number; i++) {
		set_bitmap_value(pdu_group_info->relay_map, i, 0);
	}
}

static int pdu_group_info_free_power_module_group_priority(pdu_group_info_t *pdu_group_info)
{
	int ret = -1;

	//清理继电器
	clean_up_relay_map(pdu_group_info);

	//链式分配模块间会相互牵扯,释放所有模块,重新计算
	//释放要停机通道的模块
	free_power_module_group_for_stop_channel(pdu_group_info);
	//释放多余模块,并恢复需要的继电器
	free_power_module_group_for_active_channel_priority(pdu_group_info);

	ret = 0;
	return ret;
}

static void channel_info_assign_power_module_group(channel_info_t *channel_info)
{
	pdu_group_info_t *pdu_group_info = channel_info->pdu_group_info;
	channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
	channels_config_t *channels_config = channels_info->channels_config;
	struct list_head *head;
	channel_info_t *channel_info_item;
	channel_info_t *channel_info_item_prev;
	power_module_group_info_t *power_module_group_info_item;
	uint8_t assigned;
	channel_relay_fb_node_info_t *channel_relay_fb_node_info;

	power_module_group_info_item = channels_info->power_module_group_info + channel_info->channel_id;

	if(list_contain(&power_module_group_info_item->list, &pdu_group_info->power_module_group_idle_list) == 0) {
		power_module_item_info_t *power_module_item_info;
		head = &power_module_group_info_item->power_module_item_list;
		list_for_each_entry(power_module_item_info, head, power_module_item_info_t, list) {
			if(power_module_item_info->status.state != POWER_MODULE_ITEM_STATE_IDLE) {
				debug("power module state is not idle:%s!!!", get_power_module_item_state_des(power_module_item_info->status.state));
			}

			power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_ACTIVE;
		}
		power_module_group_info_item->channel_info = channel_info;
		list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);
		debug("assign module group_id %d to channel_id %d", power_module_group_info_item->group_id, channel_info->channel_id);
	}

	assigned = list_size(&channel_info->power_module_group_list);

	if(assigned >= channel_info->status.reassign_module_group_number) {
		debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
		return;
	}

	//left search
	channel_info_item_prev = channel_info;
	channel_info_item = channel_info;

	while(channel_info_item != NULL) {
		uint8_t next_channel_id;
		uint8_t next_power_module_group_id;
		uint8_t find_power_module_group = 0;
		relay_node_info_t *relay_node_info;

		if(channel_info_item->channel_id == 0) {
			next_channel_id = channels_info->channel_number - 1;
		} else {
			next_channel_id = channel_info_item->channel_id - 1;
		}

		channel_info_item = channels_info->channel_info + next_channel_id;

		if(channel_info_item->pdu_group_info->pdu_group_id != pdu_group_info->pdu_group_id) {
			continue;
		}

		if(list_contain(&channel_info_item->list, &pdu_group_info->channel_active_list) == 0) {
			break;
		}

		channel_relay_fb_node_info = get_channel_relay_fb_node_info(channels_config, pdu_group_info->pdu_group_id, channel_info_item->channel_id);
		OS_ASSERT(channel_relay_fb_node_info != NULL);

		while(HAL_GPIO_ReadPin(channel_relay_fb_node_info->gpio_port_fb, channel_relay_fb_node_info->gpio_pin_fb) == GPIO_PIN_SET) {
			debug("");
		}

		next_power_module_group_id = next_channel_id;
		power_module_group_info_item = channels_info->power_module_group_info + next_power_module_group_id;

		//当前模块组已存在于该枪
		if(list_contain(&power_module_group_info_item->list, &channel_info->power_module_group_list) == 0) {
			find_power_module_group = 1;
		}

		//当前模块组在空闲列表
		if(list_contain(&power_module_group_info_item->list, &pdu_group_info->power_module_group_idle_list) == 0) {
			power_module_item_info_t *power_module_item_info;
			find_power_module_group = 1;
			//set relay channel_info_item_prev---channel_info_item, by id
			//todo
			relay_node_info = get_relay_node_info_by_channel_id(channels_config,
			                  pdu_group_info->pdu_group_id,
			                  channel_info_item_prev->channel_id,
			                  channel_info_item->channel_id);
			OS_ASSERT(relay_node_info != NULL);
			set_bitmap_value(pdu_group_info->relay_map, relay_node_info->relay_id, 1);

			head = &power_module_group_info_item->power_module_item_list;
			list_for_each_entry(power_module_item_info, head, power_module_item_info_t, list) {
				if(power_module_item_info->status.state != POWER_MODULE_ITEM_STATE_IDLE) {
					debug("power module state is not idle:%s!!!", get_power_module_item_state_des(power_module_item_info->status.state));
				}

				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_ACTIVE;
			}
			power_module_group_info_item->channel_info = channel_info;
			list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);
			debug("assign module group_id %d to channel_id %d", power_module_group_info_item->group_id, channel_info->channel_id);

			assigned = list_size(&channel_info->power_module_group_list);

			if(assigned >= channel_info->status.reassign_module_group_number) {
				debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
				return;
			}
		}

		if(find_power_module_group == 0) {
			break;
		}

		channel_info_item_prev = channel_info_item;
	}

	//right search
	channel_info_item_prev = channel_info;
	channel_info_item = channel_info;

	while(channel_info_item != NULL) {
		uint8_t next_channel_id;
		uint8_t next_power_module_group_id;
		power_module_group_info_t *power_module_group_info_item;
		uint8_t find_power_module_group = 0;
		relay_node_info_t *relay_node_info;

		if(channel_info_item->channel_id == (channels_info->channel_number - 1)) {
			next_channel_id = 0;
		} else {
			next_channel_id = channel_info_item->channel_id + 1;
		}

		channel_info_item = channels_info->channel_info + next_channel_id;

		if(channel_info_item->pdu_group_info->pdu_group_id != pdu_group_info->pdu_group_id) {
			continue;
		}

		if(list_contain(&channel_info_item->list, &pdu_group_info->channel_active_list) == 0) {
			break;
		}

		channel_relay_fb_node_info = get_channel_relay_fb_node_info(channels_config, pdu_group_info->pdu_group_id, channel_info_item->channel_id);
		OS_ASSERT(channel_relay_fb_node_info != NULL);

		while(HAL_GPIO_ReadPin(channel_relay_fb_node_info->gpio_port_fb, channel_relay_fb_node_info->gpio_pin_fb) == GPIO_PIN_SET) {
			debug("");
		}

		next_power_module_group_id = next_channel_id;
		power_module_group_info_item = channels_info->power_module_group_info + next_power_module_group_id;

		//当前模块组已存在于该枪
		if(list_contain(&power_module_group_info_item->list, &channel_info->power_module_group_list) == 0) {
			find_power_module_group = 1;
		}

		//当前模块组在空闲列表
		if(list_contain(&power_module_group_info_item->list, &pdu_group_info->power_module_group_idle_list) == 0) {
			power_module_item_info_t *power_module_item_info;
			find_power_module_group = 1;
			//set relay channel_info_item_prev---channel_info_item, by id
			//todo
			relay_node_info = get_relay_node_info_by_channel_id(channels_config,
			                  pdu_group_info->pdu_group_id,
			                  channel_info_item_prev->channel_id,
			                  channel_info_item->channel_id);
			OS_ASSERT(relay_node_info != NULL);
			set_bitmap_value(pdu_group_info->relay_map, relay_node_info->relay_id, 1);

			head = &power_module_group_info_item->power_module_item_list;
			list_for_each_entry(power_module_item_info, head, power_module_item_info_t, list) {
				if(power_module_item_info->status.state != POWER_MODULE_ITEM_STATE_IDLE) {
					debug("power module state is not idle:%s!!!", get_power_module_item_state_des(power_module_item_info->status.state));
				}

				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_ACTIVE;
			}
			power_module_group_info_item->channel_info = channel_info;
			list_move_tail(&power_module_group_info_item->list, &channel_info->power_module_group_list);
			debug("assign module group_id %d to channel_id %d", power_module_group_info_item->group_id, channel_info->channel_id);

			assigned = list_size(&channel_info->power_module_group_list);

			if(assigned >= channel_info->status.reassign_module_group_number) {
				debug("channel %d reassign_module_group_number:%d, assigned:%d", channel_info->channel_id, channel_info->status.reassign_module_group_number, assigned);
				return;
			}
		}

		if(find_power_module_group == 0) {
			break;
		}

		channel_info_item_prev = channel_info_item;
	}
}

static void active_pdu_group_info_power_module_group_assign_priority(pdu_group_info_t *pdu_group_info)
{
	channel_info_t *channel_info;
	struct list_head *head;

	head = &pdu_group_info->channel_active_list;

	list_for_each_entry(channel_info, head, channel_info_t, list) {
		channel_info_assign_power_module_group(channel_info);
	}
}

static void pdu_group_info_assign_power_module_group_priority(pdu_group_info_t *pdu_group_info)
{
	//充电中的枪数
	uint8_t active_channel_count;

	//获取需要充电的枪数
	active_channel_count = list_size(&pdu_group_info->channel_active_list);
	debug("active_channel_count:%d", active_channel_count);

	if(active_channel_count == 0) {//如果没有枪需要充电,不分配
		return;
	}

	active_pdu_group_info_power_module_group_assign_priority(pdu_group_info);
}

static int pdu_group_info_power_module_group_ready_sync(pdu_group_info_t *pdu_group_info)
{
	int ret = 0;
	channel_info_t *channel_info;
	struct list_head *head;
	struct list_head *head1;
	struct list_head *head2;

	head = &pdu_group_info->channel_active_list;

	list_for_each_entry(channel_info, head, channel_info_t, list) {
		power_module_group_info_t *power_module_group_info;
		head1 = &channel_info->power_module_group_list;
		list_for_each_entry(power_module_group_info, head1, power_module_group_info_t, list) {
			power_module_item_info_t *power_module_item_info;
			head2 = &power_module_group_info->power_module_item_list;
			list_for_each_entry(power_module_item_info, head2, power_module_item_info_t, list) {
				if(power_module_item_info->status.state == POWER_MODULE_ITEM_STATE_READY) {
					continue;
				}

				if(power_module_item_info->status.state == POWER_MODULE_ITEM_STATE_ACTIVE) {
					continue;
				}

				ret = -1;
				break;
			}

			if(ret != 0) {
				break;
			}

			break;
		}
	}

	return ret;
}

static int channels_module_assign_ready(pdu_group_info_t *pdu_group_info)
{
	int ret = 0;
	channel_info_t *channel_info;
	struct list_head *head;
	channels_info_t *channels_info = (channels_info_t *)pdu_group_info->channels_info;
	channels_com_info_t *channels_com_info = (channels_com_info_t *)channels_info->channels_com_info;

	head = &pdu_group_info->channel_active_list;

	list_for_each_entry(channel_info, head, channel_info_t, list) {
		if(channel_info->status.module_ready_notify == 1) {
			channel_info->status.module_ready_notify = 0;
			debug("channel_id %d module assign ready", channel_info->channel_id);
			channels_com_channel_module_assign_info(channels_com_info, channel_info->channel_id, MODULE_ASSIGN_INFO_READY);
		}
	}
	return ret;
}

typedef int (*pdu_group_info_free_power_module_group_t)(pdu_group_info_t *pdu_group_info);
typedef void (*pdu_group_info_assign_power_module_group_t)(pdu_group_info_t *pdu_group_info);

typedef struct {
	uint8_t policy;
	pdu_group_info_free_power_module_group_t free;
	pdu_group_info_assign_power_module_group_t assign;
} pdu_group_info_power_module_group_policy_t;

static pdu_group_info_power_module_group_policy_t pdu_group_info_power_module_group_policy_priority = {
	.policy = POWER_MODULE_POLICY_PRIORITY,
	.free = pdu_group_info_free_power_module_group_priority,
	.assign = pdu_group_info_assign_power_module_group_priority,
};

static pdu_group_info_power_module_group_policy_t *pdu_group_info_power_module_group_policy_sz[] = {
	&pdu_group_info_power_module_group_policy_priority,
};

static pdu_group_info_power_module_group_policy_t *get_pdu_group_info_power_module_group_policy(uint8_t policy)
{
	int i;
	pdu_group_info_power_module_group_policy_t *pdu_group_info_power_module_group_policy = &pdu_group_info_power_module_group_policy_priority;

	for(i = 0; i < ARRAY_SIZE(pdu_group_info_power_module_group_policy_sz); i++) {
		pdu_group_info_power_module_group_policy_t *pdu_group_info_power_module_group_policy_item = pdu_group_info_power_module_group_policy_sz[i];

		if(pdu_group_info_power_module_group_policy_item->policy == policy) {
			pdu_group_info_power_module_group_policy = pdu_group_info_power_module_group_policy_item;
			break;
		}
	}

	return pdu_group_info_power_module_group_policy;
}

static void relay_map_action(pdu_group_info_t *pdu_group_info)
{
	int i;
	bitmap_t *relay_map = pdu_group_info->relay_map;
	channels_info_t *channels_info = (channels_info_t *)pdu_group_info->channels_info;
	channels_config_t *channels_config = channels_info->channels_config;

	for(i = 0; i < relay_map->size; i++) {
		relay_node_info_t *relay_node_info = get_relay_node_info_relay_id(channels_config, pdu_group_info->pdu_group_id, i);
		GPIO_PinState state = GPIO_PIN_RESET;

		if(get_bitmap_value(relay_map, i) != 0) {
			state = GPIO_PIN_SET;
		}

		HAL_GPIO_WritePin(relay_node_info->gpio_port, relay_node_info->gpio_pin, state);
	}
}

static int sync_relay_map(pdu_group_info_t *pdu_group_info)
{
	int ret = 0;
	int i;
	bitmap_t *relay_map = pdu_group_info->relay_map;
	channels_info_t *channels_info = (channels_info_t *)pdu_group_info->channels_info;
	channels_config_t *channels_config = channels_info->channels_config;

	for(i = 0; i < relay_map->size; i++) {
		relay_node_info_t *relay_node_info = get_relay_node_info_relay_id(channels_config, pdu_group_info->pdu_group_id, i);
		GPIO_PinState state = HAL_GPIO_ReadPin(relay_node_info->gpio_port_fb, relay_node_info->gpio_pin_fb);
		GPIO_PinState expect_state = GPIO_PIN_RESET;

		if(get_bitmap_value(relay_map, i) != 0) {
			expect_state = GPIO_PIN_SET;
		}

		if(state != expect_state) {
			ret = -1;
			break;
		}
	}

	return ret;
}

//根据通道状态变化，重新分配模块组
static void handle_channels_change_state(pdu_group_info_t *pdu_group_info)
{
	switch(pdu_group_info->channels_change_state) {
		case CHANNELS_CHANGE_STATE_IDLE: {
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_PREPARE_FREE: {//准备释放模块组
			channels_info_t *channels_info = (channels_info_t *)pdu_group_info->channels_info;
			pdu_config_t *pdu_config = &channels_info->channels_settings.pdu_config;
			pdu_group_info_power_module_group_policy_t *policy = get_pdu_group_info_power_module_group_policy(pdu_config->policy);

			debug("pdu_group_id %d get policy %s", pdu_group_info->pdu_group_id, get_power_module_policy_des(pdu_config->policy));

			if(policy->free(pdu_group_info) == 0) {//返回0表示释放操作完成
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_FREE;
				debug("pdu_group_id %d channels_change_state to state %s",
				      pdu_group_info->pdu_group_id,
				      get_channels_change_state_des(pdu_group_info->channels_change_state));
			}
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_FREE: {//释放模块组-停用模块列表为空
			if(list_empty(&pdu_group_info->power_module_group_deactive_list)) {//所有该停用的模块组停用完毕
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG;
				debug("pdu_group_id %d channels_change_state to state %s",
				      pdu_group_info->pdu_group_id,
				      get_channels_change_state_des(pdu_group_info->channels_change_state));
			}
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG: {//释放模块组后下发配置
			relay_map_action(pdu_group_info);
			pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG_SYNC;
			debug("pdu_group_id %d channels_change_state to state %s",
			      pdu_group_info->pdu_group_id,
			      get_channels_change_state_des(pdu_group_info->channels_change_state));
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_FREE_CONFIG_SYNC: {//释放模块组后配置同步
			if(sync_relay_map(pdu_group_info) == 0) {
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_ASSIGN;
				debug("pdu_group_id %d channels_change_state to state %s",
				      pdu_group_info->pdu_group_id,
				      get_channels_change_state_des(pdu_group_info->channels_change_state));
			}
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_ASSIGN: {//分配模块组
			channels_info_t *channels_info = (channels_info_t *)pdu_group_info->channels_info;
			pdu_config_t *pdu_config = &channels_info->channels_settings.pdu_config;
			pdu_group_info_power_module_group_policy_t *policy = get_pdu_group_info_power_module_group_policy(pdu_config->policy);

			policy->assign(pdu_group_info);
			pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_READY_SYNC;
			debug("pdu_group_id %d channels_change_state to state %s",
			      pdu_group_info->pdu_group_id,
			      get_channels_change_state_des(pdu_group_info->channels_change_state));
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_READY_SYNC: {//根据情况等待模块组预充
			if(pdu_group_info_power_module_group_ready_sync(pdu_group_info) == 0) {
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG;
				debug("pdu_group_id %d channels_change_state to state %s",
				      pdu_group_info->pdu_group_id,
				      get_channels_change_state_des(pdu_group_info->channels_change_state));
			}
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG: {//分配模块组后下发配置
			relay_map_action(pdu_group_info);
			pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG_SYNC;
			debug("pdu_group_id %d channels_change_state to state %s",
			      pdu_group_info->pdu_group_id,
			      get_channels_change_state_des(pdu_group_info->channels_change_state));
		}
		break;

		case CHANNELS_CHANGE_STATE_MODULE_ASSIGN_CONFIG_SYNC: {//分配模块组后同步配置
			if(sync_relay_map(pdu_group_info) == 0) {
				channels_module_assign_ready(pdu_group_info);
				pdu_group_info->channels_change_state = CHANNELS_CHANGE_STATE_IDLE;
				debug("pdu_group_id %d channels_change_state to state %s",
				      pdu_group_info->pdu_group_id,
				      get_channels_change_state_des(pdu_group_info->channels_change_state));
			}
		}
		break;

		default: {
		}
		break;
	}
}

static void handle_pdu_group_deactive_list(pdu_group_info_t *pdu_group_info)
{
	struct list_head *pos;
	struct list_head *n;
	struct list_head *head;

	head = &pdu_group_info->power_module_group_deactive_list;

	list_for_each_safe(pos, n, head) {
		power_module_group_info_t *power_module_group_info = list_entry(pos, power_module_group_info_t, list);
		struct list_head *head1 = &power_module_group_info->power_module_item_list;
		power_module_item_info_t *power_module_item_info;
		uint8_t power_module_group_idle = 1;
		uint8_t power_module_group_disable = 0;

		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			if(power_module_item_info->status.state != POWER_MODULE_ITEM_STATE_IDLE) {
				power_module_group_idle = 0;
				break;
			}
		}

		if(power_module_group_idle == 1) {
			power_module_group_info->channel_info = NULL;
			debug("set power module group_id %d idle!", power_module_group_info->group_id);
			list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_idle_list);
			continue;
		}

		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			if(power_module_item_info->status.state == POWER_MODULE_ITEM_STATE_DISABLE) {
				power_module_group_disable = 1;
				break;
			}
		}

		if(power_module_group_disable == 1) {
			power_module_group_info->channel_info = NULL;
			debug("set power module group_id %d disable", power_module_group_info->group_id);
			list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_disable_list);
			continue;
		}
	}
}

static void handle_pdu_group_disable_list(pdu_group_info_t *pdu_group_info)
{
	struct list_head *pos;
	struct list_head *n;
	struct list_head *head;

	head = &pdu_group_info->power_module_group_disable_list;

	list_for_each_safe(pos, n, head) {
		power_module_group_info_t *power_module_group_info = list_entry(pos, power_module_group_info_t, list);
		power_module_item_info_t *power_module_item_info;
		struct list_head *head1 = &power_module_group_info->power_module_item_list;
		uint8_t power_module_group_disable = 0;

		list_for_each_entry(power_module_item_info, head1, power_module_item_info_t, list) {
			if(power_module_item_info->status.state != POWER_MODULE_ITEM_STATE_IDLE) {
				power_module_group_disable = 1;
			}
		}

		if(power_module_group_disable == 0) {
			debug("reenable power module group_id %d", power_module_group_info->group_id);
			list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_idle_list);
		}
	}
}

char *get_power_module_item_state_des(power_module_item_state_t state)
{
	char *des = "unknow";

	switch(state) {
			add_des_case(POWER_MODULE_ITEM_STATE_IDLE);
			add_des_case(POWER_MODULE_ITEM_STATE_PREPARE_ACTIVE);
			add_des_case(POWER_MODULE_ITEM_STATE_READY);
			add_des_case(POWER_MODULE_ITEM_STATE_ACTIVE);
			add_des_case(POWER_MODULE_ITEM_STATE_ACTIVE_RELAY_CHECK);
			add_des_case(POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE);
			add_des_case(POWER_MODULE_ITEM_STATE_DEACTIVE);
			add_des_case(POWER_MODULE_ITEM_STATE_DISABLE);

		default: {
		}
		break;
	}

	return des;
}

static void update_poewr_module_item_info_status(power_module_item_info_t *power_module_item_info)
{
	power_modules_info_t *power_modules_info = (power_modules_info_t *)power_module_item_info->power_modules_info;
	power_module_info_t *power_module_info = power_modules_info->power_module_info + power_module_item_info->module_id;
	power_module_item_status_t *status = &power_module_item_info->status;
	u_power_module_status_t u_power_module_status;
	uint32_t ticks = osKernelSysTick();

	pdu_group_info_t *pdu_group_info;
	channels_info_t *channels_info;
	channels_com_info_t *channels_com_info;
	module_status_t *module_status;
	uint8_t connect_timeout;
	power_module_group_info_t *power_module_group_info = (power_module_group_info_t *)power_module_item_info->power_module_group_info;
	channel_info_t *channel_info = (channel_info_t *)power_module_group_info->channel_info;
	uint8_t fault_changed = 0;
	uint8_t over_voltage = 0;
	uint8_t low_voltage = 0;

	if(ticks_duration(ticks, power_module_item_info->query_stamp) < (1 * 1000)) {
		return;
	}

	power_module_item_info->query_stamp = ticks;

	power_modules_query_status(power_modules_info, power_module_item_info->module_id);
	power_modules_query_a_line_input_voltage(power_modules_info, power_module_item_info->module_id);
	power_modules_query_b_line_input_voltage(power_modules_info, power_module_item_info->module_id);
	power_modules_query_c_line_input_voltage(power_modules_info, power_module_item_info->module_id);

	status->module_output_voltage = power_module_info->output_voltage;

	status->module_output_current = power_module_info->output_current;

	u_power_module_status.s = power_module_info->power_module_status;

	pdu_group_info = power_module_group_info->pdu_group_info;
	channels_info = (channels_info_t *)pdu_group_info->channels_info;

	if(get_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_FAULT) != u_power_module_status.s.fault) {
		set_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_FAULT, u_power_module_status.s.fault);
		fault_changed = 1;
	}

	if(ticks_duration(ticks, get_power_module_connect_stamp(power_module_info)) >= (10 * 1000)) {
		debug("ticks:%d, power module_id %d stamps:%d", ticks, power_module_item_info->module_id, get_power_module_connect_stamp(power_module_info));
		connect_timeout = 1;
	} else {
		connect_timeout = 0;
	}

	if(get_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_CONNECT_TIMEOUT) != connect_timeout) {
		set_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_CONNECT_TIMEOUT, connect_timeout);
		fault_changed = 1;
	}

	if((power_module_info->input_aline_voltage < 1900) || (power_module_info->input_bline_voltage < 1900) || (power_module_info->input_cline_voltage < 1900)) {
		low_voltage = 1;
	} else if((power_module_info->input_aline_voltage > 2500) || (power_module_info->input_bline_voltage > 2500) || (power_module_info->input_cline_voltage > 2500)) {
		over_voltage = 1;

		if((power_module_info->input_aline_voltage > 2750) || (power_module_info->input_bline_voltage > 2750) || (power_module_info->input_cline_voltage > 2750)) {
			power_module_info->over_voltage_disable = 1;
		}
	}

	if(get_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_INPUT_LOW_VOLTAGE) != low_voltage) {
		set_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_INPUT_LOW_VOLTAGE, low_voltage);
		fault_changed = 1;
	}

	if(get_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_INPUT_OVER_VOLTAGE) != over_voltage) {
		set_fault(power_module_item_info->faults, POWER_MODULE_ITEM_FAULT_INPUT_OVER_VOLTAGE, over_voltage);
		fault_changed = 1;
	}

	if(get_first_fault(power_module_item_info->faults) != -1) {
		uint8_t power_module_group_disable = 1;

		if(fault_changed == 0) {
			power_module_group_disable = 0;
		}

		if(list_contain(&power_module_group_info->list, &pdu_group_info->power_module_group_deactive_list) == 0) {
			power_module_group_disable = 0;
		} else if(list_contain(&power_module_group_info->list, &pdu_group_info->power_module_group_disable_list) == 0) {
			power_module_group_disable = 0;
		}

		if(power_module_group_disable == 1) {
			power_module_item_info_t *power_module_item_info_deactive;
			struct list_head *head = &power_module_group_info->power_module_item_list;

			if(channel_info != NULL) {
				debug("channel_id %d stop by module_id %d fault",
				      channel_info->channel_id, power_module_item_info->module_id);
				channel_info->channel_request_state = CHANNEL_REQUEST_STATE_STOP;
			}

			list_for_each_entry(power_module_item_info_deactive, head, power_module_item_info_t, list) {
				power_module_item_info_deactive->status.state = POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE;

				debug("module_id %d to state %s",
				      power_module_item_info_deactive->module_id,
				      get_power_module_item_state_des(power_module_item_info_deactive->status.state));
			}

			list_move_tail(&power_module_group_info->list, &pdu_group_info->power_module_group_deactive_list);
		}
	}

	status->module_status = u_power_module_status.v;

	status->connect_state = get_power_module_connect_state(power_module_info);

	channels_com_info = (channels_com_info_t *)channels_info->channels_com_info;
	module_status = (module_status_t *)channels_com_info->module_status;
	module_status += power_module_item_info->module_id;

	//debug("module_id:%d, output_voltage:%d, output_current:%d",
	//      power_module_item_info->module_id,
	//      power_module_info->output_voltage,
	//      power_module_info->output_current);

	module_status->setting_voltage_l = get_u8_l_from_u16(power_module_info->setting_voltage / 100);
	module_status->setting_voltage_h = get_u8_h_from_u16(power_module_info->setting_voltage / 100);
	module_status->setting_current_l = get_u8_l_from_u16(power_module_info->setting_current / 100);
	module_status->setting_current_h = get_u8_h_from_u16(power_module_info->setting_current / 100);
	module_status->output_voltage_l = get_u8_l_from_u16(power_module_info->output_voltage);
	module_status->output_voltage_h = get_u8_h_from_u16(power_module_info->output_voltage);
	module_status->output_current_l = get_u8_l_from_u16(power_module_info->output_current);
	module_status->output_current_h = get_u8_h_from_u16(power_module_info->output_current);
	module_status->module_state_l = get_u8_l_from_u16(status->module_status);
	module_status->module_state_h = get_u8_h_from_u16(status->module_status);
	module_status->connect_state_l = get_u8_l_from_u16(status->connect_state);
	module_status->connect_state_h = get_u8_h_from_u16(status->connect_state);
}

static void power_module_item_set_out_voltage_current(power_module_item_info_t *power_module_item_info, uint16_t voltage, uint16_t current)
{
	uint32_t ticks = osKernelSysTick();
	power_modules_info_t *power_modules_info = (power_modules_info_t *)power_module_item_info->power_modules_info;
	power_module_group_info_t *power_module_group_info = (power_module_group_info_t *)power_module_item_info->power_module_group_info;
	channel_info_t *channel_info = (channel_info_t *)power_module_group_info->channel_info;
	u_power_module_status_t u_power_module_status;
	uint8_t power_off = 0;

	if(channel_info != NULL) {
		if(get_first_fault(channel_info->faults) != -1) {
			voltage = 0;
			current = 0;
		}
	}

	if(get_first_fault(power_module_item_info->faults) != -1) {
		voltage = 0;
		current = 0;
	}

	if((power_module_item_info->status.setting_output_voltage == voltage) &&
	   (power_module_item_info->status.setting_output_current == current)) {
		if(ticks_duration(ticks, power_module_item_info->setting_stamp) >= (1 * 1000)) {
		} else {
			return;
		}
	} else {
		power_module_item_info->status.setting_output_voltage = voltage;
		power_module_item_info->status.setting_output_current = current;

		power_module_item_info->status.smooth_setting_output_current = power_module_item_info->status.setting_output_current;
	}

	power_module_item_info->setting_stamp = ticks;

	if(current > power_module_item_info->status.smooth_setting_output_current) {//需求值比设置值大
		if(current - power_module_item_info->status.smooth_setting_output_current > 30 * 10) {//需求值比设置值大30a
			power_module_item_info->status.smooth_setting_output_current += 30 * 10;
			current = power_module_item_info->status.smooth_setting_output_current;
		} else {
			power_module_item_info->status.smooth_setting_output_current = current;
		}
	} else if(current < power_module_item_info->status.smooth_setting_output_current) {//只平滑增大电流，不平滑减小电流
		power_module_item_info->status.smooth_setting_output_current = current;
	}

	u_power_module_status.v = power_module_item_info->status.module_status;

	if(voltage == 0) {
		power_off = 1;
	}

	if(current == 0) {
		power_off = 1;
	}

	//debug("set module_id:%d, poweroff:%d, voltage:%d, current:%d",
	//      power_module_item_info->module_id,
	//      power_off,
	//      voltage,
	//      current);

	if(u_power_module_status.s.poweroff != power_off) {
		if(power_off == 0) {
			if(channel_info != NULL) {
				power_modules_set_channel_id(power_modules_info, power_module_item_info->module_id, channel_info->channel_id);
			}
		}

		power_modules_set_poweroff(power_modules_info, power_module_item_info->module_id, power_off);
	}

	if(power_off == 0) {
		power_modules_set_out_voltage_current(power_modules_info, power_module_item_info->module_id, voltage * 1000 / 10, current * 1000 / 10);
	} else {
		power_modules_set_out_voltage_current(power_modules_info, power_module_item_info->module_id, 0, 0);
	}
}

static void handle_power_module_item_info_state(power_module_item_info_t *power_module_item_info)
{
	switch(power_module_item_info->status.state) {
		case POWER_MODULE_ITEM_STATE_IDLE: {
			power_module_item_set_out_voltage_current(power_module_item_info, 0, 0);
		}
		break;

		case POWER_MODULE_ITEM_STATE_PREPARE_ACTIVE: {//准备启动模块组，根据情况预充
			//uint32_t ticks = osKernelSysTick();
			power_module_group_info_t *power_module_group_info = (power_module_group_info_t *)power_module_item_info->power_module_group_info;
			channel_info_t *channel_info = (channel_info_t *)power_module_group_info->channel_info;

			if(channel_info->status.require_work_state == CHANNEL_WORK_STATE_CHARGE) {
				channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
				channels_settings_t *channels_settings = &channels_info->channels_settings;
				uint16_t voltage = channel_info->status.charge_output_voltage;
				uint16_t current = 1;

				module_voltage_current_correction(channels_settings, &voltage, &current);
				module_power_limit_correction(channels_settings, &channel_info->status.charge_output_voltage, &current);
				power_module_item_set_out_voltage_current(power_module_item_info, voltage, current);

				//debug("module_id %d setting voltage:%d, current:%d",
				//      power_module_item_info->module_id,
				//      voltage,
				//      current);

				if(abs(power_module_item_info->status.setting_output_voltage - power_module_item_info->status.module_output_voltage) <= 200) {
					//if(ticks_duration(ticks, power_module_item_info->test_stamp) >= (power_module_item_info->module_id * 1000 / 4)) {
					power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_READY;
					debug("module_id %d to state %s",
					      power_module_item_info->module_id,
					      get_power_module_item_state_des(power_module_item_info->status.state));
					//}
				} else {
					//debug("module_id %d setting_output_voltage:%d, module_output_voltage:%d",
					//      power_module_item_info->module_id,
					//      power_module_item_info->status.setting_output_voltage,
					//      power_module_item_info->status.module_output_voltage);
				}
			} else {
				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_READY;
				debug("module_id %d to state %s",
				      power_module_item_info->module_id,
				      get_power_module_item_state_des(power_module_item_info->status.state));
			}

		}
		break;

		case POWER_MODULE_ITEM_STATE_READY: {//达到接通条件
			power_module_group_info_t *power_module_group_info = (power_module_group_info_t *)power_module_item_info->power_module_group_info;
			pdu_group_info_t *pdu_group_info = power_module_group_info->pdu_group_info;

			if(pdu_group_info->channels_change_state == CHANNELS_CHANGE_STATE_IDLE) {//等待配置同步完成,开关动作完成
				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_ACTIVE;
				debug("module_id %d to state %s",
				      power_module_item_info->module_id,
				      get_power_module_item_state_des(power_module_item_info->status.state));
			}
		}
		break;

		case POWER_MODULE_ITEM_STATE_ACTIVE: {
			power_module_item_info->status.setting_output_voltage = power_module_item_info->status.require_output_voltage;
			power_module_item_info->status.setting_output_current = power_module_item_info->status.require_output_current;

			power_module_item_set_out_voltage_current(power_module_item_info,
			        power_module_item_info->status.require_output_voltage,
			        power_module_item_info->status.require_output_current);
			//debug("module_id %d require_output_voltage:%d, require_output_current:%d",
			//      power_module_item_info->module_id,
			//      power_module_item_info->status.require_output_voltage,
			//      power_module_item_info->status.require_output_current);
		}
		break;

		case POWER_MODULE_ITEM_STATE_ACTIVE_RELAY_CHECK: {
			power_module_item_set_out_voltage_current(power_module_item_info, 0, 0);
		}
		break;

		case POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE: {//准备停用模块组，进行停机操作
			u_power_module_status_t u_power_module_status;
			uint8_t module_disable = 0;

			power_module_item_set_out_voltage_current(power_module_item_info, 0, 0);

			u_power_module_status.v = power_module_item_info->status.module_status;

			if(get_first_fault(power_module_item_info->faults) != -1) {
				module_disable = 1;
			}

			if(module_disable == 1) {
				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_DISABLE;
				debug("module_id %d to state %s",
				      power_module_item_info->module_id,
				      get_power_module_item_state_des(power_module_item_info->status.state));
			} else {
				if(u_power_module_status.s.poweroff == 1) {
					power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_DEACTIVE;
					debug("module_id %d to state %s",
					      power_module_item_info->module_id,
					      get_power_module_item_state_des(power_module_item_info->status.state));
				}
			}
		}
		break;

		case POWER_MODULE_ITEM_STATE_DEACTIVE: {//停机完成
			power_module_item_info->status.setting_output_voltage = 0;
			power_module_item_info->status.setting_output_current = 0;

			power_module_item_set_out_voltage_current(power_module_item_info, 0, 0);

			power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_IDLE;
			debug("module_id %d to state %s",
			      power_module_item_info->module_id,
			      get_power_module_item_state_des(power_module_item_info->status.state));
		}
		break;

		case POWER_MODULE_ITEM_STATE_DISABLE: {//禁用模块组
			u_power_module_status_t u_power_module_status;

			power_module_item_info->status.setting_output_voltage = 0;
			power_module_item_info->status.setting_output_current = 0;

			power_module_item_set_out_voltage_current(power_module_item_info, 0, 0);

			if(get_first_fault(power_module_item_info->faults) == -1) {
				u_power_module_status.v = power_module_item_info->status.module_status;

				if(u_power_module_status.s.poweroff == 1) {
					power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_IDLE;
					debug("module_id %d to state %s",
					      power_module_item_info->module_id,
					      get_power_module_item_state_des(power_module_item_info->status.state));
				}
			}
		}
		break;

		default: {
		}
		break;
	}
}

static void handle_power_module_items_info_state(channels_info_t *channels_info)
{
	int i;

	for(i = 0; i < channels_info->power_module_item_number; i++) {
		power_module_item_info_t *power_module_item_info = &channels_info->power_module_item_info[i];

		update_poewr_module_item_info_status(power_module_item_info);
		handle_power_module_item_info_state(power_module_item_info);
	}
}

static uint8_t is_power_module_item_info_idle(power_module_item_info_t *power_module_item_info)
{
	uint8_t idle = -1;

	switch(power_module_item_info->status.state) {
		case POWER_MODULE_ITEM_STATE_IDLE:
		case POWER_MODULE_ITEM_STATE_DISABLE: {
			idle = 0;
		}
		break;

		default: {
		}
		break;
	}

	return idle;
}

static void handle_fan_state(channels_info_t *channels_info)
{
	int i;
	GPIO_PinState state = GPIO_PIN_RESET;
	channels_config_t *channels_config = channels_info->channels_config;

	for(i = 0; i < channels_info->power_module_item_number; i++) {
		power_module_item_info_t *power_module_item_info = &channels_info->power_module_item_info[i];

		if(is_power_module_item_info_idle(power_module_item_info) != 0) {
			state = GPIO_PIN_SET;
			break;
		}
	}

	HAL_GPIO_WritePin(channels_config->gpio_port_fan, channels_config->gpio_pin_fan, state);
}

static uint8_t update_power_module_policy_request = 0;
static uint8_t power_module_policy_value = POWER_MODULE_POLICY_PRIORITY;

void set_power_module_policy_request(power_module_policy_t policy)
{
	update_power_module_policy_request = 1;
	power_module_policy_value = policy;
}

static int is_channels_idle(channels_info_t *channels_info)
{
	int ret = 0;
	int i;
	channel_info_t *channel_info;

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info = channels_info->channel_info + i;

		if(channel_info->status.state != CHANNEL_STATE_IDLE) {
			ret = -1;
			break;
		}
	}

	return ret;
}

static void handle_power_module_policy_request(channels_info_t *channels_info)
{
	if(update_power_module_policy_request == 1) {
		if(is_channels_idle(channels_info) == 0) {
			pdu_config_t *pdu_config = &channels_info->channels_settings.pdu_config;

			update_power_module_policy_request = 0;
			pdu_config->policy = power_module_policy_value;

			debug("set policy %s", get_power_module_policy_des(pdu_config->policy));
		}
	}
}

static void handle_power_module_type_config(channels_info_t *channels_info)
{
	power_modules_info_t *power_modules_info = (power_modules_info_t *)channels_info->power_modules_info;
	channels_settings_t *channels_settings = &channels_info->channels_settings;

	if(channels_info->channels_settings.power_module_type != power_modules_info->power_module_type) {
		power_modules_set_type(power_modules_info, channels_settings->power_module_type);
	}
}

static void channels_debug(channels_info_t *channels_info)
{
	int i;
	power_module_group_info_t *power_module_group_info;
	power_module_item_info_t *power_module_item_info;
	channel_info_t *channel_info;
	pdu_group_info_t *pdu_group_info;
	struct list_head *head;
	struct list_head *head1;

	_printf("all module items:\n");

	for(i = 0; i < channels_info->power_module_item_number; i++) {
		power_module_item_info = channels_info->power_module_item_info + i;
		print_power_module_item_info(1, power_module_item_info);
	}

	_printf("\n");

	_printf("all module groups:\n");

	for(i = 0; i < channels_info->power_module_group_number; i++) {
		power_module_group_info = channels_info->power_module_group_info + i;
		print_power_module_group_info(1, power_module_group_info);
	}

	_printf("\n");

	for(i = 0; i < channels_info->pdu_group_number; i++) {
		pdu_group_info = channels_info->pdu_group_info + i;

		_printf("pdu group %d module groups:\n", pdu_group_info->pdu_group_id);

		_printf("\tall idle module groups:\n");
		head = &pdu_group_info->power_module_group_idle_list;
		list_for_each_entry(power_module_group_info, head, power_module_group_info_t, list) {
			print_power_module_group_info(2, power_module_group_info);
		}
		_printf("\n");

		_printf("\tall deactive module groups:\n");
		head = &pdu_group_info->power_module_group_deactive_list;
		list_for_each_entry(power_module_group_info, head, power_module_group_info_t, list) {
			print_power_module_group_info(2, power_module_group_info);
		}
		_printf("\n");

		_printf("\tall disable module groups:\n");
		head = &pdu_group_info->power_module_group_disable_list;
		list_for_each_entry(power_module_group_info, head, power_module_group_info_t, list) {
			print_power_module_group_info(2, power_module_group_info);
		}
		_printf("\n");
	}

	_printf("all channels:\n");

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info = channels_info->channel_info + i;

		print_channel_info(1, channel_info);

		head = &channel_info->power_module_group_list;
		list_for_each_entry(power_module_group_info, head, power_module_group_info_t, list) {
			print_power_module_group_info(2, power_module_group_info);
		}
	}

	_printf("\n");

	for(i = 0; i < channels_info->pdu_group_number; i++) {
		pdu_group_info = channels_info->pdu_group_info + i;

		_printf("pdu group %d channels:\n", pdu_group_info->pdu_group_id);

		_printf("\tidle channels:\n");
		head = &pdu_group_info->channel_idle_list;
		list_for_each_entry(channel_info, head, channel_info_t, list) {
			print_channel_info(2, channel_info);

			head1 = &channel_info->power_module_group_list;
			list_for_each_entry(power_module_group_info, head1, power_module_group_info_t, list) {
				print_power_module_group_info(2, power_module_group_info);
			}
		}
		_printf("\n");

		_printf("\tactive channels:\n");
		head = &pdu_group_info->channel_active_list;
		list_for_each_entry(channel_info, head, channel_info_t, list) {
			print_channel_info(2, channel_info);

			head1 = &channel_info->power_module_group_list;
			list_for_each_entry(power_module_group_info, head1, power_module_group_info_t, list) {
				print_power_module_group_info(3, power_module_group_info);
			}
		}
		_printf("\n");

		_printf("\tdeactive channels:\n");
		head = &pdu_group_info->channel_deactive_list;
		list_for_each_entry(channel_info, head, channel_info_t, list) {
			print_channel_info(2, channel_info);

			head1 = &channel_info->power_module_group_list;
			list_for_each_entry(power_module_group_info, head1, power_module_group_info_t, list) {
				print_power_module_group_info(3, power_module_group_info);
			}
		}
		_printf("\n");

		_printf("\tdisable channels:\n");
		head = &pdu_group_info->channel_disable_list;
		list_for_each_entry(channel_info, head, channel_info_t, list) {
			print_channel_info(2, channel_info);

			head1 = &channel_info->power_module_group_list;
			list_for_each_entry(power_module_group_info, head1, power_module_group_info_t, list) {
				print_power_module_group_info(3, power_module_group_info);
			}
		}
		_printf("\n");
	}
}

static uint8_t dump_channels_stats = 0;

void start_dump_channels_stats(void)
{
	dump_channels_stats = 1;
}

static void do_dump_channels_stats(channels_info_t *channels_info)
{
	if(dump_channels_stats == 0) {
		return;
	}

	dump_channels_stats = 0;

	channels_debug(channels_info);
}

static void handle_channels_fault(channels_info_t *channels_info)
{
	int i;
	uint8_t fault;

	fault = 0;

	for(i = 0; i < channels_info->power_module_item_number; i++) {
		power_module_item_info_t *power_module_item_info = channels_info->power_module_item_info + i;

		if(power_module_item_info->status.state == POWER_MODULE_ITEM_STATE_DISABLE) {
			continue;
		}

		if(get_first_fault(power_module_item_info->faults) != -1) {
			fault = 1;
			break;
		}
	}

	set_fault(channels_info->faults, CHANNELS_FAULT_POWER_MODULE, fault);

	fault = 0;

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info_t *channel_info = channels_info->channel_info + i;

		if(get_first_fault(channel_info->faults) != -1) {
			fault = 1;
			break;
		}
	}

	set_fault(channels_info->faults, CHANNELS_FAULT_CHANNEL, fault);
}

static void channels_process_event(channels_info_t *channels_info)
{
	channels_event_t *channels_event = NULL;
	int ret = -1;

	if(channels_info == NULL) {
		return;
	}

	ret = event_pool_wait_event(channels_info->event_pool, CHANNEL_TASK_PERIODIC);

	if(ret != 0) {
		return;
	}

	channels_event = event_pool_get_event(channels_info->event_pool);

	if(channels_event == NULL) {
		return;
	}

	do_callback_chain(channels_info->common_event_chain, channels_event);

	if(channels_event->event != NULL) {
		os_free(channels_event->event);
	}

	os_free(channels_event);
}

static void handle_gpio_signal(channels_info_t *channels_info)
{
	uint8_t force_stop = 0;
	uint8_t door = 0;
	channels_config_t *channels_config = channels_info->channels_config;

	if(HAL_GPIO_ReadPin(channels_config->gpio_port_force_stop, channels_config->gpio_pin_force_stop) == GPIO_PIN_SET) {
		force_stop = 1;
	}

	if(HAL_GPIO_ReadPin(channels_config->gpio_port_door, channels_config->gpio_pin_door) == GPIO_PIN_SET) {
		door = 1;
	}

	if(get_fault(channels_info->faults, CHANNELS_FAULT_FORCE_STOP) != force_stop) {
		set_fault(channels_info->faults, CHANNELS_FAULT_FORCE_STOP, force_stop);
	}

	if(get_fault(channels_info->faults, CHANNELS_FAULT_DOOR) != door) {
		set_fault(channels_info->faults, CHANNELS_FAULT_DOOR, door);
	}
}

static void handle_common_periodic(void *fn_ctx, void *chain_ctx)
{
	channels_info_t *channels_info = (channels_info_t *)fn_ctx;
	int i;

	for(i = 0; i < channels_info->pdu_group_number; i++) {
		pdu_group_info_t *pdu_group_info = channels_info->pdu_group_info + i;

		handle_channels_change_state(pdu_group_info);
		handle_pdu_group_deactive_list(pdu_group_info);
		handle_pdu_group_disable_list(pdu_group_info);
	}

	handle_power_module_items_info_state(channels_info);

	handle_fan_state(channels_info);

	handle_power_module_policy_request(channels_info);

	handle_power_module_type_config(channels_info);

	do_dump_channels_stats(channels_info);

	handle_gpio_signal(channels_info);

	handle_channels_fault(channels_info);
}

static void channels_periodic(channels_info_t *channels_info)
{
	uint32_t ticks = osKernelSysTick();

	if(ticks_duration(ticks, channels_info->periodic_stamp) >= CHANNEL_TASK_PERIODIC) {
		channels_info->periodic_stamp = ticks;

		do_callback_chain(channels_info->common_periodic_chain, channels_info);
	}
}

void task_channels(void const *argument)
{
	channels_info_t *channels_info = (channels_info_t *)argument;

	if(channels_info == NULL) {
		debug("");
		app_panic();
	}

	while(1) {
		if(channels_info->configed == 0) {
			osDelay(10);
			continue;
		}

		channels_process_event(channels_info);

		//处理周期性事件
		channels_periodic(channels_info);
	}
}

int send_channels_event(channels_info_t *channels_info, channels_event_t *channels_event, uint32_t timeout)
{
	return event_pool_put_event(channels_info->event_pool, channels_event, timeout);
}

void start_stop_channel(channels_info_t *channels_info, uint8_t channel_id, channel_event_type_t channel_event_type)
{
	uint32_t timeout = 0;
	channels_event_t *channels_event = (channels_event_t *)os_calloc(1, sizeof(channels_event_t));
	channel_event_t *channel_event = (channel_event_t *)os_calloc(1, sizeof(channel_event_t));
	int ret;

	OS_ASSERT(channels_event != NULL);
	OS_ASSERT(channel_event != NULL);

	channels_event->type = CHANNELS_EVENT_CHANNEL_EVENT;
	channels_event->event = channel_event;

	channel_event->channel_id = channel_id;
	channel_event->type = channel_event_type;

	ret = send_channels_event(channels_info, channels_event, timeout);

	if(ret != 0) {
		debug("");
		os_free(channels_event);
	}
}

__weak void channels_modbus_data_action(void *fn_ctx, void *chain_ctx)
{
	//channels_info_t *channels_info = (channels_info_t *)fn_ctx;
	modbus_data_ctx_t *modbus_data_ctx = (modbus_data_ctx_t *)chain_ctx;

	debug("op:%s, addr:%d, value:%d",
	      (modbus_data_ctx->action == MODBUS_DATA_ACTION_GET) ? "get" :
	      (modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) ? "set" :
	      "unknow",
	      modbus_data_ctx->addr,
	      modbus_data_ctx->value);
}

__weak void load_channels_display_cache(channels_info_t *channels_info)
{
}

__weak void sync_channels_display_cache(channels_info_t *channels_info)
{
}

static void channels_info_channels_settings_invalid(void *fn_ctx, void *chain_ctx)
{
	channels_info_t *channels_info = (channels_info_t *)fn_ctx;
	modbus_data_ctx_t *modbus_data_ctx = (modbus_data_ctx_t *)chain_ctx;

	if(modbus_data_ctx->influence < (void *)&channels_info->channels_settings) {
		return;
	}

	if(modbus_data_ctx->influence >= (void *)(&channels_info->channels_settings + 1)) {
		return;
	}

	debug("[%p, %p, %p]", &channels_info->channels_settings, modbus_data_ctx->influence, &channels_info->channels_settings + 1);

	channels_info->channels_settings_invalid = 1;
}

static void channels_info_channels_settings_changed(void *fn_ctx, void *chain_ctx)
{
	channels_info_t *channels_info = (channels_info_t *)fn_ctx;

	sync_channels_display_cache(channels_info);

	if(channels_info->channels_settings_invalid != 0) {
		channels_info->channels_settings_invalid = 0;
		channels_info_save_config(channels_info);
	}
}

static int channels_set_channels_config(channels_info_t *channels_info, channels_config_t *channels_config)
{
	int ret = -1;
	int i;
	uint32_t ticks = osKernelSysTick();
	uint8_t power_module_group_offset = 0;
	uint8_t power_module_item_offset = 0;
	uint8_t channel_offset = 0;
	channels_settings_t *channels_settings = &channels_info->channels_settings;
	pdu_config_t *pdu_config = &channels_settings->pdu_config;
	display_info_t *display_info;
	app_info_t *app_info = get_app_info();

	OS_ASSERT(app_info->storage_info != NULL);
	channels_info->storage_info = app_info->storage_info;

	osThreadDef(task_channels, task_channels, osPriorityNormal, 0, 128 * 2 * 2);
	//osThreadDef(task_channel_event, task_channel_event, osPriorityNormal, 0, 128 * 2);

	init_channels_settings(channels_info);

	channels_info->pdu_group_number = pdu_config->pdu_group_number;
	OS_ASSERT(channels_info->pdu_group_number != 0);
	OS_ASSERT(channels_info->pdu_group_number <= PDU_GROUP_NUMBER_MAX);
	debug("channels_info->pdu_group_number:%d", channels_info->pdu_group_number);

	channels_info->power_module_group_number = get_power_module_group_number(pdu_config);
	OS_ASSERT(channels_info->power_module_group_number != 0);
	debug("channels_info->power_module_group_number:%d", channels_info->power_module_group_number);

	channels_info->power_module_item_number = get_power_module_item_number(pdu_config);
	OS_ASSERT(channels_info->power_module_item_number != 0);
	debug("channels_info->power_module_item_number:%d", channels_info->power_module_item_number);

	channels_info->channel_number = get_channel_number(pdu_config);
	OS_ASSERT(channels_info->channel_number != 0);
	debug("channels_info->channel_number:%d", channels_info->channel_number);

	//分配pdu组信息
	channels_info->pdu_group_info = (pdu_group_info_t *)os_calloc(channels_info->pdu_group_number, sizeof(pdu_group_info_t));
	OS_ASSERT(channels_info->pdu_group_info != NULL);

	//分配模块组信息
	channels_info->power_module_group_info = (power_module_group_info_t *)os_calloc(channels_info->power_module_group_number, sizeof(power_module_group_info_t));
	OS_ASSERT(channels_info->power_module_group_info != NULL);

	//分配模块信息
	channels_info->power_module_item_info = (power_module_item_info_t *)os_calloc(channels_info->power_module_item_number, sizeof(power_module_item_info_t));
	OS_ASSERT(channels_info->power_module_item_info != NULL);

	for(i = 0; i < channels_info->power_module_item_number; i++) {
		power_module_item_info_t *power_module_item_info = channels_info->power_module_item_info + i;
		power_module_item_info->faults = alloc_bitmap(POWER_MODULE_ITEM_FAULT_SIZE);
		OS_ASSERT(power_module_item_info->faults != NULL);
	}

	//分配通道信息
	channels_info->channel_info = (channel_info_t *)os_calloc(channels_info->channel_number, sizeof(channel_info_t));
	OS_ASSERT(channels_info->channel_info != NULL);

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info_t *channel_info_item = channels_info->channel_info + i;
		channel_info_item->faults = alloc_bitmap(CHANNEL_FAULT_SIZE);
		OS_ASSERT(channel_info_item->faults != NULL);
	}

	channels_config->power_module_config.channels_power_module_number = channels_info->power_module_item_number;
	channels_config->channel_number = channels_info->channel_number;

	channels_info->power_modules_info = get_or_alloc_power_modules_info(channels_info->channels_config);
	OS_ASSERT(channels_info->power_modules_info != NULL);
	power_modules_set_type(channels_info->power_modules_info, channels_settings->power_module_type);

	channels_info->channels_com_info = get_or_alloc_channels_com_info(channels_info);
	OS_ASSERT(channels_info->channels_com_info != NULL);

	for(i = 0; i < pdu_config->pdu_group_number; i++) {
		pdu_group_config_t *pdu_group_config = &pdu_config->pdu_group_config[i];
		pdu_group_info_t *pdu_group_info_item = channels_info->pdu_group_info + i;
		int j;

		OS_ASSERT(pdu_group_config->channel_number != 0);

		pdu_group_info_item->pdu_group_id = i;
		pdu_group_info_item->channel_number = pdu_group_config->channel_number;
		pdu_group_info_item->power_module_group_number = get_power_module_group_number_per_pdu_group_config(pdu_group_config);

		INIT_LIST_HEAD(&pdu_group_info_item->channel_idle_list);
		INIT_LIST_HEAD(&pdu_group_info_item->channel_active_list);
		INIT_LIST_HEAD(&pdu_group_info_item->channel_deactive_list);
		INIT_LIST_HEAD(&pdu_group_info_item->channel_disable_list);
		INIT_LIST_HEAD(&pdu_group_info_item->power_module_group_idle_list);
		INIT_LIST_HEAD(&pdu_group_info_item->power_module_group_deactive_list);
		INIT_LIST_HEAD(&pdu_group_info_item->power_module_group_disable_list);

		pdu_group_info_item->relay_map = alloc_bitmap(pdu_group_config->channel_number);
		OS_ASSERT(pdu_group_info_item->relay_map != NULL);

		pdu_group_info_item->channels_change_state = CHANNELS_CHANGE_STATE_IDLE;
		pdu_group_info_item->channels_info = channels_info;

		for(j = 0; j < pdu_group_config->channel_number; j++) {
			int k;
			//一枪对应一个模块组
			uint8_t group_id = power_module_group_offset++;
			power_module_group_info_t *power_module_group_info_item = channels_info->power_module_group_info + group_id;
			uint8_t power_module_number = pdu_group_config->power_module_number_per_power_module_group;

			OS_ASSERT(power_module_number != 0);
			OS_ASSERT(group_id < channels_info->power_module_group_number);
			list_add_tail(&power_module_group_info_item->list, &pdu_group_info_item->power_module_group_idle_list);
			power_module_group_info_item->group_id = group_id;
			power_module_group_info_item->pdu_group_info = pdu_group_info_item;
			INIT_LIST_HEAD(&power_module_group_info_item->power_module_item_list);

			for(k = 0; k < power_module_number; k++) {
				uint8_t module_id = power_module_item_offset++;
				power_module_item_info_t *power_module_item_info = channels_info->power_module_item_info + module_id;
				OS_ASSERT(module_id < channels_info->power_module_item_number);
				list_add_tail(&power_module_item_info->list, &power_module_group_info_item->power_module_item_list);
				power_module_item_info->module_id = module_id;
				power_module_item_info->power_modules_info = channels_info->power_modules_info;
				power_module_item_info->power_module_group_info = power_module_group_info_item;
				power_module_item_info->query_stamp = ticks - (1 * 1000);
				power_module_item_info->setting_stamp = ticks - (2 * 1000);
				power_module_item_info->status.state = POWER_MODULE_ITEM_STATE_PREPARE_DEACTIVE;
			}
		}

		//当前组分配通道
		for(j = 0; j < pdu_group_info_item->channel_number; j++) {
			uint8_t channel_id = channel_offset++;
			channel_info_t *channel_info_item = channels_info->channel_info + channel_id;
			//当前通道当前开关板电源模块组偏移

			OS_ASSERT(channel_id < channels_info->channel_number);
			list_add_tail(&channel_info_item->list, &pdu_group_info_item->channel_idle_list);
			channel_info_item->channel_id = channel_id;
			channel_info_item->channels_info = channels_info;
			channel_info_item->pdu_group_info = pdu_group_info_item;
			channel_info_item->status.state = CHANNEL_STATE_IDLE;
			channel_info_item->channel_request_state = CHANNEL_REQUEST_STATE_NONE;

			INIT_LIST_HEAD(&channel_info_item->power_module_group_list);
		}
	}

	OS_ASSERT(channel_offset == channels_info->channel_number);
	OS_ASSERT(power_module_group_offset == channels_info->power_module_group_number);
	OS_ASSERT(power_module_item_offset == channels_info->power_module_item_number);

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info_t *channel_info = channels_info->channel_info + i;

		channel_info->common_event_callback_item.fn = default_handle_channel_event;
		channel_info->common_event_callback_item.fn_ctx = channel_info;
		OS_ASSERT(register_callback(channels_info->common_event_chain, &channel_info->common_event_callback_item) == 0);

		channel_info->common_periodic_callback_item.fn = default_handle_channel_periodic;
		channel_info->common_periodic_callback_item.fn_ctx = channel_info;
		OS_ASSERT(register_callback(channels_info->common_periodic_chain, &channel_info->common_periodic_callback_item) == 0);
	}

	channels_info->common_periodic_callback_item.fn = handle_common_periodic;
	channels_info->common_periodic_callback_item.fn_ctx = channels_info;
	OS_ASSERT(register_callback(channels_info->common_periodic_chain, &channels_info->common_periodic_callback_item) == 0);

	display_info = alloc_display_info(channels_info);
	OS_ASSERT(display_info != NULL);
	channels_info->display_info = display_info;

	channels_info->display_data_action_callback_item.fn = channels_modbus_data_action;
	channels_info->display_data_action_callback_item.fn_ctx = channels_info;
	OS_ASSERT(register_callback(display_info->modbus_slave_info->data_action_chain, &channels_info->display_data_action_callback_item) == 0);

	channels_info->display_data_invalid_callback_item.fn = channels_info_channels_settings_invalid;
	channels_info->display_data_invalid_callback_item.fn_ctx = channels_info;
	OS_ASSERT(register_callback(display_info->modbus_slave_info->data_invalid_chain, &channels_info->display_data_invalid_callback_item) == 0);

	channels_info->display_data_changed_callback_item.fn = channels_info_channels_settings_changed;
	channels_info->display_data_changed_callback_item.fn_ctx = channels_info;
	OS_ASSERT(register_callback(display_info->modbus_slave_info->data_changed_chain, &channels_info->display_data_changed_callback_item) == 0);

	{
		int i;

		debug("modbus addr max:%d!", MODBUS_ADDR_SIZE - 1);

		for(i = 0; i < MODBUS_ADDR_SIZE; i++) {
			modbus_data_ctx_t modbus_data_ctx;
			modbus_data_ctx.ctx = NULL;
			modbus_data_ctx.action = MODBUS_DATA_ACTION_GET;
			modbus_data_ctx.addr = i;
			channels_modbus_data_action(channels_info, &modbus_data_ctx);
		}
	}

	osThreadCreate(osThread(task_channels), channels_info);
	//osThreadCreate(osThread(task_channel_event), channels_info);

	channels_info->configed = 1;

	ret = 0;

	return ret;
}

static channels_info_t *alloc_channels_info(channels_config_t *channels_config)
{
	channels_info_t *channels_info = NULL;
	channels_info = (channels_info_t *)os_calloc(1, sizeof(channels_info_t));
	OS_ASSERT(channels_info != NULL);
	channels_info->channels_config = channels_config;

	channels_info->event_pool = alloc_event_pool();
	OS_ASSERT(channels_info->event_pool != NULL);

	channels_info->faults = alloc_bitmap(CHANNELS_FAULT_SIZE);
	OS_ASSERT(channels_info->faults != NULL);

	channels_info->common_periodic_chain = alloc_callback_chain();
	OS_ASSERT(channels_info->common_periodic_chain != NULL);

	channels_info->common_event_chain = alloc_callback_chain();
	OS_ASSERT(channels_info->common_event_chain != NULL);

	OS_ASSERT(channels_set_channels_config(channels_info, channels_config) == 0);

	return channels_info;
}

static int object_filter(void *o, void *ctx)
{
	int ret = -1;
	channels_info_t *channels_info = (channels_info_t *)o;
	channels_config_t *channels_config = (channels_config_t *)ctx;

	if(channels_info->channels_config == channels_config) {
		ret = 0;
	}

	return ret;
}

channels_info_t *get_or_alloc_channels_info(channels_config_t *channels_config)
{
	channels_info_t *channels_info = NULL;

	os_enter_critical();

	if(channels_class == NULL) {
		channels_class = object_class_alloc();
	}

	os_leave_critical();

	channels_info = (channels_info_t *)object_class_get_or_alloc_object(channels_class, object_filter, channels_config, (object_alloc_t)alloc_channels_info, (object_free_t)free_channels_info);

	return channels_info;
}

channels_info_t *start_channels(void)
{
	channels_info_t *channels_info;
	channels_config_t *channels_config = get_channels_config(0);

	channels_info = get_or_alloc_channels_info(channels_config);
	OS_ASSERT(channels_info != NULL);

	return channels_info;
}
