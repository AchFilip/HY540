#pragma once

#include "./DebugMessageInterface.h"
#include "../TreeTags.h"
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <functional>

#define _CONTINUE "a"
#define STEP_OVER "b"
#define STEP_INTO "c"

class SinDebugger
{
private:
    DebugMessageInterface dmi;
    std::vector<int> breakpoints;

public:
    static bool isDebug;

    //---------------------- FOR DEBUGGER PROCESS ------------------------
    SinDebugger() = default;

    void InitDebuggerEnd()
    {
        // Set my read channel on DMI
        dmi.SetReadChannel(DEBUGGER_CHANNEL);
        // Receive breakpoints and send signal to interpreter
        FakeInputAndSignalBreakpoints();
        // Wait for breakpoint
        while (true)
        {
            std::cout << "[Debugger] Waiting for breakpoint..." << std::endl;
            std::string message = dmi.Read();
            std::cout << "[Debugger] From interpreter: " << message << std::endl;
            std::cout << "Breakpoint found! Choose action:\na: Continue\nb: Step over\nc: Step into" << std::endl;
            std::string user_input;
            std::cin >> user_input;
            // filter input
            dmi.Write(user_input);
        }
    }

    void FakeInputAndSignalBreakpoints()
    {
        // Send signal that breakpoint thingy is done.
        dmi.Write("3 7\0");
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
    //--------------------------------------------------------------------

    //---------------------- FOR INTERPRETER PROCESS ---------------------
private:
    int lastLineDebuged = -1;

    std::function<bool(Object &node)> shouldReadCommand;
    std::map<std::string, std::function<bool(Object &node)>> shouldReadCommandDispatcher;

public:
    void InitInterpreterEnd()
    {
        shouldReadCommandDispatcher.insert({"continue",
                                            [=](Object &node)
                                            {
                                                return SinDebugger::isDebug &&
                                                       IsBreakpoint(node[AST_TAG_LINE_KEY]->ToNumber()) &&
                                                       lastLineDebuged != node[AST_TAG_LINE_KEY]->ToNumber() && // Maybe use a stack to represent recursive lines?
                                                       node[AST_TAG_TYPE_KEY]->ToString() == AST_TAG_EXPR;
                                            }

        });
        shouldReadCommand = shouldReadCommandDispatcher["continue"];
    }

    void ReadBreakpoints()
    {
        dmi.SetReadChannel(INTERPRETER_CHANNEL);
        // Wait for input of breakpoints
        std::string message = dmi.Read();
        std::vector<std::string> breakpointsSplitted = dmi.SplitMessage(message, " ");
        for (auto &it : breakpointsSplitted)
        {
            // std::cout << "[Interpreter] it: " << it << std::endl;
            breakpoints.push_back(stoi(it));
        }
    }

    bool ShouldReadCommand(Object &node)
    {

        return shouldReadCommand(node);
    }

    void ReadCommand(Object &node)
    {
        // Mark debuged line
        lastLineDebuged = node[AST_TAG_LINE_KEY]->ToNumber();
        // Inform debugger that breakpoint was found
        dmi.Write("Breakpoint found: " + std::to_string((int)node[AST_TAG_LINE_KEY]->ToNumber()));
        // Ask for instructions on how to proceed;
        std::string message = dmi.Read();
    }
    //--------------------------------------------------------------------
};
bool SinDebugger::isDebug = false;