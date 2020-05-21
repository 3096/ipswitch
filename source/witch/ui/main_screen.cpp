#include "main_screen.hpp"

#include "../app.hpp"
#include "../core/files.hpp"

namespace witch {
namespace ui {

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

    auto* controls = new Aether::Controls();
    controls->addItem(new Aether::ControlItem(Aether::Button::A, "OK"));
    controls->addItem(new Aether::ControlItem(Aether::Button::PLUS, "Exit"));
    addElement(controls);

    onButtonPress(Aether::Button::PLUS, Application::exit);

    setFocused(mp_pchtxtList);
}

MainScreen::~MainScreen() {}

void MainScreen::onLoad() {
    mp_pchtxtList->removeAllElements();
    for (auto& pchtxt : core::pchtxt::getItems()) {
        mp_pchtxtList->addElement(
            new Aether::ListButton(std::string{pchtxt.path}.substr(core::pchtxt::PCHTXT_DIR_LEN), nullptr));
    }
}

}  // namespace ui
}  // namespace witch
