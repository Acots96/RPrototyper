#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include "Events.h"
#include "Map.h"
#include "UIController.h"


class MainController
{
	Map PMap;
	UIController UICtrl;

	void HierarchyObjectClicked(const Utils::MapObjectId& Id);
	void AddChildToObject(const Utils::MapObjectId& Id);
	void RemoveObject(const Utils::MapObjectId& Id);
	void AllPropertiesChanged(const Utils::MapObjectProperties& Props);
	void PropertiesChanged(const Utils::MapObjectProperties& Props);
	void TransformChanged(const Utils::MapObjectProperties& Props);

public:
	MainController();
	~MainController();

	void UpdateAndDraw();

	Events::EventInt OnAddModelSelected;
	void AddModelSelected(int Idx);

	void IsUsingUI(bool IsUsing);
};
