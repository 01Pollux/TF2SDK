#pragma once

#include "Math/Vector.hpp"
#include "Consts.hpp"

#include "Engine/AppSystem.hpp"

#include "Utils/UtlVector.hpp"


TF2_NAMESPACE_BEGIN();

namespace VGui
{
	class IEngineVGui;
}


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(VGui::IEngineVGui, EngineVGUI, "VEngineVGui001");
}

namespace VGui
{
	class IEngineVGui
	{
	public:
		virtual					~IEngineVGui() = default;

		virtual Const::VGui::VPANEL	GetPanel(Const::VGui::PanelType type) abstract;

		virtual bool			IsGameUIVisible() abstract;
	};
}

TF2_NAMESPACE_END();
