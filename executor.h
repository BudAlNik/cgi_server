#ifndef EXECUTOR_CGI_H
#define EXECUTOR_CGI_H

#include <string>
#include <vector>
#include <iostream>
#include <atomic>

using namespace std;

void on_request_recieved(vector<string> argv, const int& fd, std::atomic_bool& forked);
int execute_cgi(vector<string> argv);

#endif