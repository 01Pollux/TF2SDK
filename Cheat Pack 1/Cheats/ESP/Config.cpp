
#include <Interfaces/ImGui.hpp>
#include "ESP.hpp"

using ESPInst = ESPInfo::Shared&;
static void DrawGeneric_ImGuiSelections(ESPInst esp, SG::ConfigState& state, SG::Config<ESPInfo::HealthBarMode>* health_bar)
{
	state.add(ImGui::Checkbox("Enable", esp.Enable.data()));
	ImGui::SameLineHelp(esp.Enable);
	ImGui::Separator();

	state.add(ImGui::DragInt("Max distance", esp.MaxDistance.data()));
	ImGui::SameLineHelp(esp.MaxDistance);
	ImGui::Separator();

	state.add(ImGui::SliderFloat("Rainbow Speed", esp.Rainbow.Speed.data(), 0.f, 5.f, "%.2f", ImGuiSliderFlags_AlwaysClamp));
	ImGui::Separator();

	ImGui::Text("Modes:");
	using draw_mode = ESPInfo::ESPMode::type;
	float third_of_spacing = ImGui::GetContentRegionAvail().y / 3.f;
	for (auto [name, value] : {
		std::pair{ "None",			draw_mode::None },
		std::pair{ "Box",			draw_mode::Box },
		std::pair{ "Outlined Box",	draw_mode::BoxOutline }
	})
	{
		if (ImGui::RadioButton(name, esp.DrawMode.get() == value))
		{
			esp.DrawMode = value;
			state.set();
		}

		if (value != draw_mode::BoxOutline)
			ImGui::SameLine(0.f, third_of_spacing);
	}

	ImGui::Separator();

	state.add(ImGui::ColorEdit<false>("Text color", esp.Text.Color->get()));
	ImGui::SameLineHelp(esp.Text.Color);
	ImGui::Separator();

	state.add(ImGui::SliderFloat("Text size", esp.Text.Size.data(), 0.f, 50.f, "%.3f", ImGuiSliderFlags_AlwaysClamp));
	ImGui::SameLineHelp(esp.Text.Size);

	state.add(ImGui::SliderFloat("Text height", esp.Text.Height.data(), 0.f, 50.f, "%.3f", ImGuiSliderFlags_AlwaysClamp));
	ImGui::SameLineHelp(esp.Text.Size);

	state.add(ImGui::SliderFloat2("Text offset", esp.Text.Offset.data(), -100.f, 100.f, "%.3f", ImGuiSliderFlags_AlwaysClamp));
	ImGui::SameLineHelp(esp.Text.Size);

	ImGui::Separator();

	switch (esp.DrawMode.get())
	{
	case draw_mode::Box:
	{
		state.add(ImGui::ColorEdit<false>("Line color", esp.Box.DrawColor->get()));
		ImGui::SameLineHelp(esp.Box.DrawColor);

		state.add(ImGui::SliderFloat("Line thickness", esp.Box.LineThickness.data(), 0.f, 15.f, "%.3f", ImGuiSliderFlags_AlwaysClamp));
		ImGui::SameLineHelp(esp.Box.LineThickness);
		break;
	}
	case draw_mode::BoxOutline:
	{
		state.add(ImGui::ColorEdit<false>("Line color", esp.OutlineBox.DrawColor->get()));
		ImGui::SameLineHelp(esp.OutlineBox.DrawColor);

		state.add(ImGui::ColorEdit<false>("Outline color", esp.OutlineBox.OutlineColor->get()));
		ImGui::SameLineHelp(esp.OutlineBox.OutlineColor);

		state.add(ImGui::SliderFloat("Line thickness", esp.OutlineBox.LineThickness.data(), ESPInfo::OutlineThicknessMin + 1.f, 15.f + ESPInfo::OutlineThicknessMin, "%.3f", ImGuiSliderFlags_AlwaysClamp));
		ImGui::SameLineHelp(esp.OutlineBox.LineThickness);
		break;
	}
	}

	if (health_bar)
	{
		constexpr std::array names = ESPInfo::HealthBarMode::json_get_keyvalues();
		size_t current_type = health_bar->get();

		if (ImGui::BeginCombo("Health bar", names[current_type].first))
		{
			for (size_t i = 0; i < static_cast<size_t>(ESPInfo::HealthBarMode::type::Count); i++)
			{
				const bool is_selected = i == current_type;
				if (ImGui::Selectable(names[i].first, is_selected))
				{
					state.set();
					*health_bar = static_cast<ESPInfo::HealthBarMode::type>(i);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
}

bool GlobalESP::OnRender()
{
	SG::ConfigState state;
	constexpr float third_of_spacing = 160.f;

	if (ImGui::CollapsingHeader("Player Manager"))
	{
		ImGui::PushID("PM");
		for (int i = 0; i < std::ssize(m_PlayerESPInfo); i++)
		{
			ImGui::PushID(i);

			if (ImGui::TreeNode(ESPInfo::TeamNames[i]))
			{
				auto& cur_esp = m_PlayerESPInfo[i];
				DrawGeneric_ImGuiSelections(cur_esp, state, &cur_esp.HealthbarMode);

				//what to display
				{
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Text("Select what to display: ");
					ImGui::Dummy(ImVec2(0, 10));

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Draw Name", cur_esp.DrawName.data());				ImGui::SameLineHelp(cur_esp.DrawName);
						ImGui::Checkbox("Draw Distance", cur_esp.DrawDistance.data());		ImGui::SameLineHelp(cur_esp.DrawDistance);
						ImGui::Checkbox("Draw Class", cur_esp.DrawClass.data());			ImGui::SameLineHelp(cur_esp.DrawClass);
					}
					ImGui::EndGroup();

					ImGui::SameLine(0.f, third_of_spacing);
					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Draw Conditions", cur_esp.DrawCond.data());		ImGui::SameLineHelp(cur_esp.DrawCond);
						ImGui::Checkbox("Draw UberCharge", cur_esp.DrawUberPerc.data());	ImGui::SameLineHelp(cur_esp.DrawUberPerc);
						ImGui::Checkbox("Ignore Cloaked", cur_esp.IgnoreCloak.data());		ImGui::SameLineHelp(cur_esp.IgnoreCloak);
					}

					ImGui::EndGroup();

					ImGui::SameLine(0.f, third_of_spacing);
					{
						ImGui::Checkbox("Draw Team", cur_esp.DrawTeam.data());				ImGui::SameLineHelp(cur_esp.DrawTeam);
					}

					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Separator();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
		ImGui::PopID();
	}
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Building Manager"))
	{
		ImGui::PushID("BM");
		for (int i = 0; i < std::ssize(m_BuildingESPInfo); i++)
		{
			if (ImGui::TreeNode(ESPInfo::TeamNames[i]))
			{
				auto& cur_esp = m_BuildingESPInfo[i];
				DrawGeneric_ImGuiSelections(cur_esp, state, &cur_esp.HealthbarMode);

				//what to display
				{
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Text("Select what to display: ");
					ImGui::Dummy(ImVec2(0, 10));


					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Draw Name", cur_esp.DrawName.data());				ImGui::SameLineHelp(cur_esp.DrawName);
						ImGui::Checkbox("Draw Distance", cur_esp.DrawDistance.data());		ImGui::SameLineHelp(cur_esp.DrawDistance);
						ImGui::Checkbox("Draw Owner", cur_esp.DrawOwner.data());			ImGui::SameLineHelp(cur_esp.DrawOwner);
					}
					ImGui::EndGroup();

					ImGui::SameLine(0.f, third_of_spacing);
					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Draw Ammo", cur_esp.DrawAmmo.data());				ImGui::SameLineHelp(cur_esp.DrawAmmo);
						ImGui::Checkbox("Draw Level", cur_esp.DrawLevel.data());			ImGui::SameLineHelp(cur_esp.DrawLevel);
						ImGui::Checkbox("Draw State", cur_esp.DrawBState.data());			ImGui::SameLineHelp(cur_esp.DrawBState);
					}
					ImGui::EndGroup();

					ImGui::SameLine(0.f, third_of_spacing);
					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Draw Health", cur_esp.DrawHealth.data());			ImGui::SameLineHelp(cur_esp.DrawHealth);
						ImGui::Checkbox("Draw Team", cur_esp.DrawTeam.data());				ImGui::SameLineHelp(cur_esp.DrawTeam);
					}
					ImGui::EndGroup();


					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Separator();
				}

				ImGui::TreePop();
			}
		}
		ImGui::PopID();
	}
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Extra Manager"))
	{
		ImGui::PushID("OM");

		auto& cur_esp = m_ObjectESPInfo;
		DrawGeneric_ImGuiSelections(cur_esp, state, nullptr);

		//what to display
		{
			ImGui::Dummy(ImVec2(0, 10));
			ImGui::Text("Select what to display: ");
			ImGui::Dummy(ImVec2(0, 10));

			ImGui::BeginGroup();
			{
				ImGui::Checkbox("Draw Name", cur_esp.DrawName.data());					ImGui::SameLineHelp(cur_esp.DrawName);
				ImGui::Checkbox("Draw Distance", cur_esp.DrawDistance.data());			ImGui::SameLineHelp(cur_esp.DrawDistance);
				ImGui::Checkbox("Draw Packs", cur_esp.DrawPacks.data());				ImGui::SameLineHelp(cur_esp.DrawPacks);
			}
			ImGui::EndGroup();

			ImGui::SameLine(0.f, third_of_spacing);
			ImGui::BeginGroup();
			{
				ImGui::Checkbox("Draw Rockets", cur_esp.DrawRockets.data());			ImGui::SameLineHelp(cur_esp.DrawRockets);
				ImGui::Checkbox("Draw Pipes", cur_esp.DrawPipes.data());				ImGui::SameLineHelp(cur_esp.DrawPipes);
				ImGui::Checkbox("Draw Stickies", cur_esp.DrawStickies.data());			ImGui::SameLineHelp(cur_esp.DrawStickies);
			}
			ImGui::EndGroup();

			ImGui::SameLine(0.f, third_of_spacing);
			{
				ImGui::Checkbox("Draw NPCs", cur_esp.DrawNPC.data());					ImGui::SameLineHelp(cur_esp.DrawNPC);
			}

			ImGui::Dummy(ImVec2(0, 10));
			ImGui::Separator();
		}

		ImGui::PopID();
	}

	return state;
}

void GlobalESP::OnSaveConfig(Json& cfg)
{
	Json& esp_info = cfg["ESP"];
	auto SaveGeneric = [](const ESPInfo::Shared& cur_esp, Json& out_js)
	{
		cur_esp.Enable.to_json(out_js);
		cur_esp.Rainbow.Speed.to_json(out_js["rainbow"]);
		{
			Json& box = out_js["box"];
			cur_esp.Box.DrawColor.to_json(box);
			cur_esp.Box.LineThickness.to_json(box);
		}
		{
			Json& outline_box = out_js["outline box"];
			cur_esp.OutlineBox.DrawColor.to_json(outline_box);
			cur_esp.OutlineBox.OutlineColor.to_json(outline_box);
			cur_esp.OutlineBox.LineThickness.to_json(outline_box);
		}
		{
			cur_esp.Text.Color.to_json(out_js);
			cur_esp.Text.Offset.to_json(out_js);
			cur_esp.Text.Height.to_json(out_js);
			cur_esp.Text.Size.to_json(out_js);
		}

		cur_esp.MaxDistance.to_json(out_js);
		cur_esp.DrawMode.to_json(out_js);
		cur_esp.DrawName.to_json(out_js);
		cur_esp.DrawDistance.to_json(out_js);
	};

	{
		Json& player_esp = esp_info["Players"];
		for (int i = 0; i < std::ssize(m_PlayerESPInfo); i++)
		{
			Json& cur_team = player_esp[ESPInfo::TeamNames[i]];
			const auto& cur_esp = m_PlayerESPInfo[i];
			
			SaveGeneric(cur_esp, cur_team);

			cur_esp.DrawClass.to_json(cur_team);
			cur_esp.DrawCond.to_json(cur_team);
			cur_esp.DrawUberPerc.to_json(cur_team);
			cur_esp.IgnoreCloak.to_json(cur_team);
			cur_esp.DrawTeam.to_json(cur_team);
			cur_esp.HealthbarMode.to_json(cur_team);
		}
	}
	
	{
		Json& building_esp = esp_info["Buildings"];
		for (int i = 0; i < std::ssize(m_BuildingESPInfo); i++)
		{
			Json& cur_team = building_esp[ESPInfo::TeamNames[i]];
			const auto& cur_esp = m_BuildingESPInfo[i];
			
			SaveGeneric(cur_esp, cur_team);

			cur_esp.DrawOwner.to_json(cur_team);
			cur_esp.DrawAmmo.to_json(cur_team);
			cur_esp.DrawLevel.to_json(cur_team);
			cur_esp.DrawBState.to_json(cur_team);
			cur_esp.DrawHealth.to_json(cur_team);
			cur_esp.DrawTeam.to_json(cur_team);
			cur_esp.HealthbarMode.to_json(cur_team);
		}
	}

	{
		Json& extra_esp = esp_info["Extras"];
		const auto& cur_esp = m_ObjectESPInfo;

		SaveGeneric(cur_esp, extra_esp);

		cur_esp.DrawPacks.to_json(extra_esp);
		cur_esp.DrawRockets.to_json(extra_esp);
		cur_esp.DrawPipes.to_json(extra_esp);
		cur_esp.DrawStickies.to_json(extra_esp);
		cur_esp.DrawNPC.to_json(extra_esp);
	}
}

void GlobalESP::OnReloadConfig(const Json& cfg)
{
	auto LoadGeneric = [](ESPInfo::Shared& cur_esp, const Json& in_js)
	{
		cur_esp.Enable.from_json(in_js);
		if (auto rainbow = in_js.find("rainbow"); rainbow != in_js.end() && rainbow->is_number_float())
			cur_esp.Rainbow.Speed.from_json(*rainbow);
		if (auto box = in_js.find("box"); box != in_js.end() && box->is_object())
		{
			cur_esp.Box.DrawColor.from_json(*box);
			cur_esp.Box.LineThickness.from_json(*box);
		}

		if (auto outline_box = in_js.find("outline box"); outline_box != in_js.end() && outline_box->is_object())
		{
			cur_esp.OutlineBox.DrawColor.from_json(*outline_box);
			cur_esp.OutlineBox.OutlineColor.from_json(*outline_box);
			cur_esp.OutlineBox.LineThickness.from_json(*outline_box);
		}
		{

			cur_esp.Text.Color.from_json(in_js);
			cur_esp.Text.Offset.from_json(in_js);
			cur_esp.Text.Height.from_json(in_js);
			cur_esp.Text.Size.from_json(in_js);
		}
		cur_esp.MaxDistance.from_json(in_js);
		cur_esp.DrawMode.from_json(in_js);
		cur_esp.DrawName.from_json(in_js);
		cur_esp.DrawDistance.from_json(in_js);
	};

	auto iter = cfg.find("ESP");
	if (iter != cfg.end())
	{
		{
			auto player_esp = iter->find("Players");
			if (player_esp != iter->end())
			{
				for (int i = 0; i < std::ssize(m_PlayerESPInfo); i++)
				{
					auto cur_team_iter = player_esp->find(ESPInfo::TeamNames[i]);
					if (cur_team_iter == player_esp->end())
						continue;

					const auto& cur_team = *cur_team_iter;
					auto& cur_esp = m_PlayerESPInfo[i];

					LoadGeneric(cur_esp, cur_team);

					cur_esp.DrawClass.from_json(cur_team);
					cur_esp.DrawCond.from_json(cur_team);
					cur_esp.DrawUberPerc.from_json(cur_team);
					cur_esp.IgnoreCloak.from_json(cur_team);
					cur_esp.DrawTeam.from_json(cur_team);
					cur_esp.HealthbarMode.from_json(cur_team);
				}
			}
			
		}

		{
			auto building_esp = iter->find("Players");
			if (building_esp != iter->end())
			{
				for (int i = 0; i < std::ssize(m_BuildingESPInfo); i++)
				{
					const auto cur_team_iter = building_esp->find(ESPInfo::TeamNames[i]);
					if (cur_team_iter == building_esp->end())
						continue;

					const auto& cur_team = *cur_team_iter;
					auto& cur_esp = m_BuildingESPInfo[i];

					LoadGeneric(cur_esp, cur_team);

					cur_esp.DrawOwner.from_json(cur_team);
					cur_esp.DrawAmmo.from_json(cur_team);
					cur_esp.DrawLevel.from_json(cur_team);
					cur_esp.DrawBState.from_json(cur_team);
					cur_esp.DrawHealth.from_json(cur_team);
					cur_esp.DrawTeam.from_json(cur_team);
					cur_esp.HealthbarMode.from_json(cur_team);
				}
			}
		}

		{
			auto extra_esp_iter = iter->find("Extras");

			if (extra_esp_iter != iter->end())
			{
				const auto& extra_esp = *extra_esp_iter;
				auto& cur_esp = m_ObjectESPInfo;

				LoadGeneric(cur_esp, extra_esp);

				cur_esp.DrawPacks.from_json(extra_esp);
				cur_esp.DrawRockets.from_json(extra_esp);
				cur_esp.DrawPipes.from_json(extra_esp);
				cur_esp.DrawStickies.from_json(extra_esp);
				cur_esp.DrawNPC.from_json(extra_esp);
			}
		}
	}
}
