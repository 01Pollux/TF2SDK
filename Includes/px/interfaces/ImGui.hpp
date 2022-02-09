#pragma once

#include <array>
#include <type_traits>
#include <imgui/imgui.h>

#include <px/console.hpp>
#include <px/interfaces/InterfacesSys.hpp>

PX_NAMESPACE_BEGIN();

class ConfigState
{
public:
	[[nodiscard]] bool has_changed() const noexcept { return m_Changed; }
	[[nodiscard]] operator bool() const noexcept { return has_changed(); }

	void set() noexcept { m_Changed = true; }
	void add(bool state) noexcept { m_Changed |= state; }
	void reset() noexcept { m_Changed = false; }

	template<typename _FnTy, typename... _Args>
		requires requires {
		requires std::is_invocable_v<_FnTy>;
		requires std::disjunction_v<
			std::is_same<bool, std::invoke_result_t<_FnTy>>,
			std::is_same<void, std::invoke_result_t<_FnTy>>
		>;
	}
	void invoke(const _FnTy& fn, _Args&&... args)
	{
		using result_type = std::invoke_result_t<_FnTy>;
		if constexpr (std::is_same_v<bool, result_type>)
			add(std::invoke(fn, std::forward<_Args>(args)...));
		else
			std::invoke(fn, std::forward<_Args>(args)...);
	}

private:
	bool m_Changed{ };
};

using ImGuiPluginCallback = std::function<bool()>;
using ImGuiPlCallbackId = uint32_t;

static constexpr const char* Interface_ImGuiLoader = "IImGuiLoader";

class IImGuiLoader : public IInterface
{
public:
	/// <summary>
	/// Initialize ImGuiContext in Dll's codespace.
	/// Must be called in 
	/// </summary>
	[[nodiscard]] virtual ImGuiContext* GetContext() noexcept abstract;

	/// <summary>
	/// Register callback for ImGui drawing.
	/// </summary>
	/// <param name="is_propmgr">true for handling callback in 'Property Manager', else it will be called after main drawing as independent menu</param>
	virtual ImGuiPlCallbackId AddCallback(IPlugin* plugin, const char* name, const ImGuiPluginCallback& callback) abstract;

	/// <summary>
	/// Unregister callback by callback id.
	/// </summary>
	virtual void RemoveCallback(const ImGuiPlCallbackId id) abstract;

	/// <summary>
	/// Unregister callback by plugin.
	/// </summary>
	virtual void RemoveCallback(const px::IPlugin* plugin) noexcept  abstract;

	/// <summary>
	/// Tests whether or not the main menu is on
	/// </summary>
	[[nodiscard]] virtual bool IsMenuOn() abstract;

	/// <summary>
	/// Find a font from 'Pleiades/Fonts/*.ttf' 
	/// </summary>
	/// <returns>a pointer to ImFont texture, nullptr if the font wasn't loaded</returns>
	[[nodiscard]] virtual ImFont* FindFont(const char* font_name) abstract;
};

PX_NAMESPACE_END();

// Helpers for px::Config, etc
namespace ImGui
{
	inline void DrawHelp(const char* text, const char* end_text = nullptr)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(text);
			ImGui::PopTextWrapPos();

			ImGui::EndTooltip();
		}
	}

	template<typename _Ty>
	inline void Help(const px::ConVar<_Ty>& var)
	{
		if (var.has_description())
			DrawHelp(var.description());
	}

	inline void SameLineHelp(const char* text)
	{
		ImGui::SameLine();
		DrawHelp(text);
	}

	inline void SameLineHelp(const px::ConCommand& var)
	{
		if (var.has_description())
		{
			ImGui::SameLine();
			DrawHelp(var.description().data(), var.description().data() + var.description().size());
		}
	}
	
	template<bool _x255 = true, typename _Ty, size_t _Size>
	inline bool ColorEdit(const char* name, std::array<_Ty, _Size>& color, ImGuiColorEditFlags flags = 0)
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		float actual_color[4]{
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			(_Size >= 4 ? (static_cast<float>(color[3]) / to_ratio) : 1.f)
		};

		if constexpr (_Size < 4)
			flags |= ImGuiColorEditFlags_NoAlpha;

		if (ImGui::ColorEdit4(name, actual_color, flags))
		{
			for (size_t i = 0; i < _Size; i++)
				color[i] = static_cast<_Ty>(actual_color[i] * to_ratio);
			return true;
		}

		return false;
	}

	template<bool _x255 = true, typename _Ty, size_t _Size>
	inline bool ColorPicker(const char* name, std::array<_Ty, _Size>& color, ImGuiColorEditFlags flags = 0)
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		float actual_color[4]{
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			(_Size >= 4 ? (static_cast<float>(color[3]) / to_ratio) : 1.f)
		};

		if constexpr (_Size < 4)
			flags |= ImGuiColorEditFlags_NoAlpha;

		if (ImGui::ColorPicker4(name, actual_color, flags))
		{
			for (size_t i = 0; i < _Size; i++)
				color[i] = static_cast<_Ty>(actual_color[i] * to_ratio);
			return true;
		}

		return false;
	}

	template<bool _x255 = true, typename _Ty>
	inline bool ColorButton(const char* name, std::array<_Ty, 4>& color, ImGuiColorEditFlags flags = 0, ImVec2 size = { })
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		ImVec4 actual_color(
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			static_cast<float>(color[3]) / to_ratio
		);

		if (ImGui::ColorButton(name, actual_color, flags, size))
		{
			float* c(&actual_color.x);
			for (size_t i = 0; i < 4; i++)
				color[i] = static_cast<_Ty>(c[i] * to_ratio);
			return true;
		}

		return false;
	}

	template<bool _x255 = true, typename _Ty, size_t _Size>
	inline bool ColorEdit(const char* name, _Ty (&color)[_Size], ImGuiColorEditFlags flags = 0)
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		float actual_color[4]{
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			(_Size >= 4 ? (static_cast<float>(color[3]) / to_ratio) : 1.f)
		};

		if constexpr (_Size < 4)
			flags |= ImGuiColorEditFlags_NoAlpha;

		if (ImGui::ColorEdit4(name, actual_color, flags))
		{
			for (size_t i = 0; i < _Size; i++)
				color[i] = static_cast<_Ty>(actual_color[i] * to_ratio);
			return true;
		}

		return false;
	}

	template<bool _x255 = true, typename _Ty, size_t _Size>
	inline bool ColorPicker(const char* name, _Ty (&color)[_Size], ImGuiColorEditFlags flags = 0)
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		float actual_color[4]{
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			(_Size >= 4 ? (static_cast<float>(color[3]) / to_ratio) : 1.f)
		};

		if constexpr (_Size >= 4)
			flags |= ImGuiColorEditFlags_NoAlpha;

		if (ImGui::ColorPicker4(name, actual_color, flags))
		{
			for (size_t i = 0; i < _Size; i++)
				color[i] = static_cast<_Ty>(actual_color[i] * to_ratio);
			return true;
		}

		return false;
	}

	template<bool _x255 = true, typename _Ty, size_t _Size>
	inline bool ColorButton(const char* name, _Ty (&color)[_Size], ImGuiColorEditFlags flags = 0, ImVec2 size = { })
	{
		constexpr float to_ratio = _x255 ? 255.f : 1.f;
		ImVec4 actual_color(
			static_cast<float>(color[0]) / to_ratio,
			static_cast<float>(color[1]) / to_ratio,
			static_cast<float>(color[2]) / to_ratio,
			(_Size >= 4 ? (static_cast<float>(color[3]) / to_ratio) : 1.f)
		);

		if (ImGui::ColorButton(name, actual_color, flags, size))
		{
			float* c(&actual_color.x);
			for (size_t i = 0; i < 4; i++)
				color[i] = static_cast<_Ty>(c[i] * to_ratio);
			return true;
		}

		return false;
	}
}
