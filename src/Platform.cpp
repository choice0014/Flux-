#include "Platform.h"
#include <iostream>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    static std::wstring utf8ToWide(const std::string& str) {
        if (str.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }
#endif

namespace Flux {
namespace Platform {

void messageBox(const std::string& title, const std::string& message) {
#ifdef _WIN32
    ::MessageBoxW(NULL, utf8ToWide(message).c_str(), utf8ToWide(title).c_str(), MB_OK | MB_ICONINFORMATION);
#else
    std::cout << "[GUI MsgBox] " << title << ": " << message << std::endl;
#endif
}

} // namespace Platform
} // namespace Flux
