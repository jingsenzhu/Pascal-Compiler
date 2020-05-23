#!/bin/sh
bison -d parser.y -o parser.cpp
flex -o scanner.cpp scanner.l
g++ -c scanner.cpp -o scanner.o
g++ -c parser.cpp -o parser.o
