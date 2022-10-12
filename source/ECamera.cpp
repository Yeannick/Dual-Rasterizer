#include "pch.h"
#include "ECamera.h"
#include <SDL.h>
#include "SceneGraph.h"

namespace Elite
{
	Camera::Camera(const Elite::FPoint3& position, const Elite::FVector3& direction, float screenWidth, float screenHeight, float nearPlane, float farPlane, float FOV) :
		m_Location(position),
		m_Direction(direction),
		m_AspectRatio(screenWidth / screenHeight),
		m_NearPlane(nearPlane),
		m_FarPlane(farPlane),
		m_FOV(tanf(Elite::ToRadians(FOV) / 2)),
		m_Right(Elite::Cross(Elite::FVector3{ 0,1,0 }, direction)),
		m_Up(Elite::Cross(direction, m_Right)),
		m_LookAtMatrix(),
		m_ProjectionMatrix()

	{
		int renderMode = int(SceneGraph::GetInstance()->GetRenderMode());
		m_ProjectionMatrix = { 1 / (m_AspectRatio * m_FOV), 0, 0, 0,
								0, 1 / m_FOV, 0, 0,
								0, 0, m_FarPlane / (m_FarPlane - m_NearPlane) * renderMode, (-(m_FarPlane * m_NearPlane) / ((m_FarPlane - m_NearPlane) * renderMode)) * renderMode,
								0, 0, (float)renderMode	, 0
			};
		ReconstructONB();
	}


	//Renamed GetLookAtMatrix() to match the slides
	//Actually the inversed viewmatrix,
	//but we use the inverse as the normal viewmatrix to not make the formulas any more confusing
	const Elite::FMatrix4& Camera::GetViewMatrix() const
	{
		return m_InverseLookAtMatrix;
	}

	//Actually the original viewmatrix, but named like this to not make the formulas any more confusing
	const Elite::FMatrix4& Camera::GetInverseViewMatrix() const
	{
		return m_LookAtMatrix;
	}

	const Elite::FMatrix4& Camera::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	const Elite::FVector3& Camera::GetViewDirection() const
	{
		return m_Direction;
	}

	const float Camera::GetNearPlane() const
	{
		return m_NearPlane;
	}

	const float Camera::GetFarPlane() const
	{
		return m_FarPlane;
	}

	void Camera::RecalculateCamera()
	{
		int renderMode = int(SceneGraph::GetInstance()->GetRenderMode());
		m_ProjectionMatrix = { 1 / (m_AspectRatio * m_FOV), 0	 , 0													 , 0
						 , 0						  , 1 / m_FOV, 0													 , 0
						 , 0						  , 0		 , m_FarPlane / ((m_FarPlane - m_NearPlane) * renderMode), (-(m_FarPlane * m_NearPlane) / ((m_FarPlane - m_NearPlane) * renderMode)) * renderMode
						 , 0						  , 0		 , (float)renderMode									 , 0 };
		ReconstructONB();
	}


	void Camera::Update(float elapsedSec)
	{
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(0);
		float keyboardSpeed = pKeyboardState[SDL_SCANCODE_LSHIFT] ? m_KeyboardMoveSensitivity * m_KeyboardMoveMultiplier : m_KeyboardMoveSensitivity;
		m_RelativeTranslation.x = (pKeyboardState[SDL_SCANCODE_D] - pKeyboardState[SDL_SCANCODE_A]) * keyboardSpeed * elapsedSec;
		m_RelativeTranslation.y = 0;
		m_RelativeTranslation.z = -(pKeyboardState[SDL_SCANCODE_W] - pKeyboardState[SDL_SCANCODE_S]) * keyboardSpeed * elapsedSec;
		
		//Mouse Input
		int x, y = 0;
		uint32_t mouseState = SDL_GetRelativeMouseState(&x, &y);
		if (mouseState == SDL_BUTTON_LMASK)
		{
			m_RelativeTranslation.z += y * m_MouseMoveSensitivity * elapsedSec;
			m_AbsoluteRotation.y -= x * m_MouseRotationSensitivity;
		}
		else if (mouseState == SDL_BUTTON_RMASK)
		{
			
			m_AbsoluteRotation.x -= y * m_MouseRotationSensitivity;
			m_AbsoluteRotation.y -= x * m_MouseRotationSensitivity;
		}
		else if (mouseState == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK))
		{
			m_RelativeTranslation.y -= y * m_MouseMoveSensitivity * elapsedSec;
		}
		ReconstructONB();
	}


	void Camera::ReconstructONB()
	{
		int renderMode = int(SceneGraph::GetInstance()->GetRenderMode());

		Elite::FVector3 forward{  };
		FMatrix3 yawRotation = MakeRotationY(m_AbsoluteRotation.y * float(E_TO_RADIANS));
		forward = yawRotation * m_Direction;

		forward = { -forward.x * renderMode , -forward.y * renderMode , forward.z };
		Normalize(forward);

		Elite::FVector3 right = Elite::GetNormalized(Elite::Cross(Elite::FVector3{ 0, 1, 0 }, forward));
		
		FMatrix3 pitchRotation = MakeRotation(m_AbsoluteRotation.x * float(E_TO_RADIANS), right);
		forward = pitchRotation * forward;
	
		Elite::FVector3 up = Elite::Cross(forward, right);

		m_Location += m_RelativeTranslation.x * right;
		m_Location += m_RelativeTranslation.y * up;
		m_Location += m_RelativeTranslation.z * forward;

		m_LookAtMatrix = { right.x ,up.x	,forward.x ,m_Location.x
						, right.y  ,up.y	,forward.y ,m_Location.y
						, right.z  ,up.z	,forward.z ,(-m_Location.z )
						, 0		   ,0		, 0		   ,1 };


		m_InverseLookAtMatrix = Elite::Inverse(m_LookAtMatrix);
	}

	//void Camera::CalculateLookAt()
	//{
	//	
	//	//FORWARD (zAxis) with YAW applied
	//	FMatrix3 yawRotation = MakeRotationY(m_AbsoluteRotation.y * float(E_TO_RADIANS));
	//	FVector3 zAxis = yawRotation * m_ViewForward;
}