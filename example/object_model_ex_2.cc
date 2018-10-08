#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;
class Base {
    public :
	Base() {
	    cout << "Base" << endl;
	}
	virtual ~Base() {
	    cout << "~Base" << endl;
	}
	virtual void foo(){
	    cout << "Base::foo" << endl;
	}
    protected :
	int b;

};
class Derived : public Base {
    public :
	Derived() {
	}
	~Derived() {
	}
	void foo() {
	    cout << "Derived::foo" << endl;
	}
    private:
	int d;
};

class V : virtual public Base {
    public :
	V() {
	}
	~V() {
	}
	void foo() {
	    cout << "V::foo" << endl;
	}
    private :
	int v_;
};

int main( int argc, char *argv[] ) {
    Base b;
    Derived d;
    V v;

    v.foo();

   return 0;
}
