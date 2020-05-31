#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "../../libpchtxt/pchtxt.hpp"

namespace witch {
namespace core {
namespace files {

const auto PCHTXT_DIR = std::filesystem::path{"sdmc:/switch/ipswitch/"};
const auto PCHTXT_DIR_LEN = std::string{PCHTXT_DIR}.size();
const auto PCHTXT_EXTENSION = std::filesystem::path{".pchtxt"};
const auto IPS_EXTENSION = std::filesystem::path{".ips"};

const auto AMS_NSO_PATCH_DIR = std::filesystem::path{"sdmc:/atmosphere/exefs_patches"};
const auto AMS_NRO_PATCH_DIR = std::filesystem::path{"sdmc:/atmosphere/nro_patches"};

struct PchtxtItem {
    std::filesystem::path path;
    std::filesystem::path outPathName;
    std::string title;
    std::string program;
};

auto getItems() -> std::vector<PchtxtItem>;

void writeOutputToIps(pchtxt::PatchTextOutput& patchTextOutput, std::filesystem::path& pathName, std::ostream& logOs);

}  // namespace files
}  // namespace core
}  // namespace witch
