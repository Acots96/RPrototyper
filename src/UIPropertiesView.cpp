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


#include "UIPropertiesView.h"
#include "Utils.h"
#include <iostream>

using namespace Utils;


UIPropertiesView::UIPropertiesView(Rectangle Rect) :
	Props{}, bPropName{ false }, bPropTags{ false },
	PropPos{ "X", "Y", "Z" }, PropRot{ "P", "Y", "R" }, PropSca{ "X", "Y", "Z" },
	bListView{ false }, ListActiveIdx{}, ListScrollIdx{},
	bShowModels{ false }, bShowTexturesDif{ false },
	ModelsNames{}, TexturesNames{},
	UIWindow(Rect, "Properties")
{
}


void UIPropertiesView::DrawProperties()
{
	if (!Props.Id.IsEmpty())
	{
		float y = TitleR.y + TitleR.height;
		y += DrawNameTags(Rect, y - 1) - 1;
		y += DrawTransform(Rect, y - 1) - 1;
		y += DrawTextures(Rect, y - 1);
	}
}


float UIPropertiesView::DrawNameTags(const Rectangle& PrntRect, float DeltaY)
{
	float boxYincr{ (FieldHeight - BoxHeight) / 2 };
	float y{ DeltaY }, x{ PrntRect.x + 40 }, 
		w{ PrntRect.x + PrntRect.width - boxYincr * 2 - x };

	// Name
	y += boxYincr;
	UGuiDrawText("Name ", { PrntRect.x + 5, y, 50, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	y += boxYincr;
	if (GuiTextBox({ x, y, w, BoxHeight }, Props.Name.data(), 8, bPropName))
		ChangeProperty(bPropName);

	// Tags
	y += FieldHeight;
	UGuiDrawText("Tags ", { PrntRect.x + 5, y, 50, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	if (GuiTextBox({ x, y, w, BoxHeight }, Props.Tags.data(), 8, bPropTags))
		ChangeProperty(bPropTags);
	if (UIsPointInRect(GetMousePosition(), PrntRect))
	{
		std::string s{ "Split ';'" };
		float w{ s.size() * 7.f };
		Rectangle r{ PrntRect.x - w, y, w, FieldHeight - 4 };
		//UGuiDrawRectangle(r, 1, BLANK, { 230, 230, 230, 200 });
		//r.x -= 5;
		//UGuiDrawText(s.c_str(), r, TEXT_ALIGN_RIGHT, BLACK);
		GuiDummyRec(r, s.c_str());
	}

	// Rect
	float h{ FieldHeight * 2 + boxYincr * 2 };
	UGuiDrawRectangle({ PrntRect.x, DeltaY, PrntRect.width, h }, 1, DARKGRAY, BLANK);
	//GuiDummyRec({ PrntRect.x, DeltaY, PrntRect.width, h }, "");
	return h;
}


float UIPropertiesView::DrawTransform(const Rectangle& PrntRect, float DeltaY)
{
	float offsetY{ (FieldHeight - BoxHeight) / 2 };
	float y{ DeltaY }, height{ 0 };
	height += DrawPosition(PrntRect, y + offsetY);
	height += DrawRotation(PrntRect, y + height);
	height += DrawScale(PrntRect, y + height) + offsetY;
	UGuiDrawRectangle({ PrntRect.x, y, PrntRect.width, height }, 1, DARKGRAY, BLANK);
	//GuiDummyRec({ PrntRect.x, y, PrntRect.width, height }, "");
	return height;
}


float UIPropertiesView::DrawPosition(const Rectangle& PrntRect, float DeltaY)
{
	return DrawVector3("Position ", PropPos, PrntRect, DeltaY);
}

float UIPropertiesView::DrawRotation(const Rectangle& PrntRect, float DeltaY)
{
	auto h = DrawVector3("Rotation ", PropRot, PrntRect, DeltaY);
	if (UIsPointInRect(GetMousePosition(), PrntRect))
	{
		std::string s{ "Pitch/Yaw/Roll" };
		float w{ s.size() * 7.f };
		Rectangle r{ PrntRect.x - w, DeltaY + 2, w, FieldHeight - 4 };
		//UGuiDrawRectangle(r, 1, BLANK, {230, 230, 230, 200});
		//r.x -= 5;
		//UGuiDrawText(s.c_str(), r, TEXT_ALIGN_RIGHT, BLACK);
		GuiDummyRec(r, s.c_str());
	}
	return h;
}

float UIPropertiesView::DrawScale(const Rectangle& PrntRect, float DeltaY)
{
	return DrawVector3("Scale: ", PropSca, PrntRect, DeltaY);
}


float UIPropertiesView::DrawTextures(const Rectangle& PrntRect, float DeltaY)
{
	float boxYincr{ (FieldHeight - BoxHeight) / 2 };
	float y{ DeltaY }, x{ PrntRect.x + boxYincr * 2 },
		w{ PrntRect.x + PrntRect.width - boxYincr * 2 - x };

	// Name
	//y += boxYincr;
	//UGuiDrawText("Model ", { PrntRect.x + 5, y, 60, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	//GuiDrawIcon(ICON_BOX_CENTER, PrntRect.x + 35, y + boxYincr * 2, 1, DARKGRAY);
	//if (GuiButton({ PrntRect.x + 38, y + FieldHeight / 2 - 5, 10, 10 }, ""))
	if (GuiButton({ x, y + boxYincr * 2, 40, 20 }, "Model"))
	{
		bShowModels = !bShowModels;
	}
	if (bShowModels)
	{
		//auto m = GuiListView({}, ModelsNames, );
		AuxRect = { GetScreenWidth() / 2.0f - 80, GetScreenHeight() / 2.0f - 100, 160, 300 };
		//if (GuiDropdownBox(r, ModelsNames.c_str(), &DropDownSelected, bDropDown))
			//bDropDown = !bDropDown;
		int a = ListActiveIdx;
		GuiListView(AuxRect, ModelsNames.c_str(), &ListScrollIdx, &ListActiveIdx);
		if (a != ListActiveIdx)
		{
			if (ListActiveIdx == -1) ListActiveIdx = a;
			bShowModels = false;
			if (ListActiveIdx != a)
			{
				Props.ModelIdx = ListActiveIdx;
				if (OnPropertiesChanged) OnPropertiesChanged(Props);
			}
		}
		//bListView = !bListView;
		std::cout << ListActiveIdx << "\n";
	}

	x += 40 + 10;

	if (GuiButton({ x, y + boxYincr * 2, 40, 20 }, "TexDiffuse"))
	{
		bShowTexturesDif = !bShowTexturesDif;
	}
	if (bShowTexturesDif)
	{
		//auto m = GuiListView({}, ModelsNames, );
		AuxRect = { GetScreenWidth() / 2.0f - 80, GetScreenHeight() / 2.0f - 100, 160, 300 };
		//if (GuiDropdownBox(r, ModelsNames.c_str(), &DropDownSelected, bDropDown))
			//bDropDown = !bDropDown;
		int a = ListActiveIdx;
		GuiListView(AuxRect, TexturesNames.c_str(), &ListScrollIdx, &ListActiveIdx);
		if (a != ListActiveIdx)
		{
			if (ListActiveIdx == -1) ListActiveIdx = a;
			bShowTexturesDif = false;
			if (ListActiveIdx != a)
			{
				Props.TexDiffuseIdx = ListActiveIdx;
				if (OnPropertiesChanged) OnPropertiesChanged(Props);
			}
		}
		//bListView = !bListView;
		std::cout << ListActiveIdx << "\n";
	}

	y += boxYincr;

	return FieldHeight;
}


float UIPropertiesView::DrawVector3(const std::string& Name, 
	XYZKeyVal& XYZ, const Rectangle& PrntRect, float DeltaY)
{
	float y{ DeltaY }, boxYincr{ (FieldHeight - BoxHeight) / 2 };

	UGuiDrawRectangle({ PrntRect.x, y, PrntRect.width, FieldHeight }, 0, DARKGRAY, RAYWHITE);
	//GuiDummyRec({ PrntRect.x, y, PrntRect.width, FieldHeight }, "");
	UGuiDrawText(Name.c_str(), {PrntRect.x + 5, y, 50, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);

	float nameW{ 50 }, fieldX{ PrntRect.x + nameW },
		fieldW{ (PrntRect.width - nameW - boxYincr * 2) / 3 },
		boxW{ fieldW - 20 };

	// X
	UGuiDrawText(XYZ.XKey.c_str(), { fieldX + 10, y, 30, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	if (GuiTextBox({ fieldX + 20, y + boxYincr, boxW, BoxHeight }, XYZ.XVal.data(), 8, XYZ.XChange))
	{
		XYZ.XVal = ValidateValue(XYZ.XVal);
		XYZ.UpdateValues();
		ChangeProperty(XYZ.XChange);
	}

	// Y
	fieldX += fieldW;
	UGuiDrawText(XYZ.YKey.c_str(), { fieldX + 10, y, 30, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	if (GuiTextBox({ fieldX + 20, y + boxYincr, boxW, BoxHeight }, XYZ.YVal.data(), 8, XYZ.YChange))
	{
		XYZ.YVal = ValidateValue(XYZ.YVal);
		XYZ.UpdateValues();
		ChangeProperty(XYZ.YChange);
	}

	// Z
	fieldX += fieldW;
	UGuiDrawText(XYZ.ZKey.c_str(), { fieldX + 10, y, 30, FieldHeight }, GuiTextAlignment::TEXT_ALIGN_LEFT, BLACK);
	if (GuiTextBox({ fieldX + 20, y + boxYincr, boxW, BoxHeight }, XYZ.ZVal.data(), 8, XYZ.ZChange))
	{
		XYZ.ZVal = ValidateValue(XYZ.ZVal);
		XYZ.UpdateValues();
		ChangeProperty(XYZ.ZChange);
	}

	return FieldHeight;
}


void UIPropertiesView::ChangeProperty(bool& PropBool)
{
	if (PropBool)
	{
		if (OnPropertiesChanged)
		{
			Props.Transform.Position = PropPos.Values;
			if (PropRot.XChange || PropRot.YChange || PropRot.ZChange)
			{
				auto& rot = Props.Transform.Rotation;
				if (PropRot.XChange)
				{
					rot.SetEulerAngle(Rotator::EulerAngle::Pitch, PropRot.Values.x);
					//PropRot.PrevValues.x = PropRot.Values.x;
				}
				else if (PropRot.YChange)
				{
					rot.SetEulerAngle(Rotator::EulerAngle::Yaw, PropRot.Values.y);
					//PropRot.PrevValues.y = PropRot.Values.y;
				}
				else
				{
					rot.SetEulerAngle(Rotator::EulerAngle::Roll, PropRot.Values.z);
					//PropRot.PrevValues.z = PropRot.Values.z;
				}
			}
			Props.Transform.Scale = PropSca.Values;
			OnPropertiesChanged(Props);
		}
	}
	PropBool = !PropBool;
}


std::string UIPropertiesView::ValidateValue(const std::string Value) const
{
	auto str = Value.c_str();
	int len = (int)strlen(str);

	// TODO
	// Deberia guardar el valor anterior y devolverlo en lugar de 0
	if (str == nullptr || len <= 0) return {"0.0"};

	std::string tmp{};
	std::string nonDigits{ ".-" };
	// TODO
	// "Value" contains the correct string, but size and chars are outdated,
	// like the char* is modified but not the string...
	for (int i = 0; i < len; i++)
	{
		auto& ch = str[i];
		if (std::isdigit(ch))
		{
			tmp.append(std::string{ ch });
		}
		else
		{
			auto idx = nonDigits.find(ch);
			if (idx != std::string::npos)
			{
				tmp.append(std::string{ ch });
				nonDigits.erase(idx, 1);
			}
		}
	}
	return tmp;
}


void UIPropertiesView::Draw()
{
	UIWindow::Draw();
	DrawProperties();
}


void UIPropertiesView::Update(const Utils::MapObjectProperties& Properties)
{
	Props = Properties;
	auto t = Props.Transform;
	PropPos.Reset(Props.Transform.Position);
	PropRot.Reset(Props.Transform.Rotation.Euler);
	PropSca.Reset(Props.Transform.Scale);
}

bool UIPropertiesView::IsMouseInWindow() const
{
	return UIWindow::IsMouseInWindow()
		|| (bShowModels || bShowTexturesDif)
		&& UIsPointInRect(GetMousePosition(), AuxRect);
}
