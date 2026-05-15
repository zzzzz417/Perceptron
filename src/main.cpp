#include<iostream>
#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<vector>
#include<fstream>
#include<string>
#include <thread>
#include <variant>
#include <dwmapi.h>
#include <windowsx.h>

#include"httplib.h"
#include"nlohmann/json.hpp"
// #include"Server.h"
// #include"modle.h"
#include"html.h"
#include"resource.h"

#include <windows.h>
#define PLATFORM_NAME "Windows"
#define WEBVIEW_WINAPI
#include"webview.h"

HMODULE LoadDllFromResource(int resourceId)
{
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    HGLOBAL hGlobal = LoadResource(NULL, hRes);
    DWORD dllSize = SizeofResource(NULL, hRes);
    void* dllData = LockResource(hGlobal);
    HMODULE hMod = ::LoadLibraryExA(
        (LPCSTR)dllData,
        NULL,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE
    );
    FreeResource(hGlobal);
    return hMod;
}

std::string url="http://127.0.0.1:";
int main(){
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        LoadDllFromResource(IDR_DLL_WEBVIEW2LOADER);
        LoadDllFromResource(IDR_DLL_LIBGCC01);
        LoadDllFromResource(IDR_DLL_LIBSTD01);
        LoadDllFromResource(IDR_DLL_LIBSTD02);
    #endif

    httplib::Server basic_server;

    // std::string AUTH_TOKEN="aaaa";

    // basic_server.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
    //     res.set_content(html_content, "text/html;charset=utf-8");
    // });
    // basic_server.Get("/style.css", [&](const httplib::Request& req, httplib::Response& res) {
    //     res.set_content(css_content, "text/css;charset=utf-8");
    // });
    // basic_server.Get("/script.js", [&](const httplib::Request& req, httplib::Response& res) {
    //     res.set_content(js_content, "application/javascript;charset=utf-8");
    // });

    basic_server.Post("/api/predict",[&](const httplib::Request request, httplib::Response response)->void{
        std::cout<< "[INFO] check\n";
    });

    //--- app control endpoints (called from web UI)
    HWND app_hwnd = NULL;
    basic_server.Get("/app/minimize", [&](const httplib::Request&, httplib::Response& res) {
        std::cout << "[INFO] minimize\n";
        if (app_hwnd) ShowWindow(app_hwnd, SW_MINIMIZE);
        res.set_content("ok", "text/plain");
    });
    basic_server.Get("/app/close", [&](const httplib::Request&, httplib::Response& res) {
        std::cout << "[INFO] close\n";
        if (app_hwnd) PostMessageW(app_hwnd, WM_CLOSE, 0, 0);
        res.set_content("ok", "text/plain");
    });
    basic_server.set_mount_point("/","../Web");
    int port=basic_server.bind_to_any_port("127.0.0.1");
    if(port<0)return -1;

    url+=std::to_string(port);
    std::thread server_thread([&]() {
        basic_server.listen_after_bind();
    });

    basic_server.wait_until_ready();

    webview::webview w(true, nullptr);
    w.set_title(" ");
    w.set_size(800, 450, WEBVIEW_HINT_FIXED);
    w.navigate(url);

#ifdef _WIN32
    void* hwnd_ptr =w.window().value();
    HWND hwnd=(HWND)hwnd_ptr;
    app_hwnd = hwnd;

    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
    SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    //--- frameless: remove title bar, keep DWM shadow
    {
        LONG lstyle = GetWindowLongW(hwnd, GWL_STYLE);
        lstyle &= ~WS_CAPTION;
        SetWindowLongW(hwnd, GWL_STYLE, lstyle);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        MARGINS m = {1,1,1,1};
        DwmExtendFrameIntoClientArea(hwnd, &m);
    }

    //--- rounded corners (Win11 native, Win10 region fallback)
    {
        DWM_WINDOW_CORNER_PREFERENCE cp = DWMWCP_ROUND;
        HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                           &cp, sizeof(cp));
        if (FAILED(hr)) {
            RECT r; GetWindowRect(hwnd, &r);
            int ww = r.right - r.left;
            int wh = r.bottom - r.top;
            HRGN rgn = CreateRoundRectRgn(0, 0, ww + 1, wh + 1, 20, 20);
            SetWindowRgn(hwnd, rgn, TRUE);
        }
    }

#endif

    //--- drag endpoint (needs w for main-thread dispatch)
    basic_server.Get("/app/drag", [&](const httplib::Request&, httplib::Response& res) {
        std::cout << "[INFO] drag\n";
        HWND h = app_hwnd;
        if (h) {
            w.dispatch([h]() {
                std::cout << "[INFO] drag on main thread\n";
                ReleaseCapture();
                SendMessageW(h, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            });
        }
        res.set_content("ok", "text/plain");
    });

    w.run();

    basic_server.stop();
    if(server_thread.joinable()) {
        server_thread.join();
    }
    // system("pause");
    return 0;
}
