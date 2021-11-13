
#include <Engine/GlobalVars.hpp>

#include <Entity/BaseProjectile.hpp>
#include <Entity/BasePlayer.hpp>

#include <Utils/Draw.hpp>

#include "ESP.hpp"

bool GlobalESP::GetBoxInfo(const TF2::IBaseEntity pEnt, ESPInfo::BoxInfo& boxinfo)
{
	using namespace TF2;
	const Vector3D_F& origin = pEnt->VecOrigin;

	const bool is_zero = pEnt->Mins->is_zero();
	const Vector3D_F mins = origin + (is_zero ? Vector3D_F{ -12.5f, -12.5f, -12.5f } : pEnt->Mins);
	const Vector3D_F maxs = origin + (is_zero ? Vector3D_F{ 12.5f, 12.5f, 12.5f } : pEnt->Maxs);

	constexpr Vector3D_F c = Vector3D_F{};
	constexpr auto p = c + { 1.f, 2.f, 3.f };
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
		if (!TF2::Utils::GetVectorInHudSpace(corners[i], { ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y }, boxinfo.m_Corners[i]))
			return false;

	return true;
}

void GlobalESP::DrawSharedInfo(ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, const ESPInfo::DrawTextCallback& callback)
{
	using namespace TF2;
	using PosType = ESPInfo::BoxInfo::PosType;
	ImDrawList* pDraw = ImGui::GetBackgroundDrawList();

	switch (esp_info->DrawMode.get())
	{
	case ESPInfo::ESPMode::type::Box:
	{
		ImU32 line_color = ImGui::GetColorU32(*std::bit_cast<ImVec4*>(esp_info->Box.DrawColor.data()));
		for (size_t i = 0; i < 4; i++)
		{
			const auto cur_bot = boxinfo.GetPosition<PosType::Bottom>(i).to<ImVec2>();
			const auto cur_up = boxinfo.GetPosition<PosType::Upper>(i).to<ImVec2>();

			pDraw->AddLine(cur_bot, cur_up, line_color, esp_info->Box.LineThickness);

			const auto next_bot = boxinfo.GetPosition<PosType::Bottom>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_bot, next_bot, line_color, esp_info->Box.LineThickness);

			const auto next_up = boxinfo.GetPosition<PosType::Upper>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_up, next_up, line_color, esp_info->Box.LineThickness);
		}
		break;
	}
	case ESPInfo::ESPMode::type::BoxOutline:
	{
		ImU32 line_color = ImGui::GetColorU32(*std::bit_cast<ImVec4*>(esp_info->OutlineBox.DrawColor.data()));
		ImU32 outline_color = ImGui::GetColorU32(*std::bit_cast<ImVec4*>(esp_info->OutlineBox.OutlineColor.data()));

		for (size_t i = 0; i < 4; i++)
		{
			const auto cur_bot = boxinfo.GetPosition<PosType::Bottom>(i).to<ImVec2>();
			const auto cur_up = boxinfo.GetPosition<PosType::Upper>(i).to<ImVec2>();

			pDraw->AddLine(cur_bot, cur_up, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_bot, cur_up, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);

			const auto next_bot = boxinfo.GetPosition<PosType::Bottom>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_bot, next_bot, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_bot, next_bot, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);

			const auto next_up = boxinfo.GetPosition<PosType::Upper>((i + 1) % 4).to<ImVec2>();
			pDraw->AddLine(cur_up, next_up, line_color, esp_info->OutlineBox.LineThickness);
			pDraw->AddLine(cur_up, next_up, outline_color, esp_info->OutlineBox.LineThickness - ESPInfo::OutlineThicknessMin);
		}
	}
	}

	if (callback)
	{
		const float text_height = (this->m_DisplayFont ? this->m_DisplayFont->FontSize : ImGui::GetTextLineHeight()) + esp_info->Text.Height;

		const Vector2D_F delta_updown = boxinfo.GetPosition<PosType::Upper>(ESPInfo::BoxInfo::BOTTOM_L) - boxinfo.GetPosition<PosType::Bottom>(ESPInfo::BoxInfo::BOTTOM_L);
		const float text_size = std::min(esp_info->Text.Size.get(), esp_info->Text.Size * std::abs(delta_updown[1]) / 5.f + 1.f);

		//boxinfo.GetPosition<PosType::Upper>(ESPInfo::BoxInfo::BOTTOM_L).to<ImVec2>()
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


void GlobalESP::DrawSharedHealth(ESPInfo::BoxInfo& boxinfo, ESPInfo::Shared* esp_info, int cur_health, int max_health, bool is_player)
{
	using namespace TF2;

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


TF2::Color4_F GlobalESP::GetHealthColor(int cur, int max)
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


void GlobalESP::RenderExtraESP()
{
	using namespace TF2;
	ESPInfo::BoxInfo box_info;
	ILocalPlayer pMe;

	for (int i = Interfaces::GlobalVars->MaxClients + 1; i < IBaseEntityInternal::GetHighestEntityIndex(); i++)
	{
		IBaseEntity pEnt(i);
		if (!pEnt || !Utils::IsVectorInHudSpace(pEnt->VecOrigin))
			continue;

		Const::EntClassID cls_id = pEnt->GetClientClass()->ClassID;

		switch (cls_id)
		{
		case Const::EntClassID::CObjectSentrygun:
		case Const::EntClassID::CObjectDispenser:
		case Const::EntClassID::CObjectTeleporter:
		{
			RenderBuildingESP(pEnt, cls_id, box_info);
			continue;
		}
		}

		if (!m_ObjectESPInfo.Enable)
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
				//	data.AddEntityString(ObjectESPInfo.DrawStickies && ObjectESPInfo.DrawName, L"Stickie bomb"sv);
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
				//	data.AddEntityString(ObjectESPInfo.DrawName, L"Pipe Bomb"sv);
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

		DrawSharedInfo(
			box_info,
			&m_ObjectESPInfo,
			[item_name = name](ESPInfo::Shared* esp_info, ESPInfo::TextInfo& renderer)
			{
				renderer.AddText(item_name);
			}
		);
	}
}
