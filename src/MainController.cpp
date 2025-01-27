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


#include "MainController.h"
//#include "raygui.h"
#include "Utils.h"
#include "ResourcesManager.h"


using namespace Utils;


MainController::MainController() :
	PMap{}, UICtrl{}
{
	// Resources
	ResourcesManager::LoadResources();

	std::vector<std::string> models{ ResourcesManager::GetModelsNames() };
	models.insert(models.begin(), "Empty");
	UICtrl.InitResourcesNames(models,
		ResourcesManager::GetTexturesNames());
	UICtrl.OnHierarchyObjectClicked =
		[this](const MapObjectId& Id) { this->HierarchyObjectClicked(Id); };
	UICtrl.OnAddChildToObject =
		[this](const MapObjectId& Id) { this->AddChildToObject(Id); };
	UICtrl.OnRemoveObject =
		[this](const MapObjectId& Id) { this->RemoveObject(Id); };
	UICtrl.OnPropertiesChanged =
		[this](const MapObjectProperties& Ps) { this->PropertiesChanged(Ps); };
	UICtrl.OnUsingUI = 
		[this](bool IsUsing) { this->IsUsingUI(IsUsing); };

	PMap.OnTransformChanged = 
		[this](const MapObjectProperties& Ps) { this->TransformChanged(Ps); };

	AddChildToObject(MapObjectId::Empty());

	std::vector<MapObjectBasicData> datas{};
	PMap.GetBasicDatas(datas);
	UICtrl.UpdateHierarchy(datas);
}


MainController::~MainController()
{
	ResourcesManager::UnloadResources();
}


void MainController::UpdateAndDraw()
{
	PMap.LockMap(UICtrl.IsUsingUI());
	PMap.Update();

	BeginDrawing();
		PMap.Draw();
		UICtrl.Draw();
	EndDrawing();
}


void MainController::HierarchyObjectClicked(const MapObjectId& Id)
{
	auto& ps = PMap.SelectObject(Id);
	UICtrl.UpdateProperties(ps);
}

void MainController::AddChildToObject(const MapObjectId& Id)
{
	auto& ps = PMap.AddMapObject("Empty_Object", MapObjectType::Empty, Id);
	std::vector<MapObjectBasicData> datas{};
	PMap.GetBasicDatas(datas);
	UICtrl.UpdateHierarchy(datas);
}

void MainController::RemoveObject(const MapObjectId& Id)
{
}


// Called from UI
void MainController::AllPropertiesChanged(const Utils::MapObjectProperties& Props)
{
	PMap.SetProperties(Props);
	std::vector<MapObjectBasicData> datas{};
	PMap.GetBasicDatas(datas);
	UICtrl.UpdateHierarchy(datas);
}

// Called from UI
void MainController::PropertiesChanged(const Utils::MapObjectProperties& Props)
{
	PMap.SetProperties(Props);
	UICtrl.UpdateProperties(PMap.GetProperties(Props.Id));
}

// Called from map
void MainController::TransformChanged(const Utils::MapObjectProperties& Props)
{
	UICtrl.UpdateProperties(Props);
}


void MainController::AddModelSelected(int Idx)
{
	auto m = ResourcesManager::GetInstancedModel(Idx);
	auto n = ResourcesManager::GetModelName(Idx);
	auto t = ResourcesManager::GetTexture(0);
	//m.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = t;
	//MapObject obj{ n, m };
	//obj.SetTexture(t);
	PMap.AddMapObject(n, MapObjectType::Model, MapObjectId::Empty(), m, Idx);
	std::vector<MapObjectBasicData> datas{};
	PMap.GetBasicDatas(datas);
	UICtrl.UpdateHierarchy(datas);
}


void MainController::IsUsingUI(bool IsUsing)
{
	PMap.LockMap(IsUsing);
}
