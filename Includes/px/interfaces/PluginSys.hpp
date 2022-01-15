#pragma once

#include <map>
#include <px/interfaces/InterfacesSys.hpp>
#include <px/version.hpp>
#include <px/console.hpp>

class PluginContext;

PX_NAMESPACE_BEGIN();

struct PluginInfo
{
	const char* m_Name;
	const char* m_Author;
	const char* m_Description;
	const char* m_Date;
	version		m_Version;
};

class IPlugin
{
	friend class PluginContext;
public:
	IPlugin(const PluginInfo& info) noexcept : m_PluginInfo(info) { };
	virtual ~IPlugin() = default;

	/// <summary>
	/// Called when plugin is first loaded, use this callback to register interfaces only
	/// </summary>
	/// <returns>
	/// true if plugin is all set to be loaded, false otherwise
	/// </returns>
	virtual void OnPluginPreLoad(IPluginManager* ifacemgr) { }

	/// <summary>
	/// Called when plugin is first loaded
	/// </summary>
	/// <returns>
	/// true if plugin is all set to be loaded, false otherwise
	/// </returns>
	virtual bool OnPluginLoad(IPluginManager* ifacemgr) { return false; }

	/// <summary>
	/// Called when plugin is unloading
	/// </summary>
	virtual void OnPluginUnload() { }

	/// <summary>
	/// Called when all plugins is unloading
	/// </summary>
	virtual void OnAllPluginsLoaded() { }

	/// <summary>
	/// Called when all plugin is changing state
	/// </summary>
	/// <param name="pausing">true if plugin is paused, false otherwise</param>
	virtual void OnPluginPauseChange(bool pausing) { }

	/// <summary>
	/// Get registered plugin info
	/// </summary>
	[[nodiscard]] const PluginInfo* GetPluginInfo() const noexcept
	{
		return &m_PluginInfo;
	}

	/// <summary>
	/// Get plugin state
	/// </summary>
	bool IsPluginPaused() const noexcept
	{
		return m_IsPaused;
	}

	/// <summary>
	/// Set plugin state
	/// </summary>
	void SetPluginState(bool pause)
	{
		if (pause)
		{
			if (!m_IsPaused)
				this->OnPluginPauseChange(true);
		}
		else if (m_IsPaused)
			this->OnPluginPauseChange(false);
		
		m_IsPaused = pause;
	}

	/// <summary>
	/// Get plugin's file name
	/// </summary>
	[[nodiscard]] const std::string& GetFileName() const noexcept
	{
		return m_FileName;
	}

	struct FileConfigs
	{
		std::string_view FileName;
		std::vector<std::pair<std::string_view, std::string>> Commands;

		template<typename _Ty>
		void insert(const _Ty& cvar)
		{
			Commands.emplace_back(
				cvar.name(),
				cvar.str()
			);
		}

		FileConfigs(const std::string_view& name = "") noexcept(std::is_nothrow_constructible_v<decltype(Commands), decltype(FileName)>) :
			FileName(name)
		{ }
	};

	virtual void OnSaveConfig(std::vector<FileConfigs>& configs) const
	{ }

private:
	std::string m_FileName;
	bool m_IsPaused{ true };
	const PluginInfo m_PluginInfo;
};

PX_NAMESPACE_END();