#!/bin/bash

gcc -Wall -g -rdynamic lexer.c

./a.out testcases/case1.json
