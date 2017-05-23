#include "parser.h"
#include <string>
#include <vector>
#define szof(x) ((int) (x).size())

using namespace std;

const string cgi_path = "/usr/local/cgi-server/cgi-bin/";

vector<string> parse_path(string path) {
    string word = "/cgi-bin/";
    int pos = -1;
    for (int i = 0; i < szof(path) - szof(word) + 1; ++i) {
        if (path[i] == '?') {
            pos = i - 1;
            while (pos > 0 && (path[pos] != '/' || i - pos <= 1)) {
                --pos;
            }
            ++pos;
            break;
        }
        bool flag = true;
        for (int j = 0; j < szof(word); ++j) {
            flag &= (word[j] == path[i + j]);
        }
        if (flag) {
            pos = i + szof(word);
            break;
        }
    }
    if (pos == -1) {
        return {};
    }

    string file_path = cgi_path;
    while (pos < szof(path) && path[pos] != '?') {
        file_path += path[pos++];
    }

    ++pos;
    vector<string> ret = {file_path};

    string cur = "";
    while (pos <= szof(path)) {
        if (szof(path) == pos || path[pos] == '&') {
            if (szof(cur)) {
                ret.push_back(cur);
            }
            cur = "";
        } else {
            cur += path[pos];
        }
        ++pos;    
    }

    return ret;
}