#pragma once
#include "utils/prettyprint.h"
#include <cstdio>
#include <cstdint>
#include <string>

namespace OVS::Utils
{
    using namespace std;
    struct ParsedURL
    {
        std::wstring Protocol;
        std::wstring Host;
        std::wstring Port;
        std::wstring Path;
        std::map<std::wstring, std::wstring> QueryParams;
        bool bParseSuccess;
        bool bisHTTPS;

        ParsedURL()
        {
            this->Protocol = L"https://";
            this->Host = L"";
            this->Port = L"80";
            this->Path = L"";
            this->QueryParams = {};
            this->bParseSuccess = false;
            this->bisHTTPS = true;
        }
    };

    template <typename T>
    void DebugPrintWrapper(T* message, T* color) = delete;

    template <typename T>
    void DebugPrintWrapper(T message, T color) = delete;

    void DebugPrintWrapper(const wchar_t* message, const wchar_t* color);
    void DebugPrintWrapper(std::wstring message, std::wstring color);
    void DebugPrintWrapper(const char* message, const char* color);
    void DebugPrintWrapper(std::string message, std::string color);
    void DebugPrintWrapper(const wchar_t* message, ConsoleColors color);
    void DebugPrintWrapper(std::wstring message, ConsoleColors color);
    void DebugPrintWrapper(const char* message, ConsoleColors color);
    void DebugPrintWrapper(std::string message, ConsoleColors color);
    int AutoUpdateHttpRequest(const wchar_t* whost, int port, const wchar_t* wpath, char* outBuf, int outBufSize);
    bool AutoUpdateJsonGetString(const char* json, const char* key, char* out, int outSize);
    ParsedURL AutoUpdateParseUrl(const std::wstring& url);
}
