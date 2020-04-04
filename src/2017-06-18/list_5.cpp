#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class List {
   public:
    List() : head_(nullptr), tail_(&head_){};

    template <typename Iter>
    List(Iter begin, Iter end) : head_(), tail_(&head_) {
        for (auto it = begin; it != end; ++it) {
            push_back(*it);
        }
    }

    List(const std::initializer_list<int>& c) : List(c.begin(), c.end()) {}

    template <typename Container>
    List(const Container& c) : List(c.begin(), c.end()) {}

    void push_front(int value) {
        head_ = make_unique<Cons>(value, std::move(head_));
    }

    void push_back(int value) {
        (*tail_) = make_unique<Cons>(value, nullptr);
        tail_ = &(*tail_)->next_;
    }

    void remove(int value) {
        for (auto i = &head_; *i; i = &((*i)->next_)) {
            if ((*i)->value_ == value) {
                (*i) = std::move((*i)->next_);
                break;
            }
        }
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
    unique_ptr<Cons> * tail_;
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
    lst1.remove(1);
    cout << lst1 << endl;

    auto lst2 = List{1, 2, 3};
    lst2.remove(3);
    cout << lst2 << endl;

    return 0;
}
