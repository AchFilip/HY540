#pragma once

#include "./DebugMessageInterface.h"
#include <vector>
#include <unistd.h>
#include<algorithm>


class SinDebugger
{
private:
    DebugMessageInterface dmi;
    std::vector<int> breakpoints;

public:
    static bool isDebug;

    SinDebugger()
    {
        // Set my read channel on DMI
        dmi.SetReadChannel(DEBUGGER_CHANNEL);
        // Receive breakpoints and send signal to interpreter
        FakeInputAndSignalBreakpoints();
        // Wait for breakpoint
        while(true){
            std::cout << "[Debugger] Waiting for breakpoint..." << std::endl;
            std::string message = dmi.Read();
            dmi.Write("ACK");
            std::cout << "[Debugger] From interpreter: " << message << std::endl;
        }
    }

    void FakeInputAndSignalBreakpoints(){
        // Send signal that breakpoint thingy is done.
        dmi.Write("2 7\0");
    }
    void InputAndSignalBreakpoints()
    {
        // Create breakpoints
        std::string breakpointsMessage = "";
        std::cout << "Write number of line to add breakpoint. Write \'-1\' to continue..." << std::endl;
        while (true)
        {
            int line;
            std::cin >> line;
            if (line == -1)
                break;
            else
            {
                breakpointsMessage += std::to_string(line) + " ";
                breakpoints.push_back(line);
            }
        }
        // Substring the last " " character;
        breakpointsMessage = breakpointsMessage.substr(0, breakpointsMessage.size() - 1);

        // Send signal that breakpoint thingy is done.
        dmi.Write(breakpointsMessage);
    }
    bool IsBreakpoint(int line)
    {
        if (std::find(breakpoints.begin(), breakpoints.end(), line) != breakpoints.end())
            return true;
        else
            return false;
    }
};
bool SinDebugger::isDebug = false;