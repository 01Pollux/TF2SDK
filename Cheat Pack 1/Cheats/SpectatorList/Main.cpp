
#include <Interfaces/GameData.hpp>
#include <imgui/imgui_internal.h>
#include <User/Profiler.hpp>

#include <Engine/ClientDll.hpp>
#include <Engine/GlobalVars.hpp>

#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>

#include "SpectatorList.hpp"

static DisplaySpecList spec_list;

bool DisplaySpecList::OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config)
{
	config.Engine.GlobalVars = true;
	config.Engine.EngineClient = true;

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

	return true;
}

void DisplaySpecList::OnPluginLoad()
{
	SG::ImGuiLoader->AddCallback(SG::ThisPlugin, "Spectator list", std::bind(&DisplaySpecList::OnRender, this));

	ImGuiContextHook draw_hook;
	draw_hook.Type = ImGuiContextHookType_RenderPre;
	draw_hook.Callback = &DisplaySpecList::OnDrawSpecList;
	m_RenderHookId = ImGui::AddContextHook(ImGui::GetCurrentContext(), &draw_hook);
}


void DisplaySpecList::OnPluginUnload()
{
	ImGui::RemoveContextHook(ImGui::GetCurrentContext(), m_RenderHookId);
}

void DisplaySpecList::OnDrawSpecList(ImGuiContext* ctx, ImGuiContextHook* hook)
{
	if (SG::ThisPlugin->IsPluginPaused() || !spec_list.m_Enabled)
		return;

	using namespace TF2;
	if (ITFPlayerInternal::BadLocal())
		return;

	SG_PROFILE_SECTION("Visuals", "DisplaySpecList::OnDrawSpecList");

	ILocalPlayer pMe;
	PlayerInfo playerinfo;

	struct NameAndMode
	{
		const char* Mode;
		std::string Name;
	};
	std::vector<NameAndMode> spectators;
	spectators.reserve(4);

	for (int i = 1; i <= Interfaces::GlobalVars->MaxClients && spectators.size() <= static_cast<size_t>(spec_list.m_DisplayCount); i++)
	{
		ITFPlayer player(i);
		if (!player || player == pMe)
			continue;

		if (player->LifeState != Const::PlayerLifeState::Dead)
			continue;

		const ITFPlayer spectated(player->ObserverTarget);
		if (spectated != pMe)
			continue;

		const char* spec_mode;
		if (spec_list.m_DisplayMode)
		{
			switch (player->ObserverMode)
			{
			case Const::PlayerObserverMode::InEye:
			{
				spec_mode = "[FP] ";
				break;
			}

			case Const::PlayerObserverMode::Chase:
			{
				spec_mode = "[TP] ";
				break;
			}

			case Const::PlayerObserverMode::FreeCam:
			{
				spec_mode = "[FC] ";
				break;
			}

			default: continue;
			}
		}
		else spec_mode = "";

		if (Interfaces::EngineClient->GetPlayerInfo(i, &playerinfo))
			spectators.emplace_back(spec_mode, playerinfo.Name);
	}

	if (spectators.empty())
		return;

	const ImGuiWindowFlags background_flag = (spec_list.m_Locked ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None);

	ImGui::SetNextWindowSize({ 0.f, 0.f }, ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { spec_list.m_InnerColor[0] , spec_list.m_InnerColor[1] ,spec_list.m_InnerColor[2], spec_list.m_InnerTransparent ? 0.f : spec_list.m_InnerColor[3] });
	ImGui::PushStyleColor(ImGuiCol_Border, { spec_list.m_OuterColor[0] , spec_list.m_OuterColor[1] ,spec_list.m_OuterColor[2], spec_list.m_OuterColor[3] });
	bool is_open = ImGui::Begin("Spectator list", spec_list.m_Enabled.data(), background_flag | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::PopStyleColor(2);

	if (is_open)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { spec_list.m_TextColor[0], spec_list.m_TextColor[1], spec_list.m_TextColor[2], spec_list.m_TextColor[3] });
		for (auto& name_and_mode : spectators)
		{
			ImGui::Text("[%s]%s", name_and_mode.Mode, name_and_mode.Name.c_str());
		}
		ImGui::PopStyleColor();
	}

	ImGui::End();
}
