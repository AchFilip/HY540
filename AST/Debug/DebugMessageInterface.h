#pragma once
#define MSGSIZE 100

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>

class DebugMessageInterface
{
private:
    // Pipe and buffer for Debugger - Interpreter communication
    char inbuf[MSGSIZE];
    static int p[2];

public:
    
    DebugMessageInterface()
    {
    }

    static void InitPipe(){
        pipe(p);
    }

    std::string Read()
    {
        int nbytes = read(p[0], inbuf, MSGSIZE);
        return std::string(inbuf);
    }
    void Write(std::string message)
    {
        int messageSize = (message.size() < MSGSIZE) ? message.size() : MSGSIZE;
        write(p[1], message.c_str(), messageSize);
    }

    // for string delimiter
    std::vector<std::string> SplitMessage(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }
};