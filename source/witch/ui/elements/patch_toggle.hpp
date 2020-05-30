#pragma once

#include "../../../libpchtxt/pchtxt.hpp"
#include "Aether.hpp"

namespace witch {
namespace ui {
namespace elements {

class PatchToggle : public Aether::ListOption {
   private:
    pchtxt::Patch& m_patch;

    // helper
    static inline auto getToggleStr(bool isOn) { return isOn ? "On" : "Off"; }
    inline auto getToggleCb() {
        return [this]() {
            m_patch.enabled = !m_patch.enabled;
            setValue(getToggleStr(m_patch.enabled));
        };
    }

   public:
    PatchToggle(pchtxt::Patch& patch);
    ~PatchToggle();
};

}  // namespace elements
}  // namespace ui
}  // namespace witch
