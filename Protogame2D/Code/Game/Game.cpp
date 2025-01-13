#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Texture.hpp"

Game::Game()
{
}

Game::~Game()
{
}

void Game::StartUp()
{
	m_gameClock = new Clock(Clock::GetSystemClock());

	m_screenCamera = new Camera();
	m_worldCamera = new Camera();

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU));
	m_testTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Textures/Terrain_8x8.png");

	AddVertsForAABB2D(m_cpuMesh, AABB2(Vec2(700.0f, 300.0f), Vec2(1500.0f, 700.0f)), Rgba8::WHITE);
	AddVertsForAABB2D(m_cpuMesh, AABB2(Vec2(600.0f, 200.0f), Vec2(1000.0f, 600.0f)), Rgba8::WHITE);
	
	g_theRenderer->CopyCPUToGPU(m_cpuMesh.data(), m_cpuMesh.size() * sizeof(Vertex_PCU), m_gpuMesh);
}

void Game::Shutdown()
{
	DELETE_PTR(m_testTexture);
	DELETE_PTR(m_gpuMesh);

	DELETE_PTR(m_worldCamera);
	DELETE_PTR(m_screenCamera);
}

void Game::Update(float deltaseconds)
{
	static float rate = 0.0f;
	rate += deltaseconds;
	m_thickness = 50.0f * SinDegrees(rate);

	if (m_isAttractMode)
	{
		UpdateAttractMode(deltaseconds);

 		m_screenCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	}
	else
	{
		m_cpuMesh.clear();

		AddVertsForDisc2D(m_cpuMesh, Vec2(100.0f, 50.0f), 10.0f, Rgba8::RED);
		
		g_theRenderer->CopyCPUToGPU(m_cpuMesh.data(), m_cpuMesh.size() * sizeof(Vertex_PCU), m_gpuMesh);

		m_worldCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	}
	
	HandleInput();
	UpdateFromController(deltaseconds);
}

void Game::Render() const
{
	if (m_isAttractMode)
	{
		g_theRenderer->BeginCamera(*m_screenCamera, RootSig::DEFAULT_PIPELINE);
		
		RenderAttractMode();
		
		g_theRenderer->EndCamera(*m_screenCamera);
	}
	else
	{
		g_theRenderer->BeginCamera(*m_worldCamera, RootSig::DEFAULT_PIPELINE);
		
		VertexBuffer* gpuMesh = m_gpuMesh;

		g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
		g_theRenderer->BindShader();
		g_theRenderer->BindTexture();
		g_theRenderer->DrawVertexBuffer(gpuMesh, (int)m_cpuMesh.size(), sizeof(Vertex_PCU));
		
		g_theRenderer->EndCamera(*m_worldCamera);
	}
}

void Game::RenderAttractMode() const
{
	VertexBuffer* gpuMesh = m_gpuMesh;

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture(0, m_testTexture);
	g_theRenderer->DrawVertexBuffer(gpuMesh, (int)m_cpuMesh.size(), sizeof(Vertex_PCU));
}

void Game::HandleInput()
{
	if (m_isAttractMode)
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			g_theApp->HandleQuitRequested();
		}

		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_SPACE))
		{
			SoundID soundHit = g_theAudio->CreateOrGetSound("Data/Audio/Player_Laser.wav");
			g_theAudio->StartSound(soundHit);
			m_isAttractMode = false;
		}
	}
	else
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			m_isAttractMode = true;
		}
	}
}

void Game::UpdateAttractMode(float deltaseconds)
{
	UNUSED(deltaseconds);
	
	m_cpuMesh.clear();
	
	AddVertsForAABB2D(m_cpuMesh, AABB2(Vec2(700.0f, 300.0f), Vec2(1500.0f, 700.0f)), Rgba8::WHITE);
	AddVertsForAABB2D(m_cpuMesh, AABB2(Vec2(600.0f, 200.0f), Vec2(1000.0f, 600.0f)), Rgba8::WHITE);
	
	g_theRenderer->CopyCPUToGPU(m_cpuMesh.data(), m_cpuMesh.size() * sizeof(Vertex_PCU), m_gpuMesh);
}

void Game::UpdateFromController(float deltaseconds)
{
	UNUSED(deltaseconds);

	XboxController const& controller = g_theInputSystem->GetController(0);

	if (m_isAttractMode)
	{
		if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_B))
		{
			g_theApp->HandleQuitRequested();
		}
	}
}