#pragma once

#include "raygui.h"
#include <vector>
#include <string>
#include "Events.h"
#include "UIHierarchyObjectView.h"
#include "Utils.h"
#include "UIWindow.h"


class UIHierarchyView : public UIWindow
{
	const float LevelWidth = 15, ObjectHeight = 17;

	std::vector<UIHierarchyObjectView> Objects;

	UIHierarchyObjectView* ObjSelected;

	void DrawHierarchy();
	int CurrentFirstShownIdx;

	void ObjectClicked(const Utils::MapObjectId& Id);
	void AddChildToObject(const Utils::MapObjectId& Id);
	void RemoveObject(const Utils::MapObjectId& Id);

public:
	UIHierarchyView(Rectangle Rect);

	void UpdateObjects(const std::vector<Utils::MapObjectBasicData>& ObjectDatas);

	Events::EventMOId OnClicked;
	Events::EventMOId OnAddChild;
	Events::EventMOId OnRemove;

	void Draw() override;
};
