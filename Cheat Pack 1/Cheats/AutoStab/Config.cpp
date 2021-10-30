
#include "AutoStab.hpp"
#include "Interfaces/ImGui.hpp"

bool AutoBackstab::OnRender()
{
	SG::ConfigState state;

	if (ImGui::Checkbox("Enable", m_Enabled.data()))
		state.set();
	ImGui::SameLineHelp(m_Enabled);

	return state;
}

void AutoBackstab::OnSaveConfig(Json& cfg)
{
	Json& autostab = cfg["Auto-Backstab"];
	autostab = m_Enabled;
}

void AutoBackstab::OnReloadConfig(const Json& cfg)
{
	auto iter = cfg.find("Auto-Backstab");
	if (iter != cfg.end())
		iter->get_to(m_Enabled);
}
