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
