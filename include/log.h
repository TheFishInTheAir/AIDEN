#pragma once

#include <string>

//NOTE(Ethan): most of this is just coppied from the Game Engine
namespace Log
{

    namespace MessageType
    {
        typedef enum
        {
            Warning,
            Message,
            Success,
            Error,
            Debug,
            Verbose,
            Turbo_Verbose
        }type;
    }

    void print(std::string msg);
    void print(std::string msg, MessageType::type);

    //regular message.
    void msg(std::string);
    void msg(std::string, std::string);

    //success message.
    void scc(std::string);
    void scc(std::string, std::string);

    //debug message.
    void dbg(std::string);
    void dbg(std::string, std::string);

    //warning message.
    void wrn(std::string);
    void wrn(std::string, std::string);

    //error message.
    void err(std::string);
    void err(std::string, std::string);

    //critical error message (will exit program and print stack trace when called).
    void critErr(std::string);
    void critErr(std::string, std::string);

    //verbose message.
    void vrb(std::string);
    void vrb(std::string, std::string);

    //t u r b o verbose message.
    void tVrb(std::string);
    void tVrb(std::string, std::string);

}
