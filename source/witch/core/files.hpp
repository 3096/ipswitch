#include <filesystem>
#include <string>
#include <vector>

namespace witch {
namespace core {
namespace pchtxt {

const auto PCHTXT_DIR = std::filesystem::path{"sdmc:/switch/ipswitch/"};
const auto PCHTXT_DIR_LEN = std::string{PCHTXT_DIR}.size();
const auto PCHTXT_EXTENSION = std::filesystem::path{".pchtxt"};

struct Item {
    std::filesystem::path path;
    std::string title;
    uint64_t program;
};

auto getItems() -> std::vector<Item>;

}  // namespace pchtxt
}  // namespace core
}  // namespace witch
