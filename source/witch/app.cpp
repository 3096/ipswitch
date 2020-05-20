#include "app.hpp"

#include "ui/main_screen.hpp"

namespace witch {

Application::Application() {
    mDisplay.setBackgroundColour(Aether::Theme::Dark.bg.r, Aether::Theme::Dark.bg.g, Aether::Theme::Dark.bg.b);
    mDisplay.setHighlightAnimation(Aether::Theme::Dark.highlightFunc);
    mDisplay.setHighlightColours(Aether::Theme::Dark.highlightBG, Aether::Theme::Dark.selected);

    mDisplay.setScreen(&ui::MainScreen::getInstance());
}

Application::~Application() {}

void Application::run() {
    while (getInstance().mDisplay.loop()) {
    }
}

}  // namespace witch
