#ifndef __MARKDOWNTRANSFORM
#define __MARKDOWNTRANSFORM

#include <string>
#include <vector>
#include <map>
#include <utility>

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
        {"li", std::make_pair("<li>", "</li>")}
    };
    
    // markdown 的内容和目录
    std::string content, TOC; 
};



#endif __MARKDOWNTRANSFORM