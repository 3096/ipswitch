#include "patch_screen.hpp"

#include <fstream>

#include "../app.hpp"
#include "elements/patch_toggle.hpp"
#include "main_screen.hpp"

namespace witch {
namespace ui {

PatchScreen::PatchScreen() {
    mp_patchList = new Aether::List(LIST_X, LIST_Y, LIST_W, LIST_H);
    addElement(mp_patchList);

    setFocused(mp_patchList);

    auto* controls = new Aether::Controls();
    controls->addItem(new Aether::ControlItem(Aether::Button::A, "OK"));
    controls->addItem(new Aether::ControlItem(Aether::Button::B, "Back"));
    controls->addItem(new Aether::ControlItem(Aether::Button::Y, "Apply to IPS"));
    controls->addItem(new Aether::ControlItem(Aether::Button::MINUS, "Apply to AMS cheats"));
    controls->addItem(new Aether::ControlItem(Aether::Button::X, "More Options"));
    controls->addItem(new Aether::ControlItem(Aether::Button::PLUS, "Exit"));
    addElement(controls);

    onButtonPress(Aether::Button::B, []() { Application::setScreen(MainScreen::getInstance()); });
    onButtonPress(Aether::Button::PLUS, Application::exitApp);
}

PatchScreen::~PatchScreen() {}

void PatchScreen::load(std::filesystem::path pchtxtPath) {
    auto& s_instance = getInstance();
    s_instance.m_pchtxtPath = pchtxtPath;
    Application::setScreen(s_instance);
}

void PatchScreen::onLoad() {
    auto pchtxtIstream = std::ifstream{m_pchtxtPath};
    m_curPatchTextOutput = pchtxt::parsePchtxt(pchtxtIstream, m_parseLogSs);

    mp_patchList->removeAllElements();
    for (auto& patchCollection : m_curPatchTextOutput.collections) {
        for (auto& patch : patchCollection.patches) {
            // TODO: better list:
            // option element with title and author displayed
            // add dividers
            mp_patchList->addElement(new elements::PatchToggle(patch));
        }
    }
}

}  // namespace ui
}  // namespace witch
