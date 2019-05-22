#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

#define RM_FILE_READ         (1 << 0) //can read to file
#define RM_FILE_WRITE        (1 << 1) //can write to file
#define RM_FILE_NO_ACCESS    (1 << 2) //can't do anything to file. NOTE: useless I gues.
#define RM_FILE_EXEC         (1 << 3) //can execute the file

#define RM_FILE_CHNG_SHARED  (1 << 4) //change's affect to file
#define RM_FILE_CHNG_PRIVATE (1 << 5) //copy on write

#define RM_FILE_DR (RM_FILE_READ | RM_FILE_CHNG_SHARED)
#define RM_FILE_DW (RM_FILE_WRITE | RM_FILE_CHNG_SHARED)

struct resource_handle
{

    uint64_t size;
    void* source;

#ifdef _WIN32
    HANDLE file_handle;
    HANDLE mmap_handle;
#endif
};

//singleton that stores information about local environmet
class resource_manager
{
public:
    static void init(); //Initialises context and retrieves PID path info.
    static void clear_resource_cache();   //self explanitory
    static std::string get_exe_path();    //path to executabe (file)
	static std::string get_res_path();    //path to resources (folder)
	static std::string get_folder_path(); //path to executable (folder) NOTE(Ethan): kinda useless tbh

    //mmaps file, and adds to resource cache
    //
    //first param is absolute path,
    //second param is file access bitflags and change functionality.
    static resource_handle load_file(std::string, uint32_t);
private:
    static std::string folder_path;
    static std::string exe_path;
    static std::string res_path;
    static std::unordered_map<std::string, resource_handle> cache;
};
