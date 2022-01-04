
#include "Defines.hpp"

extern "C" BOOST_SYMBOL_EXPORT
px::IPlugin* Tella_GetPlugin()
{
	return px::ThisPlugin;
}

static constexpr px::PluginInfo Tella_GetPluginInfo()
{
	return px::PluginInfo{
		PX_PLUGIN_NAME,
		PX_PLUGIN_AUTHOR,
		PX_PLUGIN_DESC,
		__DATE__,
		px::version(PX_PLUGIN_VERSION)
	};
}

px::IPluginImpl::IPluginImpl() noexcept : IPlugin(Tella_GetPluginInfo())
{ }

#if defined PX_USING_PROFILER && !defined PX_USING_LIBRARY
#include <px/interfaces/LibrarySys.hpp>
#endif

bool px::IPluginImpl::OnPluginLoad(px::IPluginManager* ifacemgr)
{
#ifdef PX_USING_PL_MANAGER
	px::PluginManager = ifacemgr;
#endif
#ifdef PX_USING_LIBRARY
	px::GetInterface_NoFail(px::Interface_ILibrary, ifacemgr, px::LibManager);
#endif
#ifdef PX_USING_LOGGER
	px::GetInterface_NoFail(px::Interface_ILogger, ifacemgr, px::Logger);
#endif
#ifdef PX_USING_EVENT_MANAGER
	px::GetInterface_NoFail(px::Interface_EventManager, ifacemgr, px::EventManager);
#endif
#ifdef PX_USING_DETOUR_MANAGER
	px::GetInterface_NoFail(px::Interface_DetoursManager, ifacemgr, px::DetourManager);
#endif
#ifdef PX_USING_IMGUI
	if (!px::GetInterface(Interface_ImGuiLoader, ifacemgr, ImGuiLoader))
		return false;
#endif
#ifdef PX_USING_PROFILER
	{
		px::profiler::manager* pProfiler;
#ifndef PX_USING_LIBRARY
		ILibraryManager* lib_manager;
		px::GetInterface_NoFail(px::Interface_ILibrary, ifacemgr, lib_manager);
		pProfiler = lib_manager->GetProfiler();
#else
		pProfiler = px::LibManager->GetProfiler();
#endif
		px::profiler::manager::Set(pProfiler);
	}
#endif
#ifdef PX_USING_CONCOMMANDS
	if (!px::GetInterface(Interface_ConsoleManager, ifacemgr, ConsoleManager))
		return false;
#endif
	px::ConCommand::Init(px::ThisPlugin, ConsoleManager);
	return this->OnPluginLoad2(ifacemgr);
}
