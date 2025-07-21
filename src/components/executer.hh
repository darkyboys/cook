/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/

// executer - This is responsible to execute all the buildable files! And process the final stage of incremental builds 
namespace Cook{
    int thread_limis = 1; // don't use this as the parallel logic is still in development
    // int active_threads = 0;
    // void psystem (std::string command){
    //     active_threads += 1;
    //     std::system (command.c_str());
    //     active_threads -= 1;
    // }
    void Executer (){
        for (unsigned long long i = 0;i < source_file_and_commands.size();i++){
            std::string source_file_name = source_file_and_commands[i][0];
            std::string source_file_command = source_file_and_commands[i][1];
            Cook::Log("Building `" + source_file_name + "` Progress --> " + std::to_string((i/source_file_and_commands.size())*100) + "");
            Cook::Log("Building Commands:  `" + source_file_command + "`");
            std::system (source_file_command.c_str());

            std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + source_file_name + " > " + communication_file_path;
            std::system (command_for_timestamps_to_communication_file.c_str());
            std::ifstream ifile_communication_out (communication_file_path);
            std::string temp, content;
            while (std::getline(ifile_communication_out, temp))
                content += temp;

            HELL6_99MO increment_h699_file_out (increment_h699_file_path);
            increment_h699_file_out.Parse();

            HELL6_99MO_TYPE key = increment_h699_file_out.get(source_file_name);
            if (key.type == H699_UNIDEF){
                increment_h699_file_out.new_key (source_file_name, "string");
                increment_h699_file_out.set (source_file_name, content);
            }
            else {
                increment_h699_file_out.set (source_file_name, content);
            }
            increment_h699_file_out.write (increment_h699_file_path);
        }
    }
}