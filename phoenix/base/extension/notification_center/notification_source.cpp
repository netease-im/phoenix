//
//  notification_source.cpp
//  extension
//
//  Created by dudu on 2019/9/25.
//

#include "extension/notification_center/notification_source.h"
#include "extension/notification_center/notification_center.h"

EXTENSION_BEGIN_DECLS
NotificationSource::NotificationSource()
{
    PlatformInit();
}

NotificationSource::~NotificationSource()
{
    Shutdown();
}
void NotificationSource::PlatformInit()
{
    is_shutdown_ = false;
#if defined(OS_MACOSX)
    PlatformInitForMac();
#endif
}
void NotificationSource::Shutdown()
{
    NS_EXTENSION::NAutoLock lock(&lock_);
    if (is_shutdown_) {
        return;
    }
    is_shutdown_ = true;
    
#if defined(OS_MACOSX)
    PlatfomDestroyForMac();
#endif
}
void NotificationSource::onEnterBakground()
{
    if (is_shutdown_) {
        return;
    }
    
    NotificaionCenter::GetInstance()->notifyEnterBackground();
}
void NotificationSource::onEnterForeground()
{
    if (is_shutdown_) {
        return;
    }
    
    NotificaionCenter::GetInstance()->notifyEnterForeground();
}
EXTENSION_END_DECLS
