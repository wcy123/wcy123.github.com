#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Foo {
   public:
    Foo(int i) : value_(i) {
        cout << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
             << "value_ " << value_ << " " << endl;
    }

    ~Foo() {
        cout << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
             << "value_ " << value_ << " " << endl;
    }
    bool operator==(const Foo& f) { return f.value_ == value_; }
    int value_;
    friend ostream& operator<<(ostream& out, const Foo& f);
};
ostream& operator<<(ostream& out, const Foo& f) {
    out << f.value_;
    return out;
}

#if __cplusplus <= 201103L
namespace std {
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std
#endif

template <typename T>
class List {
   public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    struct Cons {
        template <typename... Args>
        Cons(unique_ptr<Cons>&& next, Args&&... args)
            : value_(std::forward<Args>(args)...), next_(std::move(next)) {}
        value_type value_;
        unique_ptr<Cons> next_;
    };

   public:
    List() : head_(nullptr), tail_(&head_){};

    template <typename Iter>
    List(Iter begin, Iter end) : head_(), tail_(&head_) {
        for (auto it = begin; it != end; ++it) {
            emplace_back(*it);
        }
    }

    List(const std::initializer_list<value_type>& c)
        : List(c.begin(), c.end()) {}

    template <typename Container>
    List(const Container& c) : List(c.begin(), c.end()) {}

    template <typename... Args>
    void emplace_front(Args&&... args) {
        head_ =
            make_unique<Cons>(std::move(head_), std::forward<Args>(args)...);
    }
    template <typename... Args>
    void emplace_back(Args&&... args) {
        (*tail_) = make_unique<Cons>(nullptr, std::forward<Args>(args)...);
        tail_ = &(*tail_)->next_;
    }

    unique_ptr<Cons> pop_front() {
        unique_ptr<Cons> ret;
        if(!empty()) {
            ret = remove_pointer(&head_);
        }
        return ret;
    }

    unique_ptr<Cons> remove_pointer(unique_ptr<Cons>* i){
        if (tail_ == &(*i)->next_) {
            tail_ = i;
        }
        unique_ptr<Cons> ret = std::move(*i);
        (*i) = std::move((*i)->next_);
        return ret;
    }

    void remove(const value_type& value) {
        for (auto i = &head_; *i; i = &((*i)->next_)) {
            if ((*i)->value_ == value) {
                remove_pointer(i);
                break;
            }
        }
    }

    bool empty() { return head_ == nullptr; }



   private:
    unique_ptr<Cons> head_;
    unique_ptr<Cons>* tail_;
    friend std::ostream& operator<<(std::ostream& out,
                                    const List<value_type>& lst);
};

ostream& operator<<(ostream& out, const List<Foo>& lst) {
    out << "(";
    for (auto i = lst.head_.get(); i; i = i->next_.get()) {
        out << " " << i->value_;
    }
    out << ")";
    return out;
}

int main(int argc, char* argv[]) {
    List<Foo> lst;

    lst.emplace_back(1);
    lst.emplace_back(2);
    lst.remove(Foo(2));
    lst.emplace_back(3);
    cout << lst << endl;
    auto x = lst.pop_front();
    cout << "x is " << x.get() << endl;
    return 0;
}
