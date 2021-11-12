#include "app.hpp"

#include "ui/main_screen.hpp"
#include "ui/patch_screen.hpp"

namespace witch {

Application::Application() : m_window("IPSwitch", 1280, 720, [](const std::string message, const bool important) {}) {
    m_window.setBackgroundColour(Aether::Theme::Dark.bg);
    m_window.setHighlightAnimation(Aether::Theme::Dark.highlightFunc);
    m_window.setHighlightBackground(Aether::Theme::Dark.highlightBG);
    m_window.setHighlightOverlay(Aether::Theme::Dark.selected);

    m_window.showScreen(&ui::MainScreen::getInstance());
    ui::PatchScreen::init();  // do it at start so it doesn't affect load time later
}

Application::~Application() {}

void Application::run() {
    try {
        while (getInstance().m_window.loop())
            ;
    } catch (const std::exception& e) {
    }
}

}  // namespace witch
