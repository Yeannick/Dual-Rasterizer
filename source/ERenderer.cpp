#include "pch.h"

//Project includes
#include "ERenderer.h"
#include "SceneGraph.h"
#include "MaterialEffect.h"
#include "BaseEffect.h"
#include "FlatEffect.h"

Elite::Renderer::Renderer(SDL_Window * pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);

	//Initialize DirectX pipeline
	//...
	m_IsInitialized = InitializedDirectX();
	std::cout << "DirectX is ready\n";

	m_pFrontBuffer = SDL_GetWindowSurface(m_pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
}

Elite::Renderer::~Renderer()
{
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
	}
	if (m_pRenderTargetBuffer)
	{
		m_pRenderTargetBuffer->Release();
	}
	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
	}
	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
	if (m_pDXGIFactory)
	{
		m_pDXGIFactory->Release();
	}
}

void Elite::Renderer::GetCamera(Elite::Camera* pCamera)
{
	m_pCamera = pCamera;
}
bool IsDegenerate(unsigned int v0, unsigned int v1, unsigned int v2)
{
	if (v0 == v1 || v0 == v2 || v1 == v2)
	{
		return true;
	}

	return false;
}
void Elite::Renderer::Render()
{

	switch (SceneGraph::GetInstance()->GetRenderMode())
	{
	case RenderMode::DirectX : 
	{
		if (!m_IsInitialized)
			return;

		// Clear buffers
		RGBColor clearColor = RGBColor(0.164f, 0.164f, 0.164f);
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		auto meshes = SceneGraph::GetInstance()->m_pMeshes;
		// Render

		for (auto m : meshes)
		{
			BaseEffect* effect = m->GetEffect();

			effect->SetWorldViewProjectionMatrix(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(), m->GetWorldMatrix());
			effect->SetDiffuseMap(m->GetTexture()->GetTextureResourceView());


			switch (effect->GetEffectType())
			{
			case EffectType::Material:
			{

				MaterialEffect* materialEffect = static_cast<MaterialEffect*>(effect);

				materialEffect->SetWorldMatrix(m->GetWorldMatrix());
				materialEffect->SetViewInverseMatrix(m_pCamera->GetInverseViewMatrix());


				materialEffect->SetNormalMap(m->GetNormalMap()->GetTextureResourceView());
				materialEffect->SetSpecularMap(m->GetSpecularMap()->GetTextureResourceView());
				materialEffect->SetGlossinessMap(m->GetGlossinessMap()->GetTextureResourceView());

			}
			break;
			case EffectType::Flat:
			{
				FlatEffect* flatEffect = static_cast<FlatEffect*>(effect);

				//flatEffect->SetWorldMatrix(m->GetWorldMatrix());
				//flatEffect->SetViewInverseMatrix(m_pCamera->GetInverseViewMatrix());


				//flatEffect->SetNormalMap(m->GetNormalMap().GetTextureResourceView());
				//flatEffect->SetSpecularMap(m->GetSpecularMap().GetTextureResourceView());
				//flatEffect->SetGlossinessMap(m->GetGlossinessMap().GetTextureResourceView());

			}
			break;
			default:
				break;
			}
			if (m->CanRender())
			{
				m->Render(m_pDeviceContext);
			}
			
		}
		m_pSwapChain->Present(0, 0);
	}
	break;
	case RenderMode::Rasterizer:
	{
		SDL_LockSurface(m_pBackBuffer);
		// create depthBuffer with size of all pixels in screen
		float* depthBuffer = new float[uint64_t(m_Width) * uint64_t(m_Height)]{};
		for (int i{}; i < uint64_t(m_Width) * uint64_t(m_Height); i++)
		{
			// fill in the depthbuffer with max float 
			depthBuffer[i] = FLT_MAX;
		}

		SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 130, 130, 130));
		// for each triangle , triangle mesh is an object consisting of multiple triangles
		for (Mesh* pMesh : SceneGraph::GetInstance()->m_pMeshes)
		{
			// create vertex and index - buffer 
			std::vector<Vertex> vertexBuffer = pMesh->GetVertexBuffer();
			std::vector<unsigned int> indexBuffer = pMesh->GetIndexBuffer();
			// get the topology method from triangle mesh
			PrimitiveTopology topologyType = pMesh->GetTopologyType();
			// get the texture pointers of different textures types
			const Texture* pDiffuseTexture = pMesh->GetTexture();
			const Texture* pNormalTexture = pMesh->GetNormalMap();
			const Texture* pSpecularTexture = pMesh->GetSpecularMap();
			const Texture*pGlossinessTexture = pMesh->GetGlossinessMap();

			//pNormalTexture->Sample(FVector2{0.5,0.5});
			// position and rotation of the triangle mesh currently being evaulated
			FPoint3 position = pMesh->GetPosition();
			FVector3 rotation = pMesh->GetRotation();
			// switch case to get increment value to go through the index buffer 
			unsigned int increment{};
			switch (topologyType)
			{
			case PrimitiveTopology::TriangleList:
				increment = 3;
				break;
			case PrimitiveTopology::TriangleStrip:
				increment = 1;
				break;
			default:
				break;
			}
			// loop through indexbuffer with increment value from previous step
			for (unsigned int i{}; i <= indexBuffer.size() - 3; i += increment)
			{
				// Model space vertices (since we're rendering the model at the center of our world, these are also world space)
				// make Triangle object of trianglemesh and vertexbuffer found by using our indexbuffer
				Triangle triangle{ vertexBuffer[indexBuffer[i]], vertexBuffer[indexBuffer[i + 1]], vertexBuffer[indexBuffer[i + 2]] };

				if ((topologyType == PrimitiveTopology::TriangleStrip) && (i % 2 != 0))
				{
					// if using triangleStrip we check if it is a degenerate triangle ( meaning no surface area so it will not be rendered)
					if (IsDegenerate(indexBuffer[i], indexBuffer[i + 1], indexBuffer[i + 2])) continue;
					// if not than we swap V1 and V2 to make this clockwise order
					triangle.SwapV1V2();
				}

				// Create WorldTranslation matrix
				FMatrix4 WorldTranslationMatrix{
					1, 0, 0, position.x,
					0, 1, 0, position.y,
					0, 0, 1, position.z,
					0, 0, 0, 1
				};

				// Create WorldRotationY matrix
				FMatrix4 WorldRotationYMatrix{
					cosf(rotation.y), 0, sinf(rotation.y), 0,
					0, 1, 0, 0,
					-sinf(rotation.y), 0, cosf(rotation.y), 0,
					0, 0, 0, 1
				};

				// Create View matrix
				FMatrix4 ViewMatrix = m_pCamera->GetViewMatrix();
			
				// Create Projection matrix
				FMatrix4 ProjectionMatrix = m_pCamera->GetProjectionMatrix();

				// Transform the vertices
				FPoint4 v0 = ProjectionMatrix * ViewMatrix * WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV0().Position.x, triangle.GetV0().Position.y, triangle.GetV0().Position.z, triangle.GetV0().Position.z };
				FPoint4 v1 = ProjectionMatrix * ViewMatrix * WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV1().Position.x, triangle.GetV1().Position.y, triangle.GetV1().Position.z, triangle.GetV1().Position.z };
				FPoint4 v2 = ProjectionMatrix * ViewMatrix * WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV2().Position.x, triangle.GetV2().Position.y, triangle.GetV2().Position.z, triangle.GetV2().Position.z };

				// Transform the normals
				FVector3 n0 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV0().Normal.x, triangle.GetV0().Normal.y, triangle.GetV0().Normal.z, 1 }).xyz;
				FVector3 n1 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV1().Normal.x, triangle.GetV1().Normal.y, triangle.GetV1().Normal.z, 1 }).xyz;
				FVector3 n2 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV2().Normal.x, triangle.GetV2().Normal.y, triangle.GetV2().Normal.z, 1 }).xyz;

				// Transform the tangents
				FVector3 t0 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV0().Tangent.x, triangle.GetV0().Tangent.y, triangle.GetV0().Tangent.z, 1 }).xyz;
				FVector3 t1 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV1().Tangent.x, triangle.GetV1().Tangent.y, triangle.GetV1().Tangent.z, 1 }).xyz;
				FVector3 t2 = (WorldTranslationMatrix * WorldRotationYMatrix * FVector4{ triangle.GetV2().Tangent.x, triangle.GetV2().Tangent.y, triangle.GetV2().Tangent.z, 1 }).xyz;

				// Calculate the camera's view vector towards each vertex in world space
				FVector3 viewDir0 = (WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV0().Position.x, triangle.GetV0().Position.y, triangle.GetV0().Position.z, 1 }).xyz - m_pCamera->GetPosition();
				FVector3 viewDir1 = (WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV1().Position.x, triangle.GetV1().Position.y, triangle.GetV1().Position.z, 1 }).xyz - m_pCamera->GetPosition();
				FVector3 viewDir2 = (WorldTranslationMatrix * WorldRotationYMatrix * FPoint4{ triangle.GetV2().Position.x, triangle.GetV2().Position.y, triangle.GetV2().Position.z, 1 }).xyz - m_pCamera->GetPosition();

				v0.x /= v0.w;
				v0.y /= v0.w;
				v0.z /= v0.w;

				v1.x /= v1.w;
				v1.y /= v1.w;
				v1.z /= v1.w;

				v2.x /= v2.w;
				v2.y /= v2.w;
				v2.z /= v2.w;

				// Calculate screenspace vertices
				FPoint2 v0ss{ (v0.x + 1) / 2 * m_Width, (1 - v0.y) / 2 * m_Height };
				FPoint2 v1ss{ (v1.x + 1) / 2 * m_Width, (1 - v1.y) / 2 * m_Height };
				FPoint2 v2ss{ (v2.x + 1) / 2 * m_Width, (1 - v2.y) / 2 * m_Height };

				// IPoint2 to create bounding box's topleft and bottomRight points
				IPoint2 topLeft, btmRight;
				// create bounding box 
				Triangle::GetBounds(topLeft, btmRight, int(m_Width), int(m_Height), v0ss, v1ss, v2ss);
				// where we'll store our current pixel
				FPoint2 p{};

				for (uint32_t r = uint32_t(topLeft.y); r < uint32_t(btmRight.y); ++r) //Loop over the triangle's bounding box
				{
					p.y = float(r);
					for (uint32_t c = uint32_t(topLeft.x); c < uint32_t(btmRight.x); ++c)
					{
						p.x = float(c);
						// if pixel point is in triangle
						if (triangle.IsPointInTriangle(p, v0ss, v1ss, v2ss))
						{

							float zBufferValue = 1 / (1 / v0.z * triangle.GetW0() + 1 / v1.z * triangle.GetW1() + 1 / v2.z * triangle.GetW2());

							if ((zBufferValue >= 0) && (zBufferValue <= 1) && (zBufferValue < depthBuffer[c + (r * m_Width)]))
							{
								// final calculation and depthbuffer check
								depthBuffer[c + (r * m_Width)] = zBufferValue;
								float interpolatedW = 1 / (1 / v0.w * triangle.GetW0() + 1 / v1.w * triangle.GetW1() + 1 / v2.w * triangle.GetW2());

								FVector2 uv = (triangle.GetV0().Uv / v0.w * triangle.GetW0() + triangle.GetV1().Uv / v1.w * triangle.GetW1() + triangle.GetV2().Uv / v2.w * triangle.GetW2()) * interpolatedW;
								FVector3 n = (n0 / v0.w * triangle.GetW0() + n1 / v1.w * triangle.GetW1() + n2 / v2.w * triangle.GetW2()) * interpolatedW;
								FVector3 t = (t0 / v0.w * triangle.GetW0() + t1 / v1.w * triangle.GetW1() + t2 / v2.w * triangle.GetW2()) * interpolatedW;
								FVector3 v = (viewDir0 / v0.w * triangle.GetW0() + viewDir1 / v1.w * triangle.GetW1() + viewDir2 / v2.w * triangle.GetW2()) * interpolatedW;
								Normalize(v);

								RGBColor finalColor{};
							/*	if (m_ShowDepthBuffer)
								{
									float depth = Remap(zBufferValue, 0.985f, 1.f);
									finalColor = RGBColor{ depth, depth, depth };
								}
								else
								{*/
									// make our final color by calculating it
								finalColor = PixelShading(triangle, pDiffuseTexture, pNormalTexture, pSpecularTexture, pGlossinessTexture, uv, n, t, v);
								/*}*/

								m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
									static_cast<uint8_t>(finalColor.r * 255.f),
									static_cast<uint8_t>(finalColor.g * 255.f),
									static_cast<uint8_t>(finalColor.b * 255.f));
							}
						}
					}
				}
			}
		}
		// delete depthbuffer;
		delete[] depthBuffer;
		depthBuffer = nullptr;
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
		
	}
	break;
	default:
		break;
	}
	
}
RGBColor Phong(float specularReflectance, int phongExponent, const FVector3& l, const FVector3& v, const FVector3& hitNormal)
{
	const FVector3 reflect = -l + 2 * Dot(hitNormal, l) * hitNormal;
	const float cosa = Dot(reflect, v);

	float value = specularReflectance * powf(abs(cosa), float(phongExponent)); //phongSpecularReflection

	return RGBColor{ value, value, value };
}
RGBColor Elite::Renderer::PixelShading(const Triangle& triangle, const Texture* pTexture, const Texture* pNormalTexture, const Texture* pSpecularTexture, const Texture* pGlossinessTexture, const FVector2& uv, const FVector3& n, const FVector3& t, const FVector3& v)
{
	RGBColor finalColor{};
	RGBColor diffuseMapColor{};
	RGBColor phongColor{};

	FVector3 lightDirection = { .577f, -.577f, -.577f };
	RGBColor lightColor{ 1.0f, 1.0f, 1.0f };
	float intensity{ 7.f };

	float observedArea{};

	if (pNormalTexture != nullptr)
	{

		FVector3 binormal = GetNormalized(Cross(t, n));
		FMatrix3 tangentSpaceAxis = FMatrix3(t, binormal, n);

		RGBColor normalMapColor = pNormalTexture->Sample(uv);
		FVector3 newNormal = FVector3{ normalMapColor.r, normalMapColor.g, normalMapColor.b };

		newNormal = (newNormal * 2.f) - FVector3{ 1.f, 1.f, 1.f };
		newNormal = tangentSpaceAxis * newNormal;

		observedArea = (Dot(-newNormal, lightDirection));

		if (pSpecularTexture != nullptr && pGlossinessTexture != nullptr)
		{
			float shininess{ 25.f };

			RGBColor specularColor = pSpecularTexture->Sample(uv);
			RGBColor glossinessColor = pGlossinessTexture->Sample(uv);
			glossinessColor *= shininess;
		
			
			phongColor = Phong(specularColor.r, int(glossinessColor.r), lightDirection, v, FVector3{ normalMapColor.r,normalMapColor.g,normalMapColor.b });
			
		}



	}
	else
	{
		observedArea = (Dot(-n, lightDirection));
	}

	if (observedArea >= 0)
	{
		if (pTexture != nullptr)
		{
			diffuseMapColor = pTexture->Sample(uv);
		}
		else
		{
			diffuseMapColor = triangle.GetV0().Color * triangle.GetW0() + triangle.GetV1().Color * triangle.GetW1() + triangle.GetV2().Color * triangle.GetW2();
		}

		float diffuseReflectance{ 1.f };
		RGBColor diffuseLambert = diffuseMapColor * diffuseReflectance / float(E_PI);

		finalColor = lightColor * intensity * (diffuseLambert + phongColor) * observedArea;
		finalColor.MaxToOne();
	}

	return finalColor;
}
		
	
	

	// Present
	
	
	


bool Elite::Renderer::InitializedDirectX()
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t createDeviceFlags = 0;
#if defined (DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result))
	{
		return result;
	}

	// Create DXGI Factory to create SwapChain based on hardware

	result = CreateDXGIFactory(_uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result))
	{
		return result;
	}

	//CREATE SWAPCHAIN DESCRIPTOR
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// get the handle HWND from the SDL backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	// create swapchain and hook it into the handle of the SDL window

	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
	{
		return result;
	}

	//Create the Depth/Stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
	{
		return result;
	}
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
	{
		return result;
	}

	// create the RenderTargetView
	result = m_pSwapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
	{
		return result;
	}
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
	{
		return result;
	}
	// Bind the views to the Output Merger Stage

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// Set the ViewPort
	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return true;

}


