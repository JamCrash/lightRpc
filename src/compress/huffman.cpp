
#include <queue>
#include <functional>
#include <algorithm>
#include <assert.h>

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
    //Internal(int wei): weight(wei), left(nullptr), right(nullptr) {}
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

struct CodeTableItem
{
    uint256_t code;  
    uint64_t len;
};

CompressData compress(string& s, string& serializedHuffman)
{
    vector<int> counter(256, 0);
    priority_queue<Node*, vector<Node*>, NodeCmp> q;
    vector<CodeTableItem> ch2code(256, {0, 0});
    Node* head = nullptr;
    CompressData compressed;

    for(int i = 0; i < s.size(); ++i)
    {
        counter[static_cast<uint8_t>(s[i])]++;
    }

    for(int i = 0; i < 256; ++i)
    {
        q.push(new Leaf(counter[i], i));
    }

    // 构建huffman树
    while(q.size() > 1)
    {
        Node* l = q.top();
        q.pop();
        Node* r = q.top();
        q.pop();
        Node* inter = new Internal(l->get_weight() + r->get_weight(), l, r);
        q.push(inter);
    }
    assert(q.size() == 1);
    head = q.top();

    // 构建code table并序列化huffman树
    function<string(Node*, uint8_t, uint256_t)> dfs = [&](Node* node, uint8_t depth, uint256_t code)
    {
        Internal* inter = nullptr;
        Leaf* leaf = nullptr;
        string ret;

        switch (node->get_type())
        {
        case TYPE::INTERNAL:
            inter = dynamic_cast<Internal*>(node);
            ret = 
                "(" + 
                dfs(inter->left, depth+1, code<<1) +
                "0)" + 
                "(" +
                dfs(inter->right, depth+1, (code<<1)|0x1) +
                "1)";
            break;

        case TYPE::LEAF:
            leaf = dynamic_cast<Leaf*>(node);
            ch2code[leaf->ch] = {code, depth};
            ret.push_back(leaf->ch);
            break;
            
        default:
            // TODO: 2020年08月15日20:40:44
            printf("error: 非法的节点类型\n");
            exit(1);
            break;
        }
        return ret;
    };
    serializedHuffman = dfs(head, 0, 0);

    // 压缩数据
    uint256_t cur_data;
    uint64_t cur_cap = 256;
    for(int i = 0; i < s.size(); ++i)
    {
        auto ch = static_cast<uint8_t>(s[i]);
        auto& item = ch2code[ch];
        uint64_t minl = min(cur_cap, item.len);
        uint64_t offset = item.len - minl;   // 偏移量(code第二部分长度)
        cur_data |= (item.code >> offset);
        cur_cap -= minl;
        compressed.data_len += minl;
        if(cur_cap == 0)
        {
            uint8_t* ptr = reinterpret_cast<uint8_t*>(item.code.get_raw_data());
            for(int j = 0; j < 256/8; ++j, ++ptr)
            {
                compressed.data.push_back(*ptr);
            }
            cur_cap = 256 - offset;
            cur_data = (item.code << cur_cap >> cur_cap);
            compressed.data_len += offset;
        }
    }
    // 处理末尾数据
    if(cur_cap > 0 && cur_cap < 256)
    {
        cur_data <<= cur_cap;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(cur_data.get_raw_data());
        int n = 1 + (256 - cur_cap) / 8;
        for(int i = 0; i < n; ++i, ++ptr)
        {
            compressed.data.push_back(*ptr);
        }
    }

    return std::move(compressed);
}

string decompress(CompressData& compressed, string& serializedHuffman)
{
    Node* head = nullptr;
    string& srlHuff = serializedHuffman;

    // 反序列化,构建huffman树
    for(int i = 0; i < srlHuff.size(); ++i)
    {
        if(srlHuff[i] == '(') continue;
        
    }
}
