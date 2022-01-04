
#include <tf2/engine/GlobalVars.hpp>
#include <tf2/engine/ClientDll.hpp>

#include <tf2/entity/BaseProjectile.hpp>
#include <tf2/entity/BasePlayer.hpp>
#include <tf2/entity/EntityIterator.hpp>

#include <tf2/studio/Model.hpp>
#include <tf2/utils/Draw.hpp>

#include "ESP.hpp"

bool GlobalESP::GetBoxInfo(const tf2::IBaseEntity pEnt, ESPInfo::BoxInfo& boxinfo)
{
	using namespace tf2;
	const Vector3D_F& origin = pEnt->VecOrigin;

	const bool is_zero = pEnt->Mins->is_zero();
	auto get_min_max = [] (const tf2::IBaseEntity pEnt, bool is_min) -> Vector3D_F
	{
		const ModelInfo* model = pEnt->GetModel();
		if (!model)
			return { };
		return (is_min ? model->Mins : model->Maxs) * 1.65;
	};

	const Vector3D_F mins = origin + (is_zero ? get_min_max(pEnt, true) : pEnt->Mins);
	const Vector3D_F maxs = origin + (is_zero ? get_min_max(pEnt, false) : pEnt->Maxs);

	const Vector3D_F delta = maxs - mins;
	const Vector3D_F corners[]{
		mins,
		mins + Vector3D_F{ delta[0],	0.f,		0.f },
		mins + Vector3D_F{ delta[0],	delta[1],	0.f },
		mins + Vector3D_F{ 0.f,			delta[1],	0.f },

		mins + Vector3D_F{ 0.f,			0.f,		delta[2] },
		mins + Vector3D_F{ delta[0],	0.f,		delta[2] },
		mins + Vector3D_F{ delta[0],	delta[1],	delta[2] },
		mins + Vector3D_F{ 0.f,			delta[1],	delta[2] },
	};

	static_assert(std::ssize(corners) == 8);

	for (size_t i = 0; i < 8; i++)
		if (!tf2::utils::GetVectorInHudSpace(corners[i], { ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y }, boxinfo.m_Corners[i]))
			return false;

	return true;
}

void GlobalESP::DrawSharedInfo(
	const ESPInfo::BoxInfo& boxinfo, 
	const ESPInfo::Shared* esp_info,
	const ESPOverride* esp_override, 
	const ESPInfo::DrawTextCallback& callback
)
{
	using namespace tf2;
	using PosType = ESPInfo::BoxInfo::PosType;
	ImDrawList* pDraw = ImGui::GetBackgroundDrawList();
	
	switch (esp_override ? esp_override->DrawMode : esp_info->DrawMode)
	{
	case ESPInfo::ESPMode::type::Box:
	{
		ImU32 line_color = ImGui::GetColorU32(esp_override ? 
			esp_override->Color.to<ImVec4>() :
			esp_info->Rainbow.Speed ? 
			esp_info->Rainbow.Color.to<ImVec4>() :
			esp_info->Box.DrawColor->to<ImVec4>()
		);

		for (size_t i = 0; i < 4; i++)
		{
			const auto& cur_bot = boxinfo.GetPosition<PosType::Bottom>(i).to<ImVec2>();
			const auto& cur_up = boxinfo.GetPosition<PosType::Upper>(i).to<ImVec2>();

			pDraw->AddLine(cur_bot, cur_up, line_color, esp_info->Box.LineThickness);

			const auto& next_bot = boxinfo.GetPosition<PosType::Bottom>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_bot, next_bot, line_color, esp_info->Box.LineThickness);

			const auto& next_up = boxinfo.GetPosition<PosType::Upper>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_up, next_up, line_color, esp_info->Box.LineThickness);
		}

		break;
	}
	case ESPInfo::ESPMode::type::BoxOutline:
	{
		ImU32 line_color = ImGui::GetColorU32(esp_override ?
			esp_override->Color.to<ImVec4>() :
			esp_info->Rainbow.Speed ?
			esp_info->Rainbow.Color.to<ImVec4>() :
			esp_info->OutlineBox.DrawColor->to<ImVec4>()
		);
		ImU32 outline_color = ImGui::GetColorU32(esp_info->OutlineBox.OutlineColor->to<ImVec4>());

		for (size_t i = 0; i < 4; i++)
		{
			const auto& cur_bot = boxinfo.GetPosition<PosType::Bottom>(i).to<ImVec2>();
			const auto& cur_up = boxinfo.GetPosition<PosType::Upper>(i).to<ImVec2>();

			pDraw->AddLine(cur_bot, cur_up, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_bot, cur_up, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);

			const auto& next_bot = boxinfo.GetPosition<PosType::Bottom>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_bot, next_bot, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_bot, next_bot, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);

			const auto& next_up = boxinfo.GetPosition<PosType::Upper>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_up, next_up, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_up, next_up, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);
		}
	}
	}

	if (!esp_override)
	{
		const float text_height = (this->m_DisplayFont ? this->m_DisplayFont->FontSize : ImGui::GetTextLineHeight()) + esp_info->Text.Height;

		const Vector2D_F delta_updown = boxinfo.GetPosition<PosType::Upper>(ESPInfo::BoxInfo::BOTTOM_L) - boxinfo.GetPosition<PosType::Bottom>(ESPInfo::BoxInfo::BOTTOM_L);
		const float text_size = std::min(esp_info->Text.Size.get(), esp_info->Text.Size * std::abs(delta_updown[1]) / 5.f + 1.f);

		const Vector2D_F* ideal_pos = nullptr;
		float ideal_delta = -FLT_MAX;

		for (size_t i = 0; i < 4; i++)
		{
			const Vector2D_F& cur_pos = boxinfo.GetPosition<ESPInfo::BoxInfo::PosType::Upper>(i);
			const Vector2D_F& next_pos = boxinfo.GetPosition<ESPInfo::BoxInfo::PosType::Upper>((i + 1) % 4);
			float delta = next_pos[0] - cur_pos[0];

			if (ideal_delta < delta)
			{
				ideal_delta = delta;
				ideal_pos = &next_pos;
			}
		}

		ESPInfo::TextInfo renderer(
			pDraw, 
			this->m_DisplayFont, 
			ImGui::GetColorU32(*std::bit_cast<ImVec4*>(esp_info->Text.Color.data())),
			text_size,
			text_height, 
			{ (*ideal_pos)[0] + esp_info->Text.Offset[0], (*ideal_pos)[1] + esp_info->Text.Offset[1] }
		);

		callback(esp_info, renderer);
	}
}


void GlobalESP::DrawSharedHealth(const ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, int cur_health, int max_health, bool is_player)
{
	using namespace tf2;

	ImDrawList* pDraw = ImGui::GetBackgroundDrawList();
	const float health_percentage = std::min(static_cast<float>(cur_health) / max_health, 1.f);
	const Vector4D_F color = GetHealthColor(cur_health, max_health);

	const Vector2D_F* ideal_min = nullptr, * ideal_max = nullptr;
	const auto draw_type = is_player ?
		static_cast<ESPInfo::Player*>(esp_info)->HealthbarMode.get() :
		static_cast<ESPInfo::Building*>(esp_info)->HealthbarMode.get();

	// Find the best and nearest position from our hud
	auto [ideal_delta, start_offset] = [&ideal_min, &ideal_max, &boxinfo](ESPInfo::HealthBarMode::type type) -> std::pair<Vector2D_F, Vector2D_F>
	{
		Vector2D_F out_delta;
		Vector2D_F start_offset;

		switch (type)
		{
		case ESPInfo::HealthBarMode::type::Horizantal_Up:
		case ESPInfo::HealthBarMode::type::Horizantal_Down:
		{
			float ideal_delta = -FLT_MAX;
			const size_t extra_offset = type == ESPInfo::HealthBarMode::type::Horizantal_Up ? 4 : 0;
			for (size_t i = 0; i < 4; i++)
			{
				const Vector2D_F& cur_pos = boxinfo.m_Corners[extra_offset + i];
				const Vector2D_F& next_pos = boxinfo.m_Corners[extra_offset + (i + 1) % 4];
				float delta = next_pos[0] - cur_pos[0];

				if (ideal_delta < delta)
				{
					ideal_delta = delta;
					ideal_min = &cur_pos;
					ideal_max = &next_pos;
				}
			}

			out_delta[0] = ideal_delta * 0.75f;
			start_offset[1] = extra_offset ? -10.f : 10.f;
			break;
		}

		case ESPInfo::HealthBarMode::type::Vertical_Left:
		case ESPInfo::HealthBarMode::type::Vertical_Right:
		{
			const bool is_left = type == ESPInfo::HealthBarMode::type::Vertical_Left;
			float ideal_delta = -FLT_MAX;

			for (size_t i = 0; i < 4; i++)
			{
				const Vector2D_F& cur_pos = boxinfo.m_Corners[i];
				const Vector2D_F& next_pos = boxinfo.m_Corners[(i + 1) % 4];

				float delta = next_pos[0] - cur_pos[0];

				if (ideal_delta < delta)
				{
					ideal_delta = delta;

					if (is_left)
					{
						ideal_min = &boxinfo.m_Corners[i];
						ideal_max = &boxinfo.m_Corners[4 + i];
					}
					else
					{
						ideal_min = &boxinfo.m_Corners[(i + 1) % 4];
						ideal_max = &boxinfo.m_Corners[4 + (i + 1) % 4];
					}
				}
			}

			out_delta[1] = ((*ideal_max)[1] - (*ideal_min)[1]) * 0.75f;
			start_offset[0] = is_left ? -10.f : 10.f;
			break;
		}
		}

		return { out_delta, start_offset };
	}(draw_type);

	const ImVec2 pos_min = { (*ideal_min)[0] + start_offset[0], (*ideal_min)[1] + start_offset[1]};
	const ImVec2 pos_max = { pos_min[0] + start_offset[0], pos_min[1] + start_offset[1] };

	constexpr ImU32 white_color =
		255 << IM_COL32_R_SHIFT |
		255 << IM_COL32_G_SHIFT |
		255 << IM_COL32_B_SHIFT |
		255 << IM_COL32_A_SHIFT;

	// Draw Health backgroud
	pDraw->AddRectFilled(
		pos_min,
		{ pos_max[0] + ideal_delta[0], pos_max[1] + ideal_delta[1]},
		white_color
	);

	// Draw Health
	pDraw->AddRectFilled(
		pos_min,
		{ pos_max[0] + ideal_delta[0] * health_percentage, pos_max[1] + ideal_delta[1] * health_percentage},
		ImGui::GetColorU32(*std::bit_cast<ImVec4*>(color.data()))
	);
}


tf2::Color4_F GlobalESP::GetHealthColor(int cur, int max)
{
	const float ratio = static_cast<float>(cur) / static_cast<float>(max);
	if (ratio > 1.f)
		return {
		0.26f,
		0.5f,
		1.f,
		1.f
	};
	else if (ratio <= 0.5f)
		return {
		1.f,
		ratio * 2.f,
		0,
		255
	};
	else
		return {
		1.f - (2.f * (ratio - 0.5f)),
		1.f,
		0.f,
		1.f
	};
}


void GlobalESP::RenderESP()
{
	using namespace tf2;

	ESPInfo::BoxInfo box_info;
	ILocalPlayer pMe;
	Const::TFTeam my_team = pMe->TeamNum;


	for (auto pEnt : utils::IBaseEntityIterator{})
	{
		if (pEnt->IsDormantEx() || !utils::IsVectorInHudSpace(pEnt->VecOrigin))
			continue;
		
		Const::EntClassID cls_id = pEnt->GetClientClass()->ClassID;
		switch (cls_id)
		{
		case Const::EntClassID::CTFPlayer:
		{
			ITFPlayer player(pEnt);
			if (pMe != pEnt && player->Class != Const::TFClass::Unknown && player->LifeState == Const::PlayerLifeState::Alive)
				RenderPlayerESP(player, box_info);
			continue;
		}
		case Const::EntClassID::CObjectSentrygun:
		case Const::EntClassID::CObjectDispenser:
		case Const::EntClassID::CObjectTeleporter:
		{
			RenderBuildingESP(pEnt, cls_id, box_info, pEnt->entindex());
			continue;
		}
		}

		if (!m_ObjectESPInfo.Enable  || pEnt->TeamNum == my_team)
			continue;

		const char* name;
		switch(cls_id)
		{
		case Const::EntClassID::CTFProjectile_Rocket:
		case Const::EntClassID::CTFProjectile_SentryRocket:
		{
			if (!m_ObjectESPInfo.DrawRockets)
				continue;
			else
			{
				name = "Rocket";
				break;
			}
		}

		case Const::EntClassID::CTFGrenadePipebombProjectile:
		{
			IBaseGrenade proj{ pEnt };
			switch (proj->Type)
			{
			case Const::EntGrenadeProjType::Detonate:
			case Const::EntGrenadeProjType::DetonateRemote:
			{
				if (!m_ObjectESPInfo.DrawStickies)
					continue;
				else
				{
					name = "Stickie bomb";
					break;
				}
			}

			default:
			{
				if (!m_ObjectESPInfo.DrawPipes)
					continue;
				else
				{
					name = "Pipe bomb";
					break;
				}
			}
			}
			break;
		}

		case Const::EntClassID::CTFAmmoPack:
		{
			if (!m_ObjectESPInfo.Enable || !m_ObjectESPInfo.DrawPacks)
				continue;
			else
			{
				name = "Ammo pack";
				break;
			}
		}

		case Const::EntClassID::CMerasmus:
		case Const::EntClassID::CMerasmusDancer:
		{
			if (!m_ObjectESPInfo.DrawNPC)
				continue;
			else
			{
				name = "Merasmus";
				break;
			}
		}

		case Const::EntClassID::CTFTankBoss:
		{
			if (!m_ObjectESPInfo.DrawNPC)
				continue;
			else
			{
				name = "Tank";
				break;
			}
		}

		case Const::EntClassID::CZombie:
		{
			if (!m_ObjectESPInfo.DrawNPC)
				continue;
			else
			{
				name = "Zombie";
				break;
			}
		}
		
		case Const::EntClassID::CEyeballBoss:
		{
			if (!m_ObjectESPInfo.DrawNPC)
				continue;
			else
			{
				name = "Monoculus";
				break;
			}
		}
		
		case Const::EntClassID::CHeadlessHatman:
		{
			if (!m_ObjectESPInfo.DrawNPC)
				continue;
			else
			{
				name = "Horsemann";
				break;
			}
		}
		
		case Const::EntClassID::CHalloweenGiftPickup:
		{
			if (!m_ObjectESPInfo.DrawPacks)
				continue;
			else
			{
				name = "Gargoyle";
				break;
			}
		}


		default:
		{
			if (!m_ObjectESPInfo.Enable)
				continue;

			if (m_ObjectESPInfo.DrawPacks)
			{
				if (pEnt->IsHealthKit())
				{
					name = "Health pack";
					break;
				}
			}
			else if (m_ObjectESPInfo.DrawNPC)
			{
				if (pEnt->IsGhost())
				{
					name = "Ghost";
					break;
				}
			}

			continue;
		}
		}

		double distance = pEnt->VecOrigin->distance_to(pMe->VecOrigin);
		if (m_ObjectESPInfo.MaxDistance > 50.f && m_ObjectESPInfo.MaxDistance < distance)
			return;
		else if (!GetBoxInfo(pEnt, box_info))
			return;

		auto iter_espoverride = m_ESPOverride.find(pEnt->entindex());
		ESPOverride* esp_override = iter_espoverride == m_ESPOverride.end() ? nullptr : &iter_espoverride->second;

		DrawSharedInfo(
			box_info,
			&m_ObjectESPInfo,
			esp_override,
			[item_name = name](const ESPInfo::Shared*, ESPInfo::TextInfo& renderer)
			{
				renderer.AddText(item_name);
			}
		);
	}
}
