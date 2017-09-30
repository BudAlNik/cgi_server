
#include "executor.h"
#include <unistd.h>

using namespace std;
const int SZ = 256;


void on_request_recieved(vector<string> argv, int fd) {
    pid_t p = fork();
    if (p == -1) {
        // raise error
        return;
    } else if (p != 0) {
        return;
    } else {
        execute_cgi(argv, fd);
        exit(0);
    }
}

void execute_cgi(vector<string> argv, int fd) {
    string command = "";
    for (auto s : argv) {
        command += s + " ";
    }
    
    auto p = popen(command.c_str(), "r");
    
    if (!p) {
        return;
    }
    
    char buf[SZ];
    while (!feof(p)) {
        size_t cnt = fread(buf, 1, SZ, p);
        if (cnt == 0) {
            // raise error
            break;
        }

        int res = write_no_signal(fd, buf, cnt);
        if (res == -1) {
            // raise error
            break;
        }
    }
    
    if (pclose(p) == -1) {
        return;
    }
    return;
}

ssize_t write_no_signal(int fd, const void *buf, size_t count) {
    int res;
    if (count == 0) {
        return 0;
    }
    while(true) {
        res = write(fd, buf, count);
        // TODO: check if partial writes are possible
        if (res == -1 && errno == EINTR) {
            continue;
        }
        break;
    }
    return res;
}
