#ifndef __MARKDOWNTRANSFORM
#define __MARKDOWNTRANSFORM

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <new>

using string = std::string;
const int MAXLENGTH = 10000;

// mkdown 和 html具有类似的dom树结构，因此我们需要先实现一个树结构
struct Cnode {
    // 保存目录的结构
    std::vector<Cnode*> ch;
    string heading;
    string tag;

    Cnode(std::string_view hd): heading{hd} {}
};

struct node {
    // 保存正文内容的结构
    string type; // 节点代表的类型
    std::vector<node*> ch;
    string elem[3]; // 存放3个属性：1.显示的内容 2.保存链接 3.保存title

    node(std::string_view _type): type{_type} {};
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
    std::pair<int, char*> Start(char *src);

    // 行类型判断
    std::pair<string, char*> JudgeType(char *src);

    // 类型获取
    inline bool IsHeading(node *v) {return v->type == }

private:
    // Token 枚举 和 html 元素
    std::map<std::string, std::pair<std::string, std::string>> ht {
        {"nul", std::make_pair("", "")},
        {"p", std::make_pair("<p>", "</p>")},
        {"href", std::make_pair("", "")},
        {"ul", std::make_pair("<ul>", "</ul>")},
        {"ol", std::make_pair("<ol>", "</ol>")},
        {"li", std::make_pair("<li>", "</li>")},
        {"em", std::make_pair("<em>", "</em>")},
        {"strong", std::make_pair("<strong>", "</strong>")},
        {"hr", std::make_pair("<hr color=#CCCCCC size=1 />", "")},
        {"br", std::make_pair("<br />", "")},
        {"br", std::make_pair("<br />", "")},
        {"img", std::make_pair("", "")},
        {"quote", std::make_pair("<blockquote>", "</blockquote>")},
        {"h1", std::make_pair("<h1>", "</h1>")},
        {"h2", std::make_pair("<h2>", "</h2>")},
        {"h3", std::make_pair("<h3>", "</h3>")},
        {"h4", std::make_pair("<h4>", "</h4>")},
        {"h5", std::make_pair("<h5>", "</h5>")},
        {"h6", std::make_pair("<h6>", "</h6>")},
        {"blockcode", std::make_pair("<pre><code>", "</code>")},
        {"code", std::make_pair("<code>", "</code>")},
    };
    
    // markdown 的内容和目录
    std::string content, TOC; 

    // 
    node *root, *now;
    Cnode *root; 

    char s[MAXLENGTH]; // 缓存要处理的行

    int cntTag = 0; // 索引
};



#endif ___MARKDOWNTRANSFORM