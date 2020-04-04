#include <iostream>
using namespace std;

class List {
   public:
    List(): head_(nullptr) {};
    void push_front(int value) {
        head_ = new Cons(value, head_);
    }
    bool empty() {
        return head_ == nullptr;
    }
   private:
    struct Cons{
        Cons(int value, Cons* next) : value_(value), next_(next) {}
        int value_;
        struct Cons* next_;
    };
    Cons * head_;
    friend ostream& operator<<(ostream& out, const List& lst);
};

ostream& operator<<(ostream& out, const List& lst) {
    out << "(";
    for (auto i = lst.head_; i; i = i->next_) {
        out << " " << i->value_;
    }
    out << ")";
    return out;
}

int main(int argc, char* argv[]) {
    List lst;
    lst.push_front(1);
    lst.push_front(2);
    lst.push_front(3);
    cout << lst << endl;
    return 0;
}
