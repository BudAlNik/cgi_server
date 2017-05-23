#include "executor.h"
#include <unistd.h>

using namespace std;
const int SZ = 256;
char buf[SZ];

string execute_cgi(vector<string> argv) {
    string command = "";
    for (auto s : argv) {
        command += s + " ";
    }
    
    auto p = popen(command.c_str(), "r");
    
    if (!p) {
        return "";
    }
    
    string res = "";

    while (!feof(p)) {
        if (fgets(buf, SZ, p) != NULL)
            res += buf;
    }
    
    if (pclose(p) == -1) {
        return "";
    }
    
    return res;
}
