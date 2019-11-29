#include <MarkdownTransform.hpp>

MarkDownTransform::MarkDownTransform(const std::string &filename) {
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

void MarkDownTransform::DFSCNode(node *v) {

}

void MarkDownTransform::DFSNode(node *v) {

}

void MarkDownTransform::MakePara(node *v) {

}

bool MarkDownTransform::IsCutline(char *src) {
    return false;
}

void MarkDownTransform::InsertNode(node *v, const string &src) {

}

void MarkDownTransform::InsertCnode(Cnode *v, int x, std::string_view hd, Tag tag) {

}

// 给定树的深度寻找节点
// depth: 树的深度
// 返回值: 找到的节点指针
node* MarkDownTransform::FindNode(int depth) {
    return nullptr;
}

// 判断当前行的类型
// src: 源串
// 返回值: 当前行的类型和除去行标志性关键字的正是内容的 char* 指针组成的 std::pair
std::pair<Tag, char *> MarkDownTransform::JudgeType(char *src) {
    char *ptr = src;

    // 判断是不是标题，跳过#
    while (*ptr != '#') ptr++;
    // 如果出现空格，则说明是 h标签
    if (ptr-src>0 && *ptr==' ') {
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

template<typename T>
void MarkDownTransform::destory(T *v) {

}


