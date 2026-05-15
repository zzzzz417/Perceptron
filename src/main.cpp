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



    });


    basic_server.set_mount_point("/","../Web");
    int port=basic_server.bind_to_any_port("127.0.0.1");
    if(port<0)return -1;

    url+=std::to_string(port);
    std::thread server_thread([&]() {
        basic_server.listen_after_bind();
    });

    basic_server.wait_until_ready();

    webview::webview w(false, nullptr);
    w.set_title(" ");
    w.set_size(550, 900, WEBVIEW_HINT_FIXED);
    w.navigate(url);

#ifdef _WIN32
    void* hwnd_ptr =w.window().value();
    HWND hwnd=(HWND)hwnd_ptr;
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
    SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    //------------------------
#endif

    w.run();

    basic_server.stop();
    if(server_thread.joinable()) {
        server_thread.join();
    }
    return 0;    
}

