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
