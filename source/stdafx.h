/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================
stdafx.h: Precompiled header for engine. Includes all standard files, as well as 
		  defining and including all platform specific files.
=============================================================================*/

/*===========================================================================
					--- STANDARD LIBRARY INCLUDES ---
===========================================================================*/
#pragma once
#pragma region StandardLibraryIncludes
#include <iostream>
#include <cstdarg>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <math.h>
#include <fstream>
#include <random>
#include <stdio.h>
#include <vector>
#include <list>
#include <queue>
#include <functional>
#include <unordered_map>
#include <map>
#pragma endregion //StandardLibraryIncludes

/*===========================================================================
						--- PLATFROM SETUP ---
===========================================================================*/
/* --- DEFINES --- */
#define USE_BOX2D
#define USE_CRTDBG
//#define USE_VLD

/* --- PLATFORMS --- */
#define PLATFORM_WINDOWS 0
#define PLATFORM_PS4 1

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_ID PLATFORM_WINDOWS
#elif _ORBIS
	#define PLATFORM_ID PLATFORM_PS4
#endif

/* --- PLATFORM SPECIFIC DEFINES */
#pragma region PlatformSpecificDefines
#if (PLATFORM_ID == PLATFORM_WINDOWS)
	#define ALIGN_8  __declspec(align(8))
	#define ALIGN_16 __declspec(align(16))
	#define ALIGN_32 __declspec(align(32))
	#define ALIGN_64 __declspec(align(64))
#elif
	#define ELITE_ALIGN_8 
	#define ELITE_ALIGN_16
	#define ELITE_ALIGN_32
	#define ELITE_ALIGN_64
#endif

#if defined(_DEBUG) && defined(USE_VLD)
	#include <vld.h>
#elif defined(_DEBUG) && defined(USE_CRTDBG) && (PLATFORM_ID == PLATFORM_WINDOWS)
	#define _CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC_NEW
	#include <crtdbg.h>
	#define ENABLE_LEAK_REPORT _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
	#define ENABLE_LEAK_REPORT ((void)0);
#endif

#if defined(_DEBUG) && (PLATFORM_ID == PLATFORM_WINDOWS)
	#define DEBUG_BREAK __debugbreak();
	#define DEBUG_BREAK_ON_ALLOCATION_ID(id) _crtBreakAlloc = static_cast<int>(id);
#else
	#define DEBUG_BREAK ((void)0);
	#define DEBUG_BREAK_ON_ALLOCATION_ID(id) ((void)0);
#endif

#pragma endregion //PlatformSpecificDefines

/*===========================================================================
							--- UTILITIES ---
===========================================================================*/
#pragma region Utilities
//=== Suppressors ===
#define UNREFERENCED_PARAMETER(x) (x)

//=== Management ===
#define SAFE_DELETE(p) if (p) { delete (p); (p) = nullptr; }

//=== Exception ===
struct Elite_Exception
{
	explicit Elite_Exception(const std::string& msg) :_msg(msg) {}
	const std::string _msg;
};

//=== Assert ===
#ifndef _DEBUG
	#define ELITE_ASSERT(condition, message) ((void)0);
#else
	#define ELITE_ASSERT(condition, message){ \
		std::wstringstream wss; \
		wss << message; \
		std::wstring msg = wss.str(); \
		_ASSERT_EXPR(condition, const_cast<LPWSTR>(msg.c_str()));}
#endif

//=== Function that returns a FNV-1a compile-time hash of a literal string, using template to fix size based on input ===
//https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
/* USAGE INFORMATION
constexpr auto ID_T4 = GENERATE_UNIQUE_ID("Hello World");
SomeRandomFunction(ID_T4); //OK
----
SomeRandomFunction(GENERATE_UNIQUE_ID("BABA")); //ERROR
*/
template<unsigned int N>
constexpr unsigned int CompileTimeLiteralStringHash(const char(&s)[N], unsigned int hash = 2166136261)
{
	const unsigned int length = N - 1;

	if (length == 0 || s[0] == '\0')
		return hash;

	for (unsigned int i = 0; i < length; ++i)
	{
		hash ^= s[i];
		hash *= 16777619;
	}
	return hash;
}
#define GENERATE_UNIQUE_ID(x) CompileTimeLiteralStringHash(x);

//=== Messaging === 
// Note: message cannot be passed by reference due to va_start
inline void LogMessageEx(std::string message, va_list arg)
{
	vprintf(message.c_str(), arg);
}

inline void LogMessage(std::string message, ...)
{
	va_list ap;
	va_start(ap, message);
	LogMessageEx(message, ap);
	va_end(ap);
}
#pragma endregion //Utilities

//=== HOTFIX EVENT ===
//Hotfix for genetic algorithms project
extern bool gRequestShutdown;

/*===========================================================================
						--- GENERAL INCLUDES ---
===========================================================================*/
#pragma region GeneralIncludes
#ifdef USE_BOX2D
	#pragma warning(push)
	#pragma warning(disable: 26495)
	#include <box2d/b2_settings.h>
	#include <box2d/b2_math.h>
	#pragma warning(pop)
#endif

#if (PLATFORM_ID == PLATFORM_WINDOWS)
	//OpenGl
	#include <GL/gl3w.h>
	//SDL Window
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_syswm.h>
	//ImGUI
	#pragma warning(push)
	#pragma warning(disable: 26495)
	#include <imgui.h>
	#pragma warning(pop)
#endif
#pragma endregion //GeneralIncludes

/*===========================================================================
						--- FRAMEWORK INCLUDES ---
===========================================================================*/
#pragma region FrameworkIncludes
#include "framework/EliteHelpers/ESingleton.h"
#include "framework/EliteMath/EMath.h"
#include "framework/ElitePhysics/EPhysics.h"
#include "framework/EliteInput/EInputCodes.h"
#include "framework/EliteInput/EInputData.h"
#include "framework/EliteInput/EInputManager.h"
#include "framework/EliteWindow/EWindow.h"
#include "framework/EliteTimer/ETimer.h"
#include "framework/EliteRendering/ERendering.h"
#include "framework/EliteUI/EImmediateUI.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EHeuristic.h"
#include "framework/EliteAI/EliteDecisionMaking/EDecisionMaking.h"
#pragma endregion //FrameworkIncludes

/* --- FRAMEWORK MACROS ---- */
#define INPUTMANAGER Elite::EInputManager::GetInstance()
#define TIMER Elite::ETimer<PLATFORM_ID>::GetInstance()
#define DEBUGRENDERER2D EliteDebugRenderer2D::GetInstance()
#define PHYSICSWORLD PhysicsWorld::GetInstance()
#define LEVELLOADER LevelLoader::GetInstance()

/* --- PLATFORM SPECIFIC INCLUDES --- */
#pragma region PlatformIncludes
#pragma endregion //PlatformIncludes