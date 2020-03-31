//
//  notification_source.hpp
//  extension
//
//  Created by dudu on 2019/9/25.
//

#ifndef __BASE_EXTENSION_NOTIFICATON_SOURCE_H__
#define __BASE_EXTENSION_NOTIFICATON_SOURCE_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/synchronization/lock.h"

#include <vector>

#if defined(OS_MACOSX)
#include <objc/runtime.h>
#endif  // OS_IOS

EXTENSION_BEGIN_DECLS

class NotificationSource
{
public:
    NotificationSource();
    ~NotificationSource();
    void PlatformInit();
    void Shutdown();
protected:
    void onEnterBakground();
    void onEnterForeground();
    
#if defined(OS_MACOSX)
    void PlatformInitForMac();
    void PlatfomDestroyForMac();
#endif
private:
    NS_EXTENSION::NLock lock_;
    bool is_shutdown_;
#if defined(OS_MACOSX)
    std::vector<id> notification_observers_;
#endif
};

EXTENSION_END_DECLS

#endif //__BASE_EXTENSION_NOTIFICATON_SOURCE_H__
