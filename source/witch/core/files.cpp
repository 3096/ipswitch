#include "files.hpp"

#include <fstream>

namespace witch {
namespace core {
namespace files {

auto getItems() -> std::vector<PchtxtItem> {
    auto result = std::vector<PchtxtItem>();
    for (auto& outerDirEntry : std::filesystem::directory_iterator(PCHTXT_DIR)) {
        if (outerDirEntry.is_directory()) {
            for (auto& innerDirEntry : std::filesystem::directory_iterator(outerDirEntry)) {
                if (not innerDirEntry.is_regular_file() or innerDirEntry.path().extension() != PCHTXT_EXTENSION)
                    continue;
                auto pchtxtIstream = std::ifstream{innerDirEntry.path()};
                auto pchtxtMeta = pchtxt::getPchtxtMeta(pchtxtIstream);
                auto outPathName = std::filesystem::path{innerDirEntry.path().parent_path().stem()};
                outPathName += "-";
                outPathName += innerDirEntry.path().stem();
                result.push_back(
                    PchtxtItem{innerDirEntry.path(), outPathName, pchtxtMeta.title, pchtxtMeta.programId});
            }
        } else if (outerDirEntry.is_regular_file() and outerDirEntry.path().extension() == PCHTXT_EXTENSION) {
            auto pchtxtIstream = std::ifstream{outerDirEntry.path()};
            auto pchtxtMeta = pchtxt::getPchtxtMeta(pchtxtIstream);
            result.push_back(
                PchtxtItem{outerDirEntry.path(), outerDirEntry.path().stem(), pchtxtMeta.title, pchtxtMeta.programId});
        }
    }
    return result;
}

void writeOutputToIps(pchtxt::PatchTextOutput& patchTextOutput, std::filesystem::path& pathName, std::ostream& logOs) {
    for (auto& patchCollection : patchTextOutput.collections) {
        auto outPath = std::filesystem::path{};
        if (patchCollection.targetType == pchtxt::NRO) {
            outPath = AMS_NRO_PATCH_DIR;
        } else {
            outPath = AMS_NSO_PATCH_DIR;
        }
        std::filesystem::create_directory(outPath);
        outPath /= pathName;
        std::filesystem::create_directory(outPath);
        outPath /= patchCollection.buildId;
        outPath += IPS_EXTENSION;
        auto outIpsFstream = std::ofstream{outPath, std::ios::out | std::ios::binary};
        pchtxt::writeIps(patchCollection, outIpsFstream);
        outIpsFstream.close();
        logOs << "Written: " << outPath << std::endl;
    }
}

}  // namespace files
}  // namespace core
}  // namespace witch
