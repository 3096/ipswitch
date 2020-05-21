#include "patch_screen.hpp"

#include "../app.hpp"
#include "../core/files.hpp"

namespace witch {
namespace ui {

PatchScreen::PatchScreen() {
    auto* controls = new Aether::Controls();
    controls->addItem(new Aether::ControlItem(Aether::Button::A, "OK"));
    controls->addItem(new Aether::ControlItem(Aether::Button::B, "Back"));
    controls->addItem(new Aether::ControlItem(Aether::Button::Y, "Apply"));
    controls->addItem(new Aether::ControlItem(Aether::Button::A, "More Options"));
    controls->addItem(new Aether::ControlItem(Aether::Button::PLUS, "Exit"));
    addElement(controls);

    onButtonPress(Aether::Button::PLUS, Application::exit);
}

PatchScreen::~PatchScreen() {}

void PatchScreen::onLoad() {}

}  // namespace ui
}  // namespace witch
