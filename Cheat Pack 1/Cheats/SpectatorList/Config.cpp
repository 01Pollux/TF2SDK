
#include <Interfaces/ImGui.hpp>
#include "SpectatorList.hpp"

bool DisplaySpecList::OnRender()
{
	SG::ConfigState state;

	if (ImGui::Checkbox(m_Enabled.key(), m_Enabled.data()))
		state.set();
	ImGui::SameLineHelp(m_Enabled);

	ImGui::SameLine();
	if (ImGui::Checkbox(m_DisplayMode.key(), m_DisplayMode.data()))
		state.set();
	ImGui::SameLineHelp(m_DisplayMode);
	
	ImGui::SameLine();
	if (ImGui::Checkbox(m_Locked.key(), m_Locked.data()))
		state.set();
	ImGui::SameLineHelp(m_Locked);

	ImGui::SameLine();
	if (ImGui::DragInt(m_DisplayCount.key(), m_DisplayCount.data(), 0.35f, 0, TF2::Const::MaxPlayers))
		state.set();
	ImGui::SameLineHelp(m_DisplayCount);

	if (ImGui::ColorEdit4(m_OuterColor.key(), m_OuterColor.data()))
		state.set();
	ImGui::SameLineHelp(m_OuterColor);

	if (ImGui::Checkbox(m_InnerTransparent.key(), m_InnerTransparent.data()))
		state.set();
	ImGui::SameLineHelp(m_InnerTransparent);

	if (ImGui::ColorEdit4(m_InnerColor.key(), m_InnerColor.data()))
		state.set();
	ImGui::SameLineHelp(m_InnerColor);

	if (ImGui::ColorEdit4(m_TextColor.key(), m_TextColor.data()))
		state.set();
	ImGui::SameLineHelp(m_TextColor);

	return state;
}

void DisplaySpecList::OnSaveConfig(Json& cfg)
{
	Json& spec_list = cfg["Spectator list"];

	m_Enabled.to_json(spec_list);
	m_DisplayMode.to_json(spec_list);
	m_DisplayCount.to_json(spec_list);
	m_Locked.to_json(spec_list);
	m_OuterColor.to_json(spec_list);
	m_InnerColor.to_json(spec_list);
	m_InnerTransparent.to_json(spec_list);
	m_TextColor.to_json(spec_list);
}

void DisplaySpecList::OnReloadConfig(const Json& cfg)
{
	auto iter = cfg.find("Spectator list");
	if (iter != cfg.end())
	{
		auto& spec_list = *iter;
		m_Enabled.from_json(spec_list);
		m_DisplayMode.from_json(spec_list);
		m_DisplayCount.from_json(spec_list);
		m_Locked.from_json(spec_list);
		m_OuterColor.from_json(spec_list);
		m_InnerColor.from_json(spec_list);
		m_InnerTransparent.from_json(spec_list);
		m_TextColor.from_json(spec_list);
	}
}
