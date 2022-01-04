
#include <px/interfaces/ImGui.hpp>
#include "AutoStab.hpp"
#include <tf2/math/Vector.hpp>

bool AutoBackstab::OnRender()
{
	px::ConfigState state;

	if (ImGui::Checkbox("Enable", m_Enabled.data()))
		state.set();

	ImGui::SameLineHelp(m_Enabled);

	if (ImGui::Checkbox("Check uber", m_CheckUber.data()))
		state.set();
	ImGui::SameLineHelp(m_CheckUber);

	ImGui::SameLine();

	if (ImGui::Checkbox("Check invisible", m_CheckInvisible.data()))
		state.set();
	ImGui::SameLineHelp(m_CheckInvisible);

	return state;
}

void AutoBackstab::OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const
{
	px::IPlugin::FileConfigs backstab;

	backstab.insert(m_Enabled);
	backstab.insert(m_CheckUber);
	backstab.insert(m_CheckInvisible);

	cfg.emplace_back(std::move(backstab));
}
