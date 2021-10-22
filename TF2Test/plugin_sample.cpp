#include "Interfaces/InterfacesSys.hpp"
#include "Defines.hpp"

#include "Config.hpp"

#include "Engine/ClientDll.hpp"
#include "Client/EntityList.hpp"
#include "Entity/BaseEntity.hpp"

class PluginSample : public SG::IPluginImpl
{
public:
	bool OnPluginLoad2(SG::IPluginManager* ifacemgr) override;
	

private:
	TF2::Interfaces::SDKManager m_TF2Sdk;
};

// Important for exporting plugin
SG_DLL_EXPORT(PluginSample);

bool PluginSample::OnPluginLoad2(SG::IPluginManager* ifacemgr)
{
	ImGui::SetCurrentContext(SG::ImGuiLoader->GetContext());
	TF2::Interfaces::SDKManager::Config tf2_config;
	tf2_config.Engine.EngineClient = true;
	tf2_config.Client.ClientDLL = true;
	tf2_config.Client.ClientList = true;


	m_TF2Sdk.init(SG::LibManager->OpenGameData(this), tf2_config, std::initializer_list{ "/GameData/Entity" });

	SG::ImGuiLoader->AddCallback(
		this,
		"Tf2 Test",
		[] () -> bool
	{
		static bool display = false;
		ImGui::Checkbox("Display", &display);

		if (display)
		{
			ImGui::Text("Is on: %i", TF2::Interfaces::EngineClient->IsInGame());
			if (TF2::Interfaces::ClientList && TF2::Interfaces::EngineClient->IsInGame())
			{
				auto pMe = TF2::IBaseEntityInternal::GetLocalPlayer();
			}
		}

		return false;
	}
	);
	return true;
}