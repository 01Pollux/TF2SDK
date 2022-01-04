#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/engine/AppSystem.hpp>
#include "Consts.hpp"


TF2_NAMESPACE_BEGIN();

namespace VGui
{
	class ISurface;
}


namespace interfaces
{
	TF2_EXPORT_INTERFACE(VGui::ISurface, Surface, "VGUI_Surface030");
}

namespace VGui
{
	class Panel;
	class SurfacePlat;
	class IHTMLEvents;
	class IHTML;
	class IVguiMatInfo;
	class IImage;

	//-----------------------------------------------------------------------------
	// Purpose: Wraps contextless windows system functions
	//-----------------------------------------------------------------------------
	class ISurface : public IAppSystem
	{
	public:
		// call to Shutdown surface; surface can no longer be used after this is called
		virtual void Shutdown() abstract;

		// frame
		virtual void RunFrame() abstract;

		// hierarchy root
		virtual Const::VGui::VPANEL GetEmbeddedPanel() abstract;
		virtual void SetEmbeddedPanel(Const::VGui::VPANEL pPanel) abstract;

		// drawing context
		virtual void PushMakeCurrent(Const::VGui::VPANEL panel, bool useInsets) abstract;
		virtual void PopMakeCurrent(Const::VGui::VPANEL panel) abstract;

		// rendering functions
		virtual void DrawSetColor(int r, int g, int b, int a) abstract;
		virtual void DrawSetColor(Color4_8 col) abstract;

		virtual void DrawFilledRect(int x0, int y0, int x1, int y1) abstract;
		virtual void DrawFilledRectArray(Const::VGui::IntRect* pRects, int numRects) abstract;
		virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) abstract;

		virtual void DrawLine(int x0, int y0, int x1, int y1) abstract;
		virtual void DrawPolyLine(int* px, int* py, int numPoints) abstract;

		virtual void DrawSetTextFont(Const::VGui::HFont font) abstract;
		virtual void DrawSetTextColor(int r, int g, int b, int a) abstract;
		virtual void DrawSetTextColor(Color4_8 col) abstract;
		virtual void DrawSetTextPos(int x, int y) abstract;
		virtual void DrawGetTextPos(int& x, int& y) abstract;
		virtual void DrawPrintText(const wchar_t* text, int textLen, Const::VGui::FontDrawType drawType = Const::VGui::FontDrawType::Default) abstract;
		virtual void DrawUnicodeChar(wchar_t wch, Const::VGui::FontDrawType drawType = Const::VGui::FontDrawType::Default) abstract;

		virtual void DrawFlushText() abstract;		// flushes any buffered text (for rendering optimizations)
		virtual IHTML* CreateHTMLWindow(IHTMLEvents* events, Const::VGui::VPANEL context) abstract;
		virtual void PaintHTMLWindow(IHTML* htmlwin) abstract;
		virtual void DeleteHTMLWindow(IHTML* htmlwin) abstract;

		virtual int	 DrawGetTextureId(char const* filename) abstract;
		virtual bool DrawGetTextureFile(int id, char* filename, int maxlen) abstract;
		virtual void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload) abstract;
		virtual void DrawSetTextureRGBA(int id, const unsigned char* rgba, int wide, int tall, int hardwareFilter, bool forceReload) abstract;
		virtual void DrawSetTexture(int id) abstract;
		virtual void DrawGetTextureSize(int id, int& wide, int& tall) abstract;
		virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) abstract;
		virtual bool IsTextureIDValid(int id) abstract;
		virtual bool DeleteTextureByID(int id) abstract;

		virtual int CreateNewTextureID(bool procedural = false) abstract;

		virtual void GetScreenSize(int& wide, int& tall) abstract;
		virtual void SetAsTopMost(Const::VGui::VPANEL panel, bool state) abstract;
		virtual void BringToFront(Const::VGui::VPANEL panel) abstract;
		virtual void SetForegroundWindow(Const::VGui::VPANEL panel) abstract;
		virtual void SetPanelVisible(Const::VGui::VPANEL panel, bool state) abstract;
		virtual void SetMinimized(Const::VGui::VPANEL panel, bool state) abstract;
		virtual bool IsMinimized(Const::VGui::VPANEL panel) abstract;
		virtual void FlashWindow(Const::VGui::VPANEL panel, bool state) abstract;
		virtual void SetTitle(Const::VGui::VPANEL panel, const wchar_t* title) abstract;
		virtual void SetAsToolBar(Const::VGui::VPANEL panel, bool state) abstract;		// removes the window's task bar entry (for context menu's, etc.)

		// windows stuff
		virtual void CreatePopup(Const::VGui::VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) abstract;
		virtual void SwapBuffers(Const::VGui::VPANEL panel) abstract;
		virtual void Invalidate(Const::VGui::VPANEL panel) abstract;
		virtual void SetCursor(Const::VGui::HCursor cursor) abstract;
		virtual void SetCursorAlwaysVisible(bool visible) abstract;
		virtual bool IsCursorVisible() abstract;
		virtual void ApplyChanges() abstract;
		virtual bool IsWithin(int x, int y) abstract;
		virtual bool HasFocus() abstract;

		// returns true if the surface supports minimize & maximize capabilities
		virtual bool SupportsFeature(Const::VGui::SurfaceFeature feature) abstract;

		// restricts what gets drawn to one panel and it's children
		// currently only works in the game
		virtual void RestrictPaintToSinglePanel(Const::VGui::VPANEL panel) abstract;

		// these two functions obselete, use IInput::SetAppModalSurface() instead
		virtual void SetModalPanel(Const::VGui::VPANEL) abstract;
		virtual Const::VGui::VPANEL GetModalPanel() abstract;

		virtual void UnlockCursor() abstract;
		virtual void LockCursor() abstract;
		virtual void SetTranslateExtendedKeys(bool state) abstract;
		virtual Const::VGui::VPANEL GetTopmostPopup() abstract;

		// engine-only focus handling (replacing WM_FOCUS windows handling)
		virtual void SetTopLevelFocus(Const::VGui::VPANEL panel) abstract;

		// fonts
		// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
		virtual Const::VGui::HFont CreateFont() abstract;

		// adds to the font
		virtual bool SetFontGlyphSet(Const::VGui::HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) abstract;

		// adds a custom font file (only supports true type font files (.ttf) for now)
		virtual bool AddCustomFontFile(const char* fontName, const char* fontFileName) abstract;

		// returns the details about the font
		virtual int GetFontTall(Const::VGui::HFont font) abstract;
		virtual int GetFontTallRequested(Const::VGui::HFont font) abstract;
		virtual int GetFontAscent(Const::VGui::HFont font, wchar_t wch) abstract;
		virtual bool IsFontAdditive(Const::VGui::HFont font) abstract;
		virtual void GetCharABCwide(Const::VGui::HFont font, int ch, int& a, int& b, int& c) abstract;
		virtual int GetCharacterWidth(Const::VGui::HFont font, int ch) abstract;
		virtual void GetTextSize(Const::VGui::HFont font, const wchar_t* text, int& wide, int& tall) abstract;

		// notify icons?!?
		virtual Const::VGui::VPANEL GetNotifyPanel() abstract;
		virtual void SetNotifyIcon(Const::VGui::VPANEL context, Const::VGui::HTexture icon, Const::VGui::VPANEL panelToReceiveMessages, const char* text) abstract;

		// plays a sound
		virtual void PlaySound(const char* fileName) abstract;

		//!! these functions should not be accessed directly, but only through other vgui items
		//!! need to move these to seperate interface
		virtual int GetPopupCount() abstract;
		virtual Const::VGui::VPANEL GetPopup(int index) abstract;
		virtual bool ShouldPaintChildPanel(Const::VGui::VPANEL childPanel) abstract;
		virtual bool RecreateContext(Const::VGui::VPANEL panel) abstract;
		virtual void AddPanel(Const::VGui::VPANEL panel) abstract;
		virtual void ReleasePanel(Const::VGui::VPANEL panel) abstract;
		virtual void MovePopupToFront(Const::VGui::VPANEL panel) abstract;
		virtual void MovePopupToBack(Const::VGui::VPANEL panel) abstract;

		virtual void SolveTraverse(Const::VGui::VPANEL panel, bool forceApplySchemeSettings = false) abstract;
		virtual void PaintTraverse(Const::VGui::VPANEL panel) abstract;

		virtual void EnableMouseCapture(Const::VGui::VPANEL panel, bool state) abstract;

		// returns the size of the workspace
		virtual void GetWorkspaceBounds(int& x, int& y, int& wide, int& tall) abstract;

		// gets the absolute coordinates of the screen (in windows space)
		virtual void GetAbsoluteWindowBounds(int& x, int& y, int& wide, int& tall) abstract;

		// gets the base resolution used in proportional mode
		virtual void GetProportionalBase(int& width, int& height) abstract;

		virtual void CalculateMouseVisible() abstract;
		virtual bool NeedKBInput() abstract;

		virtual bool HasCursorPosFunctions() abstract;
		virtual void SurfaceGetCursorPos(int& x, int& y) abstract;
		virtual void SurfaceSetCursorPos(int x, int y) abstract;

		// SRC only functions!!!
		virtual void DrawTexturedLine(const Const::VGui::Vertex& a, const Const::VGui::Vertex& b) abstract;
		virtual void DrawOutlinedCircle(int x, int y, int radius, int segments) abstract;
		virtual void DrawTexturedPolyLine(const Const::VGui::Vertex* p, int n) abstract; // (Note: this connects the first and last points).
		virtual void DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1) abstract;
		virtual void DrawTexturedPolygon(int n, Const::VGui::Vertex* pVertice, bool bClipVertices = true) abstract;
		virtual const wchar_t* GetTitle(Const::VGui::VPANEL panel) abstract;
		virtual bool IsCursorLocked(void) const abstract;
		virtual void SetWorkspaceInsets(int left, int top, int right, int bottom) abstract;

		// Lower level char drawing code, call DrawGet then pass in info to DrawRender
		virtual bool DrawGetUnicodeCharRenderInfo(wchar_t ch, Const::VGui::CharRenderInfo& info) abstract;
		virtual void DrawRenderCharFromInfo(const Const::VGui::CharRenderInfo& info) abstract;

		// global alpha setting functions
		// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
		virtual void DrawSetAlphaMultiplier(float alpha /* [0..1] */) abstract;
		virtual float DrawGetAlphaMultiplier() abstract;

		// web browser
		virtual void SetAllowHTMLJavaScript(bool state) abstract;

		// video mode changing
		virtual void OnScreenSizeChanged(int nOldWidth, int nOldHeight) abstract;

		virtual Const::VGui::HCursor CreateCursorFromFile(char const* curOrAniFile, char const* pPathID = 0) abstract;

		// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
		virtual IVguiMatInfo* DrawGetTextureMatInfoFactory(int id) abstract;

		virtual void PaintTraverseEx(Const::VGui::VPANEL panel, bool paintPopups = false) abstract;

		virtual float GetZPos() const abstract;

		// From the Xbox
		virtual void SetPanelForInput(Const::VGui::VPANEL panel) abstract;
		virtual void DrawFilledRectFastFade(int x0, int y0, int x1, int y1, int fadeStartPt, int fadeEndPt, unsigned int alpha0, unsigned int alpha1, bool bHorizontal) abstract;
		virtual void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal) abstract;
		virtual void DrawSetTextureRGBAEx(int id, const unsigned char* rgba, int wide, int tall, Const::VGui::ImageFormat imageFormat) abstract;
		virtual void DrawSetTextScale(float sx, float sy) abstract;
		virtual bool SetBitmapFontGlyphSet(Const::VGui::HFont font, const char* windowsFontName, float scalex, float scaley, int flags) abstract;
		// adds a bitmap font file
		virtual bool AddBitmapFontFile(const char* fontFileName) abstract;
		// sets a symbol for the bitmap font
		virtual void SetBitmapFontName(const char* pName, const char* pFontFilename) abstract;
		// gets the bitmap font filename
		virtual const char* GetBitmapFontName(const char* pName) abstract;
		virtual void ClearTemporaryFontCache(void) abstract;

		virtual IImage* GetIconImageForFullPath(char const* pFullPath) abstract;
		virtual void DrawUnicodeString(const wchar_t* pwString, Const::VGui::FontDrawType drawType = Const::VGui::FontDrawType::Default) abstract;
		virtual void PrecacheFontCharacters(Const::VGui::HFont font, const wchar_t* pCharacters) abstract;
		// Console-only.  Get the string to use for the current video mode for layout files.
		virtual const char* GetResolutionKey(void) const abstract;

		virtual const char* GetFontName(Const::VGui::HFont font) abstract;
		virtual const char* GetFontFamilyName(Const::VGui::HFont font) abstract;
		virtual void GetKernedCharWidth(Const::VGui::HFont font, wchar_t ch, wchar_t chBefore, wchar_t chAfter, float& wide, float& abcA) abstract;

		virtual bool ForceScreenSizeOverride(bool bState, int wide, int tall) abstract;
		// LocalToScreen, ParentLocalToScreen fixups for explicit PaintTraverse calls on Panels not at 0, 0 position
		virtual bool ForceScreenPosOffset(bool bState, int x, int y) abstract;
		virtual void OffsetAbsPos(int& x, int& y) abstract;


		// Causes fonts to get reloaded, etc.
		virtual void ResetFontCaches() abstract;

		virtual int GetTextureNumFrames(int id) abstract;
		virtual void DrawSetTextureFrame(int id, int nFrame, unsigned int* pFrameCache) abstract;
		virtual bool IsScreenSizeOverrideActive(void) abstract;
		virtual bool IsScreenPosOverrideActive(void) abstract;

		virtual void DestroyTextureID(int id) abstract;

		virtual void DrawUpdateRegionTextureRGBA(int nTextureID, int x, int y, const unsigned char* pchData, int wide, int tall, Const::VGui::ImageFormat imageFormat) abstract;
		virtual bool BHTMLWindowNeedsPaint(IHTML* htmlwin) abstract;

		virtual const char* GetWebkitHTMLUserAgentString() abstract;

		virtual void* Deprecated_AccessChromeHTMLController() abstract;

		// the origin of the viewport on the framebuffer (Which might not be 0,0 for stereo)
		virtual void SetFullscreenViewport(int x, int y, int w, int h) abstract; // this uses NULL for the render target.
		virtual void GetFullscreenViewport(int& x, int& y, int& w, int& h) abstract;
		virtual void PushFullscreenViewport() abstract;
		virtual void PopFullscreenViewport() abstract;

		// handles support for software cursors
		virtual void SetSoftwareCursor(bool bUseSoftwareCursor) abstract;
		virtual void PaintSoftwareCursor() abstract;


		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !! WARNING! YOU MUST NOT ADD YOUR NEW METHOD HERE OR YOU WILL BREAK MODS !!
		// !! Add your new stuff to the bottom of IMatSystemSurface instead.        !!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	};

}

TF2_NAMESPACE_END();
