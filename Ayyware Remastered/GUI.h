/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Common.h"

class CControl;
class CTab;
class CWindow;
class CGUI;

extern CGUI GUI;

//
// Flags set by control types
//
enum UIFlags
{
	UI_None = 0x00, // No Flags
	UI_Drawable = 0x01, // This control should be drawn
	UI_Clickable = 0x02, // This control has a click event
	UI_Focusable = 0x04, // This control can hold an input focus
	UI_RenderFirst = 0x08, // This control should be drawn under all others
	UI_SaveFile = 0x10 // This control can be saved to the config files
};

//
// Types of controls
//
enum UIControlTypes
{
	UIC_CheckBox = 1,
	UIC_Slider,
	UIC_KeyBind,
	UIC_ComboBox
};

// Base class for GUI controls
class CControl
{
	friend class CGUI;
	friend class CTab;
	friend class CWindow;
public:
	inline void SetPosition(int x, int y) { m_x = x; m_y = y; }
	inline void SetSize(int w, int h) { m_iWidth = w; m_iHeight = h; }
	inline void GetSize(int &w, int &h) { w = m_iWidth; h = m_iHeight; }
	inline void SetFileID(DWORD ID) { FileID = ID; }

	bool Flag(int f);
protected:
	int m_x, m_y;
	int m_iWidth, m_iHeight;
	int m_Flags;
	DWORD FileID;
	int FileControlType;
	CWindow* parent;

	virtual void Draw(bool) = 0;
	virtual void OnUpdate() = 0;
	virtual void OnClick() = 0;

	POINT GetAbsolutePos();
};

// A GUI Control Container
class CTab
{
	friend class CControl;
	friend class CGUI;
	friend class CWindow;
public:
	inline void SetTitle(std::string name) { Title = name; }
	void RegisterControl(CControl* control);
	int GetNextGroupboxY(CControl* control);
private:
	std::string Title;
	std::vector<CControl*> Controls;
	CWindow* parent;
};

// Base class for a simple GUI window
class CWindow
{
	friend class CControl;
	friend class CGUI;
public:
	inline void SetPosition(int x, int y) { m_x = x; m_y = y; }
	inline void SetSize(int w, int h) { m_iWidth = w;	m_iHeight = h; }
	inline void SetTitle(std::string title) { Title = title; }
	inline void Open() { m_bIsOpen = true; }
	inline void Close() { m_bIsOpen = false; }
	inline bool IsOpen() { return m_bIsOpen; }
	void Toggle();
	inline CControl* GetFocus() { return FocusedControl; }

	void RegisterTab(CTab* Tab);
	inline void EnableTabs(bool tabs) { m_HasTabs = tabs; }

	RECT GetClientArea();
	RECT GetTabArea();
	void inline SetConfigFile(std::string p) { ConfigFilePath = p; }
	void SaveToCurrentConfig();

private:
	void DrawControls();
	std::string ConfigFilePath;

	bool m_bIsOpen;

	bool m_HasTabs;

	std::vector<CTab*> Tabs;
	CTab* SelectedTab;

	bool IsFocusingControl;
	CControl* FocusedControl;

	std::string Title;
	int m_x;
	int m_y;
	int m_iWidth;
	int m_iHeight;
};

// User interface manager
class CGUI
{
public:
	CGUI();

	// Draws all windows 
	void Draw();

	// Handle all input etc
	void Update();

	// Draws a single window
	bool DrawWindow(CWindow* window);

	// Registers a window
	void RegisterWindow(CWindow* window);

	// Config saving/loading
	void SaveWindowState(CWindow* window, std::string Filename);
	void LoadWindowState(CWindow* window, std::string Filename);

	// Window Binds
	void BindWindow(unsigned char Key, CWindow* window);

	// Input
	bool GetKeyPress(unsigned int key);
	bool GetKeyState(unsigned int key);
	bool IsMouseInRegion(int x, int y, int x2, int y2);
	bool IsMouseInRegion(RECT region);
	POINT GetMouse();

private:
	// Input
	// keyboard
	bool keys[256];
	bool oldKeys[256];
	// Mouse
	struct MousePoint
	{
		int x; int y;
	} Mouse;
	bool MenuOpen;

	// Window Dragging
	bool IsDraggingWindow;
	CWindow* DraggingWindow;
	int DragOffsetX; int DragOffsetY;

	// Windows
	std::vector<CWindow*> Windows;

	// KeyBinds -> Windows Map
	std::map<int, CWindow*> WindowBinds;

};