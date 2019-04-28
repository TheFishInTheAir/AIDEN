#pragma once
#include <string>

//singleton that stores information about local environmet
class resource_manager
{
public:
    static void init();
    static std::string get_exe_path();
	static std::string get_res_path();
	static std::string get_folder_path();
private:
    static std::string folder_path;
    static std::string exe_path;
    static std::string res_path;

};
