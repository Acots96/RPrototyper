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
