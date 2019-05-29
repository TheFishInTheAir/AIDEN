#include <log.h>

#include <mutex>
#include <iostream>

#define ERROR_EXIT_CODE 0xBAD11BAD

#ifdef _WIN32
#include <windows.h>
#endif

#define ENABLE_TURBO_VERBOSE

namespace Log
{
    std::mutex canPrint;

    void RawOut(std::string m)
    {
#ifdef FILE_LOGGING //TODO: implement
        //static std::string logPos = ResourceUtil::getResPath("../GREEN_ENGINE_LOG.txt");
        //static std::ofstream out_log(logPos);
        //out_log << m;
#endif

        std::cout << m;
    }

    void print(std::string msg)
    {
        RawOut(msg);
    }

    void print(std::string msg, MessageType::type t) //super messy shhh.
    {

        //canPrint.lock(); //TODO: reimplement print mutex
#ifndef ENABLE_TURBO_VERBOSE
        if(t==MessageType::Turbo_Verbose)
        {
            return;
        }
#endif
#ifndef NO_COLOUR

#ifdef _WIN32

        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        switch(t)
        {


        case MessageType::Warning:          // bright red
            SetConsoleTextAttribute(hConsole, 12);
            RawOut(msg);

            SetConsoleTextAttribute(hConsole, 15);
            break;
        case MessageType::Message:          // white
            RawOut(msg);
            break;
        case MessageType::Success:          // green
            SetConsoleTextAttribute(hConsole, 2);
            RawOut(msg);
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case MessageType::Error:            // dark red
            SetConsoleTextAttribute(hConsole, 4);
            RawOut(msg);
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case MessageType::Debug:            // purple
            SetConsoleTextAttribute(hConsole, 5);
            RawOut(msg);
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case MessageType::Verbose:          // grey
            SetConsoleTextAttribute(hConsole, 7);
            RawOut(msg);
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case MessageType::Turbo_Verbose:    // dark grey
            SetConsoleTextAttribute(hConsole, 8);
            RawOut(msg);
            SetConsoleTextAttribute(hConsole, 15);
            break;
        default:
            RawOut(msg);
        }

#else

        switch(t)
        {


        case MessageType::Warning:          // yellow
            RawOut("\033[33m"+msg+"\033[0m");

            break;
        case MessageType::Message:          // white
            RawOut("\033[0m"+msg+"\033[0m");

            break;
        case MessageType::Success:          // green
            RawOut("\033[32;1m"+msg+"\033[0m");

            break;
        case MessageType::Error:            // dark red
            RawOut("\033[31;1;7m"+msg+"\033[0m");

            break;
        case MessageType::Debug:            // purple
            RawOut("\033[35m"+msg+"\033[0m");

            break;
        case MessageType::Verbose:          // grey
            RawOut("\033[37m"+msg+"\033[0m");

            break;
        case MessageType::Turbo_Verbose:    // dark grey
            RawOut("\033[90m"+msg+"\033[0m");

            break;
        default:
            RawOut(msg);
        }

#endif

#else
        RawOut(msg);
#endif
        //canPrint.unlock(); //TODO: reimplement print mutex
    }

const std::string DEFAULT_TAG = "AIDEN";

		void msg(std::string t)
		{
			msg(DEFAULT_TAG, t);
		}

		void msg(std::string tag, std::string t)
		{
			print("M  [" + tag + "]: " + t+'\n', MessageType::Message);
		}



		void scc(std::string msg)
		{
			scc(DEFAULT_TAG, msg);
		}

        void scc(std::string tag, std::string msg)
		{
			print("S  [" + tag + "]: " + msg+'\n', MessageType::Success);
		}



		void dbg(std::string msg)
		{
			dbg(DEFAULT_TAG, msg);
		}

        void dbg(std::string tag, std::string msg)
		{
			print("D  [" + tag + "]: " + msg+'\n', MessageType::Debug);
		}



		void wrn(std::string msg)
		{
			wrn(DEFAULT_TAG, msg);
        }

        void wrn(std::string tag, std::string msg)
		{
			print("W  [" + tag + "]: " + msg+'\n', MessageType::Warning);
		}



		void err(std::string msg)
		{
			err(DEFAULT_TAG, msg);
		}

        void err(std::string tag, std::string msg)
		{
            print("E  [" + tag + "]: " + msg+'\n', MessageType::Error);
		}



		void critErr(std::string msg)
        {
            critErr(DEFAULT_TAG, msg);
        }

		void critErr(std::string tag, std::string msg) //TODO: get stack trace and stuff
        {
            print("CE [" + tag + "]: " + msg+'\n', MessageType::Error);
            std::exit(ERROR_EXIT_CODE);
        }



		void vrb(std::string msg)
        {
            vrb(DEFAULT_TAG, msg);
        }

		void vrb(std::string tag, std::string msg)
        {
            print("V  [" + tag + "]: " + msg+'\n', MessageType::Verbose);
        }



		void tVrb(std::string msg)
        {
            tVrb(DEFAULT_TAG, msg);
        }

		void tVrb(std::string tag, std::string msg)
        {
            print("TV [" + tag + "]: " + msg+'\n', MessageType::Turbo_Verbose);
        }


}
