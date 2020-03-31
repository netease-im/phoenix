//
//  NotificationCenter.cpp
//  extension
//
//  Created by dudu on 2019/9/25.
//

#include "extension/notification_center/notification_center.h"
#include "extension/notification_center/notification_source.h"

EXTENSION_BEGIN_DECLS

NotificaionCenter::NotificaionCenter()
{
    source_ = std::make_shared<NotificationSource>();
}
NotificaionCenter::~NotificaionCenter()
{
    if (source_ != nullptr) {
        source_->Shutdown();
    }
}
void NotificaionCenter::AddObserver(NotificaionObserver* observer)
{
    NS_EXTENSION::NAutoLock lock(&lock_);
    observers_.AddObserver(observer);
}
void NotificaionCenter::RemoveObserver(NotificaionObserver* observer)
{
    NS_EXTENSION::NAutoLock lock(&lock_);
    observers_.RemoveObserver(observer);
}
std::shared_ptr<NotificationSource> NotificaionCenter::Source()
{
    return source_;
}
void NotificaionCenter::notifyEnterBackground()
{
//    for (const auto& obs : observers_) {
//        obs.enterBackground();
//    }
    
    FOR_EACH_OBSERVER(NotificaionObserver, observers_, enterBackground());
}
void NotificaionCenter::notifyEnterForeground()
{
//    for (auto& obs : observers_) {
//        obs.enterForeground();
//    }
//    observers_->Notify(FROM_HERE, &NotificaionObserver::enterForeground);
    FOR_EACH_OBSERVER(NotificaionObserver, observers_, enterForeground());
}

EXTENSION_END_DECLS
