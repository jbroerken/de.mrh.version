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
#include <cstring>

// External
#include <libmrhbf.h>

// Project
#include "./Version.h"

// Pre-defined
#ifndef MRH_VERSION_FILE_PATH
    #define MRH_VERSION_FILE_PATH "/usr/local/etc/mrh/MRH_Version.conf"
#endif
#define VERSION_OUTPUT_TIMEOUT_MS 30000

namespace
{
    const char* p_MRHBlock = "MRH";
    const char* p_NameKey = "Name";
    const char* p_VersionKey = "Version";
}


//*************************************************************************************
// Constructor / Destructor
//*************************************************************************************

Version::Version() : MRH_Module("Version",
                                true),
                     c_Timer(VERSION_OUTPUT_TIMEOUT_MS),
                     u32_SentOutputID((rand() % ((MRH_Uint32) - 1)) + 1),
                     u32_ReceivedOutputID(0)
{
    /**
     *  File 
     */
    
    std::string s_Version = "";
    
    try
    {
        MRH_BlockFile c_File(MRH_VERSION_FILE_PATH);
        
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
            throw MRH_ModuleException("Version",
                                      "Failed to read version from file!");
        }
    }
    catch (std::exception& e)
    {
        throw MRH_ModuleException("Version",
                                  e.what());
    }
    
    /**
     *  Output
     */
    
    MRH_ModuleLogger::Singleton().Log("Version", "Sending version output: " +
                                                 s_Version +
                                                 " (ID: " +
                                                 std::to_string(u32_SentOutputID) +
                                                 ")",
                                      "Version.cpp", __LINE__);
    // Setup event data
    MRH_EvD_S_String_U c_Data;
    
    memset((c_Data.p_String), '\0', MRH_EVD_S_STRING_BUFFER_MAX_TERMINATED);
    strncpy(c_Data.p_String, s_Version.c_str(), MRH_EVD_S_STRING_BUFFER_MAX);
    c_Data.u32_ID = u32_SentOutputID;
    
    // Create event
    MRH_Event* p_Event = MRH_EVD_CreateSetEvent(MRH_EVENT_SAY_STRING_U, &c_Data);
    
    if (p_Event == NULL)
    {
        throw MRH_ModuleException("Version", 
                                  "Failed to create output event!");
    }
    
    // Attempt to add to out storage
    try
    {
        MRH_EventStorage::Singleton().Add(p_Event);
    }
    catch (MRH_ABException& e)
    {
        MRH_EVD_DestroyEvent(p_Event);
        throw MRH_ModuleException("Version", 
                                  "Failed to send version output: " + e.what2());
    }
}

Version::~Version() noexcept
{}

//*************************************************************************************
// Update
//*************************************************************************************

void Version::HandleEvent(const MRH_Event* p_Event)
{
    // @NOTE: CanHandleEvent() allows skipping event type check!
    MRH_EvD_S_String_S c_String;
    
    if (MRH_EVD_ReadEvent(&c_String, p_Event->u32_Type, p_Event) < 0)
    {
        MRH_ModuleLogger::Singleton().Log("Version", "Failed to read string event!",
                                          "Version.cpp", __LINE__);
    }
    else
    {
        MRH_ModuleLogger::Singleton().Log("Version", "Received output performed: " +
                                          std::to_string(c_String.u32_ID),
                                          "Version.cpp", __LINE__);
        
        u32_ReceivedOutputID = c_String.u32_ID;
    }
}

MRH_Module::Result Version::Update()
{
    if (c_Timer.GetTimerFinished() == true || u32_SentOutputID == u32_ReceivedOutputID)
    {
        return MRH_Module::FINISHED_POP;
    }
    
    return MRH_Module::IN_PROGRESS;
}

std::unique_ptr<MRH_Module> Version::NextModule()
{
    throw MRH_ModuleException("Version",
                              "No module to switch to!");
}

//*************************************************************************************
// Getters
//*************************************************************************************

bool Version::CanHandleEvent(MRH_Uint32 u32_Type) noexcept
{
    switch (u32_Type)
    {
        case MRH_EVENT_SAY_STRING_S:
            return true;
            
        default:
            return false;
    }
}
