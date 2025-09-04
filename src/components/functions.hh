/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under MIT License
 * Checkout the README.md for more information
 * This file is responsible to deal with CLI
*/

// functions

namespace Cook {
    bool increment = false,
         can_increment_skip = false;


    std::string bin = "bin",
                system = "",
                psystem = "",
                compiler_parguments = "",
                compiler_arguments = "",
                compiler = "g++";
    std::vector <std::string> pkg_in,
                              combine, 
                              include,
                              lib;
    std::vector <std::string> compare_pkg_in; // only for pkg_config comparision
    bool forced_pkg_in_fetch = false;
    std::string pkg_in_libs_data = ""; // contains the -I -l for gcc 

    std::string cookcache_directory = "CookCache/";
    std::string increment_h699_file_path = cookcache_directory + "increment.h699";
    std::string communication_file_path = cookcache_directory + "communication.out";
    std::vector <std::vector <std::string>> source_file_and_commands = {};
    std::vector <std::string> FunctionsCurrentTarget = {};


    int Check_File_Timestamps (HELL6_99MO &File, std::string Name){
        can_increment_skip = false;
        std::ifstream increment_h699_ifile (increment_h699_file_path);
        if (!increment_h699_ifile.is_open()){ // When the file don't exists then create the file and add the first file directly to it with h699
            std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + Name + " > " + communication_file_path  + " 2> /dev/null";
            std::system (command_for_timestamps_to_communication_file.c_str());
            std::ifstream ifile_communication_out (communication_file_path);
            std::string temp, content;
            while (std::getline(ifile_communication_out, temp))
                content += temp;
            std::ofstream increment_file_h699_open_write (increment_h699_file_path);
            HELL6_99MO increment_file_h699_open (increment_h699_file_path);
            increment_file_h699_open.new_key (Name, "string");
            increment_file_h699_open.set (Name, content);
            // std::cout << Name << ", Timestamps: "<<increment_file_h699_open.get (Name).string_value<<"\n"; // for debugging only
            increment_file_h699_open.write (increment_h699_file_path);
        }
        else {
            HELL6_99MO increment_file_h699_open (increment_h699_file_path);
            increment_file_h699_open.Parse();
            // Check for the file name
            HELL6_99MO_TYPE current_source_file_name_in_h699_file = increment_file_h699_open.get(Name);
            if (current_source_file_name_in_h699_file.type == H699_UNIDEF){ // When the file name which is Name is not a key in the h699 file then this block will execute
                std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + Name + " > " + communication_file_path + " 2> /dev/null";
                std::system (command_for_timestamps_to_communication_file.c_str());
                std::ifstream ifile_communication_out (communication_file_path);
                std::string temp, content;
                while (std::getline(ifile_communication_out, temp))
                    content += temp;
                increment_file_h699_open.new_key (Name, "string");
                increment_file_h699_open.set (Name, content);
                // std::cout << Name << ", Timestamps: "<<increment_file_h699_open.get (Name).string_value<<"\n"; // for debugging only
                increment_file_h699_open.write (increment_h699_file_path);
            }
            else { // When the file name which is Name is a key in the h699 file then this block will execute, Simply compare the time stamps and see if it's changed or not , if yes then send it for incremental build othervise don't send it and hold it
                std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + Name + " > " + communication_file_path + " 2> /dev/null";
                std::system (command_for_timestamps_to_communication_file.c_str());
                std::ifstream ifile_communication_out (communication_file_path);
                std::string temp, current_file_time_stamp_from_system;
                while (std::getline(ifile_communication_out, temp))
                    current_file_time_stamp_from_system += temp;
                if (current_source_file_name_in_h699_file.type == "string"){
                    if (current_source_file_name_in_h699_file.string_value == current_file_time_stamp_from_system){ // Both time stamps matches means the file was not edited so it can be easily ignored but we also need to check the binary which we will check later as if the binary doesn't exists then even if incremental builds are supported , The file must be compiled
                        // std::cout << "Both Timestamps matches for the file `"<<Name<<"` So Incremental Build is supported! On this file\n"; // For Debugging only
                        can_increment_skip = true;
                    }
                    else {
                        // std::cout << "Both Timestamps don't matches for the file `"<<Name<<"` So Incremental Build is not supported! On this file\n"; // For Debugging only
                        can_increment_skip = false;
                    }
                    increment_file_h699_open.set (Name, current_file_time_stamp_from_system);
                    increment_file_h699_open.write (increment_h699_file_path);
                }
                else {
                    Cook::Warning ("A Non-String type detected in the `" + increment_h699_file_path + "` file so deleting it via the rm command as it can cause bugs in incremental builds, Note: Do not modify `" + increment_h699_file_path + "` as it is generated by the Cook Build System.");
                    std::string command_to_delete_increment_h699_file = "rm -rf " + increment_h699_file_path;
                    std::system(command_to_delete_increment_h699_file.c_str());
                }
            }
        }

        return can_increment_skip;
    }

    void Functions(HELL6_99MO &File){
        std::vector <std::string> combine_files_timestamps = {};
        std::vector <std::string> increment_files_captured = {};
        std::filesystem::create_directory(cookcache_directory);
        bool can_build_skip = false;

        for (unsigned long long scopes_index = 0;scopes_index < File.scopes.size();scopes_index++){
            std::ifstream ifile (File.scopes[scopes_index]);
            if (File.scopes[scopes_index] == "global"){
                can_build_skip = true;
            }
            if (!ifile.is_open() and not can_build_skip){
                // std::cout << File.scopes[scopes_index].length()<<"\n"; // for debugging
                Cook::Error("Invalid target detected , Can't open the file `" + File.scopes[scopes_index] + "`, Please make sure that the entered path was valid.");
            }
            else {
                can_increment_skip = false;
                std::ifstream increment_h699_ifile (increment_h699_file_path);
                if (!increment_h699_ifile.is_open()){ // When the file don't exists then create the file and add the first file directly to it with h699
                    std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + File.scopes[scopes_index] + " > " + communication_file_path  + " 2> /dev/null";
                    std::system (command_for_timestamps_to_communication_file.c_str());
                    std::ifstream ifile_communication_out (communication_file_path);
                    std::string temp, content;
                    while (std::getline(ifile_communication_out, temp))
                        content += temp;
                    std::ofstream increment_file_h699_open_write (increment_h699_file_path);
                    HELL6_99MO increment_file_h699_open (increment_h699_file_path);
                    increment_file_h699_open.new_key (File.scopes[scopes_index], "string");
                    increment_file_h699_open.set (File.scopes[scopes_index], content);
                    // std::cout << File.scopes[scopes_index] << ", Timestamps: "<<increment_file_h699_open.get (File.scopes[scopes_index]).string_value<<"\n"; // for debugging only
                    increment_file_h699_open.write (increment_h699_file_path);
                }
                else {
                    HELL6_99MO increment_file_h699_open (increment_h699_file_path);
                    increment_file_h699_open.Parse();

                    // Check for the file name
                    HELL6_99MO_TYPE current_source_file_name_in_h699_file = increment_file_h699_open.get(File.scopes[scopes_index]);

                    if (current_source_file_name_in_h699_file.type == H699_UNIDEF){ // When the file name which is File.scopes[scopes_index] is not a key in the h699 file then this block will execute
                        std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + File.scopes[scopes_index] + " > " + communication_file_path + " 2> /dev/null";
                        std::system (command_for_timestamps_to_communication_file.c_str());
                        std::ifstream ifile_communication_out (communication_file_path);
                        std::string temp, content;
                        while (std::getline(ifile_communication_out, temp))
                            content += temp;
                        increment_file_h699_open.new_key (File.scopes[scopes_index], "string");
                        increment_file_h699_open.set (File.scopes[scopes_index], content);
                        // std::cout << File.scopes[scopes_index] << ", Timestamps: "<<increment_file_h699_open.get (File.scopes[scopes_index]).string_value<<"\n"; // for debugging only
                        increment_file_h699_open.write (increment_h699_file_path);
                    }

                    else { // When the file name which is File.scopes[scopes_index] is a key in the h699 file then this block will execute, Simply compare the time stamps and see if it's changed or not , if yes then send it for incremental build othervise don't send it and hold it
                        std::string command_for_timestamps_to_communication_file = "ls -l --time-style=full-iso " + File.scopes[scopes_index] + " > " + communication_file_path + " 2> /dev/null";
                        std::system (command_for_timestamps_to_communication_file.c_str());
                        std::ifstream ifile_communication_out (communication_file_path);
                        std::string temp, current_file_time_stamp_from_system;
                        while (std::getline(ifile_communication_out, temp))
                            current_file_time_stamp_from_system += temp;

                        if (current_source_file_name_in_h699_file.type == "string"){
                            if (current_source_file_name_in_h699_file.string_value == current_file_time_stamp_from_system){ // Both time stamps matches means the file was not edited so it can be easily ignored but we also need to check the binary which we will check later as if the binary doesn't exists then even if incremental builds are supported , The file must be compiled
                                // std::cout << "Both Timestamps matches for the file `"<<File.scopes[scopes_index]<<"` So Incremental Build is supported! On this file\n"; // For Debugging only
                                can_increment_skip = true;
                            }
                            else {
                                // std::cout << "Both Timestamps don't matches for the file `"<<File.scopes[scopes_index]<<"` So Incremental Build is not supported! On this file\n"; // For Debugging only
                                can_increment_skip = false;
                            }
                        }
                        else {
                            Cook::Warning ("A Non-String type detected in the `" + increment_h699_file_path + "` file so deleting it via the rm command as it can cause bugs in incremental builds, Note: Do not modify `" + increment_h699_file_path + "` as it is generated by the Cook Build System.");
                            std::string command_to_delete_increment_h699_file = "rm -rf " + increment_h699_file_path;
                            std::system(command_to_delete_increment_h699_file.c_str());
                        }
                    }
                }


                // functionality implementation
                if (can_increment_skip){ // When increment build is possible then run this block , This will check if the executable exists or not, If the executable already exists then okay and skip this target, Othervise compile it!!!!
                    HELL6_99MO_TYPE get_out_value = File.get (File.scopes[scopes_index] + ".out");
                    std::string out = File.scopes[scopes_index].substr(0, File.scopes[scopes_index].find("."));
                    if (get_out_value.type != H699_UNIDEF){
                        if (get_out_value.type == "string"){
                            out = get_out_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `out` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // check if the file is present or not.
                    std::ifstream ifile_binary (bin + "/" + out);
                    if (ifile_binary.is_open()){ // file is open so ignore
                        can_increment_skip = true;
                    }
                    else { // the file is not open to build it.
                        can_increment_skip = false;
                    }
                }

                // Check the combine to register it self too!
                 // Check the combined files's timestamps
                // combine
                // std::cout << "Combine Check!\n";
                HELL6_99MO_TYPE get_combine_value = File.get (File.scopes[scopes_index] + ".combine");
                if (get_combine_value.type != H699_UNIDEF){
                    if (get_combine_value.type == "string"){
                        combine = {get_combine_value.string_value};
                    }
                    else if (get_combine_value.type == "array"){
                        combine = get_combine_value.array_value;
                    }
                    else {
                        Cook::Error("Syntax Error --> `combine` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                    }
                }
                // combine_add
                HELL6_99MO_TYPE get_combine_add_value = File.get (File.scopes[scopes_index] + ".combine_add");
                if (get_combine_add_value.type != H699_UNIDEF){
                    if (get_combine_add_value.type == "string"){
                        combine.push_back(get_combine_add_value.string_value);
                    }
                    else if (get_combine_add_value.type == "array"){
                        for (unsigned long long combine_add_index = 0;combine_add_index < get_combine_add_value.array_value.size();combine_add_index++){
                            combine.push_back(get_combine_add_value.array_value[combine_add_index]);
                        }
                    }
                    else {
                        Cook::Error("Syntax Error --> `combine_add` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                    }
                }
                // combine_rem
                HELL6_99MO_TYPE get_combine_rem_value = File.get (File.scopes[scopes_index] + ".combine_rem");
                if (get_combine_rem_value.type != H699_UNIDEF){
                    if (get_combine_rem_value.type == "string"){
                        for (unsigned long long combine_rem_index_combine = 0;combine_rem_index_combine < combine.size();combine_rem_index_combine++){
                            if (combine[combine_rem_index_combine] == get_combine_rem_value.string_value){
                                combine[combine_rem_index_combine] = "";
                            }
                        }
                    }
                    else if (get_combine_rem_value.type == "array"){
                        for (unsigned long long combine_rem_index = 0;combine_rem_index < get_combine_rem_value.array_value.size();combine_rem_index++){
                            for (unsigned long long combine_rem_index_combine = 0;combine_rem_index_combine < combine.size();combine_rem_index_combine++){
                                if (combine[combine_rem_index_combine] == get_combine_rem_value.array_value[combine_rem_index]){
                                    combine[combine_rem_index_combine] = "";
                                }
                            }
                        }
                    }
                    else {
                        Cook::Error("Syntax Error --> `combine_rem` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                    }
                }
                // std::cout << "FInal Check!\n";
                for (std::string file : combine){
                    bool is_timestamp_found = false;
                    for (std::string file_Name_In_Timestamps : combine_files_timestamps){
                        if (file == file_Name_In_Timestamps){
                            // std::cout << "Time stamps didn't matched\n";
                            can_increment_skip = false;
                            is_timestamp_found = true;
                            break;
                        }
                    }
                    if (not is_timestamp_found){
                        if (not Check_File_Timestamps(File, file)){ // Timestamps didn't matched
                            combine_files_timestamps.push_back (file);
                            can_increment_skip = false;
                            break;
                        }
                    }
                }
                

                // Check the increment back
                // std::cout << "Increment status for: "<<File.scopes[scopes_index]<<" is "<<can_increment_skip<<"\n"; // Only For Debugging
                if (can_increment_skip and not increment){
                    can_increment_skip = not can_increment_skip;
                }
                if (can_increment_skip){ // increment can be still skipped then skip it
                    continue;
                }
                else { // build it if increment can't be skipped - Also remember any new feature will go inside this else block , Do not touch the increment build logic which is on the top
                    HELL6_99MO_TYPE get_out_value = File.get (File.scopes[scopes_index] + ".out");
                    std::string out = File.scopes[scopes_index].substr(0, File.scopes[scopes_index].find("."));
                    if (get_out_value.type != H699_UNIDEF){
                        if (get_out_value.type == "string"){
                            out = get_out_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `out` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // show_logs logging_allowed
                    HELL6_99MO_TYPE get_show_logs_value = File.get (File.scopes[scopes_index] + ".show_logs");
                    if (get_show_logs_value.type != H699_UNIDEF){
                        // std::cout << "Logging !\n"; // for debugging only
                        if (get_show_logs_value.type == "bool"){
                            // std::cout << "Logs: " << get_show_logs_value.bool_value <<"\n"; // for debugging only 
                            logging_allowed = get_show_logs_value.bool_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `show_logs` requires a boolean value but the entered value was a Non-Boolean Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // bin
                    HELL6_99MO_TYPE get_bin_value = File.get (File.scopes[scopes_index] + ".bin");
                    if (get_bin_value.type != H699_UNIDEF){
                        if (get_bin_value.type == "string"){
                            bin = get_bin_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `bin` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    // compiler
                    HELL6_99MO_TYPE get_compiler_value = File.get (File.scopes[scopes_index] + ".compiler");
                    if (get_compiler_value.type != H699_UNIDEF){
                        if (get_compiler_value.type == "string"){
                            compiler = get_compiler_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `compiler` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    // compiler_arguments
                    HELL6_99MO_TYPE get_compiler_arguments_value = File.get (File.scopes[scopes_index] + ".compiler_arguments");
                    if (get_compiler_arguments_value.type != H699_UNIDEF){
                        if (get_compiler_arguments_value.type == "string"){
                            compiler_arguments = get_compiler_arguments_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `compiler_arguments` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    HELL6_99MO_TYPE get_compiler_parguments_value = File.get (File.scopes[scopes_index] + ".compiler_parguments");
                    if (get_compiler_parguments_value.type != H699_UNIDEF){
                        if (get_compiler_parguments_value.type == "string"){
                            compiler_parguments = get_compiler_parguments_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `compiler_parguments` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    // system
                    HELL6_99MO_TYPE get_system_value = File.get (File.scopes[scopes_index] + ".system");
                    if (get_system_value.type != H699_UNIDEF){
                        if (get_system_value.type == "string"){
                            system = get_system_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `system` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }



                    // psystem
                    HELL6_99MO_TYPE get_psystem_value = File.get (File.scopes[scopes_index] + ".psystem");
                    if (get_psystem_value.type != H699_UNIDEF){
                        if (get_psystem_value.type == "string"){
                            psystem = get_psystem_value.string_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `psystem` requires an string value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    // vector based utils
                    // include
                    HELL6_99MO_TYPE get_include_value = File.get (File.scopes[scopes_index] + ".include");
                    if (get_include_value.type != H699_UNIDEF){
                        if (get_include_value.type == "string"){
                            include = {get_include_value.string_value};
                        }
                        else if (get_include_value.type == "array"){
                            include = get_include_value.array_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `include` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // include_add
                    HELL6_99MO_TYPE get_include_add_value = File.get (File.scopes[scopes_index] + ".include_add");
                    if (get_include_add_value.type != H699_UNIDEF){
                        if (get_include_add_value.type == "string"){
                            include.push_back(get_include_add_value.string_value);
                        }
                        else if (get_include_add_value.type == "array"){
                            for (unsigned long long include_add_index = 0;include_add_index < get_include_add_value.array_value.size();include_add_index++){
                                include.push_back(get_include_add_value.array_value[include_add_index]);
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `include_add` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // include_rem
                    HELL6_99MO_TYPE get_include_rem_value = File.get (File.scopes[scopes_index] + ".include_rem");
                    if (get_include_rem_value.type != H699_UNIDEF){
                        if (get_include_rem_value.type == "string"){
                            for (unsigned long long include_rem_index_include = 0;include_rem_index_include < include.size();include_rem_index_include++){
                                if (include[include_rem_index_include] == get_include_rem_value.string_value){
                                    include[include_rem_index_include] = "";
                                }
                            }
                        }
                        else if (get_include_rem_value.type == "array"){
                            for (unsigned long long include_rem_index = 0;include_rem_index < get_include_rem_value.array_value.size();include_rem_index++){
                                for (unsigned long long include_rem_index_include = 0;include_rem_index_include < include.size();include_rem_index_include++){
                                    if (include[include_rem_index_include] == get_include_rem_value.array_value[include_rem_index]){
                                        include[include_rem_index_include] = "";
                                    }
                                }
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `include_rem` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }



                    // pkg_in
                    HELL6_99MO_TYPE get_pkg_in_value = File.get (File.scopes[scopes_index] + ".pkg_in");
                    if (get_pkg_in_value.type != H699_UNIDEF){
                        if (get_pkg_in_value.type == "string"){
                            forced_pkg_in_fetch = false;
                            pkg_in = {get_pkg_in_value.string_value};
                        }
                        else if (get_pkg_in_value.type == "array"){
                            forced_pkg_in_fetch = false;
                            pkg_in = get_pkg_in_value.array_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `pkg_in` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // pkg_in_add
                    HELL6_99MO_TYPE get_pkg_in_add_value = File.get (File.scopes[scopes_index] + ".pkg_in_add");
                    if (get_pkg_in_add_value.type != H699_UNIDEF){
                        if (get_pkg_in_add_value.type == "string"){
                            pkg_in.push_back(get_pkg_in_add_value.string_value);
                        }
                        else if (get_pkg_in_add_value.type == "array"){
                            for (unsigned long long pkg_in_add_index = 0;pkg_in_add_index < get_pkg_in_add_value.array_value.size();pkg_in_add_index++){
                                pkg_in.push_back(get_pkg_in_add_value.array_value[pkg_in_add_index]);
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `pkg_in_add` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // pkg_in_rem
                    HELL6_99MO_TYPE get_pkg_in_rem_value = File.get (File.scopes[scopes_index] + ".pkg_in_rem");
                    if (get_pkg_in_rem_value.type != H699_UNIDEF){
                        if (get_pkg_in_rem_value.type == "string"){
                            for (unsigned long long pkg_in_rem_index_pkg_in = 0;pkg_in_rem_index_pkg_in < pkg_in.size();pkg_in_rem_index_pkg_in++){
                                if (pkg_in[pkg_in_rem_index_pkg_in] == get_pkg_in_rem_value.string_value){
                                    pkg_in[pkg_in_rem_index_pkg_in] = "";
                                    forced_pkg_in_fetch = true;
                                    break;
                                }
                            }
                        }
                        else if (get_pkg_in_rem_value.type == "array"){
                            for (unsigned long long pkg_in_rem_index = 0;pkg_in_rem_index < get_pkg_in_rem_value.array_value.size();pkg_in_rem_index++){
                                for (unsigned long long pkg_in_rem_index_pkg_in = 0;pkg_in_rem_index_pkg_in < pkg_in.size();pkg_in_rem_index_pkg_in++){
                                    if (pkg_in[pkg_in_rem_index_pkg_in] == get_pkg_in_rem_value.array_value[pkg_in_rem_index]){
                                        pkg_in[pkg_in_rem_index_pkg_in] = "";
                                        forced_pkg_in_fetch = true;
                                        break;
                                    }
                                }
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `pkg_in_rem` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }



                    // lib
                    HELL6_99MO_TYPE get_lib_value = File.get (File.scopes[scopes_index] + ".lib");
                    if (get_lib_value.type != H699_UNIDEF){
                        if (get_lib_value.type == "string"){
                            lib = {get_lib_value.string_value};
                        }
                        else if (get_lib_value.type == "array"){
                            lib = get_lib_value.array_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `lib` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // lib_add
                    HELL6_99MO_TYPE get_lib_add_value = File.get (File.scopes[scopes_index] + ".lib_add");
                    if (get_lib_add_value.type != H699_UNIDEF){
                        if (get_lib_add_value.type == "string"){
                            lib.push_back(get_lib_add_value.string_value);
                        }
                        else if (get_lib_add_value.type == "array"){
                            for (unsigned long long lib_add_index = 0;lib_add_index < get_lib_add_value.array_value.size();lib_add_index++){
                                lib.push_back(get_lib_add_value.array_value[lib_add_index]);
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `lib_add` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // lib_rem
                    HELL6_99MO_TYPE get_lib_rem_value = File.get (File.scopes[scopes_index] + ".lib_rem");
                    if (get_lib_rem_value.type != H699_UNIDEF){
                        if (get_lib_rem_value.type == "string"){
                            for (unsigned long long lib_rem_index_lib = 0;lib_rem_index_lib < lib.size();lib_rem_index_lib++){
                                if (lib[lib_rem_index_lib] == get_lib_rem_value.string_value){
                                    lib[lib_rem_index_lib] = "";
                                    break;
                                }
                            }
                        }
                        else if (get_lib_rem_value.type == "array"){
                            for (unsigned long long lib_rem_index = 0;lib_rem_index < get_lib_rem_value.array_value.size();lib_rem_index++){
                                for (unsigned long long lib_rem_index_lib = 0;lib_rem_index_lib < lib.size();lib_rem_index_lib++){
                                    if (lib[lib_rem_index_lib] == get_lib_rem_value.array_value[lib_rem_index]){
                                        lib[lib_rem_index_lib] = "";
                                        break;
                                    }
                                }
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `lib_rem` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    

                    // combine
                    HELL6_99MO_TYPE get_combine_value = File.get (File.scopes[scopes_index] + ".combine");
                    if (get_combine_value.type != H699_UNIDEF){
                        if (get_combine_value.type == "string"){
                            combine = {get_combine_value.string_value};
                        }
                        else if (get_combine_value.type == "array"){
                            combine = get_combine_value.array_value;
                        }
                        else {
                            Cook::Error("Syntax Error --> `combine` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // combine_add
                    HELL6_99MO_TYPE get_combine_add_value = File.get (File.scopes[scopes_index] + ".combine_add");
                    if (get_combine_add_value.type != H699_UNIDEF){
                        if (get_combine_add_value.type == "string"){
                            combine.push_back(get_combine_add_value.string_value);
                        }
                        else if (get_combine_add_value.type == "array"){
                            for (unsigned long long combine_add_index = 0;combine_add_index < get_combine_add_value.array_value.size();combine_add_index++){
                                combine.push_back(get_combine_add_value.array_value[combine_add_index]);
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `combine_add` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }

                    // combine_rem
                    HELL6_99MO_TYPE get_combine_rem_value = File.get (File.scopes[scopes_index] + ".combine_rem");
                    if (get_combine_rem_value.type != H699_UNIDEF){
                        if (get_combine_rem_value.type == "string"){
                            for (unsigned long long combine_rem_index_combine = 0;combine_rem_index_combine < combine.size();combine_rem_index_combine++){
                                if (combine[combine_rem_index_combine] == get_combine_rem_value.string_value){
                                    combine[combine_rem_index_combine] = "";
                                }
                            }
                        }
                        else if (get_combine_rem_value.type == "array"){
                            for (unsigned long long combine_rem_index = 0;combine_rem_index < get_combine_rem_value.array_value.size();combine_rem_index++){
                                for (unsigned long long combine_rem_index_combine = 0;combine_rem_index_combine < combine.size();combine_rem_index_combine++){
                                    if (combine[combine_rem_index_combine] == get_combine_rem_value.array_value[combine_rem_index]){
                                        combine[combine_rem_index_combine] = "";
                                    }
                                }
                            }
                        }
                        else {
                            Cook::Error("Syntax Error --> `combine_rem` requires an string or an array value but the entered value was a Non-String Value! Error On File `" + File.scopes[scopes_index] + "` Please fix this error by changing the type to string.");
                        }
                    }


                    // for debugging only
                    // std::cout << "lib: "<<string_lib<<"\n";
                    // for (unsigned long long lib_index = 0;lib_index < lib.size();lib_index++){
                    //     std::cout << "lib: "<<lib[lib_index]<<"\n";
                    // }



                    // Executer Builder

                    std::vector <std::string> required_fetching_for_these_packages = {};
                    if (forced_pkg_in_fetch){
                        // std::cout << "Package will be force added!\n"; // for debugging only
                        pkg_in_libs_data.clear();
                        for (unsigned long long pkg_in_index = 0;pkg_in_index < pkg_in.size();pkg_in_index++){
                            if (pkg_in[pkg_in_index] == ""){
                                continue;
                            }
                            required_fetching_for_these_packages.push_back(pkg_in[pkg_in_index]);
                        }
                    }
                    else {
                        for (unsigned long long pkg_in_index = 0;pkg_in_index < pkg_in.size();pkg_in_index++){
                            bool is_already_present = false;
                            for (unsigned long long compare_pkg_in_index = 0;compare_pkg_in_index < compare_pkg_in.size();compare_pkg_in_index++){
                                if (pkg_in[pkg_in_index] == ""){
                                    continue;
                                }
                                if (compare_pkg_in[compare_pkg_in_index] == pkg_in[pkg_in_index]){
                                    is_already_present = true;
                                    break;
                                }
                            }


                            if (not is_already_present){
                                // std::cout << "Package will be fetched!\n"; // for debugging only
                                required_fetching_for_these_packages.push_back(pkg_in[pkg_in_index]);
                            }
                            // std::cout << "Package will not be fetched!\n"; // for debugging only
                            continue;
                        }
                    }

                    // fetch all the packages
                    for (unsigned long long fetch_packages_index = 0;fetch_packages_index < required_fetching_for_these_packages.size();fetch_packages_index++){
                        if (required_fetching_for_these_packages[fetch_packages_index] == ""){
                            continue;
                        }
                        else {
                            std::string command = "pkg-config --cflags --libs " + required_fetching_for_these_packages[fetch_packages_index] + " > " + communication_file_path + " 2>/dev/null";
                            if (not std::system (command.c_str())){ // command ran as std::system returns 0 and in C++ 0 is always false so not is just shortcut for == 0
                                compare_pkg_in.push_back(required_fetching_for_these_packages[fetch_packages_index]);
                                std::ifstream communication_file_read (communication_file_path);
                                std::string packages = "";
                                std::getline (communication_file_read, packages);
                                pkg_in_libs_data += packages + " ";
                            }
                            else {
                                Cook::Error("The package `" + required_fetching_for_these_packages[fetch_packages_index] + "` was not found on your system via pkg-config, Please make sure that the package name is correct and it is correctly installed and the Cook Build System has the permissions to access it.\n\n\t As This is a critical error so the compilation for your file `" + File.scopes[scopes_index] + "` was terminated before calling the executer to call your compiler. The Rest of the recipe was also terminated.");
                            }
                        }
                    }
                    required_fetching_for_these_packages = {};

                    // std::cout << pkg_in_libs_data << "\n"; // for debugging only

                    std::string include_arguments = "";
                    for (unsigned long long include_index = 0;include_index < include.size();include_index++){
                        if (include[include_index] == "" or include_arguments.find(include[include_index]) != std::string::npos){
                            continue;
                        }
                        include_arguments += "-I" + include[include_index] + " ";
                    }

                    // std::cout << "Include: "<<include_arguments << "\n"; // for debugging only


                    std::string lib_arguments = "";
                    for (unsigned long long lib_index = 0;lib_index < lib.size();lib_index++){
                        if (lib[lib_index] == "" or lib_arguments.find(lib[lib_index]) != std::string::npos){
                            continue;
                        }
                        lib_arguments += "-l" + lib[lib_index] + " ";
                    }

                    // std::cout << "Lib: "<<lib_arguments << "\n"; // for debugging only


                    std::string combine_arguments = "";
                    for (unsigned long long combine_index = 0;combine_index < combine.size();combine_index++){
                        // std::cout << combine[combine_index]<<"\n"; // for debugging only
                        if (combine[combine_index] == "" or combine_arguments.find(combine[combine_index]) != std::string::npos){
                            continue;
                        }
                        combine_arguments += combine[combine_index] + " ";
                    }

                    // std::cout << "Combine: "<<combine_arguments << "\n"; // for debugging only

                    if (not can_build_skip){
                        // Build Commands for the executor to execute
                        std::string command = psystem + "\n" + compiler + " " + compiler_parguments + " " + File.scopes[scopes_index] + " " + combine_arguments + "-o " + bin + "/" + out + " " + include_arguments + lib_arguments + pkg_in_libs_data + " " + compiler_arguments + "\n" + system;
                        // std::cout << "Compiler command: "<<command<<"\n"; // for debugging only
                        std::filesystem::create_directory(bin);
                        source_file_and_commands.push_back({File.scopes[scopes_index], command});
                    }
                    else {
                        can_build_skip = false;
                    }
                }
            }
        }
    }
}