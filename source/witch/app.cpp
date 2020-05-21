#include "app.hpp"

#include "ui/main_screen.hpp"

namespace witch {

Application::Application() {
    m_Display.setBackgroundColour(Aether::Theme::Dark.bg.r, Aether::Theme::Dark.bg.g, Aether::Theme::Dark.bg.b);
    m_Display.setHighlightAnimation(Aether::Theme::Dark.highlightFunc);
    m_Display.setHighlightColours(Aether::Theme::Dark.highlightBG, Aether::Theme::Dark.selected);

    m_Display.setScreen(&ui::MainScreen::getInstance());
}

Application::~Application() {}

void Application::run() {
    try {
        while (getInstance().m_Display.loop())
            ;
    } catch (const std::exception& e) {
    }
}

}  // namespace witch
