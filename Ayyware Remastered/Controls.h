/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "GUI.h"
#include "SDK.h"

class CCheckBox : public CControl
{
public:
	CCheckBox();
	void SetState(bool s);
	bool GetState();
protected:
	bool Checked;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CLabel : public CControl
{
public:
	CLabel();
	inline void SetText(std::string text) { Text = text; }
	inline void SetTextColor(Color c) { FontColor = c; }
protected:
	Color FontColor;
	std::string Text;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CGroupBox : public CControl
{
public:
	CGroupBox();
	void SetText(std::string text);
	void PlaceLabledControl(std::string Label, CTab *Tab, CControl* control, int c, int r);
	inline void SetColumns(int c) { Columns = c; }
protected:
	int Columns;
	int MaxRow;
	std::string Text;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CSlider : public CControl
{
public:
	CSlider();
	float GetValue();
	void SetValue(float v);
	void SetBoundaries(float min, float max);
protected:
	float Value;
	float Min;
	float Max;
	bool DoDrag;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CComboBox : public CControl
{
public:
	CComboBox();
	void AddItem(std::string text);
	void SelectIndex(int idx);
	int GetIndex();
	std::string GetItem();
	typedef void(*EVENT_t)(void);
	inline void SetEventItemSelect(EVENT_t e) { OnItemSelect = e; }

protected:
	EVENT_t OnItemSelect;
	std::vector<std::string> Items;
	int SelectedIndex;
	bool IsOpen;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CKeyBind : public CControl
{
public:
	CKeyBind();
	int GetKey();
	void SetKey(int key);
protected:
	int Key;
	bool IsGettingKey;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CListBox : public CControl
{
public:
	CListBox();
	void AddItem(std::string text, int value = 0);
	void SelectIndex(int idx);
	inline int GetIndex() { return SelectedIndex; }
	std::string GetItem();
	inline int GetValue() { if (SelectedIndex >= 0 && SelectedIndex < Items.size()) return Values[SelectedIndex]; else return -1; };
	inline void ClearItems() { Items.clear(); Values.clear(); }
	void SetHeightInItems(int items);
	inline void SelectItem(int idx) { if (idx >= 0 && idx < Items.size()) SelectedIndex = idx;}

protected:
	std::vector<std::string> Items;
	std::vector<int> Values;

	int ScrollTop;

	int SelectedIndex;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};