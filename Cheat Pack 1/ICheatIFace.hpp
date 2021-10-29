#pragma once

#include <Interfaces/PluginSys.hpp>
#include "Utils/AutoList.hpp"
#include "TF2Config.hpp"

class ICheatIFace : public SG::IPlugin, public TF2::Utils::IAutoList<ICheatIFace>
{
public:

	/// <summary>
	/// Called when plugin is first loaded
	/// </summary>
	/// <returns>
	/// true if plugin is all set to be loaded, false otherwise
	/// </returns>
	virtual bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) { return true; }

	virtual void OnPluginLoad() { }
};