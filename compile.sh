#!/usr/bin/env bash
g++ -std=c++11 -o test test.cpp
g++ -std=c++11 -Wall -Wextra -fsanitize=address -o test_cgi_parser test_cgi_parser.cpp parser.cpp
g++ -std=c++11 -Wall -Wextra -fsanitize=address -o test_cgi_executor test_cgi_executor.cpp executor.cpp