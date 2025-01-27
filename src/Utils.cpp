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
