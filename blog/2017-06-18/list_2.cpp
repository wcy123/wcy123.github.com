#include <iostream>
#include <memory>
using namespace std;

class List {
   public:
    List() : head_(nullptr){};
    void push_front(int value) {
        head_ = make_unique<Cons>(value, std::move(head_));
    }
    bool empty() { return head_ == nullptr; }

   private:
    struct Cons {
        Cons(int value, unique_ptr<Cons>&& next)
            : value_(value), next_(std::move(next)) {}
        int value_;
        unique_ptr<Cons> next_;
    };
    unique_ptr<Cons> head_;
    friend ostream& operator<<(ostream& out, const List& lst);
};

ostream& operator<<(ostream& out, const List& lst) {
    out << "(";
    for (auto i = lst.head_.get(); i; i = i->next_.get()) {
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
