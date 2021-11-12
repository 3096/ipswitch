#pragma once

#include "Aether/Aether.hpp"

namespace consts {

auto constexpr SCREEN_H = 720;
auto constexpr SCREEN_W = 1280;
auto constexpr HEADER_BAR_H = 88;
auto constexpr CONTROL_BAR_H = 73;
auto constexpr GAP_SIZE = 12;

static auto const COLOR_SUCCESS = Aether::Colour{92, 184, 92, 0xFF};
static auto const COLOR_FAIL = Aether::Colour{217, 83, 79, 0xFF};

}  // namespace consts
