#include <iostream>
#include "parser.hpp"

extern FILE *yyin;

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;
    yyin = fopen(argv[1], "r");
    spc::parser().parse();
    return 0;
}
