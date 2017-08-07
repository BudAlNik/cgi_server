#ifndef EXECUTOR_CGI_H
#define EXECUTOR_CGI_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

void on_request_recieved(vector<string> argv, int fd);
void execute_cgi(vector<string> argv, int fd);
ssize_t write_no_signal(int fd, const void *buf, size_t count);

#endif