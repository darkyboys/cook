/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/

#ifndef COOK_VERSION
#define COOK_VERSION 1

// Headers
#include "needed.hh"

// main
int main (int argc, char* argv[]){
    Cook::increment = true;
    if (argc == 2){
        std::string argument = argv[1];
        if (argument == "--version" or argument == "-v")
        {
            std::cout << "Cook Version "<<COOK_VERSION<<"\n";
        }
        else {
            Cook::Parse(argv[1]);
        }
        std::exit ( 0 );
    }
    else if (argc == 3){
        std::string argument = argv[2];
        if (argument == "-i" or argument == "--increment"){
            Cook::increment = true;
        }
    }

    else if (argc == 4){
        std::string argument = argv[2];
        if (argument == "-i" or argument == "--increment"){
            std::string increment = argv[3];
            if (increment.find("true") != std::string::npos){
                Cook::increment = true;
            }
            else if (increment.find("false") != std::string::npos){
                Cook::increment = false;
            }
            else {
                Cook::Error("-i or --increment needs either true or false as turn on/off flags, Please pass either true or false to ignore this error");
            }
        }
    }


    Cook::Parse(".");
}


#endif