#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <typeinfo>
using namespace std;

class Base {
    protected:
	int _x;
    public :
	virtual void f() {cout << "Base::f" << endl;}
	virtual void g() {cout << "Base::g" << endl;}
	virtual void h() {cout << "Base::h" << endl;}
};

class Derived : public Base {
    private :
	int _y;

};
class Point {
    public :
	Point( float xval);
	virtual ~Point();
	float x() const;
	static int PointCount();
    protected:
	virtual ostream& pint(ostream &os) const;
	float _x;
	static int _point_count;
};

typedef void(*Fun)(void);
int main( int argc, char *argv[] ) {
    Base b;
    Fun pfun = nullptr;
    cout << "address:" << (int*)(&b) << endl;
    cout << "address:" << (int*)*(int*)(&b) << endl;
    pfun = (Fun)*((int*)*(int*)(&b));
    pfun();
    pfun = (Fun)*((int*)*(int*)(&b)+2);
    pfun();
    pfun = (Fun)*((int*)*(int*)(&b)+4);
    pfun();
    // type info
    const type_info &bti = typeid(b);
    cout <<"type_info address:" <<  (int64_t*)&bti << endl;
    cout <<"sizeof type_info:" << sizeof(type_info) << endl;
    cout <<"sizeof typeid:" << sizeof(bti) << endl;
    cout <<"class base point type_info" << (int64_t*)&(*((int64_t*)*((int64_t*)&b)-1)) << endl;
    cout <<"class base vptr" << (int64_t*)*(int64_t*)(&b) << endl;

    Derived d;
    cout << typeid(b).name() << endl;
    cout << typeid(d).name() << endl;
    cout << sizeof(b) << endl;
    cout << sizeof(d) << endl;
    return 0;
}
