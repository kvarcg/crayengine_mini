/*
File:  Exit.cpp

Description: Exit Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// includes ////////////////////////////////////////
 #include <iostream>
#include "Global.h"

 #ifdef WINMSVCDEBUG
 #include <crtdbg.h>
 #include <windows.h>
 #endif
 #include "Exit.h"

 // defines /////////////////////////////////////////

namespace BASELIB {
    Exit e;

    // Constructor
    Exit::Exit() {
#if defined(WINMSVCDEBUG)
        int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;             // Turn on debug allocation
        //tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;        // Check heap every alloc / dealloc
        //tmpDbgFlag |= _CRTDBG_CHECK_CRT_DF;           // Leak check/diff CRT blocks
        tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;        // Don't actually free memory
        tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;            // Leak check at program exit
        _CrtSetDbgFlag(tmpDbgFlag);
#endif // WINMSVCDEBUG
    }

    // Destructor
    Exit::~Exit() {
#if defined(WINMSVCDEBUG)
        OutputDebugStringA("Detecting Memory Leaks\n");
        std::cout << "Detecting Memory Leaks" << std::endl;
#endif // WINMSVCDEBUG
    }
}

// other functions


 // eof ///////////////////////////////// class Exit
