#include "bits/stdc++.h"
#include "executor.h"
#define szof(_x) ((int) (_x).size())

using namespace std;

vector<vector<string>> tests = {
{"./test"},
{"./test", "1", "2", "3"},
{"./test", "abacaba"},
{"./tes"}};

int main() {
    for (int i = 0; i < szof(tests); ++i) {
        cout << "Test #" << i + 1 << ":\n";
        string res = execute_cgi(tests[i]);
        cout << "Here is an output:" << endl;
        cout << res << endl;
        cout << "End of output" << endl;
    }

    return 0;
}
