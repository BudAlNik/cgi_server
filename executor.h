#ifndef EXECUTOR_CGI_H
#define EXECUTOR_CGI_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

void on_request_recieved(vector<string> argv, int fd, bool& forked);
int execute_cgi(vector<string> argv, int fd);

#endif