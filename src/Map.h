#pragma once


#include "raylib.h"
#include "MapObject.h"
#include <vector>
#include <functional>
#include "Utils.h"
#include "Events.h"


enum class Action { None, Drag, Rotate, Scale };
enum class ChangeDirection { None, X, Y, Z, XY, XZ, YZ, XYZ };


class Map
{
	int ScreenW, ScreenH;

	Camera Cam, OverCam;
	void UpdateCam();

	RenderTexture RenderTex, OverRenderTex;

	std::unique_ptr<MapObject> Root;
	std::vector<std::unique_ptr<MapObject>> Objects;
	MapObject* GetLast() const { return Objects[Objects.size() - 1].get(); }

	MapObject* ObjSelected;
	double LastSelectedTime;

	// Drag
	Vector3 InitPos, Offset;

	// Rotate
	Vector3 InitDir, PlanePoint, PlaneNormal;
	float InitAngle;
	Quaternion InitQuat;

	// Scale
	float InitDist;
	Vector3 InitScale, IncrXYZ;

	Action CurrentAction; 
	ChangeDirection ChangeDir;
	bool bIsPerformingAction;
	void ComputeDrag();
	void ComputeRotate();
	void ComputeScale();
	void DrawMiniArrows();
	void DrawOverCam();

	MapObject* Map::Find(MapObject& Parent, 
		const Utils::MapObjectId& TargetId) const;

	void TraverseDFS(MapObject& Parent, 
		std::function<void(MapObject&)>& Func) const;

	bool bIsMapLocked;

public:
	Map();

	void Update();
	void Draw();

	const Utils::MapObjectProperties AddMapObject(
		std::string Name, MapObjectType Type, Utils::MapObjectId ParentId, 
		Model Model = {}, int ModelIdx = -1);
	
	void GetBasicDatas(std::vector<Utils::MapObjectBasicData>& Out) const;
	const Utils::MapObjectProperties GetProperties(const Utils::MapObjectId& Id) const;
	const Utils::MapObjectProperties SelectObject(const Utils::MapObjectId& Id);
	void SetProperties(const Utils::MapObjectProperties& Props);

	void LockMap(bool Lock) { bIsMapLocked = Lock; }

	Events::EventProps OnTransformChanged;
};
