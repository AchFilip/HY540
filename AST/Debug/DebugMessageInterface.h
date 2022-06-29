#pragma once
#define MSGSIZE 100

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <fcntl.h> /* For O_RDWR */
#include <pthread.h>
#include <cstdlib>

#define INTERPRETER_CHANNEL 0
#define DEBUGGER_CHANNEL 1

class DebugMessageInterface
{
private:
    // Pipe and buffer for Debugger - Interpreter communication
    char inbuf[MSGSIZE];
    int readChannel = -1;

public:
    static int pipes[2][2];

    DebugMessageInterface()
    {
    }

    static void Init()
    {
        pipe(pipes[0]);
        pipe(pipes[1]);
    }

    std::string Read()
    {
        int nbytes = read(pipes[readChannel][0], inbuf, MSGSIZE);
        return std::string(inbuf);
    }
    void Write(std::string message)
    {
        int messageSize = (message.size() < MSGSIZE) ? message.size() : MSGSIZE;
        write(pipes[abs(readChannel - 1) / 1][1], message.c_str(), messageSize + 1);
    }

    // Get/Set
    void SetReadChannel(int readChannel)
    {
        this->readChannel = readChannel;
    }

    // Util Functions
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
int DebugMessageInterface::pipes[2][2];
