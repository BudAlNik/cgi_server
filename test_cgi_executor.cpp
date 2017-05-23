#include "bits/stdc++.h"
#include "executor.h"
#define szof(_x) ((int) (_x).size())

using namespace std;

vector<vector<string>> tests = {
{"test.exe"},
{"test.exe", "1", "2", "3"},
{"test.exe", "abacaba"}};

int main() {
    for (int i = 0; i < szof(tests); ++i) {
        cout << "Test #" << i + 1 << ":\n";
        cout << execute_cgi(tests[i]) << endl;
    }

    return 0;
}