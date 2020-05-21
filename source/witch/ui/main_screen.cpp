#include "main_screen.hpp"

#include "../app.hpp"
#include "../core/files.hpp"

namespace witch {
namespace ui {

MainScreen::MainScreen() {
    auto* titleText = new Aether::Text(TITLE_X, TITLE_Y, "IPSwitch", TITLE_SIZE);
    titleText->setColour(Aether::Theme::Dark.text);
    addElement(titleText);

    auto* subTitleText = new Aether::Text(TITLE_X + GAP_SIZE + titleText->w(), 0, "Do no evil.", SUB_TITLE_SIZE);
    subTitleText->setY(TITLE_Y + titleText->h() - subTitleText->h() - SUB_TITLE_RAISE);
    subTitleText->setColour(SUB_TITLE_COLOR);
    addElement(subTitleText);

    auto* controls = new Aether::Controls();
    controls->addItem(new Aether::ControlItem(Aether::Button::A, "OK"));
    controls->addItem(new Aether::ControlItem(Aether::Button::PLUS, "Exit"));
    addElement(controls);

    onButtonPress(Aether::Button::PLUS, Application::exit);
}

MainScreen::~MainScreen() {}

void MainScreen::onLoad() {}

}  // namespace ui
}  // namespace witch
