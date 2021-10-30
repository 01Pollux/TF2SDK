#include "Interfaces/InterfacesSys.hpp"
#include "Defines.hpp"

#include "ICheatIFace.hpp"

class IGlobalCheatIFace : public SG::IPluginImpl
{
public:
	void OnPluginPreLoad(SG::IPluginManager* ifacemgr) override
	{
		for (auto& iface : ICheatIFace::GetEntries())
			iface->OnPluginPreLoad(ifacemgr);
	}

	bool OnPluginLoad2(SG::IPluginManager* ifacemgr) override
	{ 
		ImGui::SetCurrentContext(SG::ImGuiLoader->GetContext());

		TF2::Interfaces::SDKManager::Config tf2_config;
		for (auto iface : ICheatIFace::GetEntries())
		{
			if (!iface->OnAskPluginLoad(tf2_config))
				return false;
		}

		m_TF2Sdk.init(SG::LibManager->OpenGameData(this), tf2_config, std::initializer_list{ "/GameData/Entity" });

		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginLoad();

		return true;
	}

	void OnPluginUnload() override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginUnload();
	}

	void OnAllPluginsLoaded() override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnAllPluginsLoaded();
	}

	void OnPluginPauseChange(bool pausing) override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginPauseChange(pausing);
	}

	void OnDropInterface(SG::IInterface* dropiface) override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnDropInterface(dropiface);
	}

	void OnSaveConfig(Json& cfg) override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnSaveConfig(cfg);
	}

	void OnReloadConfig(const Json& cfg) override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnReloadConfig(cfg);
	}

private:
	TF2::Interfaces::SDKManager m_TF2Sdk;
};

// Important for exporting plugin
SG_DLL_EXPORT(IGlobalCheatIFace);
