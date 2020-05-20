#include "main_screen.hpp"

namespace witch {
namespace ui {

MainScreen::MainScreen() {
    Aether::Text* titleText = new Aether::Text(50, 50, "Hello Aether!", 18);
    titleText->setColour(Aether::Colour{200, 250, 200, 255});
    addElement(titleText);
}

MainScreen::~MainScreen() {}

}  // namespace ui
}  // namespace witch
