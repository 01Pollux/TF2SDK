#pragma once

#include <array>
#include <type_traits>
#include <imgui/imcxx/all_in_one.hpp>

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
	inline void Help(const px::con_command& var)
	{
		if (!var.help().empty())
			DrawHelp(var.help().data(), var.help().data() + var.help().size());
	}

	inline void SameLineHelp(const char* text)
	{
		ImGui::SameLine();
		DrawHelp(text);
	}

	inline void SameLineHelp(const px::con_command& var)
	{
		if (!var.help().empty())
		{
			ImGui::SameLine();
			DrawHelp(var.help().data(), var.help().data() + var.help().size());
		}
	}
}