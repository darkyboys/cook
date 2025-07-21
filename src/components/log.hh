/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/

namespace Cook{
    bool logging_allowed = false;
    void Error (std::string error){
        std::cout << "\033[1;31m[Cook Error]: \033[1;37m" << error <<"\n\033[0m";
        std::exit ( 3 );
    }
    void Log (std::string log){
        if (logging_allowed) std::cout << "\033[1;92m[Cook Log]: \033[1;37m" << log <<"\n\033[0m";
    }
    void Warning (std::string warning){
        std::cout << "\033[38;5;190m[Cook Warning]: \033[1;37m" << warning <<"\n\033[0m";
        std::exit ( 3 );
    }
}