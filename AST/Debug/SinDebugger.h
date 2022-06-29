#pragma once

#include "./DebugMessageInterface.h"
#include "../TreeTags.h"
#include "../ValueStack.h"
#include "../LibraryFunctions/Utilities.h"
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
        InputAndSignalBreakpoints();
        // Wait for breakpoint
        while (true)
        {
            // std::cout << "[Debugger] Waiting for breakpoint..." << std::endl;
            std::string message = dmi.Read();
            std::cout << "===========================================================" << std::endl;
            std::cout << "Execution stopped at line: " << message << std::endl;
            std::cout << "Choose action:\n\
                            a: Continue: \'continue\'\n\
                            b: Step over: \'step_over\'\n\
                            c: Step into: \'step_into\'\n\
                            d: Add breakpoints: \'add_breakpoints\'\n\
                            d: Write your own code to execute!"
                      << std::endl;

            // Due to fork and shit, getline does block the process.
            // For some other reason, cin does block the process.
            // For some other reason, cin does not detect \n or \0
            // So... combine the two!
            std::string first_word;
            std::string user_input;

            std::cin >> first_word;
            std::getline(std::cin, user_input);

            user_input = first_word + user_input;

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
        if(breakpointsMessage != "")    
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
    int lastLineDebugged = -1;
    int lastEnvStackSize = 1;
    Object* lastEnvStackDebugged = nullptr;
    ValueStack *envStack;

    std::function<bool(Object &node)> shouldReadCommand;
    std::map<std::string, std::function<bool(Object &node)>> shouldReadCommandDispatcher;

public:
    void InitInterpreterEnd(ValueStack *envStack)
    {
        // Set reference to envstack
        this->envStack = envStack;
        // Initialize dispatcher
        shouldReadCommandDispatcher.insert({"continue",
                                            [=](Object &node)
                                            {
                                                return (
                                                    SinDebugger::isDebug &&
                                                    IsBreakpoint(node[AST_TAG_LINE_KEY]->ToNumber()) &&
                                                    (
                                                        lastLineDebugged != node[AST_TAG_LINE_KEY]->ToNumber() || // Maybe use a stack to represent recursive lines?
                                                        lastEnvStackDebugged != envStack->Top().ToObject_NoConst()
                                                    ) &&
                                                    node[AST_TAG_TYPE_KEY]->ToString() == AST_TAG_EXPR);
                                            }

        });
        shouldReadCommandDispatcher.insert({"step_over",
                                            [=](Object &node)
                                            {
                                                return (
                                                    SinDebugger::isDebug &&
                                                    node[AST_TAG_TYPE_KEY]->ToString() == AST_TAG_EXPR &&
                                                    (
                                                        (
                                                            node[AST_TAG_LINE_KEY]->ToNumber() > lastLineDebugged && 
                                                            envStack->Size() == lastEnvStackSize
                                                        ) ||
                                                        envStack->Size() == lastEnvStackSize - 1
                                                    ) ||
                                                    shouldReadCommandDispatcher["continue"](node)
                                                );
                                            }});
        shouldReadCommandDispatcher.insert({"step_into",
                                            [=](Object &node)
                                            {
                                                return (
                                                        SinDebugger::isDebug &&
                                                        lastLineDebugged != node[AST_TAG_LINE_KEY]->ToNumber() && // Maybe use a stack to represent recursive lines?
                                                        node[AST_TAG_TYPE_KEY]->ToString() == AST_TAG_EXPR &&
                                                        envStack->Size() == lastEnvStackSize + 1) ||
                                                        shouldReadCommandDispatcher["step_over"](node);
                                            }});
        shouldReadCommandDispatcher.insert({"add_breakpoints",
                                            [=](Object &node)
                                            {
                                                return true;
                                                ;
                                            }});

        shouldReadCommand = shouldReadCommandDispatcher["continue"];
    }

    void ReadBreakpoints(std::string _message)
    {
        std::string message;
        dmi.SetReadChannel(INTERPRETER_CHANNEL);
        // Wait for input of breakpoints
        if (_message != "")
        {
            message = _message;
        }
        else
        {
            message = dmi.Read();
        }
        
        if(message != "")
        {
            std::vector<std::string> breakpointsSplitted = dmi.SplitMessage(message, " ");
            for (auto &it : breakpointsSplitted)
            {
                // std::cout << "[Interpreter] it: " << it << std::endl;
                breakpoints.push_back(stoi(it));    
            }
        }
    }

    bool ShouldReadCommand(Object &node)
    {

        return shouldReadCommand(node);
    }

    void ReadCommand(Object &node)
    {
        // Mark debuged line
        lastLineDebugged = node[AST_TAG_LINE_KEY]->ToNumber();
        lastEnvStackDebugged = envStack->Top().ToObject_NoConst();
        lastEnvStackSize = envStack->Size();
        // Inform debugger that breakpoint was found
        dmi.Write(std::to_string(node[AST_TAG_LINE_KEY]->ToNumber()));
        // Ask for instructions on how to proceed;
        std::string message = dmi.Read();

        if (shouldReadCommandDispatcher.count(message))
        {

            shouldReadCommand = shouldReadCommandDispatcher[message];
        }
        else
        {
            std::vector<std::string> messageSplitted = dmi.SplitMessage(message, " ");
            if (!strcmp(messageSplitted[0].c_str(), "add_breakpoints"))
            {
                // InputAndSignalBreakpoints();
                // std::cout << message << std::endl;
                std::string breakpointsMessage = "";
                for (int i = 1; i < messageSplitted.size(); i++)
                    breakpointsMessage += messageSplitted[i] + " ";
                breakpointsMessage = breakpointsMessage.substr(0, breakpointsMessage.size() - 1);

                // dmi.Write(breakpointsMessage);
                ReadBreakpoints(breakpointsMessage);
                return;
            }
            else
            {
                // Change shouldAsk debugger to continue, because step into will stop at next eval
                std::function<bool(Object&)> prevShouldReadCommand = shouldReadCommand;
                shouldReadCommand = shouldReadCommandDispatcher["continue"];
                // Utilities::eval pops initial library function scope, so push a fake one, and then pop it
                auto &evalScope = *(*envStack->Top().ToObject_NoConst())[TAIL_SCOPE_KEY]->ToObject_NoConst();
                envStack->Push(*new Value());
                Utilities::eval(evalScope, message);
                envStack->Pop();
                shouldReadCommand = prevShouldReadCommand;
            }

            ReadCommand(node);
        }
    }
    //--------------------------------------------------------------------
};
bool SinDebugger::isDebug = false;