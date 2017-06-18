#include <iostream>
#include <fstream>
#include <regex>
using namespace std;

int main(int argc, char *argv[])
{
    static std::regex string_value(argv[1]);
    std::ifstream fs(argv[2]);
    char line[4096];
    while (fs.getline(line, sizeof(line))) {
        std::string str_line{line};
        std::smatch pieces_match;
        if (std::regex_match(str_line, pieces_match, string_value)) {
            for(int i = 0; i < pieces_match.size() ; ++i){
                cout << "match " << i << " :" << pieces_match[i].str() << endl;
           l }
        } else {
            std::cerr << "unknown config: " << line << std::endl;
        }
    }
    fs.close();
    return 0;
}
