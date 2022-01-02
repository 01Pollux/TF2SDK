
#include "Defines.hpp"

extern "C" BOOST_SYMBOL_EXPORT
SG::IPlugin* Tella_GetPlugin()
{
	return SG::ThisPlugin;
}

SG::IPluginImpl::IPluginImpl() noexcept : IPlugin(SG::ThisPluginInfo) { }

#if defined SG_USING_PROFILER && !defined SG_USING_LIBRARY
#include <shadowgarden/interfaces/LibrarySys.hpp>
#endif

bool SG::IPluginImpl::OnPluginLoad(SG::IPluginManager* ifacemgr)
{
#ifdef SG_USING_PL_MANAGER
	SG::PluginManager = ifacemgr;
#endif
#ifdef SG_USING_LIBRARY
	SG::GetInterface_NoFail(SG::Interface_ILibrary, ifacemgr, SG::LibManager);
#endif
#ifdef SG_USING_LOGGER
	SG::GetInterface_NoFail(SG::Interface_ILogger, ifacemgr, SG::Logger);
#endif
#ifdef SG_USING_EVENT_MANAGER
	SG::GetInterface_NoFail(SG::Interface_EventManager, ifacemgr, SG::EventManager);
#endif
#ifdef SG_USING_DETOUR_MANAGER
	SG::GetInterface_NoFail(SG::Interface_DetoursManager, ifacemgr, SG::DetourManager);
#endif
#ifdef SG_USING_IMGUI
	if (!SG::GetInterface(Interface_ImGuiLoader, ifacemgr, ImGuiLoader))
		return false;
#endif
#ifdef SG_USING_PROFILER
	{
		SG::Profiler::Manager* pProfiler;
#ifndef SG_USING_LIBRARY
		ILibraryManager* lib_manager;
		SG::GetInterface_NoFail(SG::Interface_ILibrary, ifacemgr, lib_manager);
		pProfiler = lib_manager->GetProfiler();
#else
		pProfiler = SG::LibManager->GetProfiler();
#endif
		SG::Profiler::Manager::Set(pProfiler);
	}
#endif
#ifdef SG_USING_CONCOMMANDS
	if (!SG::GetInterface(Interface_ConsoleManager, ifacemgr, ConsoleManager))
		return false;
#endif
	SG::ConCommand::Init(SG::ThisPlugin, ConsoleManager);
	return this->OnPluginLoad2(ifacemgr);
}
