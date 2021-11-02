
#include "AutoStab.hpp"
#include "Interfaces/ImGui.hpp"

#include <Engine/Convar.hpp>

#include <Server/ServerTools.hpp>

#include <Entity/BasePlayer.hpp>

bool AutoBackstab::OnRender()
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

	if (ImGui::Button("ServerTools"))
	{
		auto st = TF2::Interfaces::ServerTools;
		int ents = 0;
		for (auto ent = st->FirstEntity(); ent; ent = st->NextEntity(ent))
		{
			ents++;
			TF2::Interfaces::CVar->ConsolePrintf("%x\n", ent);
		}
		TF2::Interfaces::CVar->ConsolePrintf("Count: %i %x\n", ents, st->GetIServerEntity(TF2::ILocalPlayer().get()));
	}

	return state;
}

void AutoBackstab::OnSaveConfig(Json& cfg)
{
	Json& autostab = cfg["Auto-Backstab"];
	m_Enabled.to_json(autostab);
	m_CheckUber.to_json(autostab);
	m_CheckInvisible.to_json(autostab);
}

void AutoBackstab::OnReloadConfig(const Json& cfg)
{
	auto iter = cfg.find("Auto-Backstab");
	if (iter != cfg.end())
	{
		m_Enabled.from_json(*iter);
		m_CheckUber.from_json(*iter);
		m_CheckInvisible.from_json(*iter);
	}
}
