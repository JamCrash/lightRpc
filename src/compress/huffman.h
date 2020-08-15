
#include <stdint.h>
#include <vector>
#include <utility>
#include <string>

struct CodeTableItem
{
    uint8_t code;
    uint8_t len;
};

struct CompressData
{
    std::vector<uint8_t> data;
    uint64_t data_len = 0;
    CompressData() = default;
    CompressData(CompressData&& param): data(std::move(param.data)), data_len(param.data_len) {}
    CompressData& operator=(CompressData&& rhs)
    {
        if(this == &rhs) return *this;
        data = std::move(rhs.data);
        data_len = rhs.data_len;
        return *this;
    }
};

CompressData compress(std::string& s, std::vector<CodeTableItem>& ch2code);
std::string decompress(CompressData& compressed, std::vector<CodeTableItem>& ch2code);
