#include "patch_screen.hpp"

#include <fstream>

#include "../app.hpp"
#include "elements/patch_toggle.hpp"
#include "main_screen.hpp"

namespace witch {
namespace ui {

PatchScreen::PatchScreen()
    : mp_patchList(new Aether::List(LIST_X, LIST_Y, LIST_W, LIST_H)),
      mp_logTextBlockWrap(new Aether::Scrollable(LOG_X - 64, LOG_Y, LOG_W + 128, LOG_H)) {
    addElement(mp_patchList);

    mp_logTextBlockWrap->setShowScrollBar(false);
    addElement(mp_logTextBlockWrap);

    auto* controlBar = new Aether::ControlBar();
    controlBar->addControl(Aether::Button::A, "OK");
    controlBar->addControl(Aether::Button::B, "Back");
    controlBar->addControl(Aether::Button::Y, "Apply to IPS");
    // controlBar->addControl(Aether::Button::MINUS, "Apply to AMS cheats");
    // controlBar->addControl(Aether::Button::X, "More Options");
    controlBar->addControl(Aether::Button::PLUS, "Exit");
    addElement(controlBar);

    onButtonPress(Aether::Button::B, []() { Application::setScreen(MainScreen::getInstance()); });
    onButtonPress(Aether::Button::Y, [this]() { applyIps(); });
    onButtonPress(Aether::Button::PLUS, Application::exitApp);
}

PatchScreen::~PatchScreen() {}

void PatchScreen::load(core::files::PchtxtItem pchtxtItem) {
    auto& s_instance = getInstance();
    s_instance.m_curPchtxtItem = pchtxtItem;
    Application::setScreen(s_instance);
}

void PatchScreen::onLoad() {
    // parse pchtxt
    auto parseLogSs = std::stringstream{};
    auto pchtxtIstream = std::ifstream{m_curPchtxtItem.path};
    m_curPatchTextOutput = pchtxt::parsePchtxt(pchtxtIstream, parseLogSs);
    pchtxtIstream.close();

    // set log display
    auto parseLogStr = parseLogSs.str();
    auto parseLogTextBlock = new Aether::TextBlock(0, 0, parseLogStr, LOG_FONT_SIZE, LOG_W);
    if (parseLogStr.find("ERROR") != std::string::npos) {
        parseLogTextBlock->setColour(consts::COLOR_FAIL);
    }
    mp_logTextBlockWrap->removeAllElements();
    mp_logTextBlockWrap->addElement(parseLogTextBlock);

    // set patches display
    mp_patchList->removeAllElements();
    for (auto& patchCollection : m_curPatchTextOutput.collections) {
        mp_patchList->addElement(new Aether::ListHeading(patchCollection.buildId));
        for (auto& patch : patchCollection.patches) {
            mp_patchList->addElement(new elements::PatchToggle(patch));
        }
    }

    setFocused(mp_patchList);
}

void PatchScreen::applyIps() {
    auto ipsWriteLogSs = std::stringstream{};
    core::files::writeOutputToIps(m_curPatchTextOutput, m_curPchtxtItem.outPathName, ipsWriteLogSs);
    auto ipsWriteLogTextBlock = new Aether::TextBlock(0, 0, ipsWriteLogSs.str(), LOG_FONT_SIZE, LOG_W);
    ipsWriteLogTextBlock->setColour(consts::COLOR_SUCCESS);
    mp_logTextBlockWrap->addElement(ipsWriteLogTextBlock);
    mp_logTextBlockWrap->setScrollPos(mp_logTextBlockWrap->maxScrollPos());
}

}  // namespace ui
}  // namespace witch
