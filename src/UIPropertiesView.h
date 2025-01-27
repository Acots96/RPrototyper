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
#include "UIWindow.h"


struct XYZKeyVal
{
	std::string XKey, YKey, ZKey;
	Vector3 PrevValues, Values;
	std::string XVal, YVal, ZVal;
	bool XChange, YChange, ZChange;

	XYZKeyVal(std::string X, std::string Y, std::string Z) :
		XKey{ X }, YKey{ Y }, ZKey{ Z },
		PrevValues{}, Values{},
		XChange{ false }, YChange{ false }, ZChange{ false } {}

	void Reset(Vector3 V)
	{
		PrevValues = Values = V;
		XVal = std::to_string(V.x); CleanVal(XVal);
		YVal = std::to_string(V.y); CleanVal(YVal);
		ZVal = std::to_string(V.z); CleanVal(ZVal);
	}
	void UpdateValues()
	{
		Values.x = std::stof(XVal);
		Values.y = std::stof(YVal);
		Values.z = std::stof(ZVal);
	}
	void CleanVal(std::string& Val)
	{
		// 0.0000
		// 0.1000
		// 0.0101
		auto dot = Val.find('.');
		if (dot != std::string::npos)
		{
			auto z = Val.find_last_of('0');
			auto nz = Val.find_last_not_of('0');
			if (z > nz)
				Val.erase(nz == dot ? nz + 2 : nz + 1, z);
		}
	}
};


class UIPropertiesView : public UIWindow
{
	const float FieldHeight{ 26 }, BoxHeight{ 22 };

	std::string ModelsNames;
	std::string TexturesNames;

	Utils::MapObjectProperties Props;

	bool bPropName, bPropTags;
	XYZKeyVal PropPos, PropRot, PropSca;
	bool bListView, bShowModels, bShowTexturesDif;
	int ListScrollIdx, ListActiveIdx;

	Rectangle AuxRect;

	void DrawProperties();
	float DrawNameTags(const Rectangle& PrntRect, float DeltaY);
	float DrawTransform(const Rectangle& PrntRect, float DeltaY);
	float DrawPosition(const Rectangle& PrntRect, float DeltaY);
	float DrawRotation(const Rectangle& PrntRect, float DeltaY);
	float DrawScale(const Rectangle& PrntRect, float DeltaY);
	float DrawTextures(const Rectangle& PrntRect, float DeltaY);

	float DrawVector3(const std::string& Name, XYZKeyVal& XYZ,
		const Rectangle& PrntRect, float DeltaY);

	void ChangeProperty(bool& PropBool);
	std::string ValidateValue(const std::string Value) const;

public:
	UIPropertiesView(Rectangle Rect);

	void Draw() override;

	void Update(const Utils::MapObjectProperties& Properties);

	void SetModelsNames(const std::string& Models) { ModelsNames = Models; }
	void SetTexturesNames(const std::string& Textures) { TexturesNames = Textures; }

	Events::EventProps OnPropertiesChanged;

	virtual bool IsMouseInWindow() const override;
};
