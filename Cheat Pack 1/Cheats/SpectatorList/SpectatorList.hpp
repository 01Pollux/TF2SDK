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

	void OnSaveConfig(nlohmann::json& cfg);
	void OnReloadConfig(const nlohmann::json& cfg);

	bool OnRender();

	SG::Config<bool> m_Enabled{ "enable", false, "view who's spectating local player" };

	SG::Config<TF2::Vector4D_F> m_OuterColor{ "outter color", { 0.f, 0.8f, 0.f, 0.9f } };
	SG::Config<TF2::Vector4D_F> m_InnerColor{ "inner color", { 0.f, 0.2f, 1.f, 0.4f } };
	SG::Config<TF2::Vector4D_F> m_TextColor{ "text color", { 1.f, 1.f, 1.f, 1.f } };
	// TODO: add image as background aswell?

	SG::Config<bool>	m_InnerTransparent{ "transparent background", false, "don't draw inner color" };
	SG::Config<bool>	m_Locked{ "locked list", false, "don't move the list from its current position" };
	SG::Config<bool>	m_DisplayMode{ "display mode", true, "display how is the player spectating (ie: third-person, etc.)" };
	SG::Config<int>		m_DisplayCount{ "display count", TF2::Const::MaxPlayers };

	ImGuiID m_RenderHookId;
	ImFont* m_DisplayFont;
};