/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once


#include "Interfaces.h"
#include "Valve/Vector2D.h"

void Quad();


namespace Render
{
	void Initialise();

	// Normal Drawing functions
	void Clear(int x, int y, int w, int h, Color color);
	void Outline(int x, int y, int w, int h, Color color);
	void Line(int x, int y, int x2, int y2, Color color);
	void PolyLine(int *x, int *y, int count, Color color);
	void Polygon(int count, Vertex_t* Vertexs, Color color);
	void PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine);
	void PolyLine(int count, Vertex_t* Vertexs, Color colorLine);
	void OutlineCircle(int x, int y, int r, int seg, Color color);

	// Gradient Functions
	void GradientV(int x, int y, int w, int h, Color c1, Color c2);
	void GradientH(int x, int y, int w, int h, Color c1, Color c2);
	void GradientFadeV(int x, int y, int w, int h, Color c);

	// Text functions
	namespace Fonts
	{
		extern DWORD Default;
		extern DWORD MenuTitle;
		extern DWORD ESP;

		extern DWORD MenuWindowTitle;
		extern DWORD MenuWindowCloseButton;
		extern DWORD MenuWindowTab;
		extern DWORD MenuSymbols;
		extern DWORD MenuText;

	};

	void Text(int x, int y, Color color, DWORD font, const char* text);
	void Text(int x, int y, Color color, DWORD font, const wchar_t* text);
	void Textf(int x, int y, Color color, DWORD font, const char* fmt, ...);
	RECT GetTextSize(DWORD font, const char* text);

	// Other rendering functions
	bool WorldToScreen(Vector &in, Vector &out);
	RECT GetViewport();
};