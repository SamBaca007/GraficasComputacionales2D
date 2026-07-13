#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/SteeringSystem.h" 
#include "ECS/Components/Wander.h"
#include "ECS/Components/Pursuit.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include "ECS/Components/Obstacle.h"
#include "ECS/Systems/WanderSystem.h"
#include "ECS/Systems/PursuitSystem.h"
#include "ECS/Systems/ObstacleAvoidanceSystem.h"
#include "ECS/Systems/KinematicSystem.h"

Window g_window(Window(800, 600, "Surreal Engine 2D"));
ECS::Registry registry;

void destroy()
{
  ImGui::SFML::Shutdown();
}

int main() {
  // ==========================================
  // REGISTRO DE SISTEMAS
  // ==========================================
  registry.AddSystem<ECS::SteeringSystem>();

  // Registramos nuestros 3 nuevos sistemas de IA antes del Render y la UI
  registry.AddSystem<ECS::WanderSystem>();
  registry.AddSystem<ECS::PursuitSystem>();
  registry.AddSystem<ECS::ObstacleAvoidanceSystem>();

  // ---> NUEVO: El sistema que calcula la física y mueve las entidades <---
  registry.AddSystem<KinematicSystem>();

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

  // 1. TU CÍRCULO CLÁSICO (Ahora es un Obstáculo)
  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 100.f, 50.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE,
    sf::Color(100, 250, 50), "Textures/Bricks.png"));
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 40.0f });

  // 2. TU TRIÁNGULO VERDE (Seeker Original + Evasor de Obstáculos)
  ECS::EntityID agent = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ 100.f, 100.f });
  registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green));

  auto& kinematic = registry.AddComponent<ECS::Kinematic>(agent);
  kinematic.maxSpeed = 250.f;
  kinematic.maxForce = 200.f;

  auto& steering = registry.AddComponent<ECS::Steering>(agent);
  steering.currentBehavior = ECS::SteeringBehaviorType::Seek;
  steering.target = sf::Vector2f{ 600.f, 400.f };
  // Le enseñamos a esquivar el círculo de ladrillos
  registry.AddComponent<ECS::ObstacleAvoidance>(agent);

  // 3. TU TRIÁNGULO CELESTE (Ahora es el Cazador)
  ECS::EntityID tri = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
  registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

  auto& kinTri = registry.AddComponent<ECS::Kinematic>(tri);
  kinTri.maxSpeed = 160.f;
  kinTri.maxForce = 120.f;

  auto& pursuit = registry.AddComponent<ECS::Pursuit>(tri);
  pursuit.targetEntity = agent; // Persigue al triángulo verde
  registry.AddComponent<ECS::ObstacleAvoidance>(tri);

  // ==========================================
  // CÁMARA
  // ==========================================
  ECS::EntityID cam = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 0.f, 0.f });
  auto& camComp = registry.AddComponent<ECS::Camera>(cam);
  camComp.followTarget = ECS::NULL_ENTITY;
  camComp.followSpeed = 5.f;
  camComp.zoom = 1;

  g_window.handleResize(g_window.m_window->getSize());

  while (g_window.isOpen()) {

    while (const std::optional event = g_window.m_window->pollEvent()) {
      ImGui::SFML::ProcessEvent(*g_window.m_window, *event);
      if (event->is<sf::Event::Closed>()) {
        g_window.close();
      }
      else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        g_window.handleResize(resized->size);
      }
    }

    const sf::Time elapsedTime = deltaClock.restart();
    const float dt = elapsedTime.asSeconds();

    ImGui::SFML::Update(*g_window.m_window, elapsedTime);

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
    ImGui::ShowDemoWindow(&showDemoWindow);

    g_window.clear(sf::Color::Black);

    registry.UpdateSystems(dt);

    ImGui::SFML::Render(*g_window.m_window);
    g_window.display();
  }
  destroy();

  return 0;
}