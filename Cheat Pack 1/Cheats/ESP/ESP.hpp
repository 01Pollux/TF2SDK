#pragma once

#include <Entity/BaseEntity.hpp>
#include <Entity/BaseObject.hpp>
#include <Math/Vector.hpp>

#include "ICheatIFace.hpp"
#include "Defines.hpp"


namespace ESPInfo
{
	static constexpr size_t TeamBlue = 0, TeamRed = 1, MaxTeams = 2;
	static constexpr float OutlineThicknessMin = 3.5f;
	static constexpr const char* TeamNames[]{ "Blue", "Red" };

	class ESPMode
	{
	public:
		enum class type : char
		{
			None,
			Box,
			BoxOutline
		};

		constexpr ESPMode(type type = type::None) noexcept : m_Type(type) { }
		constexpr operator const type() const noexcept { return m_Type; }

		SG_CONFIG_GET_ENUM()
		{
			return std::array{
				std::pair{ "none", type::None },
				std::pair{ "box", type::Box },
				std::pair{ "box outline", type::BoxOutline },
			};
		}
		SG_CONFIG_IMPL_GET_ENUM(m_Type);

	private:
		type m_Type;
	};
		
	class HealthBarMode
	{
	public:
		enum class type : char
		{
			Disabled,
			Horizantal_Up,
			Horizantal_Down,
			
			Vertical_Left,
			Vertical_Right,

			Count
		};

		constexpr HealthBarMode(type type = type::Disabled) noexcept : m_Type(type) { }
		constexpr operator type() const noexcept { return m_Type; }
		constexpr operator size_t() const noexcept { return static_cast<size_t>(m_Type); }

		SG_CONFIG_GET_ENUM()
		{
			return std::array{
				std::pair{ "none",				type::Disabled },
				std::pair{ "horizantal up",		type::Horizantal_Up },
				std::pair{ "horizantal down",	type::Horizantal_Down },
				std::pair{ "vertical left",		type::Vertical_Right },
				std::pair{ "vertical right",	type::Vertical_Left }
			};
		}
		SG_CONFIG_IMPL_GET_ENUM(m_Type);

	private:
		type m_Type;
	};
	
	struct Shared
	{
		SG::Config<bool> Enable				{ "enable",			false };

		struct
		{
			SG::Config<TF2::Color4_F>	DrawColor		{ "draw color",	{ 1.f, 1.f, 1.f, 1.f }, "box's draw color" };
			SG::Config<float>			LineThickness	{ "line's thickness",	1.f, "box's line size" };
		} Box;

		struct
		{
			SG::Config<TF2::Color4_F>	DrawColor		{ "draw color",		{ 1.f, 1.f, 1.f, 1.f }, "box's draw color" };
			SG::Config<TF2::Color4_F>	OutlineColor	{ "outline color",	{ 1.f, 1.f, 1.f, 1.f }, "box's outline color" };
			SG::Config<float>			LineThickness	{ "line's thickness",	1.f, "box's line size" };
		} OutlineBox;

		struct
		{
			SG::Config<TF2::Color4_F>	Color		{ "text color",		{ 1.f, 1.f, 1.f, 1.f }, "ESP's text color" };
			SG::Config<TF2::Vector2D_F>	Offset		{ "text offset",	{ 0.f, 0.f }, "text's offset"};
			SG::Config<float>			Height		{ "text height",	0.5f, "text's height" };
			SG::Config<float>			Size		{ "text size",		12.f, "ESP's text size" };
		} Text;

		SG::Config<int> MaxDistance			{ "max distance",	1024, "ESP's max draw distance" };

		SG::Config<ESPMode> DrawMode		{ "draw mode",		ESPMode::type::Box, "ESP's draw mode" };

		SG::Config<bool> DrawName			{ "draw name",		false, "Draw entity's name, or player's name" };
		SG::Config<bool> DrawDistance		{ "draw distance",	false, "Draw distance from entity to local player" };
	};

	struct Player : public Shared
	{
		SG::Config<bool> DrawClass		{ "draw class",		false,	"Draw player tf-class" };
		SG::Config<bool> DrawClassIcon	{ "draw class icon",false,	"Draw player tf-class" };
		SG::Config<bool> DrawCond		{ "draw condition", false,	"Draw some player's conds" };
		SG::Config<bool> DrawUberPerc	{ "draw uber",		false,	"Draw how much uber a medic has" };
		SG::Config<bool> IgnoreCloak	{ "ignore cloak",	true,	"Don't draw cloaked spies" };
		SG::Config<bool> DrawTeam		{ "draw team",		false,	"Draw player's team" };

		SG::Config<HealthBarMode> HealthbarMode { "healthbar mode",		{ }, "Draw how much health a player has" };
	};

	struct Building : public Shared
	{
		SG::Config<bool> DrawOwner	{ "draw owner",		false, "Draw building's owner" };
		SG::Config<bool> DrawAmmo	{ "draw ammo",		false, "Draw building's ammo" };
		SG::Config<bool> DrawLevel	{ "draw level",		false, "Draw building's level" };
		SG::Config<bool> DrawBState	{ "draw state",		false, "Draw building's state" };
		SG::Config<bool> DrawTeam	{ "draw team",		false, "Draw building's team" };
		SG::Config<bool> DrawHealth	{ "draw health",	false, "Draw building's health" };

		SG::Config<HealthBarMode> HealthbarMode { "healthbar mode",		{ }, "Draw how much health a player has" };
	};

	struct Object : public Shared
	{
		SG::Config<bool> DrawPacks			{ "draw packs",		false, "Draw Packs" };
		SG::Config<bool> DrawRockets		{ "draw rockets",	false, "Draw Rockets" };
		SG::Config<bool> DrawPipes			{ "draw pipes",		false, "Draw Pipe bomb" };
		SG::Config<bool> DrawStickies		{ "draw stickies",	false, "Draw stickie bomb" };
		SG::Config<bool> DrawNPC			{ "draw npcs",		false, "Draw npcs" };
	};

	struct BoxInfo
	{
		static constexpr int UPPER_L = 0;
		static constexpr int UPPER_R = 1;
		static constexpr int BOTTOM_R = 2;
		static constexpr int BOTTOM_L = 3;

		enum class PosType
		{
			Bottom,
			Upper
		};

		template<PosType type>
		const TF2::Vector2D_F& GetPosition(size_t offset) const noexcept
		{
			if constexpr (type == PosType::Bottom)
				return m_Corners[offset];
			else
				return m_Corners[offset + 4];
		}

		std::pair<TF2::Vector2D_F, TF2::Vector2D_F >
			GetMinMax() const noexcept
		{
			TF2::Vector2D_F max{ -1, -1 }, min{ 65536, 65536 };
			
			for (const auto& vec : m_Corners)
			{
				for (size_t i = 0; i < max.size(); i++)
				{
					if (vec[i] > max[i])
						max[i] = vec[i];
					if (vec[i] < min[i])
						min[i] = vec[i];
				}
			}

			return { min, max };
		}

		TF2::Vector2D_F m_Corners[8];
	};

	struct TextInfo
	{
	public:
		TextInfo(ImDrawList* list, ImFont* font, ImU32 color, float size, float height, const ImVec2 start_pos) noexcept :
			m_DrawList(list), m_TextFont(font), m_TextColor(color), m_TextSize(size), m_SpacingHeight(height), m_DrawPos(start_pos) { }

		void AddText(const std::string& str)
		{
			m_DrawList->AddText(m_TextFont, m_TextSize, m_DrawPos, m_TextColor, str.c_str(), str.c_str() + str.size());
			m_DrawPos.y += m_SpacingHeight;
		}

		void AddText(const char* str)
		{
			m_DrawList->AddText(m_TextFont, m_TextSize, m_DrawPos, m_TextColor, str);
			m_DrawPos.y += m_SpacingHeight;
		}

	private:
		ImDrawList* m_DrawList;
		ImFont* m_TextFont;

		ImU32 m_TextColor;
		float m_TextSize;

		float m_SpacingHeight;
		ImVec2 m_DrawPos;
	};

	using DrawTextCallback = std::function<void(Shared* esp_info, TextInfo& text_renderer)>;
}


class GlobalESP : public ICheatIFace
{
private:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginUnload() override;

	static void OnDrawESP(ImGuiContext* imgui, ImGuiContextHook* ctx);

	void OnSaveConfig(Json& cfg);
	void OnReloadConfig(const Json& cfg);

	bool OnRender();

private:
	bool GetBoxInfo(const TF2::IBaseEntity pEnt, ESPInfo::BoxInfo& boxinfo);

	void DrawSharedInfo(ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, const ESPInfo::DrawTextCallback& callback);

	void DrawSharedHealth(ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, int cur_health, int max_health, bool is_player);

	TF2::Color4_F GetHealthColor(int cur, int max);

private:
	/// <summary>
	/// Handles player's esp
	/// </summary>
	void RenderPlayerESP();

	/// <summary>
	/// Handles anything else's esp and call RenderBuildingESP() and RenderItemsESP()
	/// </summary>
	void RenderExtraESP();

	/// <summary>
	/// Handles building's esp, ie sentry...
	/// </summary>
	void RenderBuildingESP(const TF2::IBaseObject pEnt, TF2::Const::EntClassID class_id, ESPInfo::BoxInfo& box_info);

	///// <summary>
	///// Handles item's esp, ie pickups, projectiles...
	///// </summary>
	//void RenderItemsESP();

private:
	ESPInfo::Player		m_PlayerESPInfo[ESPInfo::MaxTeams];
	ESPInfo::Building	m_BuildingESPInfo[ESPInfo::MaxTeams];
	ESPInfo::Object		m_ObjectESPInfo;

	ImFont* m_DisplayFont;
	ImGuiID m_RenderHookId;
};