#include "files.hpp"

namespace witch {
namespace core {
namespace pchtxt {

auto getItems() -> std::vector<Item> {
    auto result = std::vector<Item>();
    for (auto& outerDirEntry : std::filesystem::directory_iterator(PCHTXT_DIR)) {
        if (not outerDirEntry.is_directory()) continue;
        for (auto& innerDirEntry : std::filesystem::directory_iterator(outerDirEntry)) {
            if (!innerDirEntry.is_regular_file() or innerDirEntry.path().extension() != PCHTXT_EXTENSION) continue;
            result.push_back(Item{innerDirEntry.path(), "bruh", 0});  // TODO: get title and program id
        }
    }
    return result;
}

}  // namespace pchtxt
}  // namespace core
}  // namespace witch
