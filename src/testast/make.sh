#!/bin/sh
bison -d parser.y -o parser.cpp
flex -o scanner.cpp scanner.l
g++ -c scanner.cpp -o scanner.o
g++ -c parser.cpp -o parser.o
g++ -c ASTvis.cpp -o ASTvis.o
g++ -c main.cpp -o main.o
g++ scanner.o parser.o ASTvis.o main.o -o main

