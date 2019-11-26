#ifndef __MARKDOWNTRANSFORM
#define __MARKDOWNTRANSFORM

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <new>

// mkdown 和 html具有类似的dom树结构，因此我们需要先实现一个树结构
struct Cnode {
    // 保存目录的结构
    std::vector<Cnode>
};

struct node {
    // 保存正文内容的结构

};

class MarkDownTransform {
public:
    using ss_p = std::pair<std::string, std::string>;
    MarkDownTransform(const std::string &filename);

    std::string getTOC() {return TOC;}
    std::string getContent() {return content;}
    ~MarkDownTransform() {}

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
};



#endif ___MARKDOWNTRANSFORM