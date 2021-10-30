#pragma once

#include "Math/Vector.hpp"
#include "Consts.hpp"

#include "Engine/AppSystem.hpp"

#include "Utils/UtlVector.hpp"


TF2_NAMESPACE_BEGIN();

class KeyValues;

namespace VGui
{
	class IPanel;
}


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(VGui::IPanel, Panel, "VGUI_Panel009");
}

namespace VGui
{
	class Panel;
	class SurfacePlat;
	class IHTMLEvents;
	class IHTML;
	class IVguiMatInfo;
	class IImage;


	class IPanel : public IBaseInterface
	{
	public:
		virtual void Init(Const::VGui::VPANEL vguiPanel, Panel* panel) abstract;

		// methods
		virtual void SetPos(Const::VGui::VPANEL vguiPanel, int x, int y) abstract;
		virtual void GetPos(Const::VGui::VPANEL vguiPanel, int& x, int& y) abstract;
		virtual void SetSize(Const::VGui::VPANEL vguiPanel, int wide, int tall) abstract;
		virtual void GetSize(Const::VGui::VPANEL vguiPanel, int& wide, int& tall) abstract;
		virtual void SetMinimumSize(Const::VGui::VPANEL vguiPanel, int wide, int tall) abstract;
		virtual void GetMinimumSize(Const::VGui::VPANEL vguiPanel, int& wide, int& tall) abstract;
		virtual void SetZPos(Const::VGui::VPANEL vguiPanel, int z) abstract;
		virtual int  GetZPos(Const::VGui::VPANEL vguiPanel) abstract;

		virtual void GetAbsPos(Const::VGui::VPANEL vguiPanel, int& x, int& y) abstract;
		virtual void GetClipRect(Const::VGui::VPANEL vguiPanel, int& x0, int& y0, int& x1, int& y1) abstract;
		virtual void SetInset(Const::VGui::VPANEL vguiPanel, int left, int top, int right, int bottom) abstract;
		virtual void GetInset(Const::VGui::VPANEL vguiPanel, int& left, int& top, int& right, int& bottom) abstract;

		virtual void SetVisible(Const::VGui::VPANEL vguiPanel, bool state) abstract;
		virtual bool IsVisible(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void SetParent(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL newParent) abstract;
		virtual int GetChildCount(Const::VGui::VPANEL vguiPanel) abstract;
		virtual Const::VGui::VPANEL GetChild(Const::VGui::VPANEL vguiPanel, int index) abstract;
		virtual Utils::UtlVector<Const::VGui::VPANEL>& GetChildren(Const::VGui::VPANEL vguiPanel) abstract;
		virtual Const::VGui::VPANEL GetParent(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void MoveToFront(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void MoveToBack(Const::VGui::VPANEL vguiPanel) abstract;
		virtual bool HasParent(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL potentialParent) abstract;
		virtual bool IsPopup(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void SetPopup(Const::VGui::VPANEL vguiPanel, bool state) abstract;
		virtual bool IsFullyVisible(Const::VGui::VPANEL vguiPanel) abstract;

		// gets the scheme this panel uses
		virtual Const::VGui::HScheme GetScheme(Const::VGui::VPANEL vguiPanel) abstract;
		// gets whether or not this panel should scale with screen resolution
		virtual bool IsProportional(Const::VGui::VPANEL vguiPanel) abstract;
		// returns true if auto-deletion flag is set
		virtual bool IsAutoDeleteSet(Const::VGui::VPANEL vguiPanel) abstract;
		// deletes the Panel * associated with the vpanel
		virtual void DeletePanel(Const::VGui::VPANEL vguiPanel) abstract;

		// input interest
		virtual void SetKeyBoardInputEnabled(Const::VGui::VPANEL vguiPanel, bool state) abstract;
		virtual void SetMouseInputEnabled(Const::VGui::VPANEL vguiPanel, bool state) abstract;
		virtual bool IsKeyBoardInputEnabled(Const::VGui::VPANEL vguiPanel) abstract;
		virtual bool IsMouseInputEnabled(Const::VGui::VPANEL vguiPanel) abstract;

		// calculates the panels current position within the hierarchy
		virtual void Solve(Const::VGui::VPANEL vguiPanel) abstract;

		// gets names of the object (for debugging purposes)
		virtual const char* GetName(Const::VGui::VPANEL vguiPanel) abstract;
		virtual const char* GetPaintClassName(Const::VGui::VPANEL vguiPanel) abstract;

		// delivers a message to the panel
		virtual void SendPaintMessage(Const::VGui::VPANEL vguiPanel, KeyValues* params, Const::VGui::VPANEL ifromPanel) abstract;

		// these pass through to the IClientPanel
		virtual void Think(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void PerformApplySchemeSettings(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void PaintTraverse(Const::VGui::VPANEL vguiPanel, bool forceRepaint, bool allowForce = true) abstract;
		virtual void Repaint(Const::VGui::VPANEL vguiPanel) abstract;
		virtual Const::VGui::VPANEL IsWithinTraverse(Const::VGui::VPANEL vguiPanel, int x, int y, bool traversePopups) abstract;
		virtual void OnChildAdded(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL child) abstract;
		virtual void OnSizeChanged(Const::VGui::VPANEL vguiPanel, int newWide, int newTall) abstract;

		virtual void InternalFocusChanged(Const::VGui::VPANEL vguiPanel, bool lost) abstract;
		virtual bool RequestInfo(Const::VGui::VPANEL vguiPanel, KeyValues* outputData) abstract;
		virtual void RequestFocus(Const::VGui::VPANEL vguiPanel, int direction = 0) abstract;
		virtual bool RequestFocusPrev(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL existingPanel) abstract;
		virtual bool RequestFocusNext(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL existingPanel) abstract;
		virtual Const::VGui::VPANEL GetCurrentKeyFocus(Const::VGui::VPANEL vguiPanel) abstract;
		virtual int GetTabPosition(Const::VGui::VPANEL vguiPanel) abstract;

		// used by ISurface to store platform-specific data
		virtual SurfacePlat* Plat(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void SetPlat(Const::VGui::VPANEL vguiPanel, SurfacePlat* Plat) abstract;

		// returns a pointer to the vgui controls baseclass Panel *
		// destinationModule needs to be passed in to verify that the returned Panel * is from the same module
		// it must be from the same module since Panel * vtbl may be different in each module
		virtual Panel* GetPanel(Const::VGui::VPANEL vguiPanel, const char* destinationModule) abstract;

		virtual bool IsEnabled(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void SetEnabled(Const::VGui::VPANEL vguiPanel, bool state) abstract;

		// Used by the drag/drop manager to always draw on top
		virtual bool IsTopmostPopup(Const::VGui::VPANEL vguiPanel) abstract;
		virtual void SetTopmostPopup(Const::VGui::VPANEL vguiPanel, bool state) abstract;

		// sibling pins
		virtual void SetSiblingPin(Const::VGui::VPANEL vguiPanel, Const::VGui::VPANEL newSibling, std::byte iMyCornerToPin = std::byte(0), std::byte iSiblingCornerToPinTo = std::byte(0)) abstract;
	};
}

TF2_NAMESPACE_END();
