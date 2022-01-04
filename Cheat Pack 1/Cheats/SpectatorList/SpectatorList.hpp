#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/consts.hpp>

#include "ICheatIFace.hpp"
#include "Defines.hpp"

struct ImGuiContextHook;

class DisplaySpecList : public ICheatIFace
{
private:
	bool OnAskPluginLoad(tf2::interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;
	void OnPluginUnload() override;

	static void OnDrawSpecList(ImGuiContext* imgui, ImGuiContextHook* ctx);

	void OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

	px::ConVar<bool> m_Enabled{ "speclist_enable", false, "view who's spectating local player" };

	px::ConVar<tf2::Vector4D_F> m_OuterColor{ "speclist_outtercolor", { 0.f, 0.8f, 0.f, 0.9f } };
	px::ConVar<tf2::Vector4D_F> m_InnerColor{ "speclist_innercolor", { 0.f, 0.2f, 1.f, 0.4f } };
	px::ConVar<tf2::Vector4D_F> m_TextColor{ "speclist_textcolor", { 1.f, 1.f, 1.f, 1.f } };
	// TODO: add image as background aswell?

	px::ConVar<bool>	m_InnerTransparent{ "speclist_transparent", false, "don't draw inner color" };
	px::ConVar<bool>	m_Locked{ "speclist_locked", false, "don't move the list from its current position" };
	px::ConVar<bool>	m_DisplayMode{ "speclist_displaytype", true, "display how is the player spectating (ie: third-person, etc.)" };
	px::ConVar<
		int
	> m_DisplayCount{ "speclist_displaycount", tf2::Const::MaxPlayers };

	ImGuiID m_RenderHookId;
	ImFont* m_DisplayFont;
};