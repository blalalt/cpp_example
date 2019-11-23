#include "JsonWriter.hpp"

namespace jw {
    
    // 按照栈的深度缩进
    void JsonWriter::Indent() {
        for (int i=0, s=this->initialIndentDepth + this->depth.size(); i<s; i++) {
            this->Write() << this->indent;
        }
    }

    // 元素在开始和结束时需要都函数来操作，分别为 StartContainer 和 EndContainer , 
    // Container 是指对象或者数组，如果是数组就用 '[]' ,对象用‘{}’
    // 开始一个container
    void JsonWriter::startContainer(ContainerType type, ContainerLayout layout) {
        if (this->forceDefaultLayout) {
            layout = this->defaultLayout;
        } else if (layout == ContainerLayout::INHERIT)
        {
            if (this->depth.size() > 0) {
                layout = this->depth.top()->layout;
            }else {
                layout = this->defaultLayout;
            }
        }
        
        this->startChild();
        this->depth.push( new Container(type, layout) ); // 写
        this->Write() << (type == ContainerType::OBJECT ? '{' : '['); // 输出
    }

    // 结束一个container
    void JsonWriter::endContainer() {
        auto* container = depth.top();
        depth.pop();

        if (container->childCount > 0) {
            // 如果还有子节点

            if (container->layout == ContainerLayout::MULTI_LINE) {
                // 多行格式
                Write() << std::endl;
                Indent();
            } else {
                // 单行
                Write() << containerPadding;
            }
        }

        // 输出结尾符号
        Write() << (container->type == ContainerType::ARRAY ? "]" : "}");
    }

    // 新建一个节点
    void JsonWriter::startChild(bool isKey) {
        // 如果一个节点的 depth 大小为0可以认为它是根结点，根节点只用考虑缩进就好了。
        if (depth.size() == 0) {
            if (initialIndentDepth > 0) { 
                Indent();
            }
            return ;
        }

        // 如果是子节点，就要分析元素的类型了。如果是数组或者对象中的值，
        // 那么在继续写的时候需要添加 ‘，’，因为数据与数据间用这个符号隔开
        auto* container = depth.top();
        

        // 如果已经存在了子节点, 并且 子节点 是 数组或者对象中的值
        if ((container->childCount > 0) && ((container->type == ContainerType::ARRAY) || 
                (container->type == ContainerType::OBJECT && !container->iskey))) {

             Write() << ',';
             if (container->layout == ContainerLayout::MULTI_LINE) {
                 Write() << std::endl;
                 Indent();
             } else {
                 Write() << containerPadding;
             }
        } else if (container->childCount == 0) {
            // 目前还没有 子节点
            Write() << containerPadding;
            if (container->layout == ContainerLayout::MULTI_LINE) {
                Write() << std::endl;
                Indent();
            }
        }

        container->iskey = isKey;
        container->childCount ++;
    }

    // 添加字符串，在输出的时候默认没有双引号，这不符合标准，所以对于字符串需要手动添加符号，并且需要考虑转义字符：
    void JsonWriter::writeString(const char* str) {
        Write() << "\"";
        for (int i=0; str[i]!=0; i++) {
            writeChar(str[i]);
        }
        Write() << "\"";
    }

    void JsonWriter::writeChar(const char c) {
        switch (c) {
            case '"': Write () << "\\\""; break;
            case '\\': Write () << "\\\\"; break;
            case '\b': Write () << "\\b"; break;
            case '\f': Write () << "\\f"; break;
            case '\n': Write () << "\\n"; break;
            case '\r': Write () << "\\r"; break;
            case '\t': Write () << "\\t"; break;
            default: Write () << c; break;
        }
    }

    //阻止不必要的模板特化
    template <>
    void JsonWriter::Value<const char *>(const char* value) {
        startChild();
        this->writeString(value);
    }
    template <>
    void JsonWriter::Value<std::string>(std::string value) {
        startChild();
        this->writeString(value.c_str());
    }
    template <>
    void JsonWriter::Value<bool>(bool value) {
        startChild();
        this->Write() << value ? "true" : "false";
    }
}