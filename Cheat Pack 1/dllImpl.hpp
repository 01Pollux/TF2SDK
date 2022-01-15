#pragma once

#include <px/interfaces/InterfacesSys.hpp>
#include <px/interfaces/PluginSys.hpp>

namespace px
{
	class IPluginImpl : public IPlugin
	{
	public:
		IPluginImpl() noexcept;
		// Inherited via IPlugin
		bool OnPluginLoad(px::IPluginManager* ifacemgr) final;

	public:
		/// <summary>
		/// Called when plugin is first loaded
		/// </summary>
		/// <returns>
		/// true if plugin is all set to be loaded, false otherwise
		/// </returns>
		virtual bool OnPluginLoad2(px::IPluginManager* ifacemgr) { return false; }
	};

	extern IPlugin* ThisPlugin;
}


#ifdef PX_USING_PL_MANAGER
namespace px { inline IPluginManager* PluginManager; }
#endif

#ifdef PX_USING_LIBRARY
#include <px/interfaces/LibrarySys.hpp>
namespace px { inline ILibraryManager* LibManager; }
#endif

#ifdef PX_USING_LOGGER
#include <px/interfaces/Logger.hpp>
namespace px { inline ILogger* Logger; }
#endif

#ifdef PX_USING_EVENT_MANAGER
#include <px/interfaces/EventManager.hpp>
namespace px { inline IEventManager* EventManager; }
#endif

#ifdef PX_USING_DETOUR_MANAGER
#include <px/interfaces/HookArgs.hpp>
#include <px/interfaces/HooksManager.hpp>
namespace px { inline IDetoursManager* DetourManager; }
#endif

#ifdef PX_USING_IMGUI
#include <px/interfaces/ImGui.hpp>
namespace px { inline IImGuiLoader* ImGuiLoader; }
#endif

#ifdef PX_USING_PROFILER
#include <px/profiler.hpp>
#endif

#ifdef PX_USING_CONCOMMANDS
#include <px/console.hpp>
namespace px { inline IConsoleManager* ConsoleManager; }
#endif

namespace px
{
	template<typename _Ty>
		requires std::is_base_of_v<IInterface, std::remove_pointer_t<_Ty>>
	inline void GetInterface_NoFail(const char* name, IPluginManager* pFactory, _Ty& iface)
	{
		[[maybe_unused]] bool unused = pFactory->RequestInterface(name, std::bit_cast<IInterface**>(&iface));
	}

	template<typename _Ty>
		requires std::is_base_of_v<IInterface, std::remove_pointer_t<_Ty>>
	inline bool GetInterface(const char* name, IPluginManager* pFactory, _Ty& iface)
	{
		if (!pFactory->RequestInterface(name, std::bit_cast<IInterface**>(&iface)))
		{
#ifdef PX_USING_LOGGER
			PX_LOG_FATAL({
				PX_MESSAGE("Failed to find interface."),
				PX_LOGARG("Name", name)
			});
#endif
			return false;
		}
		return true;
	}
}