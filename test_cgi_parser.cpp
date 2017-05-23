#include "bits/stdc++.h"
#include "parser.h"
#define szof(_x) ((int) (_x).size())

using namespace std;

string tests[] = {
"http://example.com/cgi-bin/printenv.pl?and=a&query=string",
"https://www.google.ru/search?q=wake+up&oq=wake+up&aqs=chrome..69i57j0l5.27951j0j7&sourceid=chrome&ie=UTF-8",
"https://yandex.ru/search/?lr=2&msid=1495554246.28958.22890.21416&text=wake%20up"
};

int main() {
    for (auto path : tests) {
        auto res = parse_path(path);
        for (auto s : res) {
            cerr << s << " ";
        }
        cerr << endl;
    }
    

    return 0;
}