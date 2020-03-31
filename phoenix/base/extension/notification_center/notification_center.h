//
//  NotificationCenter.hpp
//  extension
//
//  Created by dudu on 2019/9/25.
//

#ifndef __BASE_EXTENSION_NOTIFICATON_CENTER_H__
#define __BASE_EXTENSION_NOTIFICATON_CENTER_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/synchronization/lock.h"
#include "extension/memory/singleton.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list_threadsafe.h"

EXTENSION_BEGIN_DECLS

class EXTENSION_EXPORT NotificaionObserver
{
public:
    virtual void enterBackground() {}
    virtual void enterForeground() {}
    
protected:
    virtual ~NotificaionObserver() = default;
};

class EXTENSION_EXPORT NotificationSource;
class EXTENSION_EXPORT NotificaionCenter : public NS_EXTENSION::Singleton<NotificaionCenter>
{
    friend class NotificationSource;
public:
    NotificaionCenter();
    ~NotificaionCenter();
    void AddObserver(NotificaionObserver* observer);
    void RemoveObserver(NotificaionObserver* observer);
    
    
    std::shared_ptr<NotificationSource> Source();
    
protected:
    void notifyEnterBackground();
    void notifyEnterForeground();
    
private:
    NS_EXTENSION::NLock lock_;
    base::ObserverList<NotificaionObserver> observers_;
    std::shared_ptr<NotificationSource> source_;
};

EXTENSION_END_DECLS

#endif //__BASE_EXTENSION_NOTIFICATON_CENTER_H__
