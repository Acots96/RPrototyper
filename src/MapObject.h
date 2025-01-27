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

#include "Utils.h"

#include "ThirdParty/nlohmann/json.hpp"

#include <vector>
#include <string>


enum class MapObjectType { Root, Empty, Sphere, Cube, Model };


class MapObject
{
	Utils::MapObjectId Id;

	std::string Name;
	std::string Tags;
	MapObjectType ObjType;

	Utils::MapTransform LocalTransform;
	Utils::MapTransform GlobalTransform;
	MapObject* Parent;
	std::vector<MapObject*> Children;
	void UpdateGlobalTransform();
	void UpdateLocalTransform();
	void UpdateModelTransform();

	int ModelIdx;
	Model ModelObj;
	int TexDiffuseIdx;

	int ComputeLevel() const;

public:
	int Level;

	MapObject(MapObjectType Type, std::string Name = "",
		Vector3 Position = Vector3Zeros,
		Utils::Rotator Rotator = {},
		Vector3 Scale = Vector3Ones,
		std::string Tags = "");

	MapObject(const MapObject& Other) = default;
	MapObject& operator=(const MapObject& Other) = default;

	MapObject(MapObject&& Other) noexcept = default;
	MapObject& operator=(MapObject&& Other) noexcept = default;

	operator Utils::MapObjectBasicData() const;
	operator Utils::MapObjectProperties() const;

	const Utils::MapObjectId& GetId() const { return Id; }
	const int GetModelId() const { return ModelIdx; }
	const int GetTexDiffuseId() const { return TexDiffuseIdx; }

	Vector3 GetLocalPosition() const { return LocalTransform.Position; }
	void SetLocalPosition(Vector3 Position) { LocalTransform.Position = Position; UpdateGlobalTransform(); }
	Utils::Rotator GetLocalRotation() const { return LocalTransform.Rotation; }
	void SetLocalRotation(Utils::Rotator::EulerAngle Angle, float Value) 
	{ LocalTransform.Rotation.SetEulerAngle(Angle, Value); UpdateGlobalTransform(); }
	void AddLocalRotation(Utils::Rotator::EulerAngle Angle, float Value)
	{ LocalTransform.Rotation.AddEulerAngle(Angle, Value); UpdateGlobalTransform(); }
	void SetLocalQuat(Quaternion Quat) { LocalTransform.Rotation.SetQuat(Quat); UpdateGlobalTransform(); }
	Vector3 GetLocalScale() const { return LocalTransform.Scale; }
	void SetLocalScale(Vector3 Scale) { LocalTransform.Scale = Scale; UpdateGlobalTransform(); }

	Vector3 GetGlobalPosition() const { return GlobalTransform.Position; }
	void SetGlobalPosition(Vector3 Position) { GlobalTransform.Position = Position; UpdateLocalTransform(); }
	Utils::Rotator GetGlobalRotation() const { return GlobalTransform.Rotation; }
	void SetGlobalRotation(Utils::Rotator::EulerAngle Angle, float Value) 
	{ GlobalTransform.Rotation.SetEulerAngle(Angle, Value); UpdateLocalTransform(); }
	void AddGlobalRotation(Utils::Rotator::EulerAngle Angle, float Value)
	{ GlobalTransform.Rotation.AddEulerAngle(Angle, Value); UpdateLocalTransform(); }
	void SetGlobalQuat(Quaternion Quat) { GlobalTransform.Rotation.SetQuat(Quat); UpdateLocalTransform(); }
	Vector3 GetGlobalScale() const { return GlobalTransform.Scale; }
	void SetGlobalScale(Vector3 Scale) { GlobalTransform.Scale = Scale; UpdateLocalTransform(); }

	const MapObject* GetParent() const { return Parent; }
	const std::vector<MapObject*>& GetChildren() const { return Children; }
	void UpdateParent(MapObject* NewParent);

	//~MapObject()

	void MoveChildrenTo(MapObject* NewParent);
	void AddNewChild(MapObject* NewChild);

	void Draw() const;

	void SetProperties(const Utils::MapObjectProperties& Props);
	void SetModel(Model Model, int ModelIdx);
	void SetTexture(Texture2D Texture, int TexIdx);
};
