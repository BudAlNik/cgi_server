#!/usr/bin/bash
g++ -o test test.cpp
g++ -o test_cgi_parser test_cgi_parser.cpp parser.cpp
g++ -o test_cgi_executor test_cgi_executor.cpp executor.cpp