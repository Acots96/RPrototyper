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

#include "raygui.h"
#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include "Events.h"
#include "Utils.h"


enum class Direction { None = 0, Left, Right, Top, Bottom };


class UIWindow
{
protected:
	float MinWidth{ 200 }, MinHeight{ 200 };

	float ScreenWidth;
	float ScreenHeight;

	Rectangle Rect, TitleR;
	std::string Name;

	int MouseCursor;
	Vector2 DragOffset;
	bool bIsDragging;
	virtual void ComputeDragging();

	Direction CanResizeDir;
	bool bIsResizing;
	virtual void ComputeResizing();

	bool bIsUsingUI;
	virtual void CheckUsingUI(bool Using);

public:
	UIWindow(Rectangle Rect, std::string Name);
	virtual ~UIWindow() = default;

	virtual void Draw();

	bool IsUsingUI() const { return bIsUsingUI; }
	Events::EventBool OnUsingUI;

	virtual bool IsMouseInWindow() const;
};
