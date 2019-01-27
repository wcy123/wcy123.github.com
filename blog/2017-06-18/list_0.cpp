#include <iostream>
using namespace std;

struct List {
    int value;
    struct List * next;
};

List * MakeList (int value, List * next) {
    return new List{value, next};
}

ostream& operator<<(ostream& out, const List* head) {
    out << "(";
    for(auto i = head; i ; i = i->next){
        out << " " << i->value;
    }
    out << ")";
    return out;
}

int main(int argc, char *argv[])
{
    List * lst = MakeList(1, MakeList(2, MakeList(3, nullptr)));
    cout << lst << endl;
    return 0;
}
