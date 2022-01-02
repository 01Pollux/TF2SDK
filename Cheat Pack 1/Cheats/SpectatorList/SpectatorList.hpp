#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/consts.hpp>

#include "ICheatIFace.hpp"
#include "Defines.hpp"

struct ImGuiContextHook;

class DisplaySpecList : public ICheatIFace
{
private:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;
	void OnPluginUnload() override;

	static void OnDrawSpecList(ImGuiContext* imgui, ImGuiContextHook* ctx);

	void OnSaveConfig(std::vector<SG::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

	SG::ConVar<bool> m_Enabled{ "speclist_enable", false, "view who's spectating local player" };

	SG::ConVar<TF2::Vector4D_F> m_OuterColor{ "speclist_outtercolor", { 0.f, 0.8f, 0.f, 0.9f } };
	SG::ConVar<TF2::Vector4D_F> m_InnerColor{ "speclist_innercolor", { 0.f, 0.2f, 1.f, 0.4f } };
	SG::ConVar<TF2::Vector4D_F> m_TextColor{ "speclist_textcolor", { 1.f, 1.f, 1.f, 1.f } };
	// TODO: add image as background aswell?

	SG::ConVar<bool>	m_InnerTransparent{ "speclist_transparent", false, "don't draw inner color" };
	SG::ConVar<bool>	m_Locked{ "speclist_locked", false, "don't move the list from its current position" };
	SG::ConVar<bool>	m_DisplayMode{ "speclist_displaytype", true, "display how is the player spectating (ie: third-person, etc.)" };
	SG::ConVar<
		int
	> m_DisplayCount{ "speclist_displaycount", TF2::Const::MaxPlayers };

	ImGuiID m_RenderHookId;
	ImFont* m_DisplayFont;
};