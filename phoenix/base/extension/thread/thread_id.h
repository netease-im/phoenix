#ifndef __BSE_EXTENSION_THREAD_ID_H__
#define __BSE_EXTENSION_THREAD_ID_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <stdint.h>

EXTENSION_BEGIN_DECLS

#define kFCoreInvalidID      (int16_t)-1 //无效ID
#define kFCoreIDMax          (int16_t)60  //有效ID[0,kFCoreIDMax),0特定给NIM使用

//统一的线程管理器里，每个core id拥有一段区间的线程ID（如core_id = 1, 则其对应的线程id区间为[core_id * 1000, core_id * 1000 + 999]）
#define FCORE_THREAD_ID(core_id, thread_tag)		((core_id) * 1000 + (thread_tag))

//核心线程Tag（线程id = core_id * 1000 + 线程Tag）
#define kFCoreThreadTagLink		1
#define kFCoreThreadTagCore		2

//#define kFCoreThreadTagNrtc			3
//#define kFCoreThreadTagNrtcDevice		4

//自定义线程Tag（> 100）
#define kFCoreThreadTagCustomOffset		100
#define kFCoreThreadTagCustomMax		999

#define kFCoreThreadNameLink		"Link thread"
#define kFCoreThreadNameCore		"Core thread"

//自定义全局线程
#define kFCoreGlobalThreadCustomOffset   (kFCoreIDMax*1000 + kFCoreThreadTagCustomMax)

//nim自定义全局线程
#define kNIMGlobalThreadCustomOffset (kFCoreGlobalThreadCustomOffset+1)
#define kNIMGlobalThreadCustomMax   (kNIMGlobalThreadCustomOffset+999)

//chatroom自定义全局线程
#define kNIMChatRoomGlobalThreadCustomOffset (kNIMGlobalThreadCustomMax+1)
#define kNIMChatRoomGlobalThreadCustomMax   (kNIMChatRoomGlobalThreadCustomOffset+999)

//音视频自定义全局线程
#define kNIMAVSGlobalThreadCustomOffset (kNIMChatRoomGlobalThreadCustomMax+1)
#define kNIMAVSGlobalThreadCustomMax   (kNIMAVSGlobalThreadCustomOffset+999)

EXTENSION_END_DECLS

#endif // __BSE_EXTENSION_THREAD_ID_H__
