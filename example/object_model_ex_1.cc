#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;
/**
 * the c++ object model
 * example 1 simple c++ object model
 * class Base 
 * class Derived : public Base
 */

class Base {
    public :
	Base(){
	    cout << "Base" << endl;
	}
	virtual ~Base(){
	    cout << "~Base" << endl;
	}
	virtual void foo(){
	    cout << "Base::foo" << endl;
	}
    protected :
	int x_;
    private :
	int y_;
};

class Derived : public Base {
    public :
	Derived () {
	    cout << "Derived" << endl;
	}
	~Derived() {
	    cout << "~Derived" << endl;
	}
	void foo() {
	    cout << "Derived::foo" << endl;
	}
    private :
	int d_;
};

class V : virtual public Base {
    public : 
	V() {
	    cout << "V" << endl;
	}
	~V() {
	    cout << "~V" << endl;
	}
	void foo() {
	    cout << "V::foo" << endl;
	}
    private :
	int v_;
};

typedef void (*Fun)(void);
int main( int argc, char *argv[] ) {
    Base b;
    Derived d;
    V v;
    Base *bd = &d;
    Base *bv = &v;

    return 0;
}
