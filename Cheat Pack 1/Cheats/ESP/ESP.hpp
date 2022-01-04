#pragma once

#include <tf2/entity/BaseObject.hpp>
#include <tf2/entity/BasePlayer.hpp>
#include <tf2/math/Vector.hpp>

#include "ICheatIFace.hpp"
#include "Defines.hpp"


namespace ESPInfo
{
	static constexpr size_t TeamBlue = 0, TeamRed = 1, MaxTeams = 2;
	static constexpr float OutlineThicknessMin = 3.5f;
	static constexpr const char* TeamNames[]{ "Blue", "Red" };

	struct ESPMode
	{
		enum class type : char
		{
			None,
			Box,
			BoxOutline,

			Count
		};

		static constexpr const char* description = 
R"(ESP's draw mode.
values: 
	] none.
	] box.
	] box outline.)";

		constexpr ESPMode(type _type = type::None) noexcept : m_Value(_type) { }

		constexpr operator type() const noexcept	{ return m_Value; }
		constexpr operator type&() noexcept			{ return m_Value; }

		static constexpr ESPMode from_str(const std::string_view& value)
		{
			for (auto& val : type_names())
			{
				if (value == val.first)
					return val.second;
			}
			throw std::runtime_error("Invalid type name was passed for HealthBarMode.");
		}

		static constexpr std::string to_str(const ESPMode& value)
		{
			for (auto& val : type_names())
			{
				if (value == val.second)
					return { val.first.begin(), val.first.end() };
			}
			throw std::runtime_error("Invalid type name was passed for HealthBarMode.");
		}

		constexpr static
			std::array<
			std::pair<std::string_view, type>,
			static_cast<std::underlying_type_t<type>>(type::Count)
			>
			type_names() noexcept
		{
			return {
					std::pair{ "none",			type::None },
					std::pair{ "box",			type::Box },
					std::pair{ "box outline",	type::BoxOutline }
			};
		}

	private:
		type m_Value{ };
	};
		
	struct HealthBarMode
	{
		enum class type : char
		{
			Disabled,
			Horizantal_Up,
			Horizantal_Down,
			
			Vertical_Left,
			Vertical_Right,

			Count
		};

		static constexpr const char* description =
R"(Draw how much health a player has.
values: 
	] none.
	] horizantal up.
	] horizantal down.
	] vertical up.
	] vertical down.)";
		
		constexpr HealthBarMode(type _type = type::Disabled) noexcept : m_Value(_type) { }

		constexpr operator type() const noexcept	{ return m_Value; }
		constexpr operator type&() noexcept			{ return m_Value; }

		static constexpr HealthBarMode from_str(const std::string_view& value)
		{
			for (auto& val : type_names())
			{
				if (value == val.first)
					return val.second;
			}
			throw std::runtime_error("Invalid type name was passed for HealthBarMode.");
		}

		static constexpr std::string to_str(const HealthBarMode& value)
		{
			for (auto& val : type_names())
			{
				if (value == val.second)
					return { val.first.begin(), val.first.end() };
			}
			throw std::runtime_error("Invalid type name was passed for HealthBarMode.");
		}

		constexpr static 
			std::array<
				std::pair<std::string_view, type>,
				static_cast<std::underlying_type_t<type>>(type::Count)
			>
		type_names() noexcept
		{
			return {
					std::pair{ "none",				type::Disabled },
					std::pair{ "horizantal up",		type::Horizantal_Up },
					std::pair{ "horizantal down",	type::Horizantal_Down },
					std::pair{ "vertical left",		type::Vertical_Right },
					std::pair{ "vertical right",	type::Vertical_Left }
			};
		}

	private:
		type m_Value{ };
	};
	
	struct Shared
	{
		px::ConVar<bool> Enable;
		
		struct _Rainbow_t
		{
			tf2::Color4_F				Color	{ 1.f, 1.f, 1.f, 1.f };
			px::ConVar<float>			Speed;

			_Rainbow_t(
				const char* rainbow_speed_color
			) : 
				Speed(rainbow_speed_color, .5f, "ESP rainbow speed time")
			{ }
		} Rainbow;

		struct _3DBox_t
		{
			px::ConVar<tf2::Color4_F>	DrawColor;
			px::ConVar<float>			LineThickness;

			_3DBox_t(
				const char* draw_color,
				const char* line_thickness
			) :
				DrawColor(draw_color,			{ 1.f, 1.f, 1.f, 1.f }, "ESP box's draw color"),
				LineThickness(line_thickness,	1.f, "ESP box's line size")
			{ }
		} Box;

		struct _Outline3DBox_t
		{
			px::ConVar<tf2::Color4_F>	DrawColor;
			px::ConVar<tf2::Color4_F>	OutlineColor;
			px::ConVar<float>			LineThickness;

			_Outline3DBox_t(
				const char* draw_color,
				const char* outline_color,
				const char* line_thickness
			) :
				DrawColor(draw_color,			{ 1.f, 1.f, 1.f, 1.f }, "box's draw color"),
				OutlineColor(outline_color,		{ 1.f, 1.f, 1.f, 1.f }, "box's outline color"),
				LineThickness(line_thickness,	1.f, "box's line size")
			{ }
		} OutlineBox;

		struct _Text_t
		{
			px::ConVar<tf2::Color4_F>	Color;
			px::ConVar<tf2::Vector2D_F>	Offset;
			px::ConVar<float>			Height;
			px::ConVar<float>			Size;

			_Text_t(
				const char* draw_color,
				const char* text_offset,
				const char* text_height,
				const char* text_size
			) :
				Color(draw_color,	{ 1.f, 1.f, 1.f, 1.f }, "ESP's text color"),
				Offset(text_offset, { 0.f, 0.f }, "text's offset"),
				Height(text_height,	.5f, "text's height"),
				Size(text_size,		12.f, "ESP's text size")
			{ }
		} Text;

		px::ConVar<int> MaxDistance;

		px::ConVar<ESPMode> DrawMode;

		px::ConVar<bool> DrawName;
		px::ConVar<bool> DrawDistance;

		Shared(
			const char* enable,

			const char* rainbow_speed_color,

			const char* box3d_draw_color,
			const char* box3d_line_thickness,

			const char* outlinebox3d_draw_color,
			const char* outlinebox3d_outline_color,
			const char* outlinebox3d_line_thickness,

			const char* text_color,
			const char* text_offset,
			const char* text_height,
			const char* text_size,

			const char* max_distance,
			const char* draw_mode,
			const char* draw_name,
			const char* draw_distance
		) :
			Enable(enable,				false, "Enable esp"),

			Rainbow(rainbow_speed_color),
			Box(box3d_draw_color, box3d_line_thickness),
			OutlineBox(outlinebox3d_draw_color, outlinebox3d_outline_color, outlinebox3d_line_thickness),
			Text(text_color, text_offset, text_height, text_size),

			MaxDistance(max_distance,	1024,	"ESP's max draw distance."),
			DrawMode(draw_mode,			{ },	ESPMode::description),
			DrawName(draw_name,			false,	"Draw entity's name, or player's name."),
			DrawDistance(draw_distance,	false,	"Draw distance from entity to local player.")
		{ }
	};

	struct Player : public Shared
	{
		px::ConVar<bool> DrawClass; 
		px::ConVar<bool> DrawClassIcon;
		px::ConVar<bool> DrawCond;
		px::ConVar<bool> DrawUberPerc;
		px::ConVar<bool> IgnoreCloak;
		px::ConVar<bool> DrawTeam;

		px::ConVar<HealthBarMode> HealthbarMode;

		Player(
			bool is_red
		) :
			Shared(
				is_red ? "esp_red_player_enable" : "esp_blue_player_enable",

				is_red ? "esp_red_player_rainbow_speed_color" : "esp_blue_player_rainbow_speed_color",

				is_red ? "esp_red_player_3dbox_color"	: "esp_red_player_3dbox_thickness", 
				is_red ? "esp_blue_player_3dbox_color"	: "esp_blue_player_3dbox_thickness",

				is_red ? "esp_red_player_outlinebox_incolor"	: "esp_blue_player_outlinebox_incolor",
				is_red ? "esp_red_player_outlinebox_outcolor"	: "esp_blue_player_outlinebox_outcolor",
				is_red ? "esp_red_player_outlinebox_thickness"	: "esp_blue_player_outlinebox_thickness",

				is_red ? "esp_red_player_text_color"	: "esp_blue_player_text_color",
				is_red ? "esp_red_player_text_offset"	: "esp_blue_player_text_offset",
				is_red ? "esp_red_player_text_height"	: "esp_blue_player_text_height",
				is_red ? "esp_red_player_text_size"		: "esp_blue_player_text_size",

				is_red ? "esp_red_player_max_distance"	: "esp_blue_player_max_distance",
				is_red ? "esp_red_player_draw_mode"		: "esp_blue_player_draw_mode",
				is_red ? "esp_red_player_draw_name"		: "esp_blue_player_draw_name",
				is_red ? "esp_red_player_draw_distance" : "esp_blue_player_draw_distance"
			),
			DrawClass(is_red ?		"esp_red_player_class" :		"esp_blue_player_class",		false,	"Draw player tf-class."),
			DrawClassIcon(is_red ?	"esp_red_player_class_icon" :	"esp_blue_player_class_icon",	false,	"Draw player tf-class' icon."),
			DrawCond(is_red ?		"esp_red_player_cond" :			"esp_blue_player_cond",			false,	"Draw some player's conds."),
			DrawUberPerc(is_red ?	"esp_red_player_uberperc" :		"esp_blue_player_uberperc",		false,	"Draw how much uber a medic has."),
			IgnoreCloak(is_red ?	"esp_red_player_nocloak" :		"esp_blue_player_nocloak",		false,	"Don't draw cloaked spies."),
			DrawTeam(is_red ?		"esp_red_player_team" :			"esp_blue_player_team",			false,	"Draw player's team."),

			HealthbarMode(is_red ?	"esp_red_player_hpmode" :		"esp_blue_player_hpmode",		{ },	HealthBarMode::description)
		{ }
	};

	struct Building : public Shared
	{
		px::ConVar<bool> DrawOwner;
		px::ConVar<bool> DrawAmmo;
		px::ConVar<bool> DrawLevel;
		px::ConVar<bool> DrawBState;
		px::ConVar<bool> DrawTeam;
		px::ConVar<bool> DrawHealth;

		px::ConVar<HealthBarMode> HealthbarMode;

		Building(
			bool is_red
		) :
			Shared(
				is_red ? "esp_red_building_enable" : "esp_blue_building_enable",

				is_red ? "esp_red_building_rainbow_speed_color" : "esp_blue_building_rainbow_speed_color",

				is_red ? "esp_red_building_3dbox_color"		: "esp_red_building_3dbox_thickness",
				is_red ? "esp_blue_building_3dbox_color"	: "esp_blue_building_3dbox_thickness",

				is_red ? "esp_red_building_outlinebox_incolor"		: "esp_blue_building_outlinebox_incolor",
				is_red ? "esp_red_building_outlinebox_outcolor"		: "esp_blue_building_outlinebox_outcolor",
				is_red ? "esp_red_building_outlinebox_thickness"	: "esp_blue_building_outlinebox_thickness",

				is_red ? "esp_red_building_text_color"	: "esp_blue_building_text_color",
				is_red ? "esp_red_building_text_offset" : "esp_blue_building_text_offset",
				is_red ? "esp_red_building_text_height" : "esp_blue_building_text_height",
				is_red ? "esp_red_building_text_size"	: "esp_blue_building_text_size",

				is_red ? "esp_red_building_max_distance"	: "esp_blue_building_max_distance",
				is_red ? "esp_red_building_draw_mode"		: "esp_blue_building_draw_mode",
				is_red ? "esp_red_building_draw_name"		: "esp_blue_building_draw_name",
				is_red ? "esp_red_building_draw_distance"	: "esp_blue_building_draw_distance"
			),
			DrawOwner(is_red ?		"esp_red_building_owner" :		"esp_blue_building_owner",		false,	"Draw building's owner."),
			DrawAmmo(is_red ?		"esp_red_building_ammo" :		"esp_blue_building_ammo",		false,	"Draw building's ammo."),
			DrawLevel(is_red ?		"esp_red_building_level" :		"esp_blue_building_level",		false,	"Draw building's level."),
			DrawBState(is_red ?		"esp_red_building_state" :		"esp_blue_building_state",		false,	"Draw building's state."),
			DrawTeam(is_red ?		"esp_red_building_team" :		"esp_blue_building_team",		false,	"Draw building's team."),
			DrawHealth(is_red ?		"esp_red_building_health" :		"esp_blue_building_health",		false,	"Draw building's health."),

			HealthbarMode(is_red ?	"esp_red_building_hpmode" :		"esp_blue_building_hpmode",		{ },	HealthBarMode::description)
		{ }
	};

	struct Object : public Shared
	{
		px::ConVar<bool> DrawPacks;
		px::ConVar<bool> DrawRockets;
		px::ConVar<bool> DrawPipes;
		px::ConVar<bool> DrawStickies;
		px::ConVar<bool> DrawNPC;

		Object(
		) :
			Shared(
				"esp_objects_enable",

				"esp_objects_rainbow_speed_color",

				"esp_objects_3dbox_color",
				"esp_objects_3dbox_thickness",

				"esp_objects_outlinebox_incolor",
				"esp_objects_outlinebox_outcolor",
				"esp_objects_outlinebox_thickness",

				"esp_objects_text_color",
				"esp_objects_text_offset",
				"esp_objects_text_height",
				"esp_objects_text_size",

				"esp_objects_max_distance",
				"esp_objects_draw_mode",
				"esp_objects_draw_name",
				"esp_objects_draw_distance"
			),
			DrawPacks("esp_objects_packs",	false,	"Draw Packs."),
			DrawRockets("esp_objects_rockets",	false,	"Draw Rockets."),
			DrawPipes("esp_objects_pipes",	false,	"Draw Pipe bomb."),
			DrawStickies("esp_objects_stickies",	false,	"Draw stickie bomb."),
			DrawNPC("esp_objects_npc",		false,	"Draw npcs.")
		{ }
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
		const tf2::Vector2D_F& GetPosition(size_t offset) const noexcept
		{
			if constexpr (type == PosType::Bottom)
				return m_Corners[offset];
			else
				return m_Corners[offset + 4];
		}

		std::pair<tf2::Vector2D_F, tf2::Vector2D_F >
			GetMinMax() const noexcept
		{
			tf2::Vector2D_F max{ -1, -1 }, min{ 65536, 65536 };
			
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

		tf2::Vector2D_F m_Corners[8];
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

	using DrawTextCallback = std::function<void(const Shared* esp_info, TextInfo& text_renderer)>;
}


class GlobalESP : public ICheatIFace
{
public:
	enum class ESPType : char
	{
		Player,
		Buidling,
		Other
	};
	
	struct ESPOverride
	{
		const tf2::IBaseEntity Entity;

		float RainbowSpeed;
		tf2::Color4_F Color;

		ESPInfo::ESPMode DrawMode;
		ESPType Type;
	};

	static ESPOverride& PushESPOverride(int ent_index, const ESPOverride& esp_override);

	static void PopESPOverride(const tf2::IBaseEntity pEnt);

	static void PopESPOverride(int entindex);

private:
	bool OnAskPluginLoad(tf2::interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginUnload() override;

	static void OnDrawESP(ImGuiContext* imgui, ImGuiContextHook* ctx);

	void OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

private:
	bool GetBoxInfo(const tf2::IBaseEntity pEnt, ESPInfo::BoxInfo& boxinfo);

	void DrawSharedInfo(const ESPInfo::BoxInfo& boxinfo, const ESPInfo::Shared* esp_info, const ESPOverride* esp_override, const ESPInfo::DrawTextCallback& callback);

	void DrawSharedHealth(const ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, int cur_health, int max_health, bool is_player);

	tf2::Color4_F GetHealthColor(int cur, int max);

private:
	/// <summary>
	/// Handles anything esp related by calling RenderBuildingESP() and RenderPlayerESP(()
	/// </summary>
	void RenderESP();

	/// <summary>
	/// Handles player's esp
	/// </summary>
	void RenderPlayerESP(const tf2::ITFPlayer pPlayer, ESPInfo::BoxInfo& box_info);

	/// <summary>
	/// Handles building's esp, ie sentry...
	/// </summary>
	void RenderBuildingESP(const tf2::IBaseObject pEnt, tf2::Const::EntClassID class_id, ESPInfo::BoxInfo& box_info, int ent_index);

private:
	ESPInfo::Player		m_PlayerESPInfo[ESPInfo::MaxTeams]{ true, false };
	ESPInfo::Building	m_BuildingESPInfo[ESPInfo::MaxTeams]{ true, false };
	ESPInfo::Object		m_ObjectESPInfo;

	std::unordered_map<int, ESPOverride> m_ESPOverride;

	ImFont* m_DisplayFont;
	ImGuiID m_RenderHookId;
};
