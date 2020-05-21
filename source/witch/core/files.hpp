#include <string>
#include <vector>

namespace witch {
namespace core {
namespace pchtxt {

constexpr auto PCHTXT_DIR = "sdmc:/switch/ipswitch/";

struct Item {
    std::string path;
    std::string title;
    uint64_t program;
};

auto getItems() -> std::vector<Item>;

}  // namespace pchtxt
}  // namespace core
}  // namespace witch
