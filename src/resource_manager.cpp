#include <resource_manager.h>

#include <log.h>
#include <iostream>


#ifndef WIN32
#include <libproc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOG_TAG "POSIX_RES_MNGR"

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
        Log::critErr("Couldn't get PID path! Exiting.");

    path = pathbuf;
    return path;
}


uint64_t _get_file_size(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

void _os_munmap(resource_handle h)
{
    assert(munmap(h.source, h.size) == 0);
}

resource_handle _os_mmap(std::string path, uint32_t flags) // osx
{
    int permission_flags = 0;
    int change_flags = 0;

    if(flags & RM_FILE_READ) //if more than one
        permission_flags |= PROT_READ;

    if(flags & RM_FILE_WRITE) //if more than one
        permission_flags |= PROT_WRITE;

    if(flags & RM_FILE_NO_ACCESS) //if more than one
        permission_flags |= PROT_NONE;

    if(flags & RM_FILE_EXEC) //if more than one
        permission_flags |= PROT_EXEC;

    if(flags & RM_FILE_CHNG_SHARED) //if more than one
        change_flags |= MAP_SHARED;

    if(flags & RM_FILE_CHNG_PRIVATE) //if more than one
        change_flags |= MAP_PRIVATE;

    //if(!permission_flags) // no flags
    assert(permission_flags); //TODO: make error message
    assert(change_flags);     //TODO: make error message

    int fd = open(path.c_str(), O_RDONLY, 0);
    if(fd == -1)
    {
        Log::err(LOG_TAG, "Couldn't open file '" + path + "'.");
        return {0, nullptr};
    }

    uint64_t file_size = _get_file_size(path.c_str());
    void* data = mmap(nullptr, file_size,
                      permission_flags, change_flags, fd, 0);


    return {file_size, data};

}


#else
#include <windows.h>
#define FILE_SEP '\\'
#define LOG_TAG "WIN32_RES_MNGR"


void _os_munmap(resource_handle h)
{
    Log::critErr("MUNMAP UNIMPLEMENTED IN WINDOWS\n");


}

resource_handle _os_mmap(std::string path, uint32_t flags) // osx
{
    int err = 0;
    resource_handle res_handle = {0};
    Log::tVrb(LOG_TAG, "Mapping File: '"+ path + "'");
    DWORD access, share, page_access, map_access;
    access = share = page_access = map_access = 0;

    if(flags & RM_FILE_READ) //if more than one
        access |= GENERIC_READ;

    if(flags & RM_FILE_WRITE) //if more than one
        access |= GENERIC_WRITE | GENERIC_READ;

    if(flags & RM_FILE_EXEC) //if more than one
        access |= GENERIC_EXECUTE;

    if(flags & RM_FILE_CHNG_SHARED) //if more than one
        share |= (access & GENERIC_WRITE ? FILE_SHARE_WRITE : 0) |
                 (access & GENERIC_READ  ? FILE_SHARE_READ  : 0);


    if (flags & RM_FILE_WRITE &&
        flags & RM_FILE_READ &&
        flags & RM_FILE_EXEC)
    {
        page_access |= PAGE_EXECUTE_READWRITE;
        map_access  |= FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE;
    }
    else if (flags & RM_FILE_EXEC &&
             flags & RM_FILE_READ)
    {
        page_access |= PAGE_EXECUTE_READ;
        map_access  |= FILE_MAP_READ | FILE_MAP_EXECUTE;
    }
    else if (flags & RM_FILE_WRITE)
    {
        page_access |= PAGE_READWRITE;
        map_access  |= FILE_MAP_ALL_ACCESS;

    }
    else if (flags & RM_FILE_READ)
    {
        page_access |= PAGE_READONLY;
        map_access  |= FILE_MAP_READ;
    }


    res_handle.file_handle = CreateFile(
        path.c_str(),
        access,
        share,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if((err = GetLastError()) != ERROR_ALREADY_EXISTS && err != ERROR_SUCCESS)
        Log::err(LOG_TAG, "Failed to create file handle during memmap. Error: "+std::to_string(err));
    if(res_handle.file_handle == nullptr)
        Log::critErr(LOG_TAG, "Failed to create file handle during memmap.");

    if(err == ERROR_ALREADY_EXISTS)
        SetLastError(ERROR_SUCCESS);


    res_handle.size = GetFileSize(res_handle.file_handle, NULL);

    if(res_handle.size == INVALID_FILE_SIZE)
        Log::err(LOG_TAG, "Invalid File Size Error.");
    if((err = GetLastError()))
        Log::err(LOG_TAG, "Failed to get file size. Error: "+std::to_string(err));
    //Log::dbg(LOG_TAG, "Size: "+std::to_string(res_handle.size));



    res_handle.mmap_handle = CreateFileMapping(
        res_handle.file_handle,
        NULL,
        page_access,
        0,
        res_handle.size,
        NULL);

    if(res_handle.mmap_handle == nullptr)
        Log::critErr(LOG_TAG, "Failed to create mapping handle during memmap.");
    //if((err = GetLastError()) != ERROR_ALREADY_EXISTS && err != ERROR_SUCCESS)
    if((err = GetLastError()))
        Log::err(LOG_TAG, "Failed to create mapping handle during memmap. Error: "+std::to_string(err));

    res_handle.source = MapViewOfFile(res_handle.mmap_handle,
                                      map_access,
                                      0,
                                      0,
                                      0);

    if((err = GetLastError()))
        Log::err(LOG_TAG, "Failed to map view of file. Error: "+std::to_string(err));


    return res_handle;

}

//this just gets the path to the binary
std::string _get_os_pid_bin_path() //windows
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
std::unordered_map<std::string, resource_handle> resource_manager::cache;

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
			folder_path = temp.substr(0, temp.find_last_of(FILE_SEP)+1);
		}

		//append /res/ to the path
		res_path = folder_path + "res" + FILE_SEP;

        Log::vrb(LOG_TAG, "PID    Path: " + exe_path);
        Log::vrb(LOG_TAG, "RES    Path: " + res_path);
        Log::vrb(LOG_TAG, "FOLDER Path: " + folder_path);

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

void resource_manager::clear_resource_cache()
{
    std::unordered_map<std::string, resource_handle>::iterator it;
    for ( it = cache.begin(); it != cache.end(); it++ )
    {
        _os_munmap(it->second);
    }
    cache.clear();
}

resource_handle resource_manager::load_file(std::string path, uint32_t flags)
{
    if(!cache.count(path))
    {
        resource_handle h = _os_mmap(path, flags);
        cache[path] = h;
        return h;
    }
    else
        return cache[path];
}
