
#include <Interfaces/ImGui.hpp>
#include "DisplayDead.hpp"

bool DisplaySpecList::OnRender()
{
	SG::ConfigState state;

	if (ImGui::Checkbox(m_Enabled.key(), m_Enabled.data()))
		state.set();
	ImGui::SameLineHelp(m_Enabled);

	if (ImGui::Checkbox(m_CheckUber.key(), m_CheckUber.data()))
		state.set();
	ImGui::SameLineHelp(m_CheckUber);

	ImGui::SameLine();

	if (ImGui::Checkbox(m_CheckInvisible.key(), m_CheckInvisible.data()))
		state.set();
	ImGui::SameLineHelp(m_CheckInvisible);

	return state;
}

void DisplaySpecList::OnSaveConfig(Json& cfg)
{
	Json& autostab = cfg["Auto-Backstab"];
	m_Enabled.to_json(autostab);
	m_CheckUber.to_json(autostab);
	m_CheckInvisible.to_json(autostab);
}

void DisplaySpecList::OnReloadConfig(const Json& cfg)
{
	auto iter = cfg.find("Auto-Backstab");
	if (iter != cfg.end())
	{
		m_Enabled.from_json(*iter);
		m_CheckUber.from_json(*iter);
		m_CheckInvisible.from_json(*iter);
	}
}
