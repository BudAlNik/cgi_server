
#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <atomic>

using namespace std;
const int SZ = 256;


void on_request_recieved(vector<string> argv, const int& fd, std::atomic_bool& forked) {
    int fdoldout = dup(1);
    int fdpr = dup(fd);
    int status;
    dup2(fd, 1);

    int p = execute_cgi(argv);
    waitpid(p, &status, 0);
    dup2(fdoldout, 1);
    dup2(fd, fdpr);
    close(fdoldout);
    close(fdpr);
    forked = false;
}

int execute_cgi(vector<string> argv) {
    pid_t p = fork();
    int status;
    if (p == -1) {
        // raise error
        return -1;
    } else if (p == 0) {
        const char* args[argv.size() + 1];
        for (int i = 0; i < argv.size(); i++) {
            args[i] = argv[i].c_str();
        }
        args[argv.size()] = 0;
        int c = execv(argv[0].c_str(), (char* const*)args);
        if (c == -1) {
            cerr << "can't find script " << args[0] <<  endl;
        }
        exit(EXIT_SUCCESS);
    }

    return p;
}
