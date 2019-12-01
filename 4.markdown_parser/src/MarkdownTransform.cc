#include <MarkdownTransform.hpp>

MarkDownTransform::MarkDownTransform(const std::string &filename) {
    std::cout << "start to parse document." << std::endl;
    // 初始化
    croot = {new Cnode("")};
    root = new node(Tag::nul);
    now = root;
    // 从文件流中读取文件
    std::fstream fin{filename};

    bool newpara{false}, inblock{false}; // 默认不是新的段落，默认不在代码块中
    string type{};
    string split(40, '*');
    while (!fin.eof()) {
        fin.getline(s, MAXLENGTH); // 每次处理一行

        std::cout << split << std::endl;
        std::cout << "text: " << string(s) << std::endl;

        // 不在代码块 且 需要换行的情况
        if (!inblock && IsCutline(s)) {
            type = "br";
            std::cout << "type: " << type << std::endl;
            now = root;
            now->ch.push_back(new node(Tag::hr));
            newpara = false;
            continue;
        }

        // 检查缩进
        auto ps = Start(s);
        // 不在代码块 且 无内容, 开始新的段落
        if (!inblock && !ps.second) {
            type = "new paragraph";
            std::cout << "type: " << type << std::endl;
            now = root;
            newpara = true;
            continue;
        }

        auto jt = JudgeType(ps.second);
        // 代码块
        if (jt.first == Tag::blockcode) {
            type = "blockcode";
            std::cout << "type: " << type << std::endl;
            inblock ? now->ch.push_back(new node(Tag::nul)) : now->ch.push_back(new node(Tag::blockcode));
            inblock = !inblock;
            continue;
        }
        if (inblock) {
            type = "in blockcode";
            std::cout << "type: " << type << std::endl;
            now->ch.back()->elem[0] += string(s) + "\n"; //如果在代码块中，直接添加内容到当前节点
            continue;
        }

        // 普通段落
        if (jt.first == Tag::p) {
            type = "paragraph";
            std::cout << "type: " << type << std::endl;
            if (now == root) {
                now = FindNode(ps.first);
                now->ch.push_back(new node(Tag::p));
                now = now->ch.back();
            }
            bool flag = false;

            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i: now->ch) {
                    if (i->type == Tag::nul) {
                        ptr = i;
                    }
                    if (ptr) { MakePara(ptr); }
                    flag = true;
                }
            }

            if (flag) {
                now->ch.push_back(new node(Tag::p));
                now = now->ch.back();
            }

            now->ch.push_back(new node(Tag::nul));
            InsertNode(now->ch.back(), string(jt.second));
            newpara = false;
            continue;
        }
        now = FindNode(ps.first);
        // 标题
        if ( h1 <= jt.first && h6 >= jt.first ) {
            type = "heading";
            now->ch.push_back(new node(jt.first));
            now->ch.back()->elem[0] = "tag" + std::to_string(++cntTag);
            // 插入节点
            InsertNode(now->ch.back(), string(jt.second));
            // 插入目录
            InsertCnode(croot, jt.first-h1+1, string(jt.second), cntTag);
        }

        // 无序列表
        if (jt.first == Tag::ul) {
            type = "ul";
            if (now->ch.empty() || now->ch.back()->type!=ul) {
                now->ch.push_back(new node(Tag::ul));
            }

            now = now->ch.back();
            bool flag = false;

            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i: now->ch) {
                    if (i->type == Tag::li) {
                        ptr = i;
                    }
                }
                if (!ptr) {MakePara(ptr);}
                flag = true;

            }
            now->ch.push_back(new node(Tag::li));
            now = now->ch.back();
            if (flag) {
                now->ch.push_back(new node(Tag::p));
                now = now->ch.back();
            }
            InsertNode(now, string(jt.second));
        }

        // 有序列表
        if (jt.first == Tag::ol) {
            type = "ol";
            if (now->ch.empty() || now->ch.back()->type!=ul) {
                now->ch.push_back(new node(Tag::ul));
            }

            now = now->ch.back();
            bool flag = false;

            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i: now->ch) {
                    if (i->type == Tag::li) {
                        ptr = i;
                    }
                }
                if (!ptr) {MakePara(ptr);}
                flag = true;

            }
            now->ch.push_back(new node(Tag::li));
            now = now->ch.back();
            if (flag) {
                now->ch.push_back(new node(Tag::p));
                now = now->ch.back();
            }
            InsertNode(now, string(jt.second));
        }

        // 引用
        if (jt.first == Tag::quote) {
            type = "quote";
            // 没有子节点 或者 最后一个子节点不是引用节点的话 新建一个引用节点
            if (now->ch.empty() || now->ch.back()->type!=Tag::quote) {
                now->ch.push_back(new node(Tag::quote));
            }
            now = now->ch.back();
            if (newpara || now->ch.empty()) {now->ch.push_back(new node(Tag::p));}
            InsertNode(now->ch.back(), string(jt.second));
        }
        std::cout << "type: " << type << std::endl;
        newpara = false;
    }
    std::cout << "end parse." << std::endl;
    fin.close();

    DFSNode(root); // 遍历DOM树，生成html文档
    TOC += "<ul>\n";
    for (size_t i=0; i<croot->ch.size(); i++) {
        DFSCNode(croot->ch.at(i), std::to_string(i+1) + ".");
    }
    TOC += "</ul>\n";
}

// 开始解析一行中开始的空格和 Tab
// src: 源串
// 返回值: 由空格数和有内容处的 char* 指针组成的 std::pair
std::pair<int, char *> MarkDownTransform::Start(char *src) {
    // 如果该行为空,直接返回
    if (strlen(src) == 0) {
        return std::make_pair(0, nullptr);
    }

    // 统计 空格和 TAB的个数，记录缩进
    int cntSpace{0}, cntTab{0};
    // 从头遍历，直到遇到非空格或TAB字符
    for (int i=0; src[i] !='\0'; i++) {
        if (src[i] == ' ') cntSpace ++;
        else if (src[i] == '\t') cntTab ++;
        else {
            // 如果内容前有空格和 Tab，那么将其统一按 Tab 的个数处理,
            // 其中, 一个 Tab = 四个空格
            return std::make_pair(cntTab + cntSpace/4, src+i);
        }
    }
    return std::make_pair(0, nullptr);
}

// 目录的展示方式是需要使用无需列表的形式被展示在 HTML 中
void MarkDownTransform::DFSCNode(Cnode *v, const string& index) {

    TOC += "<li>\n";
    TOC += "<a href=\"#" + v->id + "\">" + index + " " + v->heading + "</a>\n";
    size_t n = v->ch.size();
    if (n) {
        TOC += "<ul>\n";
        for (size_t i=0; i<n; i++) {
            DFSCNode(v->ch.at(i), index + "." + std::to_string(i+1)); // 2-> 2.1 2.2
        }
        TOC += "</ul>\n";
    }
    TOC += "</li>\n";
}

// 对正文内容进行深度遍历
void MarkDownTransform::DFSNode(node *v) {
    if (v->type == Tag::p && v->elem[0].empty() && v->ch.empty()) { return;}
    auto tag = ht.at(static_cast<int>(v->type));
    std::cout << v->type << tag.first << std::endl;
    content += tag.first;

    bool flag = true;

    // 处理标题, 支持用目录进行跳转
    if (IsHeading(v)) {
        content += "id=\"" + v->elem[0] + "\">";
        flag = false;
    }

    // 图片
    if (IsImage(v)) {
        content += "<img alt=\"" + v->elem[0] + "\" src=\"" + v->elem[1] + "\" title=\""+v->elem[2]+"\"/>";
        flag = false;
    }

    // 超链接
    if (IsHref(v)) {
        content += "<a href=\""+v->elem[1] +"\" title=\"" + v->elem[2] +"\">" + v->elem[0] + "</a>";
        flag = false;
    }

    // 以上三者都不是 则直接添加内容
    if (flag) {
        content += v->elem[0];
        flag = false;
    }

    // 递归遍历所有子节点
    for (auto &c: v->ch) {
        DFSNode(c);
    }

    // 拼接结束标签
    content += tag.second;
}

// 生成一个 段落节点 paragraph
void MarkDownTransform::MakePara(node *v) {
    if (v->ch.size() == 1u && v->ch.back()->type == Tag::p) { return;}
    if (v->type == Tag::p) { return;}
    if (v->type == Tag::nul) {
        v->type = Tag::p;
        return;
    }

    // 新建一个
    node *p = new node(Tag::p);
    p->ch = v->ch;
    v->ch.clear();
    v->ch.push_back(p);
}

bool MarkDownTransform::IsCutline(char *src) {
    char *ptr = src;
    int cntH {0}; // 记录分割符 出现的次数
    while (*ptr) {
        if (*ptr!='\t' && *ptr!=' ' && *ptr!='-') {
            return false;
        }
        if (*ptr == '-') {cntH++;}
        ptr ++;
    }
    // 如果出现 --- 则需要增加一个分割线, 这时需要换行
    return (cntH >= 3);
}

// 向v中添加 串（节点）
void MarkDownTransform::InsertNode(node *v, const string &src) {
    size_t n = v->ch.size();

    // 处理 两边包围的内容 如 code，strong等
    bool incode{false}, inem{false}, instrong{false}, inautolink{false};

    // 保存到新的节点
    v->ch.push_back(new node(Tag::nul));

    // 如何在一个段落中处理图片和超链接的情况。这就意味着我们需要一个字符一个字符的进行判断和处理。
    // 这也是我们为什么在处理解析代码的时候，都是使用 C 风格的字符串以及字符指针来进行处理，
    // 而不是简单的使用 std::string
    for (size_t i=0; i<n; i++) {
        char ch = src[i];

        // 转义字符
        if (ch == '\\') {
            ch = src[++i];
            v->ch.back()->elem[0] += std::string(1, ch);
            continue;
        }

        // 行内代码
        if (ch == '`' && !inautolink) {
            incode ? v->ch.push_back(new node(Tag::nul)) : v->ch.push_back(new node(Tag::code));
            incode = !incode;
            continue;
        }

        // 处理加粗
        if (ch == '*' && (i<n-1 && src[i+1]=='*') && !incode && !inautolink) {
            ++i;
            instrong ? v->ch.push_back(new node(Tag::nul)) : v->ch.push_back(new node(Tag::strong));
            instrong = !instrong;
            continue;
        }
        if (ch == '_'  && !incode && !inautolink) {
            inem ? v->ch.push_back(new node(Tag::nul)) : v->ch.push_back(new node(Tag::strong));
            inem  = !inem;
            continue;
        }

        // 处理图片,  ![alt 属性文本](图片地址 "可选标题")
        if (ch == '!' && (i<n-1 && src[i+1]=='[') && !incode && !inautolink && !inem && !instrong) {
            v->ch.push_back(new node(Tag::img));
            //  属性文本
            for (i+=2; i<n-1 && src[i]!=']'; i++) {
                v->ch.back()->elem[0] += std::string(1, src[i]);
            }
            i++; //src: " (" 或 "("

            // 链接
            for (i++; i<n-1 && src[i]!=')' && src[i]!=' '; i++) {
                v->ch.back()->elem[1] += std::string(1, src[i]);
            }
            if (src[i] != ')') { // == ' '
                // "可选标题"
                for (i++; i<n-1 && src[i]!=')'; i++) {
                    if (src[i] != '"') {
                        v->ch.back()->elem[2] += std::string(1, src[i]);
                    }
                }
            }
            v->ch.push_back(new node(Tag::nul));
            continue;
        }

        // 处理链接 [链接文字](链接网址 "标题")
        if (ch == '[' && !incode && !inautolink && !inem && !instrong) {
            v->ch.push_back(new node(Tag::href));
            //  属性文本
            for (i+=2; i<n-1 && src[i]!=']'; i++) {
                v->ch.back()->elem[0] += std::string(1, src[i]);
            }
            i++; //src: " (" 或 "("

            // 链接
            for (i++; i<n-1 && src[i]!=')' && src[i]!=' '; i++) {
                v->ch.back()->elem[1] += std::string(1, src[i]);
            }
            if (src[i] != ')') { // == ' '
                // "可选标题"
                for (i++; i<n-1 && src[i]!=')'; i++) {
                    if (src[i] != '"') {
                        v->ch.back()->elem[2] += std::string(1, src[i]);
                    }
                }
            }
            v->ch.push_back(new node(Tag::nul));
            continue;
        }

        // 否则只是普通文本
        v->ch.back()->elem[0] += string(1, ch);
        // if (inautolink) v->ch.back()->elem[1] += string(1, ch);
    }
    // 两个空格换行
    if (src.size() >= 2 && src.at(src.size()-1) == ' ' && src.at(src.size()-2) == ' ') {
        v->ch.push_back(new node(Tag::br));
    }
}

// 递归的插入h标签目录
// x代表 标题等级 即h1,h2,...
// hd是标题内容。Tag是用来指向该标题指向正文的 id（依次递增且唯一）
void MarkDownTransform::InsertCnode(Cnode *v, int x, std::string_view hd, int cntTag) {
    if (x == 1) {
        v->ch.push_back(new Cnode(hd));
        v->ch.back()->id = "tag" + std::to_string(cntTag);
        return;
    }
    size_t n = v->ch.size();

    // 如果没有上级目录，比如 h1 后是 h3
    if (!n || v->ch.back()->heading.empty()) {
        v->ch.push_back(new Cnode(""));
    }
    // 递归构建
    InsertCnode(v->ch.back(), x-1, hd, cntTag);
}

// 给定树的深度寻找节点
// depth: 树的深度
// 返回值: 找到的节点指针
node* MarkDownTransform::FindNode(int depth) {
    node* ptr = root;
    while (!(ptr->ch.empty()) && (depth!=0)) {
        ptr = ptr->ch.back();
        if (ptr->type == Tag::li) {
            depth --;
        }
    }
    return ptr;
}

// 判断当前行的类型
// src: 源串
// 返回值: 当前行的类型和除去行标志性关键字的正是内容的 char* 指针组成的 std::pair
std::pair<Tag, char *> MarkDownTransform::JudgeType(char *src) {
    char *ptr = src;

    // 判断是不是标题，跳过#
    while (*ptr == '#') ptr++;
    // 如果出现空格，则说明是 h标签
    if (ptr != src && (*ptr==' ')) {
        Tag t = static_cast<Tag> (static_cast<int>(Tag::h1) + ptr - src - 1);
        return std::make_pair(t, ptr+1);
    }
    // ---------------------
    ptr = src; // 重置位置

     // ``` 代表代码块
     if (strncmp(ptr, "```", 3) == 0) {
         return std::make_pair(Tag::blockcode, ptr+3);
     }

     // * + - 且下一个字符为空格，则说明是 无序列表
     if ((strncmp(ptr, "- ", 2)==0) or (strncmp(ptr, "+ ", 2)==0) or (strncmp(ptr, "* ", 2)==0)) {
         return std::make_pair(Tag::ul, ptr+2);
     }

     // 引用
     if ((*ptr == '>') and (ptr[1] == ' ')) {
         return std::make_pair(Tag::quote, ptr+1);
     }

     // 有序列表, 数字+.
    while (*ptr and (isdigit(*ptr))) ptr++;
    if ((ptr!=src) and (*ptr=='.') and (ptr[1]==' ')) {
        return std::make_pair(Tag::ol, ptr+1);
    }

    //普通段落
    return std::make_pair(Tag::p, src);
}

//  Cnode 和 node 的两个指针 vector
template<typename T>
void MarkDownTransform::destory(T *v) {
    for (size_t i=0; i<v->ch.size(); i++) {
        destory(v->ch[i]); // 递归的 释放节点
    }
    delete v;
}


