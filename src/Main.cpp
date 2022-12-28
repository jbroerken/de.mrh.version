/**
 *  Copyright (C) 2022 The de.mrh.version Authors.
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
#include <libmrh/Send/MRH_SendEvent.h>
#include <libmrh/MRH_AppLoop.h>
#include <libmrhevdata.h>
#include <libmrhab/MRH_ABLogger.h>
#include <libmrhbf.h>
#include <libmrhvt/Output/MRH_Generator.h>

// Project
#include "./Revision.h"

// Pre-defined
#ifndef MRH_VERSION_FILE_PATH
    #define MRH_VERSION_FILE_PATH "/usr/share/mrh/version.conf"
#endif

// Namespace
namespace
{
    // File Info
    const char* p_MRHBlock = "MRH";
    const char* p_NameKey = "Name";
    const char* p_VersionKey = "Version";

    // Output Info
    MRH_Uint32 u32_OutputID = 0;
}


// Prevent name wrangling for library header functions
#ifdef __cplusplus
extern "C"
{
#endif

    //*************************************************************************************
    // Output
    //*************************************************************************************

    MRH_Event* GenerateVersionOutput() noexcept
    {
        MRH::AB::Logger& c_Logger = MRH::AB::Logger::Singleton();

        /**
         *  File
         */

        std::string s_Version = "";

        try
        {
            MRH::BF::BlockFile c_File(MRH_VERSION_FILE_PATH);

            for (auto& Block : c_File.l_Block)
            {
                if (Block.GetName().compare(p_MRHBlock) == 0)
                {
                    s_Version = Block.GetValue(p_NameKey);
                    s_Version += ", Version ";
                    s_Version += Block.GetValue(p_VersionKey);
                    s_Version += ".";

                    break;
                }
            }

            if (s_Version.size() == 0)
            {
                c_Logger.Log(MRH::AB::Logger::ERROR, "Failed to read version from file!",
                             "Main.cpp", __LINE__);
                return NULL;
            }
        }
        catch (std::exception& e)
        {
            c_Logger.Log(MRH::AB::Logger::ERROR, e.what(),
                         "Main.cpp", __LINE__);
            return NULL;
        }

        /**
         *  Output
         */

        c_Logger.Log(MRH::AB::Logger::INFO, "Sending version output: " +
                                            s_Version +
                                            " (ID: " +
                                            std::to_string(u32_OutputID) +
                                            ")",
                     "Version.cpp", __LINE__);

        // Setup event data
        MRH_EvD_S_String_U c_Data;

        memset((c_Data.p_String), '\0', MRH_EVD_S_STRING_BUFFER_MAX_TERMINATED);
        strncpy(c_Data.p_String, s_Version.c_str(), MRH_EVD_S_STRING_BUFFER_MAX);
        c_Data.u32_ID = u32_OutputID;

        // Create event
        MRH_Event* p_Event = MRH_EVD_CreateSetEvent(MRH_EVENT_SAY_STRING_U, &c_Data);

        if (p_Event == NULL)
        {
            c_Logger.Log(MRH::AB::Logger::ERROR, "Failed to create output event!",
                         "Main.cpp", __LINE__);
            return NULL;
        }

        return p_Event;
    }

    //*************************************************************************************
    // Init
    //*************************************************************************************

    int MRH_Init(const MRH_A_SendContext* p_SendContext, const char* p_LaunchInput, int i_LaunchCommandID)
    {
        MRH::AB::Logger& c_Logger = MRH::AB::Logger::Singleton();

        c_Logger.Log(MRH::AB::Logger::INFO, "Initializing version application (Version: " +
                                            std::string(REVISION_STRING) +
                                            ")",
                     "Main.cpp", __LINE__);

        // Get output string event
        MRH_Event* p_Event = GenerateVersionOutput();

        if (p_Event == NULL)
        {
            c_Logger.Log(MRH::AB::Logger::INFO, "Failed to create version output!",
                         "Main.cpp", __LINE__);
            return -1;
        }

        // Attempt to send
        while (true)
        {
            switch (MRH_A_SendEvent(p_SendContext, &p_Event)) /* Consumes p_Event */
            {
                case MRH_A_Send_Result::MRH_A_SEND_OK:
                    c_Logger.Log(MRH::AB::Logger::INFO, "Sent output event",
                                 "Main.cpp", __LINE__);
                    return 0;
                case MRH_A_Send_Result::MRH_A_SEND_FAILURE:
                    c_Logger.Log(MRH::AB::Logger::ERROR, "Failed to send output event!",
                                 "Main.cpp", __LINE__);
                    return -1;

                default:
                    break;
            }
        }
    }

    //*************************************************************************************
    // Update
    //*************************************************************************************

    int MRH_Update(const MRH_Event* p_Event)
    {
        if (p_Event == NULL && p_Event->u32_Type != MRH_EVENT_SAY_STRING_S)
        {
            // Wait for correct event
            return 0;
        }

        MRH_EvD_S_String_S c_Data;

        if (MRH_EVD_ReadEvent(&c_Data, p_Event->u32_Type, p_Event) < 0)
        {
            MRH::AB::Logger::Singleton().Log(MRH::AB::Logger::ERROR, "Failed to read string response event!",
                                             "Main.cpp", __LINE__);
        }
        else if (c_Data.u32_ID != u32_OutputID)
        {
            MRH::AB::Logger::Singleton().Log(MRH::AB::Logger::ERROR, "Wrong output ID received!",
                                             "Main.cpp", __LINE__);
        }
        else
        {
            MRH::AB::Logger::Singleton().Log(MRH::AB::Logger::INFO, "Version output performed.",
                                             "Main.cpp", __LINE__);
        }

        // Terminate in any case
        return -1;
    }

    //*************************************************************************************
    // Exit
    //*************************************************************************************

    void MRH_Exit(void)
    {}

#ifdef __cplusplus
}
#endif
