#pragma once

#include "Math/Vector.hpp"
#include "Consts.hpp"

#include "Engine/AppSystem.hpp"

#include "Utils/UtlVector.hpp"


TF2_NAMESPACE_BEGIN();

class KeyValues;

namespace VGui
{
	class IInput;
}


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(VGui::IInput, InputSys, "VGUI_Input005");
}

namespace VGui
{
	class IInput : public IBaseInterface
	{
	public:
		virtual void SetMouseFocus(Const::VGui::VPANEL newMouseFocus) abstract;
		virtual void SetMouseCapture(Const::VGui::VPANEL panel) abstract;

		// returns the string name of a scan code
		virtual void GetKeyCodeText(Const::VGui::KeyCode code, char* buf, int buflen) abstract;

		// focus
		virtual Const::VGui::VPANEL GetFocus() abstract;
		virtual Const::VGui::VPANEL GetCalculatedFocus() abstract;// to handle cases where the focus changes inside a frame.
		virtual Const::VGui::VPANEL GetMouseOver() abstract;		// returns the panel the mouse is currently over, ignoring mouse capture

		// mouse state
		virtual void SetCursorPos(int x, int y) abstract;
		virtual void GetCursorPos(int& x, int& y) abstract;
		virtual bool WasMousePressed(Const::VGui::KeyCode code) abstract;
		virtual bool WasMouseDoublePressed(Const::VGui::KeyCode code) abstract;
		virtual bool IsMouseDown(Const::VGui::KeyCode code) abstract;

		// cursor override
		virtual void SetCursorOveride(Const::VGui::HCursor cursor) abstract;
		virtual Const::VGui::HCursor GetCursorOveride() abstract;

		// key state
		virtual bool WasMouseReleased(Const::VGui::KeyCode code) abstract;
		virtual bool WasKeyPressed(Const::VGui::KeyCode code) abstract;
		virtual bool IsKeyDown(Const::VGui::KeyCode code) abstract;
		virtual bool WasKeyTyped(Const::VGui::KeyCode code) abstract;
		virtual bool WasKeyReleased(Const::VGui::KeyCode code) abstract;

		virtual Const::VGui::VPANEL GetAppModalSurface() abstract;
		// set the modal dialog panel.
		// all events will go only to this panel and its children.
		virtual void SetAppModalSurface(Const::VGui::VPANEL panel) abstract;
		// release the modal dialog panel
		// do this when your modal dialog finishes.
		virtual void ReleaseAppModalSurface() abstract;

		virtual void GetCursorPosition(int& x, int& y) abstract;

		virtual void SetIMEWindow(void* hwnd) abstract;
		virtual void* GetIMEWindow() abstract;

		virtual void OnChangeIME(bool forward) abstract;
		virtual int  GetCurrentIMEHandle() abstract;
		virtual int  GetEnglishIMEHandle() abstract;

		// Returns the Language Bar label (Chinese, Korean, Japanese, Russion, Thai, etc.)
		virtual void GetIMELanguageName(wchar_t* buf, int unicodeBufferSizeInBytes) abstract;
		// Returns the short code for the language (EN, CH, KO, JP, RU, TH, etc. ).
		virtual void GetIMELanguageShortCode(wchar_t* buf, int unicodeBufferSizeInBytes) abstract;

		struct LanguageItem
		{
			wchar_t		shortname[4];
			wchar_t		menuname[128];
			int			handleValue;
			bool		active; // true if this is the active language
		};

		struct ConversionModeItem
		{
			wchar_t		menuname[128];
			int			handleValue;
			bool		active; // true if this is the active conversion mode
		};

		struct SentenceModeItem
		{
			wchar_t		menuname[128];
			int			handleValue;
			bool		active; // true if this is the active sentence mode
		};

		// Call with NULL dest to get item count
		virtual int	 GetIMELanguageList(LanguageItem* dest, int destcount) abstract;
		virtual int	 GetIMEConversionModes(ConversionModeItem* dest, int destcount) abstract;
		virtual int	 GetIMESentenceModes(SentenceModeItem* dest, int destcount) abstract;

		virtual void OnChangeIMEByHandle(int handleValue) abstract;
		virtual void OnChangeIMEConversionModeByHandle(int handleValue) abstract;
		virtual void OnChangeIMESentenceModeByHandle(int handleValue) abstract;

		virtual void OnInputLanguageChanged() abstract;
		virtual void OnIMEStartComposition() abstract;
		virtual void OnIMEComposition(int flags) abstract;
		virtual void OnIMEEndComposition() abstract;

		virtual void OnIMEShowCandidates() abstract;
		virtual void OnIMEChangeCandidates() abstract;
		virtual void OnIMECloseCandidates() abstract;
		virtual void OnIMERecomputeModes() abstract;

		virtual int  GetCandidateListCount() abstract;
		virtual void GetCandidate(int num, wchar_t* dest, int destSizeBytes) abstract;
		virtual int  GetCandidateListSelectedItem() abstract;
		virtual int  GetCandidateListPageSize() abstract;
		virtual int  GetCandidateListPageStart() abstract;

		//NOTE:  We render our own candidate lists most of the time...
		virtual void SetCandidateWindowPos(int x, int y) abstract;

		virtual bool GetShouldInvertCompositionString() abstract;
		virtual bool CandidateListStartsAtOne() abstract;

		virtual void SetCandidateListPageStart(int start) abstract;

		// Passes in a keycode which allows hitting other mouse buttons w/o cancelling capture mode
		virtual void SetMouseCaptureEx(Const::VGui::VPANEL panel, Const::VGui::KeyCode captureStartMouseCode) abstract;

		// Because OnKeyCodeTyped uses CallParentFunction and is therefore message based, there's no way
		//  to know if handler actually swallowed the specified keycode.  To get around this, I set a global before calling the
		//  kb focus OnKeyCodeTyped function and if we ever get to a Panel::OnKeyCodeTypes we know that nobody handled the message
		//  and in that case we can post a message to any "unhandled keycode" listeners
		// This will generate an MESSAGE_FUNC_INT( "KeyCodeUnhandled" "code" code ) message to each such listener
		virtual void RegisterKeyCodeUnhandledListener(Const::VGui::VPANEL panel) abstract;
		virtual void UnregisterKeyCodeUnhandledListener(Const::VGui::VPANEL panel) abstract;

		// Posts unhandled message to all interested panels
		virtual void OnKeyCodeUnhandled(int keyCode) abstract;

		// Assumes subTree is a child panel of the root panel for the vgui contect
		//  if restrictMessagesToSubTree is true, then mouse and kb messages are only routed to the subTree and it's children and mouse/kb focus
		//   can only be on one of the subTree children, if a mouse click occurs outside of the subtree, and "UnhandledMouseClick" message is sent to unhandledMouseClickListener panel
		//   if it's set
		//  if restrictMessagesToSubTree is false, then mouse and kb messages are routed as normal except that they are not routed down into the subtree
		//   however, if a mouse click occurs outside of the subtree, and "UnhandleMouseClick" message is sent to unhandledMouseClickListener panel
		//   if it's set
		virtual void	SetModalSubTree(Const::VGui::VPANEL subTree, Const::VGui::VPANEL unhandledMouseClickListener, bool restrictMessagesToSubTree = true) abstract;
		virtual void	ReleaseModalSubTree() abstract;
		virtual Const::VGui::VPANEL	GetModalSubTree() abstract;

		// These toggle whether the modal subtree is exclusively receiving messages or conversely whether it's being excluded from receiving messages
		// Sends a "ModalSubTree", state message
		virtual void	SetModalSubTreeReceiveMessages(bool state) abstract;
		virtual bool	ShouldModalSubTreeReceiveMessages() const abstract;

		virtual Const::VGui::VPANEL 	GetMouseCapture() abstract;
	};

}

TF2_NAMESPACE_END();
