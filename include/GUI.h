#ifndef FLUX_GUI_H
#define FLUX_GUI_H

#include <string>
#include <vector>
#include <functional>

namespace Flux {
namespace GUI {

struct Control {
    enum Type { BUTTON, LABEL };
    Type type;
    std::string text;
    float x, y, w, h;
    std::string callback;
};

void initWindow(int width, int height, const std::string& title);
void addControl(Control::Type type, const std::string& text, float x, float y, float w, float h, const std::string& callback = "");
void messageBox(const std::string& title, const std::string& message);
void runLoop(std::function<void(const std::string&)> callbackHandler);
void closeWindow();

} // namespace GUI
} // namespace Flux

#endif
