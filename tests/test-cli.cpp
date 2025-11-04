#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

#include "../utils/cli.hpp"

int main(int argc, char* argv[]) {
    Args args = parseArgs(argc, argv);

    printArgs(args);

    return 0;
}