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

#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include <string>
#include <vector>


namespace Utils
{
	extern Color LIGHTLIGHTGRAY;

	void UGuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color);
	void UGuiDrawText(const char* text, Rectangle textBounds, int alignment, Color tint);
	int UGuiScrollBar(Rectangle bounds, int value, int minValue, int maxValue);

	bool UIsPointInRect(Vector2 Point, Rectangle Rect);
	bool UIsR1ColllidingWithR2(Rectangle R1, Rectangle R2);

	std::vector<Vector3> UComputeSemicircle3D(Vector3 center, float radius,
		Vector3 startDir, Vector3 endDir, int segments, Color color);


	struct Triangle3D
	{
		Vector3 v0, v1, v2;
	};

	struct Rotator
	{
		enum class EulerAngle { Pitch, Yaw, Roll };

		Quaternion Quat;
		Vector3 Euler;

		Rotator() : Quat{ QuaternionIdentity() }, Euler{ Vector3Zeros } {}
		explicit Rotator(Quaternion Quat)
		{
			SetQuat(Quat);
		}
		/*explicit Rotator(Vector3 Euler)
		{
			//UpdateEuler(Euler);
			UpdatePitch(Euler.x);
			UpdateYaw(Euler.y);
			UpdateRoll(Euler.z);
		}*/
		void SetQuat(Quaternion Quat)
		{
			this->Quat = Quat;
			auto e = QuaternionToEuler(Quat);
			Euler = Vector3Scale({e.x, e.y, e.z}, RAD2DEG);
		}
		/*void UpdateEuler(Vector3 Euler)
		{
			this->Euler = Euler;
			auto e = Vector3Scale(Euler, DEG2RAD);
			Quat = QuaternionFromEuler(e.x, e.y, e.z);
		}*/
		void SetEulerAngle(EulerAngle Angle, float Value)
		{
			//float diff{};
			//Vector3 axis{};
			switch (Angle)
			{
				case Utils::Rotator::EulerAngle::Pitch:
					//diff = Value - this->Euler.x;
					this->Euler.x = Value;
					//axis = Vector3UnitX;
					break;
				case Utils::Rotator::EulerAngle::Yaw:
					//diff = Value - this->Euler.y;
					this->Euler.y = Value;
					//axis = Vector3UnitY;
					break;
				case Utils::Rotator::EulerAngle::Roll:
					//diff = Value - this->Euler.z;
					this->Euler.z = Value;
					//axis = Vector3UnitZ;
					break;
			}
			//auto q = QuaternionFromAxisAngle(axis, diff * DEG2RAD);
			//Quat = QuaternionMultiply(Quat, q);
			auto e = Vector3Scale(Euler, DEG2RAD);
			Quat = QuaternionFromEuler(e.x, e.y, e.z);
		}
		void AddEulerAngle(EulerAngle Angle, float Value)
		{
			Vector3 axis{};
			switch (Angle)
			{
				case Utils::Rotator::EulerAngle::Pitch:
					this->Euler.x += Value;
					axis = Vector3UnitX;
					break;
				case Utils::Rotator::EulerAngle::Yaw:
					this->Euler.y += Value;
					axis = Vector3UnitY;
					break;
				case Utils::Rotator::EulerAngle::Roll:
					this->Euler.z += Value;
					axis = Vector3UnitZ;
					break;
			}
			auto q = QuaternionFromAxisAngle(axis, Value * DEG2RAD);
			Quat = QuaternionMultiply(Quat, q);
		}
		bool operator==(const Rotator& Other) const
		{
			return Euler == Other.Euler;
		}
		bool operator!=(const Rotator& Other) const
		{
			return Euler != Other.Euler;
		}
	};


	// TODO
	// Should have its own file?
	struct MapTransform
	{
		Vector3 Position;
		Rotator Rotation;
		Vector3 Scale;

		MapTransform(Vector3 Position, Rotator Euler, Vector3 Scale) :
			Position{ Position },
			Rotation{ Euler },
			Scale{ Scale }
		{}
		MapTransform() : 
			MapTransform(Vector3Zeros, {}, Vector3Ones) {}

		bool operator==(const MapTransform& Other) const
		{
			return Position == Other.Position
				&& Rotation == Other.Rotation
				&& Scale == Other.Scale;
		}
		bool operator!=(const MapTransform& Other) const
		{
			return Position != Other.Position
				|| Rotation != Other.Rotation
				|| Scale != Other.Scale;
		}
	};


	// TODO
	// Should have its own file?
	struct MapObjectId
	{
	private:
		static int GenerateNextId()
		{
			static int ObjectsCounterId = 0;
			return ++ObjectsCounterId;
		}
		int Id;
		MapObjectId(int Id) : Id{ Id } {}
	public:
		inline int GetId() const { return Id; }
		bool operator==(const MapObjectId& OtherId) const
		{
			return Id == OtherId.GetId();
		}
		bool operator!=(const MapObjectId& OtherId) const
		{
			return Id != OtherId.GetId();
		}
		inline bool IsEmpty() const { return Id == -1; }
		static MapObjectId GenerateNewId() { return { GenerateNextId() }; }
		static MapObjectId Empty() { return { -1 }; }
		MapObjectId() : MapObjectId(-1) {}
	};


	// TODO
	// Should have its own file?
	struct MapObjectBasicData
	{
		MapObjectId Id;
		int Level;
		std::string Name;
		//MapObjectBasicData(const MapObjectBasicData& Other) = default;
		//MapObjectBasicData& operator=(const MapObjectBasicData& Other) = default;
	};

	// TODO
	// Should have its own file?
	struct MapObjectProperties
	{
		MapObjectId Id;
		std::string Name;
		MapTransform Transform;
		std::string Tags;
		int ModelIdx, TexDiffuseIdx;
	};
};
