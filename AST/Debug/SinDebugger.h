#pragma once

#include "./DebugMessageInterface.h"
#include <vector>
#include <unistd.h>

class SinDebugger
{
private:
    DebugMessageInterface *dmi;
    std::vector<int> breakpoints;

public:
    static bool isDebug;

    SinDebugger()
    {
        dmi = new DebugMessageInterface();
        // Receive breakpoints and send signal to interpreter
        InputAndSignalBreakpoints();
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
                breakpointsMessage += line + " ";
                breakpoints.push_back(line);
            }
        }

        // Send signal that breakpoint thingy is done.
        dmi->Write(breakpointsMessage);
    }
};