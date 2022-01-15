
#include <px/interfaces/ImGui.hpp>
#include "SpectatorList.hpp"

#include <atomic>

bool DisplaySpecList::OnRender()
{
	px::ConfigState state;

	if (ImGui::Checkbox("Enable", m_Enabled.data()))
		state.set();
	ImGui::SameLineHelp(m_Enabled);

	ImGui::SameLine();
	if (ImGui::Checkbox("Display mode", m_DisplayMode.data()))
		state.set();
	ImGui::SameLineHelp(m_DisplayMode);
	
	ImGui::SameLine();
	if (ImGui::Checkbox("Locked", m_Locked.data()))
		state.set();
	ImGui::SameLineHelp(m_Locked);

	ImGui::SameLine();
	if (ImGui::DragInt("Count", m_DisplayCount.data(), 0.35f, 0, tf2::Const::MaxPlayers))
		state.set();
	ImGui::SameLineHelp(m_DisplayCount);

	if (ImGui::ColorEdit4("Outter color", m_OuterColor->data()))
		state.set();
	ImGui::SameLineHelp(m_OuterColor);

	if (ImGui::Checkbox("Transparent", m_InnerTransparent.data()))
		state.set();
	ImGui::SameLineHelp(m_InnerTransparent);

	if (ImGui::ColorEdit4("Inner color", m_InnerColor->data()))
		state.set();
	ImGui::SameLineHelp(m_InnerColor);

	if (ImGui::ColorEdit4("Text color", m_TextColor->data()))
		state.set();
	ImGui::SameLineHelp(m_TextColor);

	return state;
}

void DisplaySpecList::OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const
{
	px::IPlugin::FileConfigs spec_list;
	
	spec_list.insert(m_Enabled);
	spec_list.insert(m_DisplayMode);
	spec_list.insert(m_DisplayCount);
	spec_list.insert(m_Locked);
	spec_list.insert(m_OuterColor);
	spec_list.insert(m_InnerColor);
	spec_list.insert(m_InnerTransparent);
	spec_list.insert(m_TextColor);

	cfg.emplace_back(std::move(spec_list));
}
