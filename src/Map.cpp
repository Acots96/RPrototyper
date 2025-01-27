#include "Map.h"
#include "raymath.h"
#include <iostream>
#include "ResourcesManager.h"

//using namespace RPrototyper;


Map::Map() :
	bIsMapLocked{ false }, Objects{},
	Root{ std::make_unique<MapObject>(MapObjectType::Root, "Root") },
	ObjSelected{ nullptr }, LastSelectedTime{ 0 }, bIsPerformingAction{ false },
	CurrentAction{ Action::Drag },
	ScreenW{ GetScreenWidth() }, ScreenH{ GetScreenHeight() }
{
	Cam.position = OverCam.position = { 3.0f, 3.0f, 3.0f };
	Cam.target = OverCam.target = { 0.0f, 0.0f, 0.0f }; // Camera looking at point
	Cam.up = OverCam.up = { 0.0f, 1.0f, 0.0f };
	Cam.fovy = OverCam.fovy = 60.0f;
	Cam.projection = OverCam.projection = CAMERA_PERSPECTIVE; //CAMERA_ORTHOGRAPHIC

	RenderTex = LoadRenderTexture(ScreenW, ScreenH);
	OverRenderTex = LoadRenderTexture(ScreenW, ScreenH);
}


void Map::UpdateCam()
{
	if (bIsMapLocked) return;

	bool shift = IsKeyDown(KEY_LEFT_SHIFT);

	float speed = 0.02f;
	if (shift) speed *= 10.f;

	Vector3 movement{ Vector3Zeros };
	//if (IsKeyDown(KEY_W)) movement.x += speed;
	//if (IsKeyDown(KEY_S)) movement.x -= speed;
	//if (IsKeyDown(KEY_D)) movement.y += speed;
	//if (IsKeyDown(KEY_A)) movement.y -= speed;

	Vector3 rotation{ Vector3Zeros };
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto delta = GetMouseDelta() * 0.2f;
		rotation.x = delta.x;
		rotation.y = delta.y;
	}
	else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
	{
		auto delta = GetMouseDelta() * 0.05f;
		if (shift) movement.x = -delta.y;
		else movement.z = -delta.y;
		movement.y = delta.x;
	}

	auto zoom = GetMouseWheelMove() * (shift ? 4.0f : 1.0f);

	UpdateCameraPro(&Cam, movement, rotation, -zoom);
}


MapObject* Map::Find(MapObject& Parent,
	const Utils::MapObjectId& TargetId) const
{
	auto p = &Parent;
	if (Parent.GetId() == TargetId) return &Parent;

	for (const auto& ch : Parent.GetChildren())
	{
		auto m = Find(*ch, TargetId);
		if (m != nullptr) return m;
	}
	return nullptr;
}

void Map::TraverseDFS(MapObject& Parent,
	std::function<void(MapObject&)>& Func) const
{
	Func(Parent);
	for (auto ch : Parent.GetChildren())
	{
		TraverseDFS(*ch, Func);
	}
}


/*void Map::TraverseTree(std::shared_ptr<MapObject> Parent)
{
	if (auto p = Parent.get())
	{
		for (auto& ch : p.)
	}
}*/


void Map::Update()
{
	UpdateCam();

	int w{ GetScreenWidth() }, h{ GetScreenHeight() };
	if (w != ScreenW || h != ScreenH)
	{
		ScreenW = w; ScreenH = h;
		RenderTex = LoadRenderTexture(ScreenW, ScreenH);
		OverRenderTex = LoadRenderTexture(ScreenW, ScreenH);
	}

	BeginTextureMode(RenderTex);
	ClearBackground(BLACK);
	BeginMode3D(Cam);

	DrawGrid(1000, 10.0f);
	//DrawLine3D(Vector3Zeros, { 1, 0, 0 }, RED);
	//DrawLine3D(Vector3Zeros, { 0, 1, 0 }, GREEN);
	//DrawLine3D(Vector3Zeros, { 0, 0, 1 }, BLUE);

	for (auto& o : Objects)
	{
		o->Draw();
	}

	EndMode3D();
	EndTextureMode();

	DrawOverCam();
}

void Map::Draw()
{
	DrawTextureRec(RenderTex.texture, { 0, 0, (float)ScreenW, -(float)ScreenH }, { 0, 0 }, WHITE);
	DrawTextureRec(OverRenderTex.texture, { 0, 0, (float)ScreenW, -(float)ScreenH }, { 0, 0 }, WHITE);
}


const Utils::MapObjectProperties Map::AddMapObject(
	std::string Name, MapObjectType Type, Utils::MapObjectId ParentId, 
	Model Model, int ModelIdx)
{
	Objects.push_back(std::make_unique<MapObject>(Type, Name));
	auto mo = GetLast();
	if (Type == MapObjectType::Model)
		mo->SetModel(Model, ModelIdx);
	if (ParentId.IsEmpty())
	{
		Root->AddNewChild(mo);
	}
	else
	{
		auto m = Find(*Root, ParentId);
		if (m == nullptr)
			Root->AddNewChild(mo);
		else
			m->AddNewChild(mo);
	}
	return *mo;
}


void Map::GetBasicDatas(std::vector<Utils::MapObjectBasicData>& Out) const
{
	Out.clear(); Out.reserve(Objects.size());
	auto& rId = Root->GetId();
	std::function<void(MapObject&)> func = [&Out,&rId](const MapObject& mo)
		{ if (mo.GetId() != rId) Out.push_back(mo); };
	TraverseDFS(*Root, func);
}


const Utils::MapObjectProperties Map::GetProperties(const Utils::MapObjectId& Id) const
{
	return *Find(*Root, Id);
}


const Utils::MapObjectProperties Map::SelectObject(const Utils::MapObjectId& Id)
{
	ObjSelected = Find(*Root, Id);

	auto t = GetTime();
	if (t - LastSelectedTime < 0.2f)
	{
		/*auto& pos = ObjSelected->GlobalPosition();
		Cam.position += pos - Cam.target;
		Cam.target = pos;*/
		Cam.target = ObjSelected->GetGlobalPosition();
		auto dir = Vector3Normalize(Cam.position - Cam.target);
		Cam.position = Cam.target + Vector3Scale(dir, 5.0f);
	}
	LastSelectedTime = t;

	return *ObjSelected;
}


void Map::SetProperties(const Utils::MapObjectProperties& Props)
{
	auto mo = Find(*Root, Props.Id);
	auto mId = Props.ModelIdx;
	auto tdId = Props.TexDiffuseIdx;
	mo->SetProperties(Props);
	if (mo->GetModelId() != mId)
	{
		mo->SetModel(ResourcesManager::GetInstancedModel(mId), mId);
	}
	if (mo->GetTexDiffuseId() != tdId)
	{
		mo->SetTexture(ResourcesManager::GetTexture(tdId), tdId);
	}
}


#pragma region Actions

static Vector3 ClosestPointOnLine(const Vector3& linePoint,
	const Vector3& lineDir, const Ray& ray)
{
	Vector3 w = Vector3Subtract(ray.position, linePoint);
	float a = Vector3DotProduct(lineDir, lineDir);
	float b = Vector3DotProduct(lineDir, ray.direction);
	float c = Vector3DotProduct(ray.direction, ray.direction);
	float d = Vector3DotProduct(lineDir, w);
	float e = Vector3DotProduct(ray.direction, w);

	float denominator = a * c - b * b;
	if (fabs(denominator) < 1e-6f)
	{
		// Recta y rayo son casi paralelos
		return linePoint;
	}

	float s = (b * e - c * d) / denominator;

	// Punto más cercano en la recta
	return Vector3Add(linePoint, Vector3Scale(lineDir, -s));
}
static Vector3 ClosestPointOnPlane(const Vector3& planePoint,
	const Vector3& planeNormal, const Ray& ray)
{
	// Vector desde el origen del rayo hasta un punto en el plano
	Vector3 rayToPlane = Vector3Subtract(planePoint, ray.position);

	// Calcular el denominador del cálculo de intersección (proyección del rayo sobre la normal del plano)
	float denominator = Vector3DotProduct(ray.direction, planeNormal);

	// Si el denominador es cercano a 0, el rayo es paralelo al plano y no hay intersección
	if (fabs(denominator) < 0.0001f)
	{
		// Retornar un punto en el plano (por ejemplo, el más cercano al origen del rayo)
		return ray.position;
	}

	// Calcular la distancia t desde el origen del rayo hasta la intersección con el plano
	float t = Vector3DotProduct(rayToPlane, planeNormal) / denominator;

	// Obtener el punto más cercano en el plano
	return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}


void Map::ComputeDrag()
{
	auto p = ObjSelected->GetLocalPosition();
	//float dist{ 100 };
	//Vector3 dir{ Vector3Normalize(p - Cam.position) };
	//p = Cam.position + dir * dist;
	float dist{ Vector3Length(p - Cam.position) };

	float arrowLen{ 0.1f * dist }, arrowW{ arrowLen * 0.022f },
		coneLen{ arrowLen * 0.25f }, coneBigW{ arrowLen * 0.1f },
		coneSmallW{ arrowLen * 0.01f };

	Color colorX{ RED }, colorY{ GREEN }, colorZ{ BLUE },
		colorXtoY{ RED }, colorXtoZ{ RED },
		colorYtoX{ GREEN }, colorYtoZ{ GREEN },
		colorZtoX{ BLUE }, colorZtoY{ BLUE };

	// Arrows
	auto m = QuaternionToMatrix(ObjSelected->GetLocalRotation().Quat);
	Vector3 vX{ m.m0, m.m4, m.m8 };
	Vector3 vY{ m.m1, m.m5, m.m9 };
	Vector3 vZ{ m.m2, m.m6, m.m10 };
	//std::cout << vX.x << " ; " << vX.y << " ; " << vX.z << "\n";
	auto vXDir = Vector3Scale(vX, arrowLen);
	auto vYDir = Vector3Scale(vY, arrowLen);
	auto vZDir = Vector3Scale(vZ, arrowLen);
	auto vOffsetX = p + vXDir;
	auto vOffsetY = p + vYDir;
	auto vOffsetZ = p + vZDir;

	// Planes
	float plLineLen{ 0.35f };
	auto plLineX = vXDir * plLineLen;
	auto plLineY = vYDir * plLineLen;
	auto plLineZ = vZDir * plLineLen;
	auto plPosX = p + plLineX;
	auto plPosY = p + plLineY;
	auto plPosZ = p + plLineZ;

	Ray ray{ GetScreenToWorldRay(GetMousePosition(), Cam) };

	if (!bIsPerformingAction)
	{
		RayCollision col{ GetRayCollisionSphere(ray, p, arrowLen * 1.25f) };
		//DrawSphere(p, arrowLen * 1.25f, { 255, 255, 255, 10 });
		if (col.hit)
		{
			bool hitX{ false }, hitY{ false }, hitZ{ false },
				hitXY{ false }, hitXZ{ false }, hitYZ{ false };

			float spherePos{ 0.4f }, sphereRad{ coneLen * 0.75f };
			col = GetRayCollisionSphere(ray, vOffsetX + Vector3Scale(vX, coneLen) * spherePos, sphereRad);
			//DrawSphere(vOffsetX + Vector3Scale(vX, coneLen) * spherePos, sphereRad, {255, 0, 0, 100});

			ChangeDirection vDir{};
			Vector3 vOffset{};

			if (col.hit)
			{
				colorX = YELLOW;
				hitX = true;
				vDir = ChangeDirection::X;
				vOffset = vOffsetX;
			}
			else
			{
				col = GetRayCollisionSphere(ray, vOffsetY + Vector3Scale(vY, coneLen) * spherePos, sphereRad);
				if (col.hit)
				{
					colorY = YELLOW;
					hitY = true;
					vDir = ChangeDirection::Y;
					vOffset = vOffsetY;
				}
				else
				{
					col = GetRayCollisionSphere(ray, vOffsetZ + Vector3Scale(vZ, coneLen) * spherePos, sphereRad);
					if (col.hit)
					{
						colorZ = YELLOW;
						hitZ = true;
						vDir = ChangeDirection::Z;
						vOffset = vOffsetZ;
					}
				}
			}
			if (!hitX && !hitY && !hitZ)
			{
				col = GetRayCollisionQuad(ray, p, plPosX, plPosX + plLineY, plPosY);
				if (col.hit)
				{
					colorXtoY = colorYtoX = colorX = colorY = YELLOW;
					hitXY = true;
					vDir = ChangeDirection::XY;
					vOffset = col.point;
				}
				else
				{
					col = GetRayCollisionQuad(ray, p, plPosX, plPosX + plLineZ, plPosZ);
					if (col.hit)
					{
						colorXtoZ = colorZtoX = colorX = colorZ = YELLOW;
						hitXZ = true;
						vDir = ChangeDirection::XZ;
						vOffset = col.point;
					}
					else
					{
						col = GetRayCollisionQuad(ray, p, plPosY, plPosY + plLineZ, plPosZ);
						if (col.hit)
						{
							colorYtoZ = colorZtoY = colorY = colorZ = YELLOW;
							hitYZ = true;
							vDir = ChangeDirection::YZ;
							vOffset = col.point;
						}
					}
				}
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !bIsMapLocked)
			{
				if (hitX || hitY || hitZ)
				{
					bIsPerformingAction = true;
					ChangeDir = vDir;
					InitPos = vOffset;
					Offset = vOffset - col.point;
				}
				else if (hitXY || hitXZ || hitYZ)
				{
					bIsPerformingAction = true;
					ChangeDir = vDir;
					InitPos = vOffset;
					Offset = p - col.point;
				}
			}
		}
	}
	else
	{
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			bIsPerformingAction = false;
			ChangeDir = ChangeDirection::None;
		}
		else
		{
			Vector3 targetP{};
			// Move
			switch (ChangeDir)
			{
				case ChangeDirection::X:
					targetP = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vXDir), ray);
					targetP += Offset - vXDir;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				case ChangeDirection::Y:
					targetP = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vYDir), ray);
					targetP += Offset - vYDir;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				case ChangeDirection::Z:
					targetP = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vZDir), ray);
					targetP += Offset - vZDir;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				case ChangeDirection::XY:
					targetP = ClosestPointOnPlane(InitPos - Offset,
						Vector3Normalize(vZDir), ray);
					targetP += Offset;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				case ChangeDirection::XZ:
					targetP = ClosestPointOnPlane(InitPos - Offset,
						Vector3Normalize(vYDir), ray);
					targetP += Offset;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				case ChangeDirection::YZ:
					targetP = ClosestPointOnPlane(InitPos - Offset,
						Vector3Normalize(vXDir), ray);
					targetP += Offset;
					ObjSelected->SetLocalPosition(targetP);
					if (OnTransformChanged) OnTransformChanged(*ObjSelected);
					break;

				default:
					break;
			}
		}
	}

	// Arrow X
	DrawCylinderEx(p, vOffsetX, arrowW, arrowW, 8, colorX);
	DrawCylinderEx(vOffsetX, vOffsetX + Vector3Scale(vX, coneLen),
		coneBigW, coneSmallW, 8, colorX);

	// Arrow Y
	DrawCylinderEx(p, vOffsetY, arrowW, arrowW, 8, colorY);
	DrawCylinderEx(vOffsetY, vOffsetY + Vector3Scale(vY, coneLen),
		coneBigW, coneSmallW, 8, colorY);

	// Arrow Z
	DrawCylinderEx(p, vOffsetZ, arrowW, arrowW, 8, colorZ);
	DrawCylinderEx(vOffsetZ, vOffsetZ + Vector3Scale(vZ, coneLen),
		coneBigW, coneSmallW, 8, colorZ);

	float planeLen{ 0.35f };
	// Plane X (Y)
	DrawCylinderEx(plPosX, plPosX + plLineY, arrowW, arrowW, 8, colorXtoY);
	// Plane X (Z)
	DrawCylinderEx(plPosX, plPosX + plLineZ, arrowW, arrowW, 8, colorXtoZ);

	// Plane Y (X)
	DrawCylinderEx(plPosY, plPosY + plLineX, arrowW, arrowW, 8, colorYtoX);
	// Plane Y (Z)
	DrawCylinderEx(plPosY, plPosY + plLineZ, arrowW, arrowW, 8, colorYtoZ);

	// Plane Z (X)
	DrawCylinderEx(plPosZ, plPosZ + plLineX, arrowW, arrowW, 8, colorZtoX);
	// Plane Z (Y)
	DrawCylinderEx(plPosZ, plPosZ + plLineY, arrowW, arrowW, 8, colorZtoY);
}


void Map::ComputeRotate()
{
	auto p = ObjSelected->GetGlobalPosition();
	//float dist{ 100 };
	//Vector3 dir{ Vector3Normalize(p - Cam.position) };
	//p = Cam.position + dir * dist;
	float dist{ Vector3Length(p - Cam.position) };

	float segments{ 5 };
	float circleRadB{ 0.13f * dist }, circleRadS{ circleRadB * 0.8f };

	Color colorX{ RED }, colorY{ GREEN }, colorZ{ BLUE },
		colorXbs{ 230, 41, 55, 100 }, colorXinner{ 230, 41, 55, 50 },
		colorYbs{ 0, 228, 48, 100 }, colorYinner{ 0, 228, 48, 50 },
		colorZbs{ 0, 121, 241, 100 }, colorZinner{ 0, 121, 241, 50 };

	// Arrows
	auto m = QuaternionToMatrix(ObjSelected->GetGlobalRotation().Quat);
	Vector3 vX{ m.m0, m.m4, m.m8 };
	Vector3 vY{ m.m1, m.m5, m.m9 };
	Vector3 vZ{ m.m2, m.m6, m.m10 };

	Ray ray{ GetScreenToWorldRay(GetMousePosition(), Cam) };

	// X
	auto vXBs = Utils::UComputeSemicircle3D(p, circleRadB, vY, vZ, segments, colorX);
	auto vXSs = Utils::UComputeSemicircle3D(p, circleRadS, vY, vZ, segments, colorX);
	std::vector<Utils::Triangle3D> trisX{}, ignoreTrisX{};
	for (int i = 0; i < vXBs.size() - 1; i++)
	{
		Vector3 v0{ vXSs[i] },
			v1{ vXBs[i + 1] },
			v2{ vXBs[i] };
		trisX.push_back({ v0, v1, v2 });
		trisX.push_back({ v0, v2, v1 });

		v1 = vXSs[i + 1];
		v2 = vXBs[i + 1];
		trisX.push_back({ v0, v1, v2 });
		trisX.push_back({ v0, v2, v1 });

		v1 = vXSs[i + 1];
		v2 = vXSs[i];
		ignoreTrisX.push_back({ p, v2, v1 });
		ignoreTrisX.push_back({ p, v1, v2 });
	}

	// Y
	auto vYBs = Utils::UComputeSemicircle3D(p, circleRadB, vX, vZ, segments, colorY);
	auto vYSs = Utils::UComputeSemicircle3D(p, circleRadS, vX, vZ, segments, colorY);
	std::vector<Utils::Triangle3D> trisY{}, ignoreTrisY{};
	for (int i = 0; i < vYBs.size() - 1; i++)
	{
		Vector3 v0{ vYSs[i] },
			v1{ vYBs[i + 1] },
			v2{ vYBs[i] };
		trisY.push_back({ v0, v1, v2 });
		trisY.push_back({ v0, v2, v1 });

		v1 = vYSs[i + 1];
		v2 = vYBs[i + 1];
		trisY.push_back({ v0, v1, v2 });
		trisY.push_back({ v0, v2, v1 });

		v1 = vYSs[i + 1];
		v2 = vYSs[i];
		ignoreTrisY.push_back({ p, v2, v1 });
		ignoreTrisY.push_back({ p, v1, v2 });
	}

	// Z
	auto vZBs = Utils::UComputeSemicircle3D(p, circleRadB, vX, vY, segments, colorZ);
	auto vZSs = Utils::UComputeSemicircle3D(p, circleRadS, vX, vY, segments, colorZ);
	std::vector<Utils::Triangle3D> trisZ{}, ignoreTrisZ{};
	for (int i = 0; i < vZBs.size() - 1; i++)
	{
		Vector3 v0{ vZSs[i] },
			v1{ vZBs[i + 1] },
			v2{ vZBs[i] };
		trisZ.push_back({ v0, v1, v2 });
		trisZ.push_back({ v0, v2, v1 });

		v1 = vZSs[i + 1];
		v2 = vZBs[i + 1];
		trisZ.push_back({ v0, v1, v2 });
		trisZ.push_back({ v0, v2, v1 });

		v1 = vZSs[i + 1];
		v2 = vZSs[i];
		ignoreTrisZ.push_back({ p, v2, v1 });
		ignoreTrisZ.push_back({ p, v1, v2 });
	}

	if (!bIsPerformingAction)
	{
		RayCollision col{ GetRayCollisionSphere(ray, p, circleRadB * 1.25f) };
		//DrawSphere(p, arrowLen * 1.25f, { 255, 255, 255, 10 });
		if (col.hit)
		{
			for (auto& tri : trisX)
			{
				col = GetRayCollisionTriangle(ray, tri.v0, tri.v1, tri.v2);
				if (col.hit) break;
			}
			if (col.hit)
			{
				colorXbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
				colorXinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					bIsPerformingAction = true;
					InitDir = col.point - p;
					PlanePoint = col.point;
					PlaneNormal = vX;
					ChangeDir = ChangeDirection::X;
					InitAngle = ObjSelected->GetGlobalRotation().Euler.x;
					InitQuat = ObjSelected->GetGlobalRotation().Quat;
				}
			}
			else
			{
				for (auto& tri : trisY)
				{
					col = GetRayCollisionTriangle(ray, tri.v0, tri.v1, tri.v2);
					if (col.hit) break;
				}
				if (col.hit)
				{
					colorYbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
					colorYinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						bIsPerformingAction = true;
						InitDir = col.point - p;
						PlanePoint = col.point;
						PlaneNormal = vY;
						ChangeDir = ChangeDirection::Y;
						InitAngle = ObjSelected->GetGlobalRotation().Euler.y;
						InitQuat = ObjSelected->GetGlobalRotation().Quat;
					}
				}
				else
				{
					for (auto& tri : trisZ)
					{
						col = GetRayCollisionTriangle(ray, tri.v0, tri.v1, tri.v2);
						if (col.hit) break;
					}
					if (col.hit)
					{
						colorZbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
						colorZinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
						if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
						{
							bIsPerformingAction = true;
							InitDir = col.point - p;
							PlanePoint = col.point;
							PlaneNormal = vZ;
							ChangeDir = ChangeDirection::Z;
							InitAngle = ObjSelected->GetGlobalRotation().Euler.z;
							InitQuat = ObjSelected->GetGlobalRotation().Quat;
						}
					}
				}
			}
		}
	}
	else
	{
		auto plPoint = ClosestPointOnPlane(PlanePoint, PlaneNormal, ray);
		auto angleDiff = Vector3Angle(InitDir, plPoint - p) * RAD2DEG;
		auto cross = Vector3CrossProduct(InitDir, plPoint - p);
		if (Vector3DotProduct(PlaneNormal, cross) < 0)
			angleDiff = -angleDiff;

		switch (ChangeDir)
		{
			case ChangeDirection::X:
				colorXbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
				colorXinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
				colorY.a = colorYbs.a = colorYinner.a = 0;
				colorZ.a = colorZbs.a = colorZinner.a = 0;
				break;
			case ChangeDirection::Y:
				colorYbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
				colorYinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
				colorX.a = colorXbs.a = colorXinner.a = 0;
				colorZ.a = colorZbs.a = colorZinner.a = 0;
				break;
			case ChangeDirection::Z:
				colorZbs = { YELLOW.r, YELLOW.g, YELLOW.b, 200 };
				colorZinner = { YELLOW.r, YELLOW.g, YELLOW.b, 100 };
				colorX.a = colorXbs.a = colorXinner.a = 0;
				colorY.a = colorYbs.a = colorYinner.a = 0;
				break;
		}

		auto q = QuaternionFromAxisAngle(
			PlaneNormal, -(InitAngle + angleDiff) * DEG2RAD);
		ObjSelected->SetGlobalQuat(QuaternionMultiply(InitQuat, q));
		DrawLine3D(p, p + InitDir, WHITE);
		DrawLine3D(p, plPoint, WHITE);
		if (OnTransformChanged) OnTransformChanged(*ObjSelected);
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			bIsPerformingAction = false;
	}

	Vector3 prev{};

	if (!bIsPerformingAction || ChangeDir == ChangeDirection::X)
	{
		for (int i = 0; i < vXBs.size() - 1; i++)
		{
			DrawLine3D(vXBs[i], vXBs[i + 1], colorX);
		}
		for (int i = 0; i < vXSs.size() - 1; i++)
		{
			DrawLine3D(vXSs[i], vXSs[i + 1], colorX);
		}
		for (auto& tri : trisX)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorXbs);
		}
		for (auto& tri : ignoreTrisX)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorXinner);
		}
	}

	if (!bIsPerformingAction || ChangeDir == ChangeDirection::Y)
	{
		for (int i = 0; i < vYBs.size() - 1; i++)
		{
			DrawLine3D(vYBs[i], vYBs[i + 1], colorY);
		}
		for (int i = 0; i < vYSs.size() - 1; i++)
		{
			DrawLine3D(vYSs[i], vYSs[i + 1], colorY);
		}
		for (auto& tri : trisY)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorYbs);
		}
		for (auto& tri : ignoreTrisY)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorYinner);
		}
	}

	if (!bIsPerformingAction || ChangeDir == ChangeDirection::Z)
	{
		for (int i = 0; i < vZBs.size() - 1; i++)
		{
			DrawLine3D(vZBs[i], vZBs[i + 1], colorZ);
		}
		for (int i = 0; i < vZSs.size() - 1; i++)
		{
			DrawLine3D(vZSs[i], vZSs[i + 1], colorZ);
		}
		for (auto& tri : trisZ)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorZbs);
		}
		for (auto& tri : ignoreTrisZ)
		{
			DrawTriangle3D(tri.v0, tri.v1, tri.v2, colorZinner);
		}
	}
}


void Map::ComputeScale()
{
	auto p = ObjSelected->GetGlobalPosition();
	//float dist{ 100 };
	//Vector3 dir{ Vector3Normalize(p - Cam.position) };
	//p = Cam.position + dir * dist;
	float dist{ Vector3Length(p - Cam.position) };

	float segments{ 5 };
	float circleRadB{ 0.13f * dist }, circleRadS{ circleRadB * 0.8f };

	Color colorX{ RED }, colorY{ GREEN }, colorZ{ BLUE }, colorC{ WHITE };

	float arrowLen{ 0.1f * dist }, arrowW{ arrowLen * 0.022f };
	Vector3 cubeSize{ Vector3Ones * arrowLen * 0.15f };

	// Arrows
	auto m = QuaternionToMatrix(ObjSelected->GetGlobalRotation().Quat);
	Vector3 vX{ m.m0, m.m4, m.m8 };
	Vector3 vY{ m.m1, m.m5, m.m9 };
	Vector3 vZ{ m.m2, m.m6, m.m10 };
	auto vXDir = Vector3Scale(vX, arrowLen);
	auto vYDir = Vector3Scale(vY, arrowLen);
	auto vZDir = Vector3Scale(vZ, arrowLen);
	auto vOffsetX = p + vXDir;
	auto vOffsetY = p + vYDir;
	auto vOffsetZ = p + vZDir;

	Ray ray{ GetScreenToWorldRay(GetMousePosition(), Cam) };

	if (!bIsPerformingAction)
	{
		RayCollision col{ GetRayCollisionSphere(ray, p, arrowLen * 1.25f) };
		//DrawSphere(p, arrowLen * 1.25f, { 255, 255, 255, 10 });
		if (col.hit)
		{
			bool hitX{ false }, hitY{ false }, hitZ{ false }, hitC{ false };

			float spherePos{ 0.4f }, sphereRad{ cubeSize.x * 0.75f };
			//DrawSphere(vOffsetX + Vector3Scale(vX, coneLen) * spherePos, sphereRad, {255, 0, 0, 100});

			ChangeDirection vDir{};
			Vector3 vOffset{};

			col = GetRayCollisionSphere(ray, p, sphereRad);
			if (col.hit)
			{
				colorC = YELLOW;
				hitC = true;
				vDir = ChangeDirection::XYZ;
			}
			else
			{
				col = GetRayCollisionSphere(ray, vOffsetX, sphereRad);
				if (col.hit)
				{
					colorX = YELLOW;
					hitX = true;
					vDir = ChangeDirection::X;
					vOffset = vOffsetX;
				}
				else
				{
					col = GetRayCollisionSphere(ray, vOffsetY, sphereRad);
					if (col.hit)
					{
						colorY = YELLOW;
						hitY = true;
						vDir = ChangeDirection::Y;
						vOffset = vOffsetY;
					}
					else
					{
						col = GetRayCollisionSphere(ray, vOffsetZ, sphereRad);
						if (col.hit)
						{
							colorZ = YELLOW;
							hitZ = true;
							vDir = ChangeDirection::Z;
							vOffset = vOffsetZ;
						}
					}
				}
			}
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !bIsMapLocked)
			{
				if (hitX || hitY || hitZ || hitC)
				{
					bIsPerformingAction = true;
					ChangeDir = vDir;
					InitPos = vOffset;
					if (!hitC) Offset = vOffset - col.point;
					else
					{
						auto rCenter = GetScreenToWorldRay({ GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f }, Cam);
						auto rTop = GetScreenToWorldRay({ (float)GetScreenWidth(), 0.0f }, Cam);
						IncrXYZ = (rTop.position + rTop.direction) - (rCenter.position + rCenter.direction);
					}
					if (!hitC) InitDist = Vector3Length(vOffset);
					InitScale = ObjSelected->GetGlobalScale();
				}
			}
		}
	}
	else
	{
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			bIsPerformingAction = false;
			ChangeDir = ChangeDirection::None;
		}
		else
		{
			Vector3 targetP{};
			Vector3 s{ InitScale };
			float dot{};
			// Scale
			switch (ChangeDir)
			{
				case ChangeDirection::X:
					vOffsetX = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vXDir), ray);
					vOffsetX += Offset;
					s.x *= Vector3Length(vOffsetX - p) / InitDist;
					if (Vector3DotProduct(vXDir, vOffsetX - p) < 0) s.x = -s.x;
					Vector3Length(vXDir);
					break;

				case ChangeDirection::Y:
					vOffsetY = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vYDir), ray);
					vOffsetY += Offset;
					s.y *= Vector3Length(vOffsetY - p) / InitDist;
					if (Vector3DotProduct(vYDir, vOffsetY - p) < 0) s.y = -s.y;
					break;

				case ChangeDirection::Z:
					vOffsetZ = ClosestPointOnLine(InitPos - Offset,
						Vector3Normalize(vZDir), ray);
					vOffsetZ += Offset;
					s.z *= Vector3Length(vOffsetZ - p) / InitDist;
					if (Vector3DotProduct(vZDir, vOffsetZ - p) < 0) s.z = -s.z;
					break;

				case ChangeDirection::XYZ:
					auto dir = Vector3Negate(ray.direction);
					auto pp = ClosestPointOnPlane(p, dir, ray);
					Vector3 v{ pp - p };
					dot = Vector3DotProduct(v, IncrXYZ);
					s += Vector3Ones * dot * Vector3Length(s);
					break;

				default:
					break;
			}
			ObjSelected->SetGlobalScale(s);
			if (OnTransformChanged) OnTransformChanged(*ObjSelected);
		}
	}

	// Center
	DrawCubeV(p, cubeSize, colorC);

	// Arrow X
	DrawCylinderEx(p, vOffsetX, arrowW, arrowW, 8, colorX);
	DrawCubeV(vOffsetX, cubeSize, colorX);

	// Arrow Y
	DrawCylinderEx(p, vOffsetY, arrowW, arrowW, 8, colorY);
	DrawCubeV(vOffsetY, cubeSize, colorY);

	// Arrow Z
	DrawCylinderEx(p, vOffsetZ, arrowW, arrowW, 8, colorZ);
	DrawCubeV(vOffsetZ, cubeSize, colorZ);
}


void Map::DrawMiniArrows()
{
	Ray r = GetScreenToWorldRay({ 50, GetScreenHeight() - 50.f }, Cam);
	//Ray rd = GetScreenToWorldRay({ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f }, Cam);
	//PrintV3(r.direction);
	//PrintV3(rd.direction);
	Vector3 p{ r.position + r.direction * 10 };
	Vector3 pX{ p + Vector3UnitX * 0.25f };
	Vector3 pY{ p + Vector3UnitY * 0.25f };
	Vector3 pZ{ p + Vector3UnitZ * 0.25f };
	float w = 0.01f, wC{ w * 5.0f }, lC{ 0.12f };
	// Arrow X
	DrawCylinderEx(p, pX, w, w, 4, RED);
	DrawCylinderEx(pX, pX + Vector3UnitX * lC, wC, w, 8, RED);
	// Arrow Y
	DrawCylinderEx(p, pY, w, w, 8, GREEN);
	DrawCylinderEx(pY, pY + Vector3UnitY * lC, wC, w, 8, GREEN);
	// Arrow Z
	DrawCylinderEx(p, pZ, w, w, 8, BLUE);
	DrawCylinderEx(pZ, pZ + Vector3UnitZ * lC, wC, w, 8, BLUE);
}


void Map::DrawOverCam()
{
	OverCam.position = Cam.position;
	OverCam.target = Cam.target;

	BeginTextureMode(OverRenderTex);
	ClearBackground(BLANK);
	BeginMode3D(OverCam);

	DrawMiniArrows();

	if (ObjSelected)
	{
		if (!bIsMapLocked)
		{
			if (IsKeyPressed(KEY_W)) CurrentAction = Action::Drag;
			else if (IsKeyPressed(KEY_E)) CurrentAction = Action::Rotate;
			else if (IsKeyPressed(KEY_R)) CurrentAction = Action::Scale;
		}

		switch (CurrentAction)
		{
			case Action::None:
				break;

			case Action::Drag:
				ComputeDrag();
				break;

			case Action::Rotate:
				ComputeRotate();
				break;

			case Action::Scale:
				ComputeScale();
				break;

			default:
				break;
		}
	}

	EndMode3D();
	EndTextureMode();
}

#pragma endregion

