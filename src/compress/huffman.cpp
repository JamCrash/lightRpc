
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>

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
    Leaf(uint8_t ch): weight(0), ch(ch) {}
    Leaf(int wei, uint8_t ch): weight(wei), ch(ch) {}
    TYPE get_type() { return TYPE::LEAF; }
    int get_weight() const { return weight; }
};

struct Internal: Node
{
    int weight;
    Node *left, *right;
    Internal(Node* l, Node* r): weight(0), left(l), right(r) {}
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
    CodeTableItem(uint256_t& c, uint64_t l)
    : code(c), len(l)
    {}
    CodeTableItem()
    : code(0), len(0)
    {}
    CodeTableItem& operator=(CodeTableItem&&) = default;
    uint256_t code;  
    uint64_t len;
};

CompressData compress(string& s, string& serializedHuffman)
{
    vector<int> counter(256, 0);
    priority_queue<Node*, vector<Node*>, NodeCmp> q;
    vector<CodeTableItem> ch2code(256);
    Node* head = nullptr;
    CompressData compressed;

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
            ret = "(0)";
            ret[1] = leaf->ch;
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
        cur_data <<= minl;
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
        int n = (256 - cur_cap) / 8 + (cur_cap % 8 ? 1 : 0);
#ifdef T
        uint8_t* p = ptr;
        for(int i=0;i<256/8;++i) printf("%u ", *p++);
        printf("\n");
        // TODO: 大小端
#endif 
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
    stack<Node*> nodeStk;
    stack<bool> directStk;  // 标识子节点和父节点的方位关系,0为左子树,1为右子树
    bool isLeaf = false;
    string& srlHuff = serializedHuffman;
    string ret; 

    // 反序列化,构建huffman树
    auto get_one_node_from_stack = [&]() 
    {
        bool flag = directStk.top();
        directStk.pop();
        Node* node = nodeStk.top();
        nodeStk.pop();
        return make_pair(node, flag);
    };
    for(int i = 0; i < srlHuff.size(); ++i)
    {
        if(srlHuff[i] == '(') continue;
        if(srlHuff[i] == ')')
        {
            if(isLeaf)
            {
                isLeaf = false;
                directStk.push(srlHuff[i-1] == '1');
            }
            else
            {
                auto node1 = get_one_node_from_stack();
                auto node2 = get_one_node_from_stack();
                Node* l = (node1.second == 0 ? node1.first : node2.first);
                Node* r = (l == node1.first ? node2.first : node1.first);
                Node* father = new Internal(l, r);
                nodeStk.push(father);
                directStk.push(srlHuff[i-1] == '1');
            }
        }
        else
        {
            if(srlHuff[i-1] == '(' && srlHuff[i+1] == ')')
            {
                nodeStk.push(new Leaf(srlHuff[i]));
                isLeaf = true;
            }
        }
    }
    auto node1 = get_one_node_from_stack();
    auto node2 = get_one_node_from_stack();
    Node* l = (node1.second == 0 ? node1.first : node2.first);
    Node* r = (l == node1.first ? node2.first : node1.first);
    head = new Internal(l, r);

    // 解压缩
    uint64_t counter = 0;   // 标识当前处理的比特总数
    uint8_t cur_byte;
    uint8_t cur_bit_offset; // 标识当前byte待处理bit的偏移量(右起)
    uint8_t flag;           // 标识左右子树(0左子树, 非0右子树)
    Node* node = head;
    for(int i = 0; i < compressed.data.size() && counter < compressed.data_len; ++i)
    {
        cur_byte = compressed.data[i];
        cur_bit_offset = 7;

        while(cur_bit_offset >= 0 && counter < compressed.data_len)
        {
            Internal* inter = nullptr;
            Leaf* leaf = nullptr;
            flag = (cur_byte & (1 << cur_bit_offset));

            switch (node->get_type())
            {
            case TYPE::INTERNAL:
                inter = dynamic_cast<Internal*>(node);
                node = (flag == 0) ? inter->left : inter->right;
                break;
            
            case TYPE::LEAF:
                leaf = dynamic_cast<Leaf*>(node);
                ret.push_back(leaf->ch);
                node = head;
                break;

            default:
                // TODO: 2020年08月19日16:35:31
                printf("error: 非法的节点类型\n");
                exit(1);
                break;
            }
            cur_bit_offset--;
            counter++;
        }
    }

    return ret;
}
