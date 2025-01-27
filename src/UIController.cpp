#include "UIController.h"
#include "raygui.h"
#include "Utils.h"
#include <iostream>

using namespace Utils;


UIController::UIController() :
	ScreenWidth{ (float)GetScreenWidth() }, ScreenHeight{ (float)GetScreenHeight() },
	bIsAddModelOpen{ false }, bIsUsingUI{ false },
	AddModelScrollIdx{ 0 }, AddModelActiveIdx{ -1 }
{
	GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
	GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(WHITE));
	GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(LIGHTGRAY));

	Rectangle h{ ScreenWidth * 0.75f, ScreenHeight * 0.1f, 
		ScreenWidth * 0.25f, ScreenHeight * 0.6f };
	Rectangle p{ ScreenWidth * 0.7f, ScreenHeight * 0.7f, 
		ScreenWidth * 0.3f, ScreenHeight * 0.3f };
	Windows.clear();
	Windows.push_back(std::make_unique<UIHierarchyView>(h));
	Windows.push_back(std::make_unique<UIPropertiesView>(p));

	Hierarchy = static_cast<UIHierarchyView*>(Windows[0].get());
	Properties = static_cast<UIPropertiesView*>(Windows[1].get());

	Hierarchy->OnClicked = 
		[this](const MapObjectId& Id) { this->ObjectClicked(Id); };
	Hierarchy->OnAddChild =
		[this](const MapObjectId& Id) { this->AddChildToObject(Id); };
	Hierarchy->OnRemove =
		[this](const MapObjectId& Id) { this->RemoveObject(Id); };
	Properties->OnPropertiesChanged =
		[this](const MapObjectProperties& Ps) { this->PropertiesChanged(Ps); };
}


void UIController::DrawTopButtons()
{
	/*// Add model button
	Rectangle r{ 10, 10, 20, 20 };
	if (GuiButton(r, "") || bIsAddModelOpen)
	{
		CheckUsingUI(true);
		bIsAddModelOpen = true;
		Rectangle rd{ r.x, r.y + r.height, 200, ScreenHeight * 0.25f };
		//if (GuiDropdownBox(rd, ModelsPaths.c_str(), &AddModelDropDown, true))
			//IsModelsDropDownOpen = true;
		GuiListView(rd, ModelsPathsStr.c_str(), &AddModelScrollIdx, &AddModelActiveIdx);
		//std::cout << "MODELS: " << AddModelActiveIdx << "\n";
		if (AddModelActiveIdx > -1)
		{
			OnModelSelected(AddModelActiveIdx);
			CheckUsingUI(false);
			bIsAddModelOpen = false;
		}
	}
	UGuiDrawRectangle(r, 0, DARKGRAY, WHITE);
	GuiDrawIcon(ICON_CUBE, r.x + r.width * 0.1f, r.y + r.height * 0.1f, 1, DARKGRAY);*/
}


void UIController::ObjectClicked(const MapObjectId& Id) const
{
	if (OnHierarchyObjectClicked) OnHierarchyObjectClicked(Id);
}

void UIController::AddChildToObject(const MapObjectId& Id) const
{
	if (OnAddChildToObject) OnAddChildToObject(Id);
}

void UIController::RemoveObject(const MapObjectId& Id) const
{
	if (OnRemoveObject) OnRemoveObject(Id);
}

void UIController::PropertiesChanged(const Utils::MapObjectProperties& Props) const
{
	if (OnPropertiesChanged) OnPropertiesChanged(Props);
}


void UIController::CheckUsingUI(bool Using)
{
	if (bIsUsingUI != Using)
	{
		bIsUsingUI = Using;
		OnUsingUI(Using);
	}
}


void UIController::InitResourcesNames(const std::vector<std::string>& ModelsPaths, const std::vector<std::string>& TexturesPaths)
{
	ModelsPathsStr.clear();
	for (auto& m : ModelsPaths)
		ModelsPathsStr += m + ";";
	if (!ModelsPathsStr.empty()) ModelsPathsStr.pop_back();

	TexturesPathsStr.clear();
	for (auto& t : TexturesPaths)
		TexturesPathsStr += t + ";";
	if (!TexturesPathsStr.empty()) TexturesPathsStr.pop_back();

	Properties->SetModelsNames(ModelsPathsStr);
	Properties->SetTexturesNames(TexturesPathsStr);
}


void UIController::Draw()
{
	ScreenWidth = GetScreenWidth();
	ScreenHeight = GetScreenHeight();

	bIsUsingUI = false;

	DrawTopButtons();
	for (auto& wp : Windows)
	{
		auto& w = *wp;
		w.Draw();
		bIsUsingUI |= w.IsUsingUI();
	}
	
	if (!bIsUsingUI)
	{
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}
}


void UIController::UpdateHierarchy(
	const std::vector<Utils::MapObjectBasicData>& Objects)
{
	Hierarchy->UpdateObjects(Objects);
}


void UIController::UpdateProperties(const MapObjectProperties& Object)
{
	Properties->Update(Object);
}
