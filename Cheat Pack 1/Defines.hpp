#pragma once


#define PX_PLUGIN_NAME			"tf2 Cheat Pack #1"
#define PX_PLUGIN_AUTHOR		"01Pollux"
#define PX_PLUGIN_DESC			"Collection of multiple cheats all in one plugin"
#define PX_PLUGIN_VERSION		"1.2.0.0"


//#define PX_USING_PL_MANAGER		//	IPluginManager: PluginManager
#define PX_USING_LIBRARY			//	ILibraryManager: LibManager
#define PX_USING_LOGGER				//	ILogger: Logger
//#define PX_USING_EVENT_MANAGER	//	IEventManager: EventManager
#define PX_USING_DETOUR_MANAGER		//	IDetoursManager
#define PX_USING_IMGUI				//	IImGuiLoader
#define PX_USING_PROFILER			//	Profiler::Manager
#define PX_USING_CONCOMMANDS		//	IConsoleManager


#include "dllImpl.hpp"

PX_NAMESPACE_BEGIN();

extern IPlugin* ThisPlugin;

PX_NAMESPACE_END();

#define PX_DLL_EXPORT(cls)													\
cls _dll_dummy_to_export;													\
PX_NAMESPACE_BEGIN();														\
IPlugin* ThisPlugin = static_cast<IPlugin*>(&_dll_dummy_to_export);			\
PX_NAMESPACE_END()

#ifndef PX_USING_IMGUI
#define PX_TEXTMODE
#else
#define PX_GUIMODE
#endif