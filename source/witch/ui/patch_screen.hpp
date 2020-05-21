#include <memory>

#include "Aether.hpp"

namespace witch {
namespace ui {

class PatchScreen : public Aether::Screen {
   private:
    PatchScreen();
    PatchScreen(const PatchScreen&) = delete;
    ~PatchScreen();

    // styles

    // dementions

    // members
    std::string m_pchtxtPath;

    // lifecycle
    void onLoad() override;

   public:
    static inline auto& getInstance() {
        static PatchScreen s_instance;
        return s_instance;
    }

    static inline void setPatchLoad(std::string& pchtxtPath) { getInstance().m_pchtxtPath = pchtxtPath; }
};

}  // namespace ui
}  // namespace witch
