
#include <Interfaces/GameData.hpp>
#include <imgui/imgui_internal.h>

#include <User/Profiler.hpp>
#include <User/FontAwesome_Icons.hpp>

#include <Engine/ClientDll.hpp>

#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>

#include "ESP.hpp"

static GlobalESP esp_draw;


void GlobalESP::PushESPOverride(int ent_index, const ESPOverride& esp_override)
{
	esp_draw.m_ESPOverride.emplace(ent_index, esp_override);
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

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

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

	if (esp_draw.m_PlayerESPInfo[ESPInfo::TeamRed].Enable || esp_draw.m_PlayerESPInfo[ESPInfo::TeamBlue].Enable)
		esp_draw.RenderPlayerESP();
	
	if (esp_draw.m_PlayerESPInfo[ESPInfo::TeamRed].Enable ||
		esp_draw.m_PlayerESPInfo[ESPInfo::TeamBlue].Enable || esp_draw.m_ObjectESPInfo.Enable)
		esp_draw.RenderExtraESP();
}
