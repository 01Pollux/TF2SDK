
#include <px/interfaces/GameData.hpp>
#include <imgui/imgui_internal.h>

#include <px/profiler.hpp>
#include <px/icons/FontAwesome.hpp>

#include <tf2/engine/ClientDll.hpp>
#include <tf2/engine/GlobalVars.hpp>

#include <tf2/entity/BasePlayer.hpp>
#include <tf2/entity/BaseWeapon.hpp>

#include "SpectatorList.hpp"

static DisplaySpecList spec_list;

bool DisplaySpecList::OnAskPluginLoad(tf2::interfaces::SDKManager::Config& config)
{
	config.Engine.GlobalVars = true;
	config.Engine.EngineClient = true;

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

	return true;
}

void DisplaySpecList::OnPluginLoad()
{
	m_DisplayFont = px::ImGuiLoader->FindFont("Arimo-Medium, medium");
	px::ImGuiLoader->AddCallback(px::ThisPlugin, "Spectator list", std::bind(&DisplaySpecList::OnRender, this));

	ImGuiContextHook draw_hook;
	draw_hook.Type = ImGuiContextHookType_EndFramePre;
	draw_hook.Callback = &DisplaySpecList::OnDrawSpecList;
	m_RenderHookId = ImGui::AddContextHook(ImGui::GetCurrentContext(), &draw_hook);
}


void DisplaySpecList::OnPluginUnload()
{
	ImGui::RemoveContextHook(ImGui::GetCurrentContext(), m_RenderHookId);
}

void DisplaySpecList::OnDrawSpecList(ImGuiContext* imgui, ImGuiContextHook* ctx)
{
	if (px::ThisPlugin->IsPluginPaused() || !spec_list.m_Enabled)
		return;

	using namespace tf2;
	if (ITFPlayerInternal::BadLocal())
		return;

	PX_PROFILE_SECTION("Visuals", "DisplaySpecList::OnDrawSpecList");

	ILocalPlayer pMe;
	PlayerInfo playerinfo;

	struct NameAndMode
	{
		const char* Mode;
		std::string Name;
	};
	std::vector<NameAndMode> spectators;
	spectators.reserve(4);

	for (int i = 1; i <= interfaces::GlobalVars->MaxClients && spectators.size() <= static_cast<size_t>(spec_list.m_DisplayCount); i++)
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

		if (interfaces::EngineClient->GetPlayerInfo(i, &playerinfo))
			spectators.emplace_back(spec_mode, playerinfo.Name);
	}

	if (spectators.empty())
		return;

	const ImGuiWindowFlags background_flag = spec_list.m_Locked ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;

	float p = spec_list.m_InnerColor[0];
	/*ImGui::SetNextWindowSize({ 0.f, 0.f }, ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { spec_list.m_InnerColor[0] , spec_list.m_InnerColor[1] ,spec_list.m_InnerColor[2], spec_list.m_InnerTransparent ? 0.f : spec_list.m_InnerColor[3] });
	ImGui::PushStyleColor(ImGuiCol_Border, { spec_list.m_OuterColor[0] , spec_list.m_OuterColor[1] ,spec_list.m_OuterColor[2], spec_list.m_OuterColor[3] });
	bool is_open = ImGui::Begin("Spectator list", spec_list.m_Enabled.data(), background_flag | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::PopStyleColor(2);

	if (is_open)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { spec_list.m_TextColor[0], spec_list.m_TextColor[1], spec_list.m_TextColor[2], spec_list.m_TextColor[3] });
		for (auto& name_and_mode : spectators)
		{
			ImGui::Text("%s%s", name_and_mode.Mode, name_and_mode.Name.c_str());
		}
		ImGui::PopStyleColor();
	}*/

	ImGui::End();
}
