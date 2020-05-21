#include "files.hpp"

namespace witch {
namespace core {
namespace pchtxt {

auto getItems() -> std::vector<Item> {
    auto result = std::vector<Item>();
    result.push_back(Item{"testpath", "testtitle", 0});
    return result;
}

}  // namespace pchtxt
}  // namespace core
}  // namespace witch
