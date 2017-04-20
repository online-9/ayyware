/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "GUI.h"
#include "Render.h"
#include "GUISpecifications.h"
#include "Interfaces.h"

void CWindow::Toggle()
{
	m_bIsOpen = !m_bIsOpen;
	if (m_bIsOpen)
	{
		Interfaces::Engine->ClientCmd_Unrestricted("cl_mouseenable 0");
	}
	else
	{
		GUI.SaveWindowState(this, ConfigFilePath);
		Interfaces::Engine->ClientCmd_Unrestricted("cl_mouseenable 1");
	}
		
}

void CWindow::SaveToCurrentConfig()
{
	GUI.SaveWindowState(this, ConfigFilePath);
}

void CWindow::RegisterTab(CTab* Tab)
{
	if (Tabs.size() == 0)
	{
		SelectedTab = Tab;
	}
	Tab->parent = this;
	Tabs.push_back(Tab);
}

RECT CWindow::GetClientArea()
{
	RECT client;
	if (m_HasTabs)
		client.left = m_x + UI_TAB_WIDTH;
	else
		client.left = m_x;
	client.top = m_y + UI_WIN_TOPHEIGHT + UI_WIN_TITLEHEIGHT;

	if (m_HasTabs)
		client.right = m_iWidth - UI_TAB_WIDTH;
	else 
		client.right = m_iWidth;

	client.bottom = m_iHeight - UI_WIN_TOPHEIGHT - UI_WIN_TITLEHEIGHT;
	return client;
}

RECT CWindow::GetTabArea()
{
	RECT client;
	client.left = m_x;
	client.top = m_y + UI_WIN_TOPHEIGHT + UI_WIN_TITLEHEIGHT;
	client.right = UI_TAB_WIDTH;
	client.bottom = m_iHeight - UI_WIN_TOPHEIGHT - UI_WIN_TITLEHEIGHT;
	return client;
}

//====================================
//  Tabs :::^^^)))
//====================================

void CTab::RegisterControl(CControl* control)
{
	control->parent = parent;
	Controls.push_back(control);
}

int CTab::GetNextGroupboxY(CControl* control)
{
	return control->m_y + control->m_iHeight + 10;
}