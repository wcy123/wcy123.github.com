#include <iostream>
using namespace std;


struct Point {
    Point() = delete;
    Point(const Point& other) = delete;
    Point(int ax,int ay) : x(new int(ax)), y(new int(ay)) {}
    Point(Point&& other):
            x(move(other.x)),
            y(move(other.y))
    {
    }
  public:
    unique_ptr<int> x;
    unique_ptr<int> y;

};
Point foo() {
     Point p(100,200);
     return p;
}
ostream& operator <<(ostream& out, const Point & p) {
    out << "(" << (p.x?*p.x:0) << "," << (p.y?*p.y:0) << ")";
    //out << "(" << p.x.get() << "," << p.y.get() << ")";
    return out;
}
int main(int argc, char *argv[])
{
    Point x(foo());
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "x "  << x << " "
         << endl;
    Point y(move(x));
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "x "  << x << " "
         << "y "  << y << " "
         << endl;

    return 0;
}
