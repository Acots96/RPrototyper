#pragma once

#include "raygui.h"
#include <vector>
#include <string>
#include "Events.h"
#include "Utils.h"


class UIHierarchyObjectView
{
	const float OffsetX = 5;
	float LevelWidth, ObjHeight;

	float ScreenWidth;
	float ScreenHeight;

	Rectangle Rect, PrntRect, FullRect;

	/*std::string Name;
	int Level;*/
	int ObjIdx;
	Utils::MapObjectBasicData ObjData;
	void DrawHierarchyObject();
	bool IsSelected;

public:
	UIHierarchyObjectView(float LevelWidth, float ObjectHeight, 
		const Utils::MapObjectBasicData Object);

	void Draw(Rectangle ParentRect, int Idx);
	//void SetName(std::string ObjectName);
	//void SetLevel(int ObjectLevel);

	const Utils::MapObjectBasicData& GetObject() const
	{ return ObjData; }

	Events::EventMOId OnClicked;
	Events::EventMOId OnAddChild;
	Events::EventMOId OnRemove;

	void MarkSelected(bool Selected) { IsSelected = Selected; }
};
