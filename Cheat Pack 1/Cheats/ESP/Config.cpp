
#include <shadowgarden/interfaces/ImGui.hpp>
#include "ESP.hpp"

using ESPInst = ESPInfo::Shared&;
static void DrawGeneric_ImGuiSelections(ESPInst esp, SG::ConfigState& state, SG::ConVar<ESPInfo::HealthBarMode>* health_bar)
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
		constexpr std::array names = ESPInfo::HealthBarMode::type_names();
		size_t current_type = static_cast<size_t>(static_cast<ESPInfo::HealthBarMode::type>(health_bar->get()));

		if (ImGui::BeginCombo("Health bar", names[current_type].first.data()))
		{
			for (size_t i = 0; i < static_cast<size_t>(ESPInfo::HealthBarMode::type::Count); i++)
			{
				const bool is_selected = i == current_type;
				if (ImGui::Selectable(names[i].first.data(), is_selected))
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

void GlobalESP::OnSaveConfig(std::vector<SG::IPlugin::FileConfigs>& cfg) const
{
	SG::IPlugin::FileConfigs esp_cfg("esp_config");

	auto SaveGeneric = [&esp_cfg](const ESPInfo::Shared& cur_esp)
	{
		esp_cfg.insert(cur_esp.Enable);
		esp_cfg.insert(cur_esp.Rainbow.Speed);
		{
			esp_cfg.insert(cur_esp.Box.DrawColor);
			esp_cfg.insert(cur_esp.Box.LineThickness);
		}
		{
			esp_cfg.insert(cur_esp.OutlineBox.DrawColor);
			esp_cfg.insert(cur_esp.OutlineBox.OutlineColor);
			esp_cfg.insert(cur_esp.OutlineBox.LineThickness);
		}
		{
			esp_cfg.insert(cur_esp.Text.Color);
			esp_cfg.insert(cur_esp.Text.Offset);
			esp_cfg.insert(cur_esp.Text.Height);
			esp_cfg.insert(cur_esp.Text.Size);
		}

		esp_cfg.insert(cur_esp.MaxDistance);
		esp_cfg.insert(cur_esp.DrawMode);
		esp_cfg.insert(cur_esp.DrawName);
		esp_cfg.insert(cur_esp.DrawDistance);
	};

	{
		for (int i = 0; i < std::ssize(m_PlayerESPInfo); i++)
		{
			const auto& cur_esp = m_PlayerESPInfo[i];
			SaveGeneric(cur_esp);

			esp_cfg.insert(cur_esp.DrawClass);
			esp_cfg.insert(cur_esp.DrawCond);
			esp_cfg.insert(cur_esp.DrawUberPerc);
			esp_cfg.insert(cur_esp.IgnoreCloak);
			esp_cfg.insert(cur_esp.DrawTeam);
			esp_cfg.insert(cur_esp.HealthbarMode);
		}
	}
	
	{
		for (int i = 0; i < std::ssize(m_BuildingESPInfo); i++)
		{
			const auto& cur_esp = m_BuildingESPInfo[i];
			SaveGeneric(cur_esp);

			esp_cfg.insert(cur_esp.DrawOwner);
			esp_cfg.insert(cur_esp.DrawAmmo);
			esp_cfg.insert(cur_esp.DrawLevel);
			esp_cfg.insert(cur_esp.DrawBState);
			esp_cfg.insert(cur_esp.DrawHealth);
			esp_cfg.insert(cur_esp.DrawTeam);
			esp_cfg.insert(cur_esp.HealthbarMode);
		}
	}

	{
		const auto& cur_esp = m_ObjectESPInfo;

		SaveGeneric(cur_esp);

		esp_cfg.insert(cur_esp.DrawPacks);
		esp_cfg.insert(cur_esp.DrawRockets);
		esp_cfg.insert(cur_esp.DrawPipes);
		esp_cfg.insert(cur_esp.DrawStickies);
		esp_cfg.insert(cur_esp.DrawNPC);
	}

	cfg.emplace_back(std::move(esp_cfg));
}