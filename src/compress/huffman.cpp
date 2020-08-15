
#include <stdio.h>
#include <assert.h>
#include <queue>
#include <functional>
#include <algorithm>

#include "huffman.h"

using namespace std;

enum TYPE 
{
    LEAF,
    INTERNAL
};

struct Node
{
    virtual TYPE get_type() = 0;
    virtual int get_weight() const = 0;
    virtual ~Node() = default;
};

struct Leaf: Node
{
    int weight;
    uint8_t ch;
    Leaf(int wei, uint8_t ch): weight(wei), ch(ch) {}
    TYPE get_type() { return TYPE::LEAF; }
    int get_weight() const { return weight; }
};

struct Internal: Node
{
    int weight;
    Node *left, *right;
    Internal(int wei): weight(wei), left(nullptr), right(nullptr) {}
    Internal(int wei, Node* l, Node* r): weight(wei), left(l), right(r) {}
    TYPE get_type() { return TYPE::INTERNAL; }
    int get_weight() const { return weight; } 
};

struct NodeCmp
{
    bool operator()(Node* a, Node* b)
    {
        return a->get_weight() > b->get_weight();
    }
};

CompressData compress(string& s, vector<CodeTableItem>& ch2code)
{
    ch2code.clear();
    ch2code.resize(256, CodeTableItem{0, 0});

    priority_queue<Node*, vector<Node*>, NodeCmp> q;
    vector<int> counter(256, 0);
    CompressData compressed;
    Node* head = nullptr;

    for(int i = 0; i < s.size(); ++i) 
    {
        counter[static_cast<uint8_t>(s[i])]++;
    }

    for(int i = 0; i < 256; ++i) 
    {
        if(counter[i] == 0) continue;
        q.push(new Leaf(counter[i], i));
    }

    // 构建huffman树
    while(q.size() > 1)
    {
        Node* left = q.top();
        q.pop();
        Node* right = q.top();
        q.pop();
        assert(left != nullptr && right != nullptr);
        Node* inter = new Internal(left->get_weight() + right->get_weight(), left, right);

        q.push(inter);
    }
    head = q.top();
    assert(head != nullptr);

    printf("构建code table\n");
    // 构建code table
    function<void(Node*, uint8_t, uint8_t)> dfs = [&](Node* node, uint8_t depth, uint8_t code) 
    {
        Internal* inter = nullptr;
        Leaf* leaf = nullptr;

        switch (node->get_type())
        {
        case TYPE::INTERNAL:
            inter = dynamic_cast<Internal*>(node);
            dfs(inter->left, depth+1, code<<1);
            dfs(inter->right, depth+1, (code<<1)|0x1);
            break;

        case TYPE::LEAF:
            leaf = dynamic_cast<Leaf*>(node);
            ch2code[leaf->ch] = {code, depth};
            break;
            
        default:
            printf("error: 非法的节点类型\n");
            exit(1);
            break;
        }
    };
    dfs(head, 0, 0);

    printf("压缩数据\n");
    // 压缩数据
    uint8_t cur_byte = 0;  // 当前字节
    uint8_t cur_cap = 8;   // 当前剩余比特
    for(int i = 0; i < s.size(); ++i)
    {
        auto ch = static_cast<uint8_t>(s[i]);
        auto& item = ch2code[ch];
        auto minl = min(item.len, cur_cap);
        cur_byte <<= minl;
        uint8_t offset = item.len - minl;   // 偏移量(code第二部分长度)
        cur_byte |= (item.code >> offset);
        cur_cap -= minl;
        compressed.data_len += minl;
        if(cur_cap == 0)
        {
            compressed.data.push_back(cur_byte);
            cur_cap = 8 - offset;
            cur_byte = (item.code << cur_cap >> cur_cap);
            compressed.data_len += offset;
        }
    }
    // 处理末尾数据
    if(cur_cap > 0)
    {
        cur_byte <<= cur_cap;
        compressed.data.push_back(cur_byte);
    }

    return std::move(compressed);
}

string decompress(CompressData& compressed, vector<CodeTableItem>& ch2code)
{
    if(ch2code.size() != 256)
    {
        printf("code table 大小不满足256位\n");
        exit(1);
    }

    vector<uint8_t> code2ch(0x10000, 0xff);  // 2^16
    string ret;
    uint8_t byte = 0;
    uint8_t len = 0;
    uint8_t tmp_byte = 0;
    uint8_t cur_byte = 0;       // 解压数据的当前byte
    uint8_t cur_byte_offset;    // 当前byte待匹配数据的偏移量(到右侧的距离)
    CodeTableItem item;
    uint8_t ch;
    uint64_t counter = 0;
    bool flag;  // 标识当前cur_byte是否存在匹配

    for(uint16_t i = 0; i < 256; ++i)
    {
        if(ch2code[i].len == 0) continue;
        code2ch[*reinterpret_cast<uint16_t*>(&ch2code[i])] = (uint8_t)i;
    }

    for(uint64_t i = 0; i < compressed.data.size(); ++i) 
    {
        cur_byte = compressed.data[i];
        cur_byte_offset = 8;
        flag = false;

        while(cur_byte_offset != 0 && counter < compressed.data_len) 
        {
            cur_byte_offset--;
            counter++;

            tmp_byte = (byte <<= 1);
            tmp_byte |= (cur_byte >> cur_byte_offset);
            len++;
            item = {tmp_byte, len};
            ch = code2ch[*reinterpret_cast<uint16_t*>(&item)];
            if(ch != 0xff)
            {
                // 匹配成功
                flag = true;
                ret.push_back(ch);
                len = 0;
                byte = 0;
                (cur_byte <<= (8 - cur_byte_offset)) >>= (8 - cur_byte_offset);
            }
        }
        if(cur_byte != 0)
        {
            byte = cur_byte;
        }
        
        if(!flag)
        {
            // TODO: 2020年08月15日17:19:03 加入错误处理机制
            // 
            printf("error: 解压缩出错\n");
            exit(1);
        }
    }

    return std::move(ret);
}
