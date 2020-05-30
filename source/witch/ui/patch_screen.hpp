#include <filesystem>
#include <sstream>

#include "../../libpchtxt/pchtxt.hpp"
#include "Aether.hpp"
#include "consts.hpp"

namespace witch {
namespace ui {

class PatchScreen : public Aether::Screen {
   private:
    PatchScreen();
    PatchScreen(const PatchScreen&) = delete;
    ~PatchScreen();
    static inline auto& getInstance() {
        static PatchScreen s_instance;
        return s_instance;
    }

    // styles

    // dementions
    static constexpr auto LIST_X = 90;
    static constexpr auto LIST_Y = 160;
    static constexpr auto LIST_W = 720;
    static constexpr auto LIST_H = consts::SCREEN_H - LIST_Y - consts::CONTROL_BAR_H;

    // // ui members
    Aether::List* mp_patchList;

    // // members
    std::filesystem::path m_pchtxtPath;
    pchtxt::PatchTextOutput m_curPatchTextOutput;
    std::stringstream m_parseLogSs;

    // lifecycle
    void onLoad() override;

   public:
    static inline void init() {
        getInstance();  // construct the object, but don't return it. Done for load time purpose
    }
    static void load(std::filesystem::path pchtxtPath);
};

}  // namespace ui
}  // namespace witch
