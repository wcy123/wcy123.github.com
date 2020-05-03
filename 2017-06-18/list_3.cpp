#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class List {
   public:
    List() : head_(nullptr){};

    template <typename Iter>
    List(Iter begin, Iter end) : head_() {
        for (auto it = begin; it != end; ++it) {
            push_front(*it);
        }
    }

    List(const std::initializer_list<int>& c) : List(c.begin(), c.end()) {}

    template <typename Container>
    List(const Container& c) : List(c.begin(), c.end()) {}

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
    auto lst1 = List{1, 2, 3};
    cout << lst1 << endl;

    vector<int> v1 = {4, 5, 6};
    auto lst2 = List(v1);
    cout << lst2 << endl;
    return 0;
}
