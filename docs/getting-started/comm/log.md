# nim_log

为了增加日志写入效率及完整性，日志模块采用内存映射文件的方式，当内存文件大小达到指定大小时会一次性刷入指定的日志文件。程序再次启动时会首先读取映射文件，如果映射文件有内容，会刷入到日志文件
## 日志模块的使用

+ 创建日志对象   
调用 nim_log::NIMLog::CreateLogger() 获取一个日志对象
+ 主要接口   


        class ILogger
        {
        public:
            virtual void SetLogFile(const std::string &file_path);//设置日志文件路径 UTF8格式字符串
            virtual std::string GetLogFile();//获取日志文件路径
            virtual void SetLogLevel(LOG_LEVEL lv);//设置日志级别，低于设置级别的日志不会输出
            virtual LOG_LEVEL GetLogLevel();//获取当前日志级别
            virtual bool Flush();//内存映射文件刷入到日志文件
            virtual void Release();//释放所映射的内存文件
        };
+ 写日志    
为了在代码中方便写入日志，日志功能模块定义了宏`__NIM_LOG_PRO`    
    - __NIM_LOG_PRO:

        __NIM_LOG_PRO(fmt,Logger)
        fmt: 打印日志的格式
        Logger:日志对象
    - 日志格式    
    日志中的参数采用占位符的形式来传入类似"`{`i`}`"的格式,并以输出符的方式传递给日志对象如下所示    

            __NIM_LOG_PRO("kye/value {0}:{1}",logger) << key << value;
    - 使用示例    
为了在代码中更方便的使用日志功能，还可以对`__NIM_LOG_PRO`进行再次封装例如    

            USING_NS_NIMLOG
            class TestLogger : public NS_EXTENSION::Singleton<TestLogger,false>
            {
                SingletonHideConstructor(TestLogger);
            private:
                TestLogger() :logger_(NIMLog::CreateLogger()) {}
                ~TestLogger() = default;
            public:
                inline const Logger& GetLogger() const {
                    return logger_;
                }
            private:
                Logger logger_;
            };
            #define TEST_LOG_PRO(fmt) __NIM_LOG_PRO(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_APP(fmt) __NIM_LOG_APP(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_WAR(fmt) __NIM_LOG_WAR(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_ERR(fmt) __NIM_LOG_ERR(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_KER(fmt) __NIM_LOG_KER(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_ASS(fmt) __NIM_LOG_ASS(fmt,TestLogger::GetInstance()->GetLogger())
            #define TEST_LOG_INT(fmt) __NIM_LOG_INT(fmt,TestLogger::GetInstance()->GetLogger())



            .......


            TEST_LOG_PRO("kye/value {0}:{1}") << key << value;
