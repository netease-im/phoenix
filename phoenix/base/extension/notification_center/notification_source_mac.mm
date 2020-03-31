//
//  notification_source_mac.cpp
//  extension Mac
//
//  Created by dudu on 2019/9/25.
//

#include "extension/notification_center/notification_source.h"
#include "extension/notification_center/notification_center.h"

#import <Appkit/AppKit.h>

EXTENSION_BEGIN_DECLS

void NotificationSource::PlatformInitForMac()
{
    id enterBackground = [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationDidResignActiveNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        auto source = NotificaionCenter::GetInstance()->Source();
        if(source){
            source->onEnterBakground();
        }
    }];
    
    id enterForeground = [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        auto source = NotificaionCenter::GetInstance()->Source();
        if(source){
          source->onEnterForeground();
        }
    }];
    
    notification_observers_.push_back(enterBackground);
    notification_observers_.push_back(enterForeground);
}

void NotificationSource::PlatfomDestroyForMac()
{
    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    for (id obs : notification_observers_) {
      [nc removeObserver:obs];
    }
    notification_observers_.clear();
}
EXTENSION_END_DECLS
