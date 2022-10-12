#pragma once
// Copyright 2021 Elite Engine 2.0
// Authors: Thomas Goussaert
/*=============================================================================*/
// ECamera.h: Base Camera Implementation with movement
/*=============================================================================*/
// Changed constructor , added More Getters 
#pragma once
#include "EMath.h"
namespace Elite
{
	class Camera
	{
	public:

		Camera(const FPoint3& position , const FVector3& direction , float screenWidth, float screenHeight, float nearPlane = .1f, float farPlane = 100.f, float FOVAngle = 60.f);
		~Camera() = default;

		const Elite::FPoint3&  GetPosition()const { return m_Location; }
		const Elite::FMatrix4& GetViewMatrix() const;
		const Elite::FMatrix4& GetInverseViewMatrix()const;
		const Elite::FMatrix4& GetProjectionMatrix() const;
		const Elite::FVector3& GetViewDirection()const;
		void RecalculateCamera();
		const float GetFov() const { return m_FOV; }
		const float GetNearPlane() const;
		const float GetFarPlane() const;

		

		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;

		void Update(float elapsedSec);

	/*	const FMatrix4& GetWorldToView() const { return m_WorldToView; }
		const FMatrix4& GetViewToWorld() const { return m_ViewToWorld; }*/

		
		
	private:
		//void CalculateLookAt();
		Elite::FPoint3 m_Location;
		Elite::FVector3 m_Direction;
		float m_AspectRatio;
		float m_NearPlane;
		float m_FarPlane;
		float m_FOV;

		Elite::FVector3 m_Right;
		Elite::FVector3 m_Up;
		Elite::FMatrix4 m_LookAtMatrix;
		Elite::FMatrix4 m_InverseLookAtMatrix;
		Elite::FMatrix4 m_ProjectionMatrix;

		void ReconstructONB();
		const float m_KeyboardMoveSensitivity{ 2.f };
		const float m_KeyboardMoveMultiplier{ 10.f };
		const float m_MouseRotationSensitivity{ .1f };
		const float m_MouseMoveSensitivity{ 2.f };

		FPoint2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
		FPoint3 m_RelativeTranslation{};
	};
}

		