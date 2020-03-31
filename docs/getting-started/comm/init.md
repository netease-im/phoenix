# 入门指引
## AtExitManager
提供类似于CRT atexit()的功能,这个对象主要被Singleton使用。当AtExitManager对象超出作用域时，所有向该对象注册的回调将被调用
### 示例：

      int main（...）
      {
        NS_EXTENSION::AtExitManager at_exit = NS_EXTENSION::AtExitManagerAdeptor::GetAtExitManager();
        ...
        at_exit.reset();
      }

## Singleton
### 定义

      template <typename TSingleton, bool release_atexitmanager = true>
      class Singleton
      {
          ...
      }
* `TSingleton`：指定的单例类型。
* `release_atexitmanager`：单例对象是否向AtExitManager注册回调，true/false,当该参数为true时，AtExitManager对象销毁后不应再次使用该单例对象，否则可能会引起程序崩溃。
### 示例代码

        class Test : public NS_EXTENSION::Singleton<Test,true>
		{
          //如果TSingleton的构造函数是private/protected需要在类定义时引入此宏,如下所示
			SingletonHideConstructor(Test)
		private:
			Test() = default;
			~Test() = default;
		}       

## 线程的使用
为了更好的管理及在不同的线程之间投递任务,最好把线程都托管到`ThreadManager`进行管理.
### 创建线程
可以通过`ThreadManager::CreateFrameworkThread`来创建线程
#### 函数定义

      FrameworkThread* ThreadManager::CreateFrameworkThread(int64_t identifier, const std::string& name)
* `identifier`: 线程的识别码,用于投递任务.
* `name`:线程名称.

#### 示例
    std::unique_ptr<NS_EXTENSION::FrameworkThread > main_thread = nullptr;
    main_thread.reset(NS_EXTENSION::ThreadManager::CreateFrameworkThread(0,"main"));
    main_thread->Start();
* Note:`Start()`在Windows中，不能在DllMain/全局对象构造或析构过程中/atexit()回调时调用。

### Attach当前非FrameworkThread线程
当前线程是非FrameworkThread线程时可以通过`FrameworkThread::AttachCurrentThreadWithLoop`来附加到当前线程
#### 函数定义
    void FrameworkThread::AttachCurrentThreadWithLoop(const MessageLoop::Type type)
* `type`: 消息循环类型
    * TYPE_DEFAULT
    * TYPE_UI
    * TYPE_CUSTOM
    * TYPE_IO
### 向线程投递任务
可以调用`ThreadManager::Post...Task`在线程间进行任务投递
#### 函数定义
    //向指定线程投递任务,identifier未指定时会投递到当前线程
    bool ThreadManager::PostTask(const StdClosure &task);
	bool ThreadManager::PostTask(int64_t identifier, const StdClosure &task);

    //向指定线程投递延时任务,identifier未指定时会投递到当前线程
	bool ThreadManager::PostDelayedTask(const StdClosure &task, TimeDelta delay);
	bool ThreadManager::PostDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);

    //向指定线程投递非嵌套任务,identifier未指定时会投递到当前线程
	bool ThreadManager::PostNonNestableTask(const StdClosure &task);
	bool ThreadManager::PostNonNestableTask(int64_t identifier, const StdClosure &task);

    //向指定线程投递非嵌套延时任务,identifier未指定时会投递到当前线程
	bool ThreadManager::PostNonNestableDelayedTask(const StdClosure &task, TimeDelta delay);
	bool ThreadManager::PostNonNestableDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);
	
    //向当前线程投递任务,在指定时间内没有执行完任务时会调用"cb"
    bool ThreadManager::PostTimerTask(const StdClosure &task, const StdClosure &cb, TimeDelta delay);
	
    //向指定线程投递关联任务,执行完"task"后会执行"reply"任务
    bool ThreadManager::PostTaskAndReply(int64_t identifier, const StdClosure &task, const StdClosure &reply);
## 定时器的使用
### 分类

定时器分为单次定时器(`OneShotTimer`)与循环定时器(`RepeatingTimer`)
- `OneShotTimer`:计时时间到达时进行回调，计时结束。
- `RepeatingTimer`:可以指定一定的时间间隔,计时时间到达时进行回调,并进行下次计时.
#### 示例代码
    std::unique_ptr<NS_EXTENSION::RepeatingTimer> clock = nullptr; 
    std::unique_ptr<NS_EXTENSION::OneShotTimer> shot_timer = nullptr;
    ...
    clock = std::make_unique<NS_EXTENSION::RepeatingTimer >();
    ...
    if (clock->IsRunning())
		clock->Stop();
    clock->Start(NS_EXTENSION::TimeDelta::FromSeconds(1), [](){
		...
	});	
    ...
    shot_timer = std::make_unique<NS_EXTENSION::OneShotTimer >();
    ...
    if (shot_timer->IsRunning())
		shot_timer->Stop();
    shot_timer->Start(NS_EXTENSION::TimeDelta::FromSeconds(1), []() {
        ...
    });
### 计时器线程
如果不指定定时器的计时线程,定时器以当前线程做为定时线程,也可以调用`SetTaskRunner`来指定特定的计时线程及任务回调线程
#### 示例代码
    std::unique_ptr<NS_EXTENSION::RepeatingTimer> clock = nullptr;
    std::shared_ptr<NS_EXTENSION::FrameworkThread> global_timer_thread = nullptr;
    ...
    if(global_timer_thread == nullptr)
		global_timer_thread = NS_EXTENSION::ThreadManager::GlobalTimerThreadRef();
    clock = std::make_unique<NS_EXTENSION::RepeatingTimer >();
    clock->SetTaskRunner(global_timer_thread->task_runner());
    ...
    if (clock->IsRunning())
		clock->Stop();
    clock->Start(NS_EXTENSION::TimeDelta::FromSeconds(1), [](){
		...
	});	