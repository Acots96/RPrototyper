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


#include "Utils.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <rlgl.h>


Color Utils::LIGHTLIGHTGRAY = { 225,225,225,255 };


void Utils::UGuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color)
{
	GuiDrawRectangle(rec, borderWidth, borderColor, color);
}


void Utils::UGuiDrawText(const char* text, Rectangle textBounds, int alignment, Color tint)
{
	GuiDrawText(text, textBounds, alignment, tint);
}


int Utils::UGuiScrollBar(Rectangle bounds, int value, int minValue, int maxValue)
{
	return GuiScrollBar(bounds, value, minValue, maxValue);
}


bool Utils::UIsPointInRect(Vector2 Point, Rectangle Rect)
{
	return Point.x >= Rect.x && Point.x <= Rect.x + Rect.width
		&& Point.y >= Rect.y && Point.y <= Rect.y + Rect.height;
}


bool Utils::UIsR1ColllidingWithR2(Rectangle R1, Rectangle R2)
{
	return false;
}


std::vector<Vector3> Utils::UComputeSemicircle3D(Vector3 center, float radius,
	Vector3 startDir, Vector3 endDir, int segments, Color color)
{
	Vector3 normal{ Vector3CrossProduct(startDir, endDir) };
	float angle{ Vector3Angle(startDir, endDir) };
	float segmentAngle{ angle / segments };
	// Obtener un vector en el plano perpendicular a "normal"
	// mediante el producto cruzado
	Vector3 perp1 = startDir * radius;
	// Obtener el segundo vector perpendicular
	Vector3 perp2 = endDir * radius;
	float startAngle{ 0 };
	float endAngle{ 90 };

	std::vector<Vector3> points{};

	float rads{ startAngle * DEG2RAD };
	float sin{ sinf(rads) };
	float cos{ cosf(rads) };
	Vector3 prev{ Vector3Add(
		Vector3Scale(perp1, cos), Vector3Scale(perp2, sin)) };
	points.push_back(center + prev);
	for (float angle = startAngle + segmentAngle; angle <= endAngle; angle += segmentAngle)
	{
		rads = angle * DEG2RAD;
		sin = sinf(rads);
		cos = cosf(rads);
		Vector3 next = Vector3Add(
			Vector3Scale(perp1, cos), Vector3Scale(perp2, sin));
		//DrawLine3D(center + prev, center + next, color);
		points.push_back(center + next);
		prev = next;
	}
	return points;
}
