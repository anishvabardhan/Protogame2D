#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"

class Entity;
class Clock;
class VertexBuffer;
class Texture;

class Game
{
public:
	Camera*				m_worldCamera					= nullptr;
	Camera*				m_screenCamera					= nullptr;
	Clock*				m_gameClock						= nullptr;

	std::vector<Vertex_PCU> m_cpuMesh;
	VertexBuffer*		m_gpuMesh						= nullptr;
	Texture*			m_testTexture					= nullptr;

	float				m_thickness						= 1.0f;

	Vec2				m_attractModePosition			= Vec2(500.0f, 400.0f);
	bool				m_isAttractMode = true;
public:
						Game();
						~Game();

	void				StartUp();
	void				Shutdown();

	void				Update(float deltaseconds);
	void				Render() const;

	void				HandleInput();
	void				UpdateFromController(float deltaseconds);
	void				UpdateAttractMode(float deltaseconds);

	void				RenderAttractMode() const;
};