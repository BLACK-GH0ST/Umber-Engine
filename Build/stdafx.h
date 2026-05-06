/**********************************************************************************************
*
*  File Name   : stdafx.h
*
*  Description :
*      Precompiled header for the Umbra Engine.
*
*      This file includes commonly used system, standard library, and DirectX headers
*      to reduce compilation time across the project.
*
*      It also defines utility functions and helper classes for error handling,
*      particularly for HRESULT-based DirectX operations.
*
*  Notes :
*      - Included in most source files (.cpp) across the engine.
*      - Contains ThrowIfFailed() for runtime error validation.
*      - Avoid adding heavy or rarely used headers unnecessarily.
*
**********************************************************************************************/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//--------------------------------------------------------------------------------------
// Windows Header Files
//--------------------------------------------------------------------------------------

#include <Windows.h>
#include <DirectXMath.h>

// Prevent Windows macros from interfering with std::min / std::max
#undef max
#undef min

//--------------------------------------------------------------------------------------
// C Runtime / Standard Library Headers
//--------------------------------------------------------------------------------------

#include <algorithm>
#include <cassert>
#include <codecvt>
#include <iostream>
#include <locale>
#include <memory>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <format>

//--------------------------------------------------------------------------------------
// Project Headers
//--------------------------------------------------------------------------------------

#include "Targetver.h"

//--------------------------------------------------------------------------------------
// COM Exception Helper
//--------------------------------------------------------------------------------------

/**
 * Exception class used to represent HRESULT failures.
 * Provides a readable error message for debugging purposes.
 */
class com_exception : public std::exception
{
public:

    /**
     * Constructor.
     * @param hr - HRESULT error code.
     */
  explicit com_exception (HRESULT hr) : message(std::format("Failure with HRESULT of {:08X}", static_cast<unsigned int>(hr))) {}

  /**
   * Returns a formatted error message.
   */
  const char* what () const override
  {
    return message.c_str();
  }

private:

    /** Stored HRESULT error code. */
    std::string message;
};

//--------------------------------------------------------------------------------------
// Error Handling Utility
//--------------------------------------------------------------------------------------

/**
 * Checks the result of an HRESULT call.
 *
 * If the call fails:
 *  - Outputs debug information (in Debug mode)
 *  - Breaks execution for debugging
 *  - Throws a com_exception
 *
 * @param hr - HRESULT returned from a DirectX or Win32 function.
 */
inline void ThrowIfFailed (HRESULT hr)
{
  if(FAILED (hr))
  {
#ifdef _DEBUG
    
    std::string msg = std::format ( "**ERROR** Fatal Error with HRESULT of {:08X}\n", static_cast<unsigned int>(hr));
    OutputDebugStringA (msg.c_str());
    __debugbreak (); // Break into debugger
#endif
    throw com_exception (hr);
  }
}