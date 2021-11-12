#include "main_screen.hpp"

#include "../app.hpp"
#include "../core/files.hpp"
#include "patch_screen.hpp"

namespace witch {
namespace ui {

const Aether::Colour MainScreen::SUB_TITLE_COLOR = {0xFF, 0x71, 0xE7, 0xFF};

MainScreen::MainScreen() {
    auto* titleText = new Aether::Text(TITLE_X, TITLE_Y, "IPSwitch", TITLE_SIZE);
    titleText->setColour(Aether::Theme::Dark.text);
    addElement(titleText);

    auto* subTitleText =
        new Aether::Text(TITLE_X + consts::GAP_SIZE + titleText->w(), 0, "Do no evil.", SUB_TITLE_SIZE);
    subTitleText->setY(TITLE_Y + titleText->h() - subTitleText->h() - SUB_TITLE_RAISE);
    subTitleText->setColour(SUB_TITLE_COLOR);
    addElement(subTitleText);

    mp_pchtxtList = new Aether::List(LIST_X, LIST_Y, LIST_W, LIST_H);
    addElement(mp_pchtxtList);

    auto* controlBar = new Aether::ControlBar();
    controlBar->addControl(Aether::Button::A, "OK");
    controlBar->addControl(Aether::Button::X, "Refresh");
    onButtonPress(Aether::Button::X, [this]() { this->refreshFileList(); });
    controlBar->addControl(Aether::Button::PLUS, "Exit");
    onButtonPress(Aether::Button::PLUS, Application::exitApp);
    addElement(controlBar);

    refreshFileList();
    setFocused(mp_pchtxtList);
}

MainScreen::~MainScreen() {}

void MainScreen::refreshFileList() {
    mp_pchtxtList->removeAllElements();
    for (auto& pchtxtItem : core::files::getItems()) {
        // TODO: better list element with title and program id displayed
        mp_pchtxtList->addElement(
            new Aether::ListButton(std::string{pchtxtItem.path}.substr(core::files::PCHTXT_DIR_LEN),
                                   [pchtxtItem]() { PatchScreen::load(pchtxtItem); }));
    }
    // TODO: refresh cutie pics too
}

}  // namespace ui
}  // namespace witch
