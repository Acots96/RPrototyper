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


#include "UIWindow.h"
#include "Utils.h"
#include <iostream>

using namespace Utils;


UIWindow::UIWindow(Rectangle Rect, std::string Name) : 
	Rect{ Rect }, Name{ Name }, bIsUsingUI{ false },
	ScreenWidth{ 0 }, ScreenHeight{ 0 },
	bIsDragging{ false }, DragOffset{},
	CanResizeDir{ Direction::None }, bIsResizing{ false },
	MouseCursor{ MOUSE_CURSOR_DEFAULT },
	TitleR{ Rect.x, Rect.y, Rect.width, 26 }
{
}


void UIWindow::ComputeDragging()
{
	auto p = GetMousePosition();
	Rectangle r{
		TitleR.x, TitleR.y + 2,
		std::max(TitleR.width * 0.75f, 75.f),
		TitleR.height - 2
	};
	if (UIsPointInRect(p, r))
	{
		if (!bIsDragging && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			bIsDragging = true;
			DragOffset = { Rect.x - p.x, Rect.y - p.y };
			std::cout << "DRAGGING!" << "\n";
		}
	}
	if (bIsDragging)
	{
		Rect.x = p.x + DragOffset.x;
		Rect.y = p.y + DragOffset.y;
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			bIsDragging = false;
	}
}


void UIWindow::ComputeResizing()
{
	auto p = GetMousePosition();

	if (!bIsResizing)
	{
		bool horizontal{ p.y > Rect.y && p.y < Rect.y + Rect.height };
		bool vertical{ p.x > Rect.x && p.x < Rect.x + Rect.width };
		bool left{ p.x >= Rect.x && p.x <= Rect.x + 2 };
		bool right{ p.x >= Rect.x + Rect.width - 2 && p.x <= Rect.x + Rect.width};
		bool top{ p.y >= Rect.y - 1 && p.y <= Rect.y + 1 };
		bool bottom{ p.y >= Rect.y + Rect.height - 2 && p.y <= Rect.y + Rect.height };

		if (horizontal && left) CanResizeDir = Direction::Left;
		else if (horizontal && right) CanResizeDir = Direction::Right;
		else if (vertical && top) CanResizeDir = Direction::Top;
		else if (vertical && bottom) CanResizeDir = Direction::Bottom;
		else CanResizeDir = Direction::None;
		MouseCursor =
			CanResizeDir == Direction::Left || CanResizeDir == Direction::Right ?
			MOUSE_CURSOR_RESIZE_EW : MOUSE_CURSOR_RESIZE_NS;
	}
	else
	{
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			bIsResizing = false;
			CanResizeDir = Direction::None;
			//Rect.x = ScreenWidth - Rect.width;
			//Rect.y = ScreenHeight - Rect.height;
		}
		else
		{
			//std::cout << "RESIZING! - " << CanResizeDir << "\n";
			float xMax = Rect.x + Rect.width;
			float yMax = Rect.y + Rect.height;
			float xLimit = xMax - MinWidth;
			float yLimit = yMax - MinHeight;
			switch (CanResizeDir)
			{
				case Direction::None:
					break;

				case Direction::Left:
					Rect.x = std::min(xLimit, p.x);
					Rect.width = std::max(MinWidth, xMax - Rect.x);
					break;

				case Direction::Right:
					Rect.width = std::max(MinWidth, p.x - Rect.x);
					break;

				case Direction::Top:
					Rect.y = std::min(yLimit, p.y);
					Rect.height = std::max(MinHeight, yMax - Rect.y);
					break;

				case Direction::Bottom:
					Rect.height = std::max(MinHeight, p.y - Rect.y);
					break;

				default:
					break;
			}
		}
	}

	//std::cout << "CANRESIZE " << bCanResize << "\n";
	if (CanResizeDir == Direction::None)
	{
		SetMouseCursor(MOUSE_CURSOR_ARROW);
	}
	else
	{
		SetMouseCursor(MouseCursor);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			bIsResizing = true;
	}
}


void UIWindow::CheckUsingUI(bool Using)
{
	if (bIsUsingUI != Using)
	{
		bIsUsingUI = Using;
		OnUsingUI(Using);
	}
}


void UIWindow::Draw()
{
	ScreenWidth = GetScreenWidth();
	ScreenHeight = GetScreenHeight();

	if (bIsDragging || bIsResizing ||
		UIsPointInRect(GetMousePosition(), Rect))
	{
		if (!bIsResizing) ComputeDragging();
		if (!bIsDragging) ComputeResizing();
	}

	Rect.x = Clamp(Rect.x, 0, ScreenWidth - 50);
	Rect.y = Clamp(Rect.y, 0, ScreenHeight - 50);

	TitleR = { Rect }; TitleR.height = 26;
	GuiPanel(Rect, Name.c_str());
}


bool UIWindow::IsMouseInWindow() const
{
	return bIsDragging || bIsResizing
		|| Utils::UIsPointInRect(GetMousePosition(), Rect);
}
