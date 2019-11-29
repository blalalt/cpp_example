#ifndef __MARKDOWNTRANSFORM
#define __MARKDOWNTRANSFORM

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <new>
#include <cstring>

using string = std::string;
const int MAXLENGTH = 10000;

enum Tag{
        nul = 0,
        p, href, ul, ol, li, em, strong, hr, br, img, quote, h1, h2, h3, h4, h5, h6, blockcode, code
};

// mkdown 和 html具有类似的dom树结构，因此我们需要先实现一个树结构
struct Cnode {
    // 保存目录的结构
    std::vector<Cnode*> ch;
    string heading;
    Tag tag;

    Cnode(std::string_view hd): heading{hd} {}
};

struct node {
    // 保存正文内容的结构
    Tag type; // 节点代表的类型
    std::vector<node*> ch;
    string elem[3]; // 存放3个属性：1.显示的内容 2.保存链接 3.保存title

    node(Tag _type): type{_type} {};
};

class MarkDownTransform {
public:
    using ss_p = std::pair<std::string, std::string>;
    MarkDownTransform(const std::string &filename);

    std::string GetTOC() {return TOC;}
    std::string GetContent() {return content;}
    ~MarkDownTransform() {}

private: 
    // 内部函数

    // 处理每行开始处的空格和TAB
    static std::pair<int, char*> Start(char *src);

    // 行类型判断
    static std::pair<Tag, char*> JudgeType(char *src);

    //  给定树的深度查找节点, 列表元素内 缩进
    static node* FindNode(int depth);

    // 插入元素
    // 插入目录
    static void InsertCnode(Cnode *v, int x, std::string_view hd, Tag tag);
    // 插入节点
    static void InsertNode(node *v, const string &src);

    // 换行的处理
    static bool IsCutline(char *src);
    // 段落的生成
    static void MakePara(node *v);

    // 对树进行遍历生成正文内容
    static void DFSNode(node *v);

    // 对目录进行遍历生成目录内容
    static void DFSCNode(node *v);

    // 类型获取
    static inline bool IsHeading(node *v) {
        return v->type >= Tag::h1 && v->type <= Tag::h6;
    }
    static inline bool IsImage(node *v) {
        return v->type == Tag::img;
    }
    static inline bool IsHref(node *v) {
        return v->type == Tag::href;
    }

    // 递归删除树节点
    template <typename T>
    void destory(T *v);

private:
    // Token 枚举 和 html 元素
    std::vector<std::pair<string, string>> ht {
        std::make_pair("", ""),
        std::make_pair("<p>", "</p>"),
        std::make_pair("", ""),
        std::make_pair("<ul>", "</ul>"),
        std::make_pair("<ol>", "</ol>"),
        std::make_pair("<li>", "</li>"),
        std::make_pair("<em>", "</em>"),
        std::make_pair("<strong>", "</strong>"),
        std::make_pair("<hr color=#CCCCCC  size=1 />", ""),
        std::make_pair("<br />", ""),
        std::make_pair("<br />", ""),
        std::make_pair("", ""),
        std::make_pair("<blockquote>", "</blockquote>"),
        std::make_pair("<h1>", "</h1>"),
        std::make_pair("<h2>", "</h2>"),
        std::make_pair("<h3>", "</h3>"),
        std::make_pair("<h4>", "</h4>"),
        std::make_pair("<h5>", "</h5>"),
        std::make_pair("<h6>", "</h6>"),
        std::make_pair("<pre><code>", "</code>"),
        std::make_pair("<code>", "</code>")
    };
    // std::map<std::string, std::pair<std::string, std::string>> ht {
    //     {"nul", std::make_pair("", "")},
    //     {"p", std::make_pair("<p>", "</p>")},
    //     {"href", std::make_pair("", "")},
    //     {"ul", std::make_pair("<ul>", "</ul>")},
    //     {"ol", std::make_pair("<ol>", "</ol>")},
    //     {"li", std::make_pair("<li>", "</li>")},
    //     {"em", std::make_pair("<em>", "</em>")},
    //     {"strong", std::make_pair("<strong>", "</strong>")},
    //     {"hr", std::make_pair("<hr color=#CCCCCC size=1 />", "")},
    //     {"br", std::make_pair("<br />", "")},
    //     {"br", std::make_pair("<br />", "")},
    //     {"img", std::make_pair("", "")},
    //     {"quote", std::make_pair("<blockquote>", "</blockquote>")},
    //     {"h1", std::make_pair("<h1>", "</h1>")},
    //     {"h2", std::make_pair("<h2>", "</h2>")},
    //     {"h3", std::make_pair("<h3>", "</h3>")},
    //     {"h4", std::make_pair("<h4>", "</h4>")},
    //     {"h5", std::make_pair("<h5>", "</h5>")},
    //     {"h6", std::make_pair("<h6>", "</h6>")},
    //     {"blockcode", std::make_pair("<pre><code>", "</code>")},
    //     {"code", std::make_pair("<code>", "</code>")},
    // };
    
    // markdown 的内容和目录
    std::string content, TOC; 

    // 
    node *root, *now; // 内容
    Cnode *croot; // 目录

    char s[MAXLENGTH]; // 缓存要处理的行

    // 为了让目录能够正确的索引到 HTML 的内容位置，我们不妨引入一个 cntTag 的变量进行记录
    int cntTag = 0; // 索引
};



#endif __MARKDOWNTRANSFORM