/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "GUI.h"
#include "Render.h"
#include "GUISpecifications.h"
#include <fstream>

#include <algorithm>
#include "tinyxml2.h"
#include "Controls.h"
#include "DLLMain.h"

CGUI GUI;

CGUI::CGUI()
{

}

//
// Draws all windows 
//
void CGUI::Draw()
{
	bool ShouldDrawCursor = false;

	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			ShouldDrawCursor = true;
			DrawWindow(window);
		}

	}

	if (ShouldDrawCursor)
	{
		static Vertex_t MouseVt[3];

		MouseVt[0].Init(Vector2D(Mouse.x, Mouse.y));
		MouseVt[1].Init(Vector2D(Mouse.x + UI_CURSORSIZE, Mouse.y));
		MouseVt[2].Init(Vector2D(Mouse.x, Mouse.y + UI_CURSORSIZE));

		Render::PolygonOutline(3, MouseVt, UI_CURSORFILL, UI_CURSOROUTLINE);
	}
}

// Handle all input etc
void CGUI::Update()
{
	//Key Array
	std::copy(keys, keys + 255, oldKeys);
	for (int x = 0; x < 255; x++) {
		keys[x] = static_cast<bool>(GetAsyncKeyState(x));
	}

	// Mouse Location
	POINT mp; GetCursorPos(&mp);
	ScreenToClient(GetForegroundWindow(), &mp);
	Mouse.x = mp.x; Mouse.y = mp.y;
	//Interfaces::Surface->SurfaceGetCursorPos(Mouse.x, Mouse.y);

	RECT Screen = Render::GetViewport();

	// Window Binds
	for (auto bind : WindowBinds)
	{
		if (GetKeyPress(bind.first) && bind.second != nullptr)
		{
			bind.second->Toggle();
		}
	}

	// Stop dragging
	if (IsDraggingWindow && !GetKeyState(VK_LBUTTON))
	{
		IsDraggingWindow = false;
		DraggingWindow = nullptr;
	}

	// If we are in the proccess of dragging a window
	if (IsDraggingWindow && GetKeyState(VK_LBUTTON) && !GetKeyPress(VK_LBUTTON))
	{
		if (DraggingWindow)
		{
			DraggingWindow->m_x = Mouse.x - DragOffsetX;
			DraggingWindow->m_y = Mouse.y - DragOffsetY;
		}
	}

	// Process some windows
	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			// Used to tell the widget processing that there could be a click
			bool bCheckWidgetClicks = false;

			// If the user clicks inside the window
			if (GetKeyPress(VK_LBUTTON))
			{
				if (IsMouseInRegion(window->m_x, window->m_y, window->m_x + window->m_iWidth, window->m_y + window->m_iHeight))
				{
					// Close Button
					if (IsMouseInRegion(window->m_x + window->m_iWidth - UI_WIN_CLOSE_X, window->m_y, window->m_x + window->m_iWidth, window->m_y+UI_WIN_CLOSE_X))
					{
						window->Toggle();
					}
					else
					// User is selecting a new tab
					if (IsMouseInRegion(window->GetTabArea()))
					{
						
						bCheckWidgetClicks = true;

						int iTab = 0;
						int TabCount = window->Tabs.size();
						if (TabCount) // If there are some tabs
						{
							int TabSize = UI_TAB_HEIGHT;
							int Dist = Mouse.y - (window->m_y + UI_WIN_TITLEHEIGHT + UI_WIN_TOPHEIGHT);
							if (Dist < (UI_TAB_HEIGHT*TabCount))
							{
								while (Dist > TabSize)
								{
									if (Dist > TabSize)
									{
										iTab++;
										Dist -= TabSize;
									}
									if (iTab == (TabCount - 1))
									{
										break;
									}
								}
								window->SelectedTab = window->Tabs[iTab];

								// Loose focus on the control
								bCheckWidgetClicks = false;
								window->IsFocusingControl = false;
								window->FocusedControl = nullptr;
							}
						}

					}
					// Is it inside the client area?
					else if (IsMouseInRegion(window->GetClientArea()))
					{
							bCheckWidgetClicks = true;
					}
					else
					{
						// Must be in the around the title or side of the window
						// So we assume the user is trying to drag the window
						IsDraggingWindow = true;
						DraggingWindow = window;
						DragOffsetX = Mouse.x - window->m_x;
						DragOffsetY = Mouse.y - window->m_y;

						// Loose focus on the control
						window->IsFocusingControl = false;
						window->FocusedControl = nullptr;
					}
				}
				else
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}

			// Controls 
			if (window->SelectedTab != nullptr)
			{
				// Focused widget
				bool SkipWidget = false;
				CControl* SkipMe = nullptr;

				// this window is focusing on a widget??
				if (window->IsFocusingControl)
				{
					if (window->FocusedControl != nullptr)
					{
						// We've processed it once, skip it later
						SkipWidget = true;
						SkipMe = window->FocusedControl;

						POINT cAbs = window->FocusedControl->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, window->FocusedControl->m_iWidth, window->FocusedControl->m_iHeight };
						window->FocusedControl->OnUpdate();

						if (window->FocusedControl->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							window->FocusedControl->OnClick();

							// If it gets clicked we loose focus
							window->IsFocusingControl = false;
							window->FocusedControl = nullptr;
							bCheckWidgetClicks = false;
						}
					}
				}

				// Itterate over the rest of the control
				for (auto control : window->SelectedTab->Controls)
				{
					if (control != nullptr)
					{
						if (SkipWidget && SkipMe == control)
							continue;

						control->parent = window;

						POINT cAbs = control->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
						control->OnUpdate();

						if (control->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							control->OnClick();
							bCheckWidgetClicks = false;

							// Change of focus
							if (control->Flag(UIFlags::UI_Focusable))
							{
								window->IsFocusingControl = true;
								window->FocusedControl = control;
							}
							else
							{
								window->IsFocusingControl = false;
								window->FocusedControl = nullptr;
							}

						}
					}
				}

				// We must have clicked whitespace
				if (bCheckWidgetClicks)
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}
		}
	}
}

//
// Returns true if this key has just been pressed
//
bool CGUI::GetKeyPress(unsigned int key)
{
	if (keys[key] == true && oldKeys[key] == false)
		return true;
	else
		return false;
}

//
// Returns true if this key is being held down
//
bool CGUI::GetKeyState(unsigned int key)
{
	if (key >= 0 && key <= 255)
		return keys[key];
	else
		return false;
}

//
// Returns true if this key is inside the given region
//
bool CGUI::IsMouseInRegion(int x, int y, int x2, int y2)
{
	if (Mouse.x > x && Mouse.y > y && Mouse.x < x2 && Mouse.y < y2)
		return true;
	else
		return false;
}

//
// Returns true if this key is inside the given region
//
bool CGUI::IsMouseInRegion(RECT region)
{
	return IsMouseInRegion(region.left, region.top, region.left + region.right, region.top + region.bottom);
}

//
// Returns the mouse cursor position
//
POINT CGUI::GetMouse()
{
	POINT p; p.x = Mouse.x; p.y = Mouse.y;
	return p;
}

bool CGUI::DrawWindow(CWindow* window)
{
	// Main window and title
	Render::Clear(window->m_x, window->m_y, window->m_iWidth, window->m_iHeight, Color(25,25,25,155));
	Render::Clear(window->m_x, window->m_y + UI_WIN_TOPHEIGHT, window->m_iWidth, UI_WIN_TITLEHEIGHT, UI_COL_MAIN);
	Render::GradientV(window->m_x, window->m_y + UI_WIN_TOPHEIGHT, window->m_iWidth, UI_WIN_TITLEHEIGHT, UI_COL_SHADOW, UI_COL_MAIN);
	Render::Outline(window->m_x, window->m_y + UI_WIN_TOPHEIGHT, window->m_iWidth, UI_WIN_TITLEHEIGHT, UI_COL_SHADOW);
	Render::Outline(window->m_x - 1, window->m_y - 1, window->m_iWidth + 2, window->m_iHeight + 2, UI_COL_SHADOW);

	// Close Button
	if (IsMouseInRegion(window->m_x + window->m_iWidth - UI_WIN_CLOSE_X, window->m_y, window->m_x + window->m_iWidth, window->m_y + UI_WIN_CLOSE_X))
		Render::Text(window->m_x + window->m_iWidth - UI_WIN_CLOSE_X, window->m_y + UI_WIN_CLOSE_Y, Color(255, 20, 20, 255), Render::Fonts::MenuWindowCloseButton, L"\u2715");
	else
		Render::Text(window->m_x + window->m_iWidth - UI_WIN_CLOSE_X, window->m_y + UI_WIN_CLOSE_Y, Color(50, 50, 50, 255), Render::Fonts::MenuWindowCloseButton, L"\u2715");

	// Title
	Render::Text(window->m_x + 17, window->m_y + UI_WIN_TITLEHEIGHT + 2, UI_COL_SHADOW, Render::Fonts::MenuWindowTitle, window->Title.c_str());
	Render::Text(window->m_x + 16, window->m_y + UI_WIN_TITLEHEIGHT + 1, COL_WHITE, Render::Fonts::MenuWindowTitle, window->Title.c_str());

	// Client
	if(window->m_HasTabs)
		Render::Clear(window->m_x + UI_TAB_WIDTH, window->m_y + UI_WIN_TOPHEIGHT + UI_WIN_TITLEHEIGHT, window->m_iWidth - UI_TAB_WIDTH, window->m_iHeight - UI_WIN_TOPHEIGHT - UI_WIN_TITLEHEIGHT, UI_COL_CLIENTBACK);
	else
		Render::Clear(window->m_x , window->m_y + UI_WIN_TOPHEIGHT + UI_WIN_TITLEHEIGHT, window->m_iWidth, window->m_iHeight - UI_WIN_TOPHEIGHT - UI_WIN_TITLEHEIGHT, UI_COL_CLIENTBACK);

	// Tabs
	int TabCount = window->Tabs.size();
	if (TabCount) // If there are some tabs
	{
		for (int i = 0; i < TabCount; i++)
		{
			RECT TabArea = { window->m_x, window->m_y + UI_WIN_TITLEHEIGHT + UI_WIN_TOPHEIGHT + (i*UI_TAB_HEIGHT) , UI_TAB_WIDTH, UI_TAB_HEIGHT };
			CTab *tab = window->Tabs[i];

			Render::Clear(TabArea.left, TabArea.top, UI_TAB_WIDTH, 1, UI_COL_TABSEPERATOR);
			Color txtColor = UI_COL_TABSEPERATOR;
			
			if (window->SelectedTab == tab)
			{
				// Selected
				txtColor = UI_COL_MAIN;
			}
			else if (IsMouseInRegion(TabArea))
			{
				// Hover
				txtColor = UI_COL_SHADOW;
			}
			
			Render::Text(TabArea.left + 32, TabArea.top + 8, txtColor, Render::Fonts::MenuWindowTab, tab->Title.c_str());
		}
	}

	// Controls 
	if (window->SelectedTab != nullptr)
	{
		// Focused widget
		bool SkipWidget = false;
		CControl* SkipMe = nullptr;

		// this window is focusing on a widget??
		if (window->IsFocusingControl)
		{
			if (window->FocusedControl != nullptr)
			{
				// We need to draw it last, so skip it in the regular loop
				SkipWidget = true;
				SkipMe = window->FocusedControl;
			}
		}


		// Itterate over all the other controls
		for (auto control : window->SelectedTab->Controls)
		{
			if (SkipWidget && SkipMe == control)
				continue;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				control->parent = window;
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

		// Draw the skipped widget last
		if (SkipWidget)
		{
			auto control = window->FocusedControl;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

	}
	return true;
}

void CGUI::RegisterWindow(CWindow* window)
{
	Windows.push_back(window);

	// Resorting to put groupboxes at the start
	for (auto tab : window->Tabs)
	{
		for (auto control : tab->Controls)
		{
			if (control->Flag(UIFlags::UI_RenderFirst))
			{
				CControl * c = control;
				tab->Controls.erase(std::remove(tab->Controls.begin(), tab->Controls.end(), control), tab->Controls.end());
				tab->Controls.insert(tab->Controls.begin(), control);
			}
		}
	}
}

void CGUI::BindWindow(unsigned char Key, CWindow* window)
{
	if (window)
		WindowBinds[Key] = window;
	else
		WindowBinds.erase(Key);
}

void CGUI::SaveWindowState(CWindow* window, std::string Filename)
{
	char filenameBuffer[260];
	//sprintf_s(filenameBuffer, "%s\\cfg\\%s.xml", BackEndInfo::g_LoaderInfo.LoaderPath, Filename.c_str());
	sprintf_s(filenameBuffer, "%s\\cfg\\%s.xml", BackEndInfo::g_LoaderInfo.LoaderPath, Filename.c_str());
	Filename = filenameBuffer;

	// Create a whole new document and we'll just save over top of the old one
	tinyxml2::XMLDocument Doc;

	// Root Element is called "ayy"
	tinyxml2::XMLElement *Root = Doc.NewElement("ayy");
	Doc.LinkEndChild(Root);

	// If the window has some tabs..
	if (Root && window->Tabs.size() > 0)
	{
		for (auto Tab : window->Tabs)
		{
			// Add a new section for this tab
			tinyxml2::XMLElement *TabElement = Doc.NewElement(Tab->Title.c_str());
			Root->LinkEndChild(TabElement);


			// Now we itterate the controls this tab contains
			if (TabElement && Tab->Controls.size() > 0)
			{
				for (auto Control : Tab->Controls)
				{
					// If the control is ok to be saved
					if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileID > 1 && Control->FileControlType)
					{
						char fileIdBuf[40];
						sprintf_s(fileIdBuf, "ctrl%x", Control->FileID);

						// Create an element for the control
						tinyxml2::XMLElement *ControlElement = Doc.NewElement(fileIdBuf);
						TabElement->LinkEndChild(ControlElement);

						if (!ControlElement)
						{
							return;
						}

						CCheckBox* cbx = nullptr;
						CComboBox* cbo = nullptr;
						CKeyBind* key = nullptr;
						CSlider* sld = nullptr;

						// Figure out what kind of control and data this is
						switch (Control->FileControlType)
						{
						case UIControlTypes::UIC_CheckBox:
							cbx = (CCheckBox*)Control;
							ControlElement->SetText(cbx->GetState());
							break;
						case UIControlTypes::UIC_ComboBox:
							cbo = (CComboBox*)Control;
							ControlElement->SetText(cbo->GetIndex());
							break;
						case UIControlTypes::UIC_KeyBind:
							key = (CKeyBind*)Control;
							ControlElement->SetText(key->GetKey());
							break;
						case UIControlTypes::UIC_Slider:
							sld = (CSlider*)Control;
							ControlElement->SetText(sld->GetValue());
							break;
						}
					}
				}
			}
		}
	}

	//Save the file
	if (Doc.SaveFile(Filename.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		//MessageBox(NULL, "Failed To Save Config File!", "AyyWare", MB_OK);
		Utilities::Log("Failed to save config file to %s", Filename.c_str());
	}

}

void CGUI::LoadWindowState(CWindow* window, std::string Filename)
{
	char filenameBuffer[260];
	sprintf_s(filenameBuffer, "%s\\cfg\\%s.xml", BackEndInfo::g_LoaderInfo.LoaderPath, Filename.c_str());
	Filename = filenameBuffer;

	// Lets load our meme
	tinyxml2::XMLDocument Doc;
	tinyxml2::XMLError loadErr = Doc.LoadFile(Filename.c_str());
	if (loadErr == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *Root = Doc.RootElement();

		// The root "ayy" element
		if (Root)
		{
			// If the window has some tabs..
			if (Root && window->Tabs.size() > 0)
			{
				for (auto Tab : window->Tabs)
				{
					// We find the corresponding element for this tab
					tinyxml2::XMLElement *TabElement = Root->FirstChildElement(Tab->Title.c_str());
					if (TabElement)
					{
						// Now we itterate the controls this tab contains
						if (TabElement && Tab->Controls.size() > 0)
						{
							
							for (auto Control : Tab->Controls)
							{
								// If the control is ok to be saved
								if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileID && Control->FileControlType)
								{
									char fileIdBuf[40];
									sprintf_s(fileIdBuf, "ctrl%x", Control->FileID);
									// Get the controls element
									tinyxml2::XMLElement *ControlElement = TabElement->FirstChildElement(fileIdBuf);

									if (ControlElement)
									{
										CCheckBox* cbx = nullptr;
										CComboBox* cbo = nullptr;
										CKeyBind* key = nullptr;
										CSlider* sld = nullptr;

										// Figure out what kind of control and data this is
										switch (Control->FileControlType)
										{
										case UIControlTypes::UIC_CheckBox:
											cbx = (CCheckBox*)Control;
											cbx->SetState(ControlElement->GetText()[0] == '1' ? true : false);
											break;
										case UIControlTypes::UIC_ComboBox:
											cbo = (CComboBox*)Control;
											cbo->SelectIndex(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_KeyBind:
											key = (CKeyBind*)Control;
											key->SetKey(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_Slider:
											sld = (CSlider*)Control;
											sld->SetValue(atof(ControlElement->GetText()));
											break;
										}
									}
									
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		Utilities::Log("Failed to open config file \"%s\" - error code %d", Filename.c_str(), loadErr);
	}
}