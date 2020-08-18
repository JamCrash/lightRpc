
#include <stdint.h>
#include <string.h>
#include <iostream>

using namespace std;

#define uint256_t uint256_t

struct uint256_t
{
    friend bool operator==(uint256_t& lhs, uint256_t& rhs);
    friend uint256_t operator<<(uint256_t src, uint64_t offset);
    friend uint256_t& operator<<=(uint256_t& src, uint64_t offset);
    friend uint256_t operator>>(uint256_t src, uint64_t offset);
    friend uint256_t& operator>>=(uint256_t& src, uint64_t offset);
    friend uint256_t operator|(uint256_t src, uint256_t val);
    friend uint256_t operator&(uint256_t src, uint256_t val);
    friend uint256_t& operator|=(uint256_t& src, uint256_t val);
    friend ostream& operator<<(ostream& out, uint256_t& src);

public:
    uint256_t();
    uint256_t(uint256_t& src);
    uint256_t(uint256_t&& src);
    ~uint256_t() = default;
    uint256_t(uint64_t a, uint64_t b, uint64_t c, uint64_t d)
    : data{a, b, c, d} {}
    uint256_t(uint64_t a)
    : data{0, 0, 0, a} {}

public:
    uint256_t& operator=(uint256_t& rhs);
    uint256_t& operator=(uint256_t&& rhs);

public:
    void* get_raw_data() { return reinterpret_cast<void*>(data); }
    
private:
    uint64_t data[4];
};

uint256_t::uint256_t()
: data{0, 0, 0, 0} 
{}

uint256_t::uint256_t(uint256_t& src)
{
    memcpy(&data, &src.data, 4*sizeof(uint64_t));
}

uint256_t::uint256_t(uint256_t&& src)
{
    memcpy(&data, &src.data, 4*sizeof(uint64_t));
}

uint256_t& uint256_t::operator=(uint256_t& rhs)
{
    memcpy(&data, &rhs.data, 4*sizeof(uint64_t));
    return *this;
}

uint256_t& uint256_t::operator=(uint256_t&& rhs)
{
    memcpy(&data, &rhs.data, 4*sizeof(uint64_t));
    return *this;
}

bool operator==(uint256_t& lhs, uint256_t& rhs)
{
    return memcmp(&rhs.data, &lhs.data, 4*sizeof(uint64_t)) == 0;
}

uint256_t operator<<(uint256_t src, uint64_t offset)
{
    uint256_t ret;
    auto& data = src.data;
    offset %= 257;
    int64_t new_idx;
    for(int i=3;i>=0;--i)
    {
        new_idx = i*64 - offset;
        if(new_idx >= 0)
        {
            ret.data[new_idx / 64] |= (data[i] >> (64 - offset % 64)); 
        }
        new_idx += 63;
        if(new_idx >= 0)
        {
            ret.data[new_idx / 64] |= (data[i] << (offset % 64));
        }
    }
    return ret;
}

uint256_t& operator<<=(uint256_t& src, uint64_t offset)
{
    uint256_t new_data = src << offset;
    memcpy(&src.data, &new_data.data, 4*sizeof(uint64_t));
    return src;
}

uint256_t operator>>(uint256_t src, uint64_t offset)
{
    uint256_t ret;
    auto& data = src.data;
    offset %= 257;
    int64_t new_idx;
    for(int i=0;i<=3;++i)
    {
        new_idx = i*64 + offset;
        if(new_idx <= 256)
        {
            ret.data[new_idx / 64] |= (data[i] >> (offset % 64));
        }
        new_idx += 63;
        if(new_idx <= 256)
        {
            ret.data[new_idx / 64] |= (data[i] << (64 - offset % 64));
        }
    }
    return ret;
}

uint256_t& operator>>=(uint256_t& src, uint64_t offset)
{
    uint256_t new_data = src >> offset;
    memcpy(&src.data, &new_data.data, 4*sizeof(uint64_t));
    return src;
}

uint256_t operator|(uint256_t src, uint256_t val)
{
    for(int i = 0; i < 4; ++i)
    {
        src.data[i] |= val.data[i];
    }
    return src;
}

uint256_t operator&(uint256_t src, uint256_t val)
{
    for(int i = 0; i < 4; ++i)
    {
        src.data[i] &= val.data[i];
    }
    return src;
}

uint256_t& operator|=(uint256_t& src, uint256_t val)
{
    for(int i = 0; i < 4; ++i)
    {
        src.data[i] |= val.data[i];
    }
    return src;
}

ostream& operator<<(ostream& out, uint256_t& src)
{
    for(int i=0;i<4;++i)
    {
        out << bitset<64>(src.data[i]);
    }
    return out;
}
