#include "app.hpp"

#include "ui/main_screen.hpp"
#include "ui/patch_screen.hpp"

namespace witch {

Application::Application() {
    m_Display.setBackgroundColour(Aether::Theme::Dark.bg.r, Aether::Theme::Dark.bg.g, Aether::Theme::Dark.bg.b);
    m_Display.setHighlightAnimation(Aether::Theme::Dark.highlightFunc);
    m_Display.setHighlightColours(Aether::Theme::Dark.highlightBG, Aether::Theme::Dark.selected);

    m_Display.setScreen(&ui::MainScreen::getInstance());
    ui::PatchScreen::init();  // do it at start so it doesn't affect load time later
}

Application::~Application() {}

void Application::run() {
    try {
        while (getInstance().m_Display.loop())
            ;
    } catch (const std::exception& e) {
    }
    getInstance().~Application();
}

}  // namespace witch
