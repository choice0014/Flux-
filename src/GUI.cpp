#include "GUI.h"
#include "Platform.h"
#include <raylib.h>
#include <iostream>

namespace Flux {
namespace GUI {

static std::vector<Control> g_controls;

void initWindow(int width, int height, const std::string& title) {
    ::InitWindow(width, height, title.c_str());
    ::SetTargetFPS(60);
}

void addControl(Control::Type type, const std::string& text, float x, float y, float w, float h, const std::string& callback) {
    g_controls.push_back({type, text, x, y, w, h, callback});
}

void messageBox(const std::string& title, const std::string& message) {
    Platform::messageBox(title, message);
}

void runLoop(std::function<void(const std::string&)> callbackHandler) {
    while (!::WindowShouldClose()) {
        ::BeginDrawing();
        ::ClearBackground(RAYWHITE);

        for (auto& ctrl : g_controls) {
            ::Rectangle r = { ctrl.x, ctrl.y, ctrl.w, ctrl.h };

            if (ctrl.type == Control::BUTTON) {
                ::DrawRectangleRec(r, LIGHTGRAY);
                ::DrawRectangleLinesEx(r, 2, GRAY);
                ::DrawText(ctrl.text.c_str(), (int)ctrl.x + 10, (int)ctrl.y + 10, 20, BLACK);
                
                if (::IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && ::CheckCollisionPointRec(::GetMousePosition(), r)) {
                    if (!ctrl.callback.empty()) {
                        callbackHandler(ctrl.callback);
                    }
                }
            } else if (ctrl.type == Control::LABEL) {
                ::DrawText(ctrl.text.c_str(), (int)ctrl.x, (int)ctrl.y, 20, BLACK);
            }
        }

        ::EndDrawing();
    }
}

void closeWindow() {
    ::CloseWindow();
    g_controls.clear();
}

} // namespace GUI
} // namespace Flux
