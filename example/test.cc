#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <typeinfo>
#include <cstdint>
using namespace std;

class Base {
    public:
	Base(){ cout << "Base()" << endl;}
	~Base(){ cout << "Base()" << endl;}
	virtual void show() {
	    cout << "Base" << endl;
	}
	virtual void func() {
	    cout << "func in Base" << endl;
	}
    protected :
	int a = 15;
};
class Derived : public Base{
    public:
	Derived() { cout << "Derived()" << endl; }
	~Derived() { cout << "Derived()" << endl; }
	void show() override { cout << "Derived" << endl; }
	virtual void foo() { cout << "foo in Derived" << endl; }
    protected:
	int b = 31;
};
int main( int argc, char *argv[] ) {
    cout << "init seq of obj_a:" << endl;
    Base obj_a;
    cout << "init seq of obj_b:" << endl;
    Derived obj_b;
    Base *b = &obj_b;
    cout << typeid(b).name() << endl;

    cout << "---" << endl;
    return 0;
}
