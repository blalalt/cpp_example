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
        this->depth.push( new Container(type, layout) );
        this->Write() << (type == ContainerType::OBJECT ? '{' : '[');
    }

    void JsonWriter::endContainer() {
        
    }
}