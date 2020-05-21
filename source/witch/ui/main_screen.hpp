#include "Aether.hpp"

namespace witch {
namespace ui {

class MainScreen : public Aether::Screen {
   private:
    MainScreen();
    MainScreen(const MainScreen&) = delete;
    ~MainScreen();

    // styles
    static constexpr auto SUB_TITLE_COLOR = Aether::Colour{0xFF, 0x71, 0xE7, 0xFF};

    // dementions
    static constexpr auto GAP_SIZE = 12;
    static constexpr auto TITLE_X = 50;
    static constexpr auto TITLE_Y = 50;
    static constexpr auto TITLE_SIZE = 72;

    static constexpr auto SUB_TITLE_RAISE = 3;
    static constexpr auto SUB_TITLE_SIZE = 31;

    // members

    // lifecycle
    void onLoad() override;

   public:
    static inline auto& getInstance() {
        static MainScreen s_instance;
        return s_instance;
    }
};

}  // namespace ui
}  // namespace witch
