
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <algorithm>
#include <utility>

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
};

struct Leaf: Node
{
    int weight;
    char ch;
    Leaf(int wei, char ch): weight(wei), ch(ch) {}
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

struct CompressData
{
    vector<uint8_t> data;
    uint64_t data_len = 0;
    CompressData(CompressData&& param): data(std::move(param.data)), data_len(param.data_len) {}
    CompressData& operator=(CompressData&& rhs)
    {
        if(this == &rhs) return *this;
        data = std::move(rhs.data);
        data_len = rhs.data_len;
        return *this;
    }
};

struct CodeTableItem
{
    uint8_t code;
    uint8_t len;
};

vector<CodeTableItem> code_table(256);

CompressData compress(string& s)
{
    priority_queue<Node*, vector<Node*>, NodeCmp> q;
    vector<int> counter(256, 0);
    CompressData compressed;
    Node* head;

    for(char ch: s)
    {
        counter[ch]++;
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
        Node* inter = new Internal(left->get_weight() + right->get_weight(), left, right);
        q.push(inter);
    }
    head = q.top();

    // 构建code table
    function<void(Node*, uint8_t, uint8_t)> dfs = [&](Node* node, uint8_t depth, uint8_t code) 
    {
        switch (node->get_type())
        {
        case TYPE::INTERNAL:
            Internal* inter = dynamic_cast<Internal*>(node);
            dfs(inter->left, depth+1, code<<1);
            dfs(inter->right, depth+1, (code<<1)|0x1);
            break;
        
        case TYPE::LEAF:
            Leaf* leaf = dynamic_cast<Leaf*>(node);
            code_table[leaf->ch] = {code, depth};
            break;
            
        default:
            assert(0);
            break;
        }
    };
    dfs(head, 0, 0);

    // 压缩数据
    uint8_t cur_byte = 0;  // 当前字节
    uint8_t cur_cap = 8;   // 当前剩余比特
    for(char ch: s)
    {
        auto& item = code_table[ch];
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
        compressed.data_len += (8 - cur_cap);
    }

    return std::move(compressed);
}
