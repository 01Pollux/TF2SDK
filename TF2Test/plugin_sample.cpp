#include "Interfaces/InterfacesSys.hpp"
#include "Defines.hpp"

#include "TF2Config.hpp"

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
	m_TF2Sdk.init(SG::LibManager->OpenGameData(this), tf2_config, std::initializer_list{ "/GameData/Entity" });

	return true;
}