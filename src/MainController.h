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
