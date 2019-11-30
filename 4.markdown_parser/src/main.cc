#include <iostream>
#include <fstream>
#include "MarkdownTransform.hpp"

int main() {

    MarkDownTransform mt("../test_examples/test.md");
    auto table = mt.GetTOC();
    auto contents = mt.GetContent();

    std::string head = "<!DOCTYPE html><html><head>\
        <meta charset=\"utf-8\">\
        <title>Markdown</title>\
        <link rel=\"stylesheet\" href=\"github-markdown.css\">\
        </head><body><article class=\"markdown-body\">";
    std::string end = "</article></body></html>";

    // 将结果写入到文件
    std::ofstream out;
    out.open("../output/index.html");
    // 将转换后的内容构造到 HTML 的 <article></article> 标签内部
    out << head+table+contents+end;
    out.close();
    return 0;
}