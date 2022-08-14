/**
 *  Copyright (C) 2022 The MRH Project Authors.
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

// C / C++
#include <cstdio>
#include <string>
#include <iostream>

// External
#include <libmrh/MRH_AppLoop.h>
#include <libmrhab.h>

// Project
#include "./Module/Version/Version.h"
#include "./Revision.h"

// Pre-defined
namespace
{
    libmrhab* p_Context = NULL;
}


// Prevent name wrangling for library header functions
#ifdef __cplusplus
extern "C"
{
#endif
    
    //*************************************************************************************
    // Init
    //*************************************************************************************

    int MRH_Init(const char* p_LaunchInput, int i_LaunchCommandID)
    {
        MRH_ModuleLogger& c_Logger = MRH_ModuleLogger::Singleton();
        c_Logger.Log("MRH_Init", "Initializing version application (Version: " +
                                 std::string(REVISION_STRING) +
                                 ")",
                     "Main.cpp", __LINE__);
    
        try
        {
            p_Context = new libmrhab(std::make_unique<Version>());
            return 0;
        }
        catch (std::exception& e)
        {
            c_Logger.Log("MRH_Init", "Failed to initialize: " +
                                     std::string(e.what()),
                         "Main.cpp", __LINE__);
            return -1;
        }
    }

    //*************************************************************************************
    // Receive Event
    //*************************************************************************************

    void MRH_ReceiveEvent(const MRH_Event* p_Event)
    {
        try
        {
            p_Context->AddReceivedEvent(p_Event);
        }
        catch (MRH_ABException& e)
        {
            MRH_ModuleLogger::Singleton().Log("MRH_ReceiveEvent", "Failed to add received event: " +
                                                                  e.what2(),
                                              "Main.cpp", __LINE__);
        }
    }

    //*************************************************************************************
    // Send Event
    //*************************************************************************************

    MRH_Event* MRH_SendEvent(void)
    {
        return p_Context->GetSendEvent();
    }

    //*************************************************************************************
    // Exit
    //*************************************************************************************

    int MRH_CanExit(void)
    {
        return p_Context->GetQuit() ? 0 : -1;
    }

    void MRH_Exit(void)
    {
        if (p_Context != NULL)
        {
            delete p_Context;
        }
    }

#ifdef __cplusplus
}
#endif
