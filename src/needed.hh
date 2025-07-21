/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/


// Header
// C++ STL  
#include <filesystem>
#include   <iostream>
#include    <fstream>
#include     <string>
#include     <vector>
#include     <thread>

// Local headers
#include "components/log.hh"
#include "../include/h699/h699.hh"
#include "components/functions.hh"
#include "components/executer.hh"

// Required
namespace Cook {
    void Parse (std::string path){
        // recipe
        std::ifstream recipe_ifile (path + "/recipe");
        if (!recipe_ifile.is_open()){
            std::ifstream cookshell_ifile (path + "/cookshell");
            if (!cookshell_ifile.is_open()){
                Cook::Error("Could not find either `recipe` file nor the `cookshell` file. These is nothing to do");
            }
            else {
                std::string line;
                while (std::getline(cookshell_ifile, line)){
                    if (line == "cook.allow.logging")
                        logging_allowed = true;
                    else {
                        Cook::Log("Running `" + line + "`");
                        std::system (line.c_str());
                    }
                }
            }
        }
        else {
            HELL6_99MO Recipe_H699_IFILE (path + "/recipe");
            Recipe_H699_IFILE.Parse();
            Cook::Functions(Recipe_H699_IFILE);
            Cook::Executer();
        }
    }
}