#include <resource_manager.h>
#include <iostream>

#ifndef WIN32
#include <libproc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


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
        printf("bad stuff '%s'  .\n", path.c_str()); //TODO: @LOG
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


void _os_munmap(resource_handle h)
{
    printf("MUNMAP UNIMPLEMENTED IN WINDOWS\n");


}

resource_handle _os_mmap(std::string path, uint32_t flags) // osx
{
    resource_handle res_handle;

    DWORD access, share, page_access;
    access = share = page_access = 0;

    if(flags & RM_FILE_READ) //if more than one
        access |= GENERIC_READ;

    if(flags & RM_FILE_WRITE) //if more than one
        access |= GENERIC_WRITE;

    if(flags & RM_FILE_EXEC) //if more than one
        access |= GENERIC_EXECUTE;

    if(flags & RM_FILE_CHNG_SHARED) //if more than one
        share |= FILE_SHARE_WRITE | FILE_SHARE_READ;


    if (flags & (RM_FILE_WRITE | RM_FILE_READ | RM_FILE_EXEC))
        page_access |= PAGE_EXECUTE_READWRITE;
    else if (flags & (RM_FILE_EXEC | RM_FILE_READ))
        page_access |= PAGE_EXECUTE_READ;
    else if (flags & (RM_FILE_WRITE | RM_FILE_READ))
        page_access |= PAGE_READWRITE;
    else if (flags & (RM_FILE_READ))
        page_access |= PAGE_READONLY;


    res_handle.file_handle = CreateFile(
        path.c_str(),
        access,
        share,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    res_handle.mmap_handle = CreateFileMapping(
        res_handle.mmap_handle,
        NULL,
        page_access,
        0,
        0,
        path.c_str());

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
