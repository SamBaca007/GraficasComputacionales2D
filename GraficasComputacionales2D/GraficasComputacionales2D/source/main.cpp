#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/SteeringSystem.h" // Ya lo tenías incluido, ¡perfecto!

Window g_window(Window(800, 600, "Surreal Engine 2D"));
ECS::Registry registry;

void destroy()
{
	ImGui::SFML::Shutdown();
}

int main() {
	// ==========================================
	// REGISTRO DE SISTEMAS
	// El orden importa: Primero calculamos IA/Física, luego Cámara, luego Render y al final la UI
	// ==========================================
	registry.AddSystem<ECS::SteeringSystem>(); // <-- NUEVO: Sistema de Steering agregado aquí
	registry.AddSystem<ECS::CameraSystem>(g_window);
	registry.AddSystem<ECS::RenderSystem>(g_window);
	registry.AddSystem<ECS::UISystem>();

	if (!ImGui::SFML::Init(*g_window.m_window)) {
		return -1;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	sf::Clock deltaClock;
	bool showDemoWindow = true;

	// ==========================================
	// CREACIÓN DE ENTIDADES
	// ==========================================
	ECS::EntityID circle = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
	registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE,
		sf::Color(100, 250, 50), "Textures/Bricks.png"));

	ECS::EntityID tri = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
	registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

	// ---> NUEVO: Nuestro Agente Inteligente <---
	ECS::EntityID agent = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ 100.f, 100.f });
	registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green));

	auto& kinematic = registry.AddComponent<ECS::Kinematic>(agent);
	kinematic.maxSpeed = 250.f;
	kinematic.maxForce = 200.f;

	auto& steering = registry.AddComponent<ECS::Steering>(agent);
	steering.currentBehavior = ECS::SteeringBehaviorType::Seek; // Inicia buscando
	steering.target = sf::Vector2f{ 600.f, 400.f };             // Va hacia este punto
	// -------------------------------------------

	ECS::EntityID cam = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 400.f, 300.f });
	auto& camComp = registry.AddComponent<ECS::Camera>(cam);
	camComp.followTarget = circle; // la cámara sigue a su objetivo
	camComp.followSpeed = 5.f; // sube para que se pegue más rápido
	camComp.zoom = 1;

	// Forzamos a la ventana a inicializar su View interna con su propio tamaño actual
	g_window.handleResize(g_window.m_window->getSize());

	while (g_window.isOpen()) {

		while (const std::optional event = g_window.m_window->pollEvent()) {
			// ImGui debe recibir todos los eventos de SFML
			ImGui::SFML::ProcessEvent(*g_window.m_window, *event);
			if (event->is<sf::Event::Closed>()) {
				g_window.close();
			}
			// Resize event: Actualiza la vista al nuevo tamaño de la ventana
			else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
				g_window.handleResize(resized->size);
			}
		}

		const sf::Time elapsedTime = deltaClock.restart();
		const float dt = elapsedTime.asSeconds();

		// Inicia el frame de ImGui.
		ImGui::SFML::Update(*g_window.m_window, elapsedTime);

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
		ImGui::ShowDemoWindow(&showDemoWindow);

		// Limpia la ventana
		g_window.clear(sf::Color::Black);

		// Esto ejecutará SteeringSystem, luego CameraSystem, luego RenderSystem y al final UISystem
		registry.UpdateSystems(dt);

		// Presentar el frame
		ImGui::SFML::Render(*g_window.m_window);
		g_window.display();
	}
	destroy();

	return 0;
}