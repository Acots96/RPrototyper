#include "MapObject.h"

//using namespace RPrototyper;


MapObject::MapObject(MapObjectType Type, std::string Name,
	Vector3 Position, Utils::Rotator Euler, 
	Vector3 Scale, std::string Tags) :
	GlobalTransform{ Position, Euler, Scale },
	LocalTransform{ Position, Euler, Scale },
	Name{ Name }, Tags{ Tags }, ObjType{ Type },
	ModelIdx{ -1 }, ModelObj{}, TexDiffuseIdx{ -1 },
	Parent{ nullptr }, Id{ Utils::MapObjectId::GenerateNewId() },
	Level{ ComputeLevel() }
{
}


/*MapObject& MapObject::operator=(const MapObject& Other)
{
	if (this == &Other) return *this;
}*/


/*MapObject::MapObject(const MapObject& Other) :
	MapTransform{ Other.MapTransform }, Name{ Other.Name },
	Tags{ Other.Tags }, ModelObj{ Other.ModelObj }, 
	ObjType{ Other.ObjType }, Parent{ Other.Parent }, Id{ Other.Id }
{
}*/


MapObject::operator Utils::MapObjectBasicData() const
{
	return { Id, Level, Name };
}

MapObject::operator Utils::MapObjectProperties() const
{
	return { Id, Name, LocalTransform, Tags, ModelIdx, TexDiffuseIdx };
}


void MapObject::UpdateGlobalTransform()
{
	auto& g = GlobalTransform;
	auto& l = LocalTransform;
	auto& pg = Parent->GlobalTransform;
	auto& pl = Parent->LocalTransform;

	if (Parent)
	{
		g.Position = Vector3Add(
			Vector3RotateByQuaternion(l.Position, pg.Rotation.Quat), pg.Position);
		g.Scale = Vector3Multiply(l.Scale, pg.Scale);
		g.Rotation.SetQuat(QuaternionMultiply(pg.Rotation.Quat, l.Rotation.Quat));
	}
	else
	{
		GlobalTransform = LocalTransform;
	}

	UpdateModelTransform();

	for (auto& ch : Children)
		ch->UpdateGlobalTransform();
}

void MapObject::UpdateLocalTransform()
{
	auto& g = GlobalTransform;
	auto& l = LocalTransform;
	auto& pg = Parent->GlobalTransform;
	auto& pl = Parent->LocalTransform;

	if (Parent)
	{
		auto relPos = Vector3Subtract(g.Position, pg.Position);
		l.Position = Vector3RotateByQuaternion(
			relPos, QuaternionInvert(pg.Rotation.Quat));
		l.Rotation.SetQuat(QuaternionMultiply(
			QuaternionInvert(pg.Rotation.Quat), g.Rotation.Quat));
		l.Scale = Vector3Divide(g.Scale, pg.Scale);
	}
	else
	{
		LocalTransform = GlobalTransform;
	}

	UpdateModelTransform();

	for (auto& ch : Children)
		ch->UpdateGlobalTransform();
}

void MapObject::UpdateModelTransform()
{
	if (ObjType != MapObjectType::Model) return;

	auto& g = GlobalTransform;
	ModelObj.transform = MatrixRotateXYZ(
		Vector3Scale(g.Rotation.Euler, -DEG2RAD));
	//ModelObj.transform = MatrixRotateZYX(
		//Vector3Scale(g.Rotation.Euler, -DEG2RAD));
	//ModelObj.transform = MatrixRotateXYZ
	/*auto s = MatrixScale(g.Scale.x, g.Scale.y, g.Scale.z);
	auto r = QuaternionToMatrix(g.Rotation.Quat);
	auto t = MatrixTranslate(g.Position.x, g.Position.y, g.Position.z);
	ModelObj.transform = MatrixMultiply(MatrixMultiply(s, r), t);*/
}


int MapObject::ComputeLevel() const
{
	int l = 0;
	auto current = Parent;
	while (current != nullptr)
	{
		++l;
		current = current->Parent;
	}
	return l;
}


void MapObject::UpdateParent(MapObject* NewParent)
{
	Parent = NewParent;
	Level = ComputeLevel();
	UpdateGlobalTransform();
}


void MapObject::MoveChildrenTo(MapObject* NewParent)
{
	for (auto ch : Children)
	{
		NewParent->AddNewChild(ch);
	}
	Children.clear();
}


void MapObject::AddNewChild(MapObject* NewChild)
{
	NewChild->UpdateParent(this);
	Children.push_back(NewChild);
}


void MapObject::Draw() const
{
	auto g = GlobalTransform;
	switch (ObjType)
	{
		/*case MapObjectType::Empty:
			DrawSphere(g.Position, 0.2, SKYBLUE);
			break;*/

		case MapObjectType::Model:
			DrawModelEx(ModelObj, g.Position, Vector3UnitY, 0, g.Scale, WHITE);
			//DrawModel(ModelObj, {}, 1.0f, WHITE);
			break;
	}
}


void MapObject::SetProperties(const Utils::MapObjectProperties& Props)
{
	Name = Props.Name;
	Tags = Props.Tags;
	if (LocalTransform != Props.Transform)
	{
		LocalTransform = Props.Transform;
		UpdateGlobalTransform();
	}
}

void MapObject::SetModel(Model Model, int ModelIdx)
{
	if (Model.meshes == nullptr || ModelIdx < 0) return;
	this->ModelIdx = ModelIdx;
	Texture2D td{};
	if (ObjType == MapObjectType::Model)
		td = ModelObj.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;
	ModelObj = Model;
	if (ObjType == MapObjectType::Model)
		ModelObj.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = td;
	ObjType = MapObjectType::Model;
}

void MapObject::SetTexture(Texture2D Texture, int TexIdx)
{
	if (ModelObj.materials != nullptr)
		ModelObj.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Texture;
}
