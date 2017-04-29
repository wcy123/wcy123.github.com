#include <iostream>
using namespace std;


class Point {
  public:
    Point(int x, int y): x_(x), y_(y) {
    }
    int GetX() const {
        return x_;
    }
    int GetY() const {
        return y_;
    }
  private:
    const int x_;
    const int y_;
};
ostream& operator<<(ostream& out, const Point& pt) {
    out << "(" << pt.GetX() << "," << pt.GetY() << ")";
    return out;
}

int main(int argc, char *argv[])
{
    Point pt(100,200);
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "pt "  << pt << " "
         << endl;
    return 0;
}
