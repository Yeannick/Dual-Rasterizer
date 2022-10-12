#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"

#include "ECamera.h"
#include "SceneGraph.h"
#include "Obj.h"
#include "EffectManager.h"
#include "MaterialEffect.h"

#ifdef _DEBUG
#include <vld.h>
#endif // _DEBUG

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 1280;
	const uint32_t height = 960;
	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - **Yannick_Godeau**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };
	ID3D11Device* pDevice = pRenderer->GetDevice();
	Elite::Camera* pCamera;
	EffectManager::GetInstance()->AddEffect("FlameEffect", new FlatEffect{ pDevice,L"Resources/Transparency.fx" });
	Mesh* FlameEffect = new Mesh(FPoint3{ 0, -5.f, 55.f }, "Resources/fireFX_diffuse.png", "", "", "", pRenderer->GetDevice(), EffectManager::GetInstance()->GetEffect("FlameEffect"), "Resources/fireFX.obj");
	if (SceneGraph::GetInstance()->GetRenderMode() == RenderMode::DirectX)
	{
		// For DirectX version you cn comment out the flameEffect and the Flame mesh , i commented it because our Software version cannot load this correctly 
		 pCamera = new Elite::Camera{ { 0.f, 0.f, -10.f }, {0.f, 0.f, 1.f}, width, height };
		ID3D11Device* pDevice = pRenderer->GetDevice();

		EffectManager::GetInstance()->AddEffect("VehicleEffect", new MaterialEffect{ pDevice, L"Resources/PosCol3D.fx" });
		


		SceneGraph::GetInstance()->AddObjectToGraph(new Mesh(FPoint3{ 0, -5.f, 55.f }, "Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png", "Resources/vehicle_specular.png", "Resources/vehicle_gloss.png", pDevice, EffectManager::GetInstance()->GetEffect("VehicleEffect"), "Resources/vehicle.obj"));
		SceneGraph::GetInstance()->AddObjectToGraph(FlameEffect);

	}
	else
	{
		 pCamera = new Elite::Camera{ { 0.f, 0.f, 10.f }, {0.f, 0.f, -1.f}, width, height };
		

		EffectManager::GetInstance()->AddEffect("VehicleEffect", new MaterialEffect{ pDevice, L"Resources/PosCol3D.fx" });
		//EffectManager::GetInstance()->AddEffect("FlameEffect", new FlatEffect{ pDevice,L"Resources/Transparency.fx" });

		SceneGraph::GetInstance()->AddObjectToGraph(new Mesh(FPoint3{ 0, -5.f, 55.f }, "Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png", "Resources/vehicle_specular.png", "Resources/vehicle_gloss.png", pDevice, EffectManager::GetInstance()->GetEffect("VehicleEffect"), "Resources/vehicle.obj"));

	}
	
	
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	
	float amount = 0;
	
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F)
				{
					if (SceneGraph::GetInstance()->GetRenderMode() == RenderMode::DirectX)
					{
						Mesh::ToggleFilterMethod();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_E) 
				{

					SceneGraph::GetInstance()->ToggleRenderMode();
					if (SceneGraph::GetInstance()->GetRenderMode()== RenderMode::Rasterizer)
					{
						delete pCamera;
						pCamera = new Elite::Camera{ { 0.f, 0.f, 10.f }, {0.f, 0.f, -1.f}, width, height };
						pCamera->RecalculateCamera();
						//EffectManager::GetInstance()->RemoveEffect("FlameEffect");
						SceneGraph::GetInstance()->RemoveFromGraph(FlameEffect);
					}
					else if (SceneGraph::GetInstance()->GetRenderMode() == RenderMode::DirectX)
					{
						delete pCamera;
						pCamera = new Elite::Camera{ { 0.f, 0.f, -10.f }, {0.f, 0.f, 1.f}, width, height };
						pCamera->RecalculateCamera();
						//EffectManager::GetInstance()->AddEffect("FlameEffect", new FlatEffect{ pDevice,L"Resources/Transparency.fx" });
						SceneGraph::GetInstance()->AddObjectToGraph(FlameEffect);
					}
				
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
				{
					if (amount != 0)
					{
						amount = 0;
					}
					else
					{
						amount = 25.f;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_T)
				{
					if (SceneGraph::GetInstance()->GetRenderMode() == RenderMode::DirectX)
					{
						SceneGraph::GetInstance()->m_pMeshes.back()->ToggleRender();
					}
					
				}
				
				break;
			}
		}
		SceneGraph::GetInstance()->m_pMeshes[0]->RotateY(-amount ,pTimer->GetElapsed());
		pRenderer->GetCamera(pCamera);
		if (SceneGraph::GetInstance()->GetRenderMode() == RenderMode::DirectX)
		{
			EffectManager::GetInstance()->GetEffect("VehicleEffect")->Rotate(amount, pTimer->GetElapsed());
			EffectManager::GetInstance()->GetEffect("FlameEffect")->Rotate(amount, pTimer->GetElapsed());

		}
		//--------- Render ---------
		pRenderer->Render();
		
		//--------- Timer ---------
		pTimer->Update();
		pCamera->Update(pTimer->GetElapsed());
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

	}
	pTimer->Stop();
	
	
	//Shutdown "framework"
	delete SceneGraph::GetInstance();
	delete EffectManager::GetInstance();
	delete pCamera;

	ShutDown(pWindow);
	return 0;
}
