#include <loader.h>

#include <log.h>
#include <resource_manager.h>

#include <json.hpp>
#include <string>

namespace Loader
{
    const std::string LOG_TAG = "Loader";

    //This function is just to test if the json parser is working. Reads a file then prints it.
    void json_print_test(std::string abs_path)
    {
        nlohmann::json j = nlohmann::json::parse((char*) resource_manager::load_file(abs_path, RM_FILE_DR).source);
        Log::dbg(LOG_TAG, "\nJson Dump:\n" + j.dump(4));
    }

}
