#include <iostream>
#include <stack>


namespace jw {

    // 考虑到 json 主要就两种类型， array 和 object ，所以写一个枚举类型：
    enum class ContainerType {ARRAY, OBJECT};

    // json 的布局主要分为3种：继承父类的格式，单行元素，多行元素：
    enum class ContainerLayout {INHERIT, MULTI_LINE, SINGLE_LINE};

    // json中的结构
    struct Container {
        ContainerType type;
        ContainerLayout layout;
        
        // isKey 用来判断元素是键还是值，键和值的布局，符号有所不同，所以需要分开看。
        bool iskey{false};
        int childCount{0};

        Container(ContainerType tp, ContainerLayout lo)
            : type{tp}, layout{lo}
        {}
    };


    class JsonWriter {
    public:
        // 本程序是将 json 对象输出到屏幕上或者文件中，
        // 所以我们重载了 write 函数，在没有指定文件时，输出到标准屏幕上，当有输出文件时输出到文件中：
        std::ostream& Write() {
            if (this->writer == nullptr) {
                return std::cout;
            }
            return *this->writer;
        }

        // 缩进相关函数
        int getInitialIndentDepth() const {return this->initialIndentDepth;}
        void setInitialIndentDepth(int depth) {this->initialIndentDepth = depth;}

        const char* getIndent() const {return this->indent;}
        void setIndent(const char* in) {this->indent=in;}

        // 布局相关函数, 布局其实就是左右的空格控制
        const char* getContainerPadding() const {return this->containerPadding;}
        void setContainerPadding(const char* pad) {this->containerPadding=pad;}

        const char* getkeyPaddingLeft() const {return this->keyPaddingLeft;}
        void setKeyPaddingLeft(const char* pad) {this->keyPaddingLeft=pad;}

        const char* getKeyPaddingRight() const {return this->keyPaddingRight;}
        void setKeyPaddingRight(const char* pad) {this->keyPaddingRight=pad;}

        // 压缩输出，减少空格和换行
        void configCompressedOut();

        // 输出函数
        void writerChar(const char c);
        void writerString(const char* str);
        void startChild(bool iskey=false);
        void startContainer(ContainerType type, ContainerLayout layout);
        void endContainer();

        
        void Key(const char* key);
        void NullValue();
        void KeyNullValue(const char* key) {Key(key); NullValue();}

        void startArray(ContainerLayout layout) {this->startContainer(ContainerType::ARRAY, layout);};
        void endArray() {this->endContainer();}
        void startObject(ContainerLayout layout) {this->startContainer(ContainerType::OBJECT, layout);}
        void endObject() {this->endContainer();}

        template<typename T>
        void Value(T value) {startChild(); Wirte() << value;}
        template<typename T>
        void KeyValue(const char* key, T value) {Key(key); Value(value);}
    private:
        std::ostream* writer{nullptr};

        // 使用 stack 来存储 json 对象的深度，这个深度被用来控制打印输出 json 时的格式
        // ：比如标点符号；缩进对齐；换行
        std::stack<Container*> depth;
        
        // 缩进
        int initialIndentDepth{0};
        const char* indent{"  "};

        // 布局
        const char* containerPadding{" "};
        const char* keyPaddingLeft{""};
        const char* keyPaddingRight{" "};

        ContainerLayout defaultLayout{ContainerLayout::MULTI_LINE};
        bool forceDefaultLayout{false};

        void Indent();
    };

}

