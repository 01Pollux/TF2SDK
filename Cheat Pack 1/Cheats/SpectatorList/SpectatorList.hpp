#pragma once

#include "ICheatIFace.hpp"
#include "Math/Vector.hpp"
#include "TF2Consts.hpp"
#include "Defines.hpp"

struct ImGuiContextHook;

class DisplaySpecList : public ICheatIFace
{
private:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;
	void OnPluginUnload() override;

	static void OnDrawSpecList(ImGuiContext* ctx, ImGuiContextHook* hook);

	void OnSaveConfig(Json& cfg);
	void OnReloadConfig(const Json& cfg);

	bool OnRender();

	SG::Config<bool> m_Enabled{ "Enable", false, "view who's spectating local player" };

	SG::Config<TF2::Vector4D_F> m_OuterColor{ "Outter color", { 0.f, 0.8f, 0.f, 0.9f } };
	SG::Config<TF2::Vector4D_F> m_InnerColor{ "Inner color", { 0.f, 0.2f, 1.f, 0.4f } };
	SG::Config<TF2::Vector4D_F> m_TextColor{ "Text color", { 1.f, 1.f, 1.f, 1.f } };
	// TODO: add image as background aswell?

	SG::Config<bool>	m_InnerTransparent{ "transparent background", false, "don't draw inner color" };
	SG::Config<bool>	m_Locked{ "locked list", false, "don't move the list from its current position" };
	SG::Config<bool>	m_DisplayMode{ "Display mode", true, "display how is the player spectating (ie: third-person, etc.)" };
	SG::Config<int>		m_DisplayCount{ "Display count", TF2::Const::MaxPlayers };

	ImGuiID m_RenderHookId;
};