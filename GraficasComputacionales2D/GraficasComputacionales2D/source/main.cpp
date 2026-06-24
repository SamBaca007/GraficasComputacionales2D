#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/UISystem.h"

Window g_window(Window(800, 600, "Surreal Engine 2D"));
ECS::Registry registry;

void destroy()
{
	ImGui::SFML::Shutdown();
}

int main() {
	registry.AddSystem<ECS::RenderSystem>(g_window);
	registry.AddSystem<ECS::UISystem>();

	if (!ImGui::SFML::Init(*g_window.m_window)) {
		return -1;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	sf::Clock deltaClock;
	bool showDemoWindow = true;

	ECS::EntityID circle = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
	registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50)));

	ECS::EntityID tri = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
	registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

	while (g_window.isOpen()) {

		while (const std::optional event = g_window.m_window->pollEvent()) {
			ImGui::SFML::ProcessEvent(*g_window.m_window, *event);
			if (event->is<sf::Event::Closed>()) {
				g_window.close();
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

		// Esto ejecutará RenderSystem::OnUpdate y luego UISystem::OnUpdate
		registry.UpdateSystems(dt);

		// Presentar el frame
		ImGui::SFML::Render(*g_window.m_window);
		g_window.display();
	}
	destroy();

	return 0;
}