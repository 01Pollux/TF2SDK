#pragma once

#include "Defines.hpp"
#include <tf2/utils/AutoList.hpp>
#include <tf2/config.hpp>

struct ImGuiContextHook;

class ICheatIFace : public TF2::Utils::IAutoList<ICheatIFace>
{
public:
	ICheatIFace() { }

	/// <summary>
	/// Called when plugin is first loaded, use this callback to register interfaces only
	/// </summary>
	/// <returns>
	/// true if plugin is all set to be loaded, false otherwise
	/// </returns>
	virtual void OnPluginPreLoad(SG::IPluginManager* ifacemgr) { }

	/// <summary>
	/// Called when plugin is first loaded
	/// </summary>
	/// <returns>
	/// true if plugin is all set to be loaded, false otherwise
	/// </returns>
	virtual bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) { return true; }

	/// <summary>
	/// Called when the plugin is fully loaded after 'OnAskPluginLoad' return true
	/// </summary>
	virtual void OnPluginLoad() { }

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
	/// Called when an interface is being unloaded from the process
	/// </summary>
	virtual void OnDropInterface(SG::IInterface* iface) { }

	/// <summary>
	/// Called when a plugin wants to save current data to json file
	/// </summary>
	virtual void OnSaveConfig(std::vector<SG::IPlugin::FileConfigs>& cfg) const { }
};