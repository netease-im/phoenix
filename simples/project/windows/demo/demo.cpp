// demo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "demo.h"
#include "extension/wrappers.h"
#include "extension/thread/thread_manager.h"
#include "extension/timer/timer.h"
#include "extension/strings/string_util.h"
#include "extension/at_exit_manager.h"
#include "log/demo_log.h"
#define MAX_LOADSTRING 100
std::unique_ptr<NS_EXTENSION::FrameworkThread > g_main_thread = nullptr;
std::unique_ptr<NS_EXTENSION::RepeatingTimer> g_clock = nullptr; 
std::unique_ptr<NS_EXTENSION::OneShotTimer> g_shot_timer = nullptr;
std::shared_ptr<NS_EXTENSION::FrameworkThread> g_global_timer_thread = nullptr;
HWND g_MainWnd = NULL;
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void InvalidateMainWnd()
{
	if (::IsWindow(g_MainWnd))
	{
		RECT rc = { 0 };
		::GetClientRect(g_MainWnd, &rc);
		::InvalidateRect(g_MainWnd, &rc, false);
	}
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	NS_EXTENSION::AtExitManager at_exit = NS_EXTENSION::AtExitManagerAdeptor::GetAtExitManager();
	g_main_thread.reset(NS_EXTENSION::ThreadManager::CreateFrameworkThread(0,"main"));
	DemoLogger::GetInstance()->GetLogger()->SetLogFile("d:/testlog/test.log");
	DemoLogger::GetInstance()->GetLogger()->SetLogLevel(NS_NIMLOG::LOG_LEVEL::LV_PRO);
	g_main_thread->RegisterInitCallback([hInstance, nCmdShow]() {
		// 初始化全局字符串
		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_DEMO, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);
		InitInstance(hInstance, nCmdShow);			
		if(g_global_timer_thread == nullptr)
			g_global_timer_thread = NS_EXTENSION::ThreadManager::GlobalTimerThreadRef();
		if(g_clock == nullptr)
			g_clock = std::make_unique<NS_EXTENSION::RepeatingTimer >();		
		NS_EXTENSION::ThreadManager::PostTask([]() {
			g_clock->Start(NS_EXTENSION::TimeDelta::FromSeconds(1), []() {
				InvalidateMainWnd();
				});
			});		
	});
	g_main_thread->RegisterCleanupCallback([]() {
		if (g_clock != nullptr)
			if(g_clock->IsRunning())
				g_clock->Stop();
		g_clock.release();
		});	
	g_main_thread->AttachCurrentThreadWithLoop(base::MessageLoop::Type::TYPE_UI);
	g_global_timer_thread.reset();
	g_main_thread.release();
	at_exit.reset();
	DEMO_LOG_PRO("end");
    return (int) 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   g_MainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!g_MainWnd)
   {
      return FALSE;
   }

   ShowWindow(g_MainWnd, nCmdShow);
   UpdateWindow(g_MainWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				NS_EXTENSION::ThreadManager::PostTask([]() {
					g_main_thread->DettachCurrentThread();
					});
                break;
			case ID_TEST_TIMER:
			{
				if (g_shot_timer == nullptr)
					g_shot_timer = std::make_unique<NS_EXTENSION::OneShotTimer >();
				if (g_shot_timer->IsRunning())
					g_shot_timer->Stop();
				g_shot_timer->Start(NS_EXTENSION::TimeDelta::FromSeconds(1), [hWnd]() {
					MessageBoxA(hWnd, "Time out", "shot timer", 0);
					});
			}				
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			NS_EXTENSION::TimeStruct qt;
			NS_EXTENSION::Time::Now().LocalExplode(&qt);
			std::string text = NS_EXTENSION::StringPrintf("%04d-%02d-%02d %02d:%02d:%02d.%03d",
				qt.year,qt.month, qt.day_of_month, qt.hour, qt.minute, qt.second, qt.millisecond);
			RECT rc = { 0 },rc_clock,rc_http_rsp;
			::GetClientRect(hWnd, &rc);
			rc_clock = rc;
			rc_http_rsp = rc;
			rc_clock.bottom = rc_clock.top + 20;
			rc_http_rsp.top = rc_clock.bottom + 5;
			rc_http_rsp.left += 10;
			rc_http_rsp.right -= 10;
			rc_http_rsp.bottom -= 30;
			::DrawTextA(hdc, text.c_str(), text.length(), &rc_clock, DT_CENTER | DT_VCENTER | DT_SINGLELINE);			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
