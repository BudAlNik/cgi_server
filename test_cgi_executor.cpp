#include "bits/stdc++.h"
#include "executor.h"
#define szof(_x) ((int) (_x).size())

using namespace std;

vector<vector<string>> tests = {
{"./test"},
{"./test", "1", "2", "3"},
{"./test", "abacaba"},
{"./tes"},
{"ls", "-la"}};

int main() {
    for (int i = 0; i < szof(tests); ++i) {
        cout << "Test #" << i + 1 << ":\n";
        cout << "Here is an output:" << endl;
        execute_cgi(tests[i], 1);
        // cout << res << endl;
        cout << "End of output" << endl;
    }

    cout << "Parallel execution:" << endl;
    for (int i = 0; i < szof(tests); ++i) {
    	on_request_recieved(tests[i], 1);
    }

    return 0;
}
