/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Controls.h"
#include "GUISpecifications.h"
#include "Render.h"
#include "Valve/CRC32.h"


#pragma region Base Control

bool CControl::Flag(int f)
{
	if (m_Flags & f)
		return true;
	else
		return false;
}

POINT CControl::GetAbsolutePos()
{
	POINT p;
	
	if (parent)
	{
		RECT client = parent->GetClientArea();
		p.x = m_x + client.left;
		p.y = m_y + client.top;
	}

	return p;
}

#pragma endregion Implementations of the Base control functions

#pragma region CheckBox
CCheckBox::CCheckBox()
{
	Checked = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iWidth = 13;
	m_iHeight = 13;

	FileControlType = UIControlTypes::UIC_CheckBox;
}

void CCheckBox::SetState(bool s)
{
	Checked = s;
}

bool CCheckBox::GetState()
{
	return Checked;
}

void CCheckBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Color outline = UI_COL_TABTEXT;

	if (hover)
	{
		if (Checked)
		{
			outline = UI_COL_MAINDARK;
		}
		else
		{
			outline = UI_COL_SHADOW;
		}
	}
	else if (Checked)
	{
		outline = UI_COL_MAIN;
	}

	if(Checked)
		Render::Text(a.x + 1, a.y, UI_COL_MAIN, Render::Fonts::MenuSymbols, L"\uE10B");

	Render::Outline(a.x, a.y, UI_CHK_SIZE, UI_CHK_SIZE, outline);
}

void CCheckBox::OnUpdate() { m_iWidth = UI_CHK_SIZE; 	m_iHeight = UI_CHK_SIZE; }

void CCheckBox::OnClick()
{
	Checked = !Checked;
}
#pragma endregion Implementations of the Check Box functions

#pragma region Label
CLabel::CLabel()
{
	m_Flags = UIFlags::UI_Drawable;
	FontColor = UI_COL_TABTEXT;
	Text = "";
}

void CLabel::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Text(a.x, a.y, FontColor, Render::Fonts::MenuText, Text.c_str());
}

void CLabel::OnUpdate() {}
void CLabel::OnClick() {}
#pragma endregion Implementations of the Label functions

#pragma region GroupBox
CGroupBox::CGroupBox()
{
	MaxRow = 1;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
}

void CGroupBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	
	Render::Clear(a.x, a.y, m_iWidth, m_iHeight, COL_WHITE);
	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, UI_COL_GROUPOUTLINE);

	Render::Clear(a.x + 8, a.y + 20, m_iWidth - 16, 1, UI_COL_GROUPOUTLINE);

	RECT txtSize = Render::GetTextSize(Render::Fonts::MenuText, Text.c_str());
	Render::Text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y + 4, UI_COL_TABTEXT, Render::Fonts::MenuText, Text.c_str());
}

void CGroupBox::SetText(std::string text)
{
	Text = text;
}

void CGroupBox::PlaceLabledControl(std::string Label, CTab *Tab, CControl* control, int c, int r)
{
	int CellW = (m_iWidth / Columns);
	int x = m_x + 10 + (c * CellW);
	int y = m_y + 26 + r * 22;

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	Tab->RegisterControl(label);

	x += CellW / 2;

	int cw, ch;
	control->SetPosition(x, y);
	control->GetSize(cw, ch);
	control->SetSize((CellW / 2) - 20, ch);
	Tab->RegisterControl(control);

	if (r > MaxRow)
		MaxRow = r;

	char szCRCBuffer[256];
	sprintf_s(szCRCBuffer, "%s%s", Text.c_str(), Label.c_str());
	control->SetFileID(CRC32(szCRCBuffer, strlen(szCRCBuffer)));
	
	m_iHeight = (MaxRow + 1) * 22 + 26;
}

void CGroupBox::OnUpdate() { m_iWidth = parent->GetClientArea().right - 20; }
void CGroupBox::OnClick() {}
#pragma endregion Implementations of the Group Box functions

#pragma region Sliders
CSlider::CSlider()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Clear(a.x, a.y + 5, m_iWidth, 5, Color(110, 183, 212, 255));
	Render::Outline(a.x-1, a.y + 4, m_iWidth+2, 7, UI_COL_SHADOW);
	Render::GradientV(a.x, a.y + 5, m_iWidth, 3, Color(28, 136, 0, 255), Color(28, 136, 0, 255));

	float Ratio = Value / (Max - Min);
	float Location = Ratio*m_iWidth;

	Render::Clear(a.x, a.y + 5, Location, 5, UI_COL_MAIN);
	Render::Clear(a.x, a.y + 9, Location, 1, Color(50, 50, 50, 50));

	char buffer[24];
	sprintf_s(buffer, "%.2f", Value);
	RECT txtSize = Render::GetTextSize(Render::Fonts::MenuText, buffer);
	Render::Text(a.x + (m_iWidth / 2) - txtSize.right / 2, a.y + 10, UI_COL_TABTEXT, Render::Fonts::MenuText, buffer);
}

void CSlider::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 11;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x - 1;
			if (NewX < 0) NewX = 0;
			if (NewX > m_iWidth) NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider::GetValue()
{
	return Value;
}

void CSlider::SetValue(float v)
{
	Value = v;
}

void CSlider::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}
#pragma endregion Implementations of the Slider functions

#pragma region ComboBox
CComboBox::CComboBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_ComboBox;
}

void CComboBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };
	Render::Outline(a.x, a.y, m_iWidth, 16, UI_COL_CLIENTBACK);

	bool topHover = false;
	// Hover for the Top Box
	if (GUI.IsMouseInRegion(Region))
	{
		Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, 12, UI_COL_CLIENTBACK);
		topHover = true;
	}


	// If we have some items
	if (Items.size() > 0)
	{
		// The current item
		Render::Text(a.x + 4, a.y + 1, UI_COL_TABTEXT, Render::Fonts::MenuText, GetItem().c_str());

		// If the drop down part is open
		if (IsOpen)
		{
			Render::Clear(a.x, a.y + 17, m_iWidth, Items.size() * 16, UI_COL_CLIENTBACK);

			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 17 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					Render::Clear(a.x, a.y + 17 + i * 16, m_iWidth, 16, UI_COL_GROUPOUTLINE);
				}

				Render::Text(a.x + 4, a.y + 19 + i * 16, UI_COL_TABTEXT, Render::Fonts::MenuText, Items[i].c_str());
			}
		}
	}
}

void CComboBox::AddItem(std::string text)
{
	Items.push_back(text);
	SelectedIndex = 0;
}

void CComboBox::OnUpdate()
{
	if (IsOpen)
	{
		m_iHeight = 16 + 16 * Items.size();

		if (parent->GetFocus() != this)
			IsOpen = false;
	}
	else
	{
		m_iHeight = 16;
	}

}

void CComboBox::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };

	if (IsOpen)
	{
		// If we clicked one of the items(Not in the top bar)
		if (!GUI.IsMouseInRegion(Region))
		{
			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 16 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					SelectedIndex = i;

					if (OnItemSelect != nullptr)
						OnItemSelect();
				}
			}
		}

		// Close the drop down
		IsOpen = false;
	}
	else
	{
		IsOpen = true;
	}
}

int CComboBox::GetIndex()
{
	return SelectedIndex;
}

std::string CComboBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

void CComboBox::SelectIndex(int idx)
{
	if (idx >= 0 && idx < Items.size())
	{
		SelectedIndex = idx;
	}
}

#pragma endregion Implementations of the ComboBox functions

#pragma region KeyBinders

char* KeyStrings[254] = { nullptr, "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr, "SPACEBAR",
"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen", "Insert",
"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6",
"NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause", nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CKeyBind::CKeyBind()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	Key = -1;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

void CKeyBind::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, UI_COL_CLIENTBACK);
	if (hover)
		Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, UI_COL_CLIENTBACK);
	bool GoodKeyName = false;
	char NameBuffer[128];
	char* KeyName = "Not Bound";

	if (IsGettingKey)
	{
		KeyName = "<Press A Key>";
	}
	else
	{
		if (Key >= 0)
		{
			KeyName = KeyStrings[Key];
			if (KeyName)
			{
				GoodKeyName = true;
			}
			else
			{
				if (GetKeyNameText(Key << 16, NameBuffer, 127))
				{
					KeyName = NameBuffer;
					GoodKeyName = true;
				}
			}
		}

		if (!GoodKeyName)
		{
			KeyName = "No Key Bound";
		}
	}


	Render::Text(a.x + 4, a.y + 1, UI_COL_TABTEXT, Render::Fonts::MenuText, KeyName);
}

void CKeyBind::OnUpdate() {
	m_iHeight = 16;
	POINT a = GetAbsolutePos();
	if (IsGettingKey)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE)
				{
					IsGettingKey = false;
					Key = -1;
					return;
				}

				Key = i;
				IsGettingKey = false;
				return;
			}
		}
	}
}

void CKeyBind::OnClick() {
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}

int CKeyBind::GetKey()
{
	return Key;
}

void CKeyBind::SetKey(int key)
{
	Key = key;
}

#pragma endregion Implementations of the KeyBind Control functions

#define LIST_ITEM_HEIGHT 16
#define LIST_SCROLL_WIDTH 16

#pragma region ListBox
CListBox::CListBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	SelectedIndex = 0;
}

void CListBox::Draw(bool hover)
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, m_iHeight+2, UI_COL_CLIENTBACK);

	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = UI_COL_TABTEXT;
			RECT ItemRegion = { a.x+1, a.y + 1+ drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };

			if (i == SelectedIndex)
			{
				textColor = COL_WHITE;
				Render::Clear(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, UI_COL_MAIN);
			}
			else if (GUI.IsMouseInRegion(ItemRegion))
			{
				textColor = COL_WHITE;
				Render::Clear(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, UI_COL_TABTEXT);
			}

			Render::Text(ItemRegion.left+4, ItemRegion.top+2, textColor, Render::Fonts::MenuText, Items[i].c_str());
			drawnItems++;
		}

		// Ratio of how many visible to how many are hidden
		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		if (sizeRatio > 1.f) sizeRatio = 1.f;
		float posRatio = float(ScrollTop) / float(Items.size());
		if (posRatio > 1.f) posRatio = 1.f;

		sizeRatio *= m_iHeight;
		posRatio *= m_iHeight;

		Render::Clear(a.x + m_iWidth - LIST_SCROLL_WIDTH, a.y + posRatio, LIST_SCROLL_WIDTH, sizeRatio, UI_COL_TABTEXT);
	}

}

void CListBox::AddItem(std::string text, int value)
{
	Items.push_back(text);
	Values.push_back(value);
}

void CListBox::OnClick()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Check the items
	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = UI_COL_TABTEXT;
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };
			if (GUI.IsMouseInRegion(ItemRegion))
			{
				SelectItem(i);
				return;
			}
			drawnItems++;
		}
	}

	// Scrollbar checks are in update so we can just drag

}

void CListBox::OnUpdate()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Did we click in the scrollbar??
	RECT Scroll = { a.x + m_iWidth - LIST_SCROLL_WIDTH , a.y + 1, LIST_SCROLL_WIDTH - 2 ,m_iHeight };
	if (GUI.IsMouseInRegion(Scroll) && GUI.GetKeyState(VK_LBUTTON))
	{
		// Ratio of how many visible to how many are hidden
		float ratio = float(ItemsToDraw) / float(Items.size());
		POINT m = GUI.GetMouse();
		m.y -= a.y;
		
		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		sizeRatio *= m_iHeight;
		float heightDelta = m.y + sizeRatio - m_iHeight;
		if (heightDelta > 0)
			m.y -= heightDelta;

		float mPosRatio = float(m.y) / float(m_iHeight);
		ScrollTop = mPosRatio*Items.size();
		if (ScrollTop < 0)
			ScrollTop = 0;

	}
}

void CListBox::SetHeightInItems(int items)
{
	m_iHeight = items*LIST_ITEM_HEIGHT;
}

std::string CListBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}


#pragma endregion Implementations of the ListBox functions