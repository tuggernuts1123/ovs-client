#pragma once
#include "eSettingsManager/eSettingsManager.h"
#include "utils/Utils.hpp"
#include "utils/prettyprint.h"
#include <string>
#include <sstream>
#include <cstdlib>
#include <ws2tcpip.h>

using namespace std;
using namespace OVS::Utils;

namespace OVS::Utils
{
    // Overload for wide string pointers
    void DebugPrintWrapper(const wchar_t* message, const wchar_t* color)
    {
        std::wstring wMessage = message ? message : L"" ;
        std::wstring wColor = color ? color : L"" ;
        if (SettingsMgr->bDebug)
        {
            printfColor(wColor, wMessage);
        }
    }

    // Overload for wide string objects
    void DebugPrintWrapper(std::wstring message, std::wstring color)
    {
        DebugPrintWrapper(message.c_str(), color.c_str());
    }

    // Overload for narrow string pointers
    void DebugPrintWrapper(const char* message, const char* color)
    {
        // Handle null message
        const char* safeMessage = message ? message : "";
        const char* safeColor = color ? color : "";
        
        size_t wMessageSize = strlen(safeMessage) + 1;
        wchar_t* wMessageBuffer = new wchar_t[wMessageSize];
        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, wMessageBuffer, wMessageSize, safeMessage, wMessageSize - 1);
        std::wstring wMessage(wMessageBuffer);

        size_t wColorSize = strlen(safeColor) + 1;
        wchar_t* wColorBuffer = new wchar_t[wColorSize];
        convertedChars = 0;
        mbstowcs_s(&convertedChars, wColorBuffer, wColorSize, safeColor, wColorSize - 1);
        std::wstring wColor(wColorBuffer);

        delete[] wMessageBuffer;
        delete[] wColorBuffer;

        if (SettingsMgr->bDebug)
        {
            printfColor(wColor, wMessage);
        }
    }

    // Overload for narrow string objects
    void DebugPrintWrapper(std::string message, std::string color)
    {
        DebugPrintWrapper(message.c_str(), color.c_str());
    }

    void DebugPrintWrapper(const wchar_t* message, ConsoleColors color)
    {
        DebugPrintWrapper(message, ColorMap[color].c_str());
    }

    void DebugPrintWrapper(std::wstring message, ConsoleColors color)
    {
        DebugPrintWrapper(message.c_str(), ColorMap[color].c_str());
    }

    void DebugPrintWrapper(const char* message, ConsoleColors color)
    {
        std::wstring wideColor = ColorMap[color];
        std::string narrowColor;
        narrowColor.reserve(wideColor.size());
        
        for (wchar_t wc : wideColor) {
            narrowColor.push_back(static_cast<char>(wc)); // Safe for ASCII
        }
        
        DebugPrintWrapper(message, narrowColor.c_str());
    }

    void DebugPrintWrapper(std::string message, ConsoleColors color)
    {
        DebugPrintWrapper(message.c_str(), color);
    }

    int AutoUpdateHttpRequest(const wchar_t* whost, int port, const wchar_t* wpath,
        char* outBuf, int outBufSize)
    {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) return -1;

        DWORD timeout = 5000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

        // Convert wide strings to narrow strings for socket operations
        char host[256] = {};
        WideCharToMultiByte(CP_UTF8, 0, whost, -1, host, sizeof(host), nullptr, nullptr);

        char path[512] = {};
        WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path, sizeof(path), nullptr, nullptr);

        struct sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons((u_short)port);
        inet_pton(AF_INET, host, &addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
        {
            closesocket(sock); return -1;
        }

        // Build HTTP request as narrow string
        char request[1024];
        sprintf_s(request, sizeof(request),
            "GET %s HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
            path, host, port);

        if (send(sock, request, (int)strlen(request), 0) == SOCKET_ERROR)
        {
            closesocket(sock); return -1;
        }

        int totalRead = 0;
        while (totalRead < outBufSize - 1)
        {
            int n = recv(sock, outBuf + totalRead, outBufSize - 1 - totalRead, 0);
            if (n <= 0) break;
            totalRead += n;
        }
        outBuf[totalRead] = '\0';
        closesocket(sock);

        char* body = strstr(outBuf, "\r\n\r\n");
        if (!body) return -1;
        body += 4;
        int bodyLen = totalRead - (int)(body - outBuf);
        memmove(outBuf, body, bodyLen + 1);
        return bodyLen;
    }

    bool AutoUpdateJsonGetString(const char* json, const char* key, char* out, int outSize)
    {
        char pattern[128];
        sprintf_s(pattern, "\"%s\"", key);
        const char* pos = strstr(json, pattern);
        if (!pos) return false;
        pos += strlen(pattern);
        while (*pos == ' ' || *pos == ':' || *pos == '\t') pos++;
        if (*pos != '"') return false;
        pos++;
        int i = 0;
        while (*pos&&* pos != '"' && i < outSize - 1)
            out[i++] = *pos++;
        out[i] = '\0';
        return i > 0;
    }

    ParsedURL AutoUpdateParseUrl(const std::wstring& url)
    {
        const wchar_t* ptr = url.c_str();
        ParsedURL returnObject;

        // 1. Parse protocol
        if (wcsncmp(ptr, L"https://", 8) == 0)
        {
            returnObject.Protocol = L"https://";
            ptr += 8;
        }
        else if (wcsncmp(ptr, L"http://", 7) == 0)
        {
            returnObject.Protocol = L"http://";
            ptr += 7;
        }
        else
        {
            // No protocol specified, assume http
            returnObject.Protocol = L"http://";
        }

        // 2. Find delimiters
        const wchar_t* hostnameStart = ptr;
        const wchar_t* colon = wcschr(ptr, L':');
        const wchar_t* slash = wcschr(ptr, L'/');

        // 3. Parse hostname and port
        if (colon && (!slash || colon < slash))
        {
            // Port is explicitly specified
            returnObject.Host = std::wstring(hostnameStart, colon - hostnameStart);

            // Parse port number
            const wchar_t* portStart = colon + 1;
            const wchar_t* portEnd = slash ? slash : (ptr + wcslen(ptr));
            std::wstring portStr(portStart, portEnd - portStart);

            try
            {
                int PortAsInt = std::stoi(portStr);
                returnObject.Port = portStr;
            }
            catch (...)
            {
                // Fallback to sane defaults if port parsing fails
                if (returnObject.Protocol == L"https://")
                {
                    returnObject.Port = L"443";
                }
                else
                {
                    returnObject.Port = L"80";
                }
            }

            ptr = portEnd;
        }
        else if (slash)
        {
            // No port specified, but path exists
            returnObject.Host = std::wstring(hostnameStart, slash - hostnameStart);
            ptr = slash;
        }
        else
        {
            // No port, no path - just hostname
            returnObject.Host = std::wstring(hostnameStart);
            ptr = hostnameStart + wcslen(hostnameStart);
        }

        // 4. Parse path (everything remaining)
        if (*ptr == L'/')
        {
            returnObject.Path = std::wstring(ptr);
        }
        else
        {
            returnObject.Path = L"/";
        }

        std::wstringstream debugStream;
        debugStream << L"Parsed URL - \n" << L"  Original: " << url.c_str() << L"\n"
            << L"  Protocol: " << returnObject.Protocol << L"\n"
            << L"  Host: " << returnObject.Host << L"\n"
            << L"  Port: " << returnObject.Port << L"\n"
            << L"  Path: " << returnObject.Path << L"\n";

        DebugPrintWrapper(debugStream.str(), ConsoleColors::YELLOW);

        return returnObject;
    }
}
