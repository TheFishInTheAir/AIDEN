#include <resource_manager.h>
#include <iostream>

#ifndef WIN32
#include <libproc.h>
#include <unistd.h>

#define FILE_SEP '/'

//this just gets the path to the binary
std::string _get_os_pid_bin_path()
{
    std::string path;

    int ret;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    pid = getpid();
    ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));

    if(ret <= 0)
    {
        std::cout << "Couldn't get PID path! Exiting.\n" << std::endl;
        exit(0);
    }
    path = pathbuf;
    return path;
}
#else
#include <windows.h>
#define FILE_SEP '\\'

//this just gets the path to the binary
std::string _get_os_pid_bin_path()
{

	std::string path;

	HMODULE hModule = GetModuleHandleW(NULL);

	WCHAR tpath[260];
	GetModuleFileNameW(hModule, tpath, 260);

	char ch[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, tpath, -1, ch, 260, &DefChar, NULL);

	path = std::string(ch);

	return path;
}
#endif


std::string resource_manager::folder_path;
std::string resource_manager::exe_path;
std::string resource_manager::res_path;

void resource_manager::init()
{
    static bool is_initialised = false;

    if(!is_initialised)
    {
        is_initialised = true;

        //get information
        exe_path = _get_os_pid_bin_path();

		//go back to slashes
		{
			std::string temp = exe_path.substr(0, exe_path.find_last_of(FILE_SEP));
			folder_path = temp.substr(0, temp.find_last_of(FILE_SEP));
		}

		//append /res/ to the path
		res_path = folder_path + FILE_SEP + "res" + FILE_SEP;

        std::cout << "PID path: " << exe_path << std::endl;
    }
}


std::string resource_manager::get_exe_path()
{
	init();
	return exe_path;
}

std::string resource_manager::get_res_path()
{
	init();
	return res_path;
}

std::string resource_manager::get_folder_path()
{
	init();
	return folder_path;
}
