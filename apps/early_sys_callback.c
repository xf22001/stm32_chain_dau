

/*================================================================
 *
 *
 *   文件名称：early_sys_callback.c
 *   创 建 者：肖飞
 *   创建日期：2021年03月25日 星期四 15时11分14秒
 *   修改日期：2021年11月02日 星期二 12时56分27秒
 *   描    述：
 *
 *================================================================*/
#include "early_sys_callback.h"
#include "os_utils.h"
#include "usbh_user_callback.h"

void early_sys_callback(void)
{
	OS_ASSERT(usbh_event_message_queue_init(10) == 0);
}
