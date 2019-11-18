#include <memory>
#include <string>
#include <iostream>

using namespace std;

class IHello {
	public:
		IHello() {}
		~IHello() {}
		virtual void Output(const string& str){}
};

class Hello: public IHello {
	public:
		void Output(const string& str) override {
			cout << str << endl;
		}
};

class HelloProxy: public Hello {
	public:
		HelloProxy(IHello* ptr): m_ptr{ptr} {}
		~HelloProxy() {
			delete m_ptr;
			m_ptr = nullptr;
		}

		void Output(const string& str) final {
			cout << "Before" << endl;
			m_ptr->Output(str);
			cout << "After" << endl;
		}

	private:
		IHello* m_ptr;
};



int main() {

	auto h = make_shared<HelloProxy>(new Hello());
	h->Output("test");
	return 0;
}
