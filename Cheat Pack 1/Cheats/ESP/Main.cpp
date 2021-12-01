
#include <Interfaces/GameData.hpp>
#include <imgui/imgui_internal.h>

#include <User/Profiler.hpp>
#include <User/FontAwesome_Icons.hpp>

#include <Engine/ClientDll.hpp>

#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>

#include "ESP.hpp"

static GlobalESP esp_draw;


GlobalESP::ESPOverride& GlobalESP::PushESPOverride(int ent_index, const ESPOverride& esp_override)
{
	return esp_draw.m_ESPOverride.emplace(ent_index, esp_override).first->second;
}

void GlobalESP::PopESPOverride(const TF2::IBaseEntity pEnt)
{
	auto iter = std::find_if(esp_draw.m_ESPOverride.begin(), esp_draw.m_ESPOverride.end(), [pEnt] (const auto& entry) { return entry.second.Entity == pEnt; });
	if (iter != esp_draw.m_ESPOverride.end())
		esp_draw.m_ESPOverride.erase(iter);
}

void GlobalESP::PopESPOverride(int ent_index)
{
	esp_draw.m_ESPOverride.erase(ent_index);
}


bool GlobalESP::OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config)
{
	config.Engine.GlobalVars = true;
	config.Engine.EngineClient = true;

	config.Engine.Convar = true;
	config.Engine.ClientTrace = true;
	config.Engine.DebugOverlay = true;
	config.Engine.ModelInfo = true;

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;
	config.Client.GameRules = true;

	return true;
}

void GlobalESP::OnPluginLoad()
{
	m_DisplayFont = SG::ImGuiLoader->FindFont("Arimo-Medium, s=14");
	SG::ImGuiLoader->AddCallback(SG::ThisPlugin, "ESP", std::bind(&GlobalESP::OnRender, this));

	ImGuiContextHook draw_hook;
	draw_hook.Type = ImGuiContextHookType_EndFramePre;
	draw_hook.Callback = &GlobalESP::OnDrawESP;
	m_RenderHookId = ImGui::AddContextHook(ImGui::GetCurrentContext(), &draw_hook);
}


void GlobalESP::OnPluginUnload()
{
	ImGui::RemoveContextHook(ImGui::GetCurrentContext(), m_RenderHookId);
}


void GlobalESP::OnDrawESP(ImGuiContext* imgui, ImGuiContextHook* ctx)
{
	using namespace TF2;
	if (SG::ThisPlugin->IsPluginPaused() || ITFPlayerInternal::BadLocal())
		return;

	SG_PROFILE_SECTION("Visuals", "GlobalESP::OnDrawESP");

	bool should_draw = false;
	int esp_idx = 0;
	const double current_time = ImGui::GetTime();

	for (auto esp_info : std::array<ESPInfo::Shared*, 5> {
		&esp_draw.m_PlayerESPInfo[ESPInfo::TeamRed],
		&esp_draw.m_PlayerESPInfo[ESPInfo::TeamBlue],
		&esp_draw.m_BuildingESPInfo[ESPInfo::TeamRed],
		&esp_draw.m_BuildingESPInfo[ESPInfo::TeamBlue],
		&esp_draw.m_ObjectESPInfo
	})
	{
		++esp_idx;

		if (!esp_info->Enable)
			continue;
		should_draw = true;

		if (!esp_info->Rainbow.Speed)
			continue;

		Color4_F* pClr;
		switch (esp_info->DrawMode.get())
		{
		case ESPInfo::ESPMode::type::Box:
		{
			pClr = &esp_info->Rainbow.Color;
			break;
		}
		case ESPInfo::ESPMode::type::BoxOutline:
		{
			pClr = &esp_info->Rainbow.Color;
			break;
		}
		default: continue;
		}

		Color4_F& clr = *pClr;
		const double color_time = current_time * esp_info->Rainbow.Speed;

		clr[0] = static_cast<float>(std::sin(color_time) * .5 + 0.5);
		clr[1] = static_cast<float>(std::sin(color_time + 0.6666 * std::numbers::pi) * .5 + 0.5);
		clr[2] = static_cast<float>(std::sin(color_time + 1.3333 * std::numbers::pi) * .5 + 0.5);

		Interfaces::EngineClient->Con_NPrintf(esp_idx + 10, "%f %f %f", clr[0], clr[1], clr[2]);
	}

	if (should_draw)
		esp_draw.RenderESP();

	for (auto& esp_override : esp_draw.m_ESPOverride)
	{
		auto& override_data = esp_override.second;
		if (!override_data.RainbowSpeed)
			continue;

		const double color_time = current_time * override_data.RainbowSpeed;

		override_data.Color[0] = static_cast<float>(std::sin(color_time) * .5 + 0.5);
		override_data.Color[1] = static_cast<float>(std::sin(color_time + 0.6666 * std::numbers::pi) * .5 + 0.5);
		override_data.Color[2] = static_cast<float>(std::sin(color_time + 1.3333 * std::numbers::pi) * .5 + 0.5);
	}
}