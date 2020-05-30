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

    inline void updateColor() {
        setValueColour(m_patch.enabled ? Aether::Theme::Dark.accent : Aether::Theme::Dark.mutedText);
    }
    inline auto getToggleCb() {
        return [this]() {
            m_patch.enabled = !m_patch.enabled;
            setValue(getToggleStr(m_patch.enabled));
            updateColor();
        };
    }

   public:
    PatchToggle(pchtxt::Patch& patch);
    ~PatchToggle();
};

}  // namespace elements
}  // namespace ui
}  // namespace witch
