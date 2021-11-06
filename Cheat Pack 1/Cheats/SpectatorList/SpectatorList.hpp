#pragma once

#include "ICheatIFace.hpp"
#include "Math/Vector.hpp"
#include "TF2Consts.hpp"
#include "Defines.hpp"

class DisplaySpecList : public ICheatIFace
{
private:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginPauseChange(bool pausing) override;
	void OnPluginUnload() override;

	SG::MHookRes OnCreateMove(SG::PassArgs* pArgs);

	void OnSaveConfig(Json& cfg);
	void OnReloadConfig(const Json& cfg);

	bool OnRender();

	SG::ISHookToken m_CreateMove;

	SG::Config<bool> m_Enabled{ "Enable", false, "view who's spectating local player" };

	SG::Config<TF2::Vector4D_F> m_OuterColor{ "Outter color", { 0.f, 0.8f, 0.f, 0.9f } };
	SG::Config<TF2::Vector4D_F> m_InnerColor{ "Inner color", { 0.f, 0.2f, 1.f, 0.4f } };
	SG::Config<TF2::Vector4D_F> m_TextColor{ "Text color", { 1.f, 1.f, 1.f, 1.f } };
	SG::Config<TF2::Vector2D_F> m_TextPosition{ "Text position", { -FLT_MIN, FLT_MAX } };

	SG::Config<bool> m_DisplayTime{ "Display time", false, "display how for long the player has been spectating" };
	SG::Config<bool> m_DisplayMode{ "Display mode", true, "display how is the player spectating (ie: third-person, etc.)" };
	SG::Config<size_t> m_DisplayCount{ "Display count", TF2::Const::MaxPlayers };
};