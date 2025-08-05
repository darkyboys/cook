/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/

#include <string>
#ifndef COOK_VERSION
#define COOK_VERSION 3

// Headers
#include "needed.hh"

int main(int argc, char* argv[]) {
    Cook::increment = true;
    std::string path = ".";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--version" || arg == "-v") {
            std::cout << "Cook Version " << COOK_VERSION << "\n";
            std::exit(0);
        }

        else if (arg == "-i" || arg == "--increment") {
            if (i + 1 < argc) {
                std::string val = argv[i + 1];
                if (val == "true") {
                    Cook::increment = true;
                    ++i; // Skip next argument
                } else if (val == "false") {
                    Cook::increment = false;
                    ++i; // Skip next argument
                } else {
                    Cook::Error("-i or --increment needs either true or false as value");
                }
            } else {
                Cook::Error("-i or --increment expects a value (true/false)");
            }
        }

        else if (arg == "--threads" || arg == "-t") {
            if (i + 1 < argc) {
                std::string val = argv[i + 1];
                for (unsigned long long x = 0;x < val.length();x++){
                    if (val[x] >= '0' and val[x] <= '9'){
                        continue;
                    }
                    else {
                        Cook::Error("-t or --threads expects a value numeric value (0 - 9 per character)");
                    }
                }
                Cook::thread_limit = std::stoi(val);
                ++i;// Skip next argument
            } else {
                Cook::Error("-t or --threads expects a value numeric value (0 - 9 per character)");
            }
        }

        else if (arg == "--parallel" || arg == "-p") {
            Cook::parallel = true;
            // std::exit(0);
        }

        else if (arg[0] != '-') {
            // Assume any non-flag argument is the path
            path = arg;
        }

        else {
            // Unknown option
            Cook::Error("Unknown option: " + arg);
        }
    }

    Cook::Parse(path);
}



#endif
