/**********************************************************************************************
*
*   MIT License
*
*   Copyright (c) 2025 Aleix Cots Molina (@Acots96) (acots96@gmail.com)
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/


#include "UIHierarchyObjectView.h"
#include "Utils.h"
#include <iostream>
#include "UIWindow.h"

using namespace Utils;


UIHierarchyObjectView::UIHierarchyObjectView(float LevelWidth, 
	float ObjectHeight, const Utils::MapObjectBasicData Object) :
	ScreenWidth{ 0 }, ScreenHeight{ 0 }, LevelWidth{ LevelWidth }, 
	ObjHeight{ ObjectHeight }, 
	Rect{}, PrntRect{}, FullRect{}, ObjIdx{ -1 }, 
	ObjData{ Object }, IsSelected{ false }
{
}


void UIHierarchyObjectView::DrawHierarchyObject()
{
	Rect = {
		PrntRect.x + ObjData.Level * LevelWidth + OffsetX,
		PrntRect.y + ObjIdx * ObjHeight,
		PrntRect.width,
		ObjHeight
	};

	FullRect = { PrntRect.x, Rect.y, PrntRect.width, Rect.height };

	auto p = GetMousePosition();
	if (UIsPointInRect(p, FullRect))
	{
		if (GuiButton(FullRect, ""))
		{
			if (OnClicked) OnClicked(ObjData.Id);
		}
	}
	else if (IsSelected)
	{
		Color LIGHTBLUE{ 150, 220, 255, 255 };
		UGuiDrawRectangle(FullRect, 0, LIGHTBLUE, LIGHTBLUE);
	}

	// TODO
	// Find why it crashes without being duplicated
	// when this OnAddChild is called...
	Rectangle r{ Rect }; r.width = std::max(0.f, FullRect.x + r.width - r.x);
	UGuiDrawText(ObjData.Name.data(), r, TEXT_ALIGN_LEFT, DARKGRAY);
}


void UIHierarchyObjectView::Draw(Rectangle ParentRect, int Idx)
{
	if (Idx < 0) return;
	//std::cout << "  +++++++++++:" << GetObject().Name << "(" << Idx << ")";

	ScreenWidth = GetScreenWidth();
	ScreenHeight = GetScreenHeight();

	PrntRect = ParentRect;
	ObjIdx = Idx;
	DrawHierarchyObject();
}
