#include "UIHierarchyView.h"
#include <iostream>

using namespace Utils;


UIHierarchyView::UIHierarchyView(Rectangle Rect) :
	CurrentFirstShownIdx{ 0 }, ObjSelected{ nullptr },
	UIWindow(Rect, "Hierarchy")
{
}


void UIHierarchyView::UpdateObjects(
	const std::vector<Utils::MapObjectBasicData>& ObjectDatas)
{
	Objects.clear();
	for (auto& d : ObjectDatas)
	{
		UIHierarchyObjectView ho{ LevelWidth, ObjectHeight, d };
		ho.OnClicked = [this](const MapObjectId& Id) { this->ObjectClicked(Id); };
		ho.OnAddChild = [this](const MapObjectId& Id) { this->AddChildToObject(Id); };
		ho.OnRemove = [this](const MapObjectId& Id) { this->RemoveObject(Id); };
		Objects.push_back(ho);
	}
}


void UIHierarchyView::DrawHierarchy()
{
	float incr{ 8 }, h{ TitleR.height - incr };
	if (GuiButton({ Rect.x + Rect.width - 26, Rect.y + incr / 2, h, h}, "+"))
	{
		if (ObjSelected)
			AddChildToObject(ObjSelected->GetObject().Id);
		else
			AddChildToObject(MapObjectId::Empty());
	}

	/*auto p = GetMousePosition();
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && 
		UIsPointInRect(p, Rect) && !UIsPointInRect(p, TitleR))
	{
		if (ObjSelected) ObjSelected->MarkSelected(false);
		ObjSelected = nullptr;
	}*/

	Rectangle r{ Rect }; r.y += 26; r.height -= 26;
	auto maxAmountToShow = (int)(r.height / ObjectHeight);
	int lastIdx{ CurrentFirstShownIdx + maxAmountToShow };
	int totIdx{ 0 }, relIdx{ 0 };
	for (auto& o : Objects)
	{
		if (totIdx >= CurrentFirstShownIdx && totIdx <= lastIdx)
			o.Draw(r, relIdx++);
		else
			o.Draw(r, -1);
		//std::cout << o.GetObject().Name.c_str() << "\n";
		++totIdx;
	}

	if (Objects.size() > maxAmountToShow)
	{
		Rectangle scrollR{
			r.x + r.width - 10,
			r.y,
			10,
			r.height
		};
		auto s = UGuiScrollBar(scrollR, 0, r.y, r.y + 2);
	}
}


void UIHierarchyView::ObjectClicked(const MapObjectId& Id)
{
	if (ObjSelected) ObjSelected->MarkSelected(false);
	auto it = std::find_if(Objects.begin(), Objects.end(),
		[&Id](const UIHierarchyObjectView& h) { return h.GetObject().Id == Id; });
	if (it != Objects.end())
	{
		ObjSelected = &(*it);
		ObjSelected->MarkSelected(true);
	}
	if (OnClicked) OnClicked(Id);
}

void UIHierarchyView::AddChildToObject(const MapObjectId& Id)
{
	if (OnAddChild) OnAddChild(Id);
}

void UIHierarchyView::RemoveObject(const MapObjectId& Id)
{
	if (OnRemove) OnRemove(Id);
}


void UIHierarchyView::Draw()
{
	UIWindow::Draw();
	DrawHierarchy();
}
