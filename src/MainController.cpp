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
