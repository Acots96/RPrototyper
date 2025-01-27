#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include "Events.h"
#include "UIHierarchyView.h"
#include "UIPropertiesView.h"
#include "Utils.h"


class UIController
{
	float ScreenWidth;
	float ScreenHeight;

	// Separated by ';'
	std::string ModelsPathsStr;
	// Separated by ';'
	std::string TexturesPathsStr;

	void DrawTopButtons();
	bool bIsAddModelOpen;
	int AddModelScrollIdx, AddModelActiveIdx;

	std::vector<std::unique_ptr<UIWindow>> Windows;
	UIHierarchyView* Hierarchy;
	UIPropertiesView* Properties;

	void ObjectClicked(const Utils::MapObjectId& Id) const;
	void AddChildToObject(const Utils::MapObjectId& Id) const;
	void RemoveObject(const Utils::MapObjectId& Id) const;
	void PropertiesChanged(const Utils::MapObjectProperties& Props) const;

	bool bIsUsingUI;
	void CheckUsingUI(bool Using);

public:
	UIController();

	void InitResourcesNames(
		const std::vector<std::string>& ModelsPaths,
		const std::vector<std::string>& TexturesPaths);

	void Draw();

	Events::EventMOId OnHierarchyObjectClicked;
	Events::EventMOId OnAddChildToObject;
	Events::EventMOId OnRemoveObject;
	void UpdateHierarchy(const std::vector<Utils::MapObjectBasicData>& Objects);

	Events::EventProps OnPropertiesChanged;
	void UpdateProperties(const Utils::MapObjectProperties& Object);

	bool IsUsingUI() const { return bIsUsingUI; }
	Events::EventBool OnUsingUI;
};
