#ifndef __EXTENSION_MEMORY_SINGLETON_H__
#define __EXTENSION_MEMORY_SINGLETON_H__
#include "extension/config/build_config.h"
#include <memory>
#include <mutex>
#include "extension/at_exit_manager.h"

EXTENSION_BEGIN_DECLS

	//饿汉模式
#define SINGLETON_DEFINE(TypeName)				\
static TypeName* GetInstance()					\
{												\
	static TypeName type_instance;				\
	return &type_instance;						\
}												\
												\
TypeName(const TypeName&) = delete;				\
TypeName& operator=(const TypeName&) = delete
    
// 懒汉模式
//release_atexitmanager 是否由atexitmanager来释放
	template <typename TSingleton, bool release_atexitmanager = true>
	class Singleton
	{
		//如果TSingleton的构造函数是private/protected需要在类定义时引入此宏,如下所示
		//class Test : public Singleton<Test>
		//{
		//		SingletonHideConstructor(Test)
		//	private:
		//		Test() = default;
		//		~Test() = default;
		//}       
#if defined(OS_WIN)
    #define SingletonHideConstructor(TSingleton) \
    private:\
	template<class _Ty,class... _Types,std::enable_if_t<!std::is_array_v<_Ty>, int>>\
	inline friend std::unique_ptr<_Ty> std::make_unique(_Types&&... _Args);\
	template<class _Ty>\
    friend struct std::default_delete;
#else
    #define SingletonHideConstructor(TSingleton) \
    private: \
    template<class _Tp, class... _Args> \
    inline friend _LIBCPP_INLINE_VISIBILITY \
    typename std::__unique_if<_Tp>::__unique_single \
    std::make_unique(_Args&&... __args); \
    template<class _Ty> \
    friend struct std::default_delete;
#endif
        
	private:
		friend TSingleton;
		using TSingletonPtr = std::unique_ptr<TSingleton>;
	private:
		Singleton(void) = default;
		virtual ~Singleton() = default;
		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator = (const Singleton&) = delete;

		static TSingletonPtr instance;
		static std::unique_ptr<std::once_flag> oc;
	public:
		static const TSingletonPtr& GetInstance()
		{
			assert(!std::is_array<TSingleton>::value);
			std::call_once(*oc, [&] (){				
				if (release_atexitmanager)
				{
					auto at_exit = AtExitManagerAdeptor::TryGetAtExitManager();
					if (at_exit != nullptr)
					{
						instance = std::make_unique<TSingleton>();
						at_exit->RegisterCallback([](void* ptr) {
							instance.reset();
							oc.reset(new std::once_flag);
						}, nullptr);
					}	
					else
					{
						instance = nullptr;
					}
				}
				else
				{
					instance = std::make_unique<TSingleton>();
				}
			});
			return instance;
		}
	};
	template <typename TSingleton, bool release_atexitmanager>
	std::unique_ptr<TSingleton> Singleton< TSingleton, release_atexitmanager>::instance = nullptr;
	template <typename TSingleton, bool release_atexitmanager>
	std::unique_ptr<std::once_flag> Singleton< TSingleton, release_atexitmanager>::oc = std::make_unique<std::once_flag>();

	EXTENSION_END_DECLS



#endif // BASE_MEMORY_SINGLETON_H_
