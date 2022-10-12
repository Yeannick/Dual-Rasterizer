/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Mesh.h"
#include "ECamera.h"
#include "Triangle.h"

struct SDL_Window;
struct SDL_Surface;


struct BoundingBox
{
	float left, right, top, bottom;
};
namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void GetCamera(Elite::Camera* pCamera);
		ID3D11Device* GetDevice() { return m_pDevice; }
		void Render();
		 
		// rasterizer mode only 
		RGBColor PixelShading(const Triangle& triangle, const Texture* pTexture, const Texture* pNormalTexture, const Texture* pSpecularTexture, const Texture* pGlossinessTexture, const FVector2& uv, const FVector3& n, const FVector3& t, const FVector3& v);

	private:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;

		bool m_IsInitialized;

		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		IDXGIFactory* m_pDXGIFactory = nullptr;
		IDXGISwapChain* m_pSwapChain = nullptr;

		ID3D11Resource* m_pRenderTargetBuffer = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

		ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

		bool InitializedDirectX();

		Mesh* m_pMesh = nullptr;
		Elite::Camera* m_pCamera = nullptr;

		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
	};
}

#endif