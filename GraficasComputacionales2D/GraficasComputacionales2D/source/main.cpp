/**
 * @file main.cpp
 * @brief Punto de entrada principal del motor "Surreal Engine 2D" y escenario de pruebas para la IA.
 *
 * @details Este archivo inicializa la ventana principal de SFML, configura la interfaz gráfica
 * mediante ImGui, registra todos los sistemas del Entity Component System (ECS) y crea un
 * escenario de prueba interactivo con entidades inteligentes (deambulación, evasión de obstáculos
 * y persecución predictiva).
 */

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

 /** @brief Instancia global de la ventana principal del motor (800x600, "Surreal Engine 2D"). */
Window g_window(Window(800, 600, "Surreal Engine 2D"));

/** @brief Registro principal del ECS encargado de administrar todas las entidades y sistemas. */
ECS::Registry registry;

/**
 * @brief Libera los recursos del sistema antes de cerrar la aplicación.
 * @details Apaga el subsistema de interfaz gráfica ImGui acoplado a SFML.
 */
void destroy()
{
  ImGui::SFML::Shutdown();
}

/**
 * @brief Función principal de ejecución del programa.
 * @details Realiza la configuración inicial, registra el pipeline de sistemas, instancia
 * el escenario de pruebas para comportamientos de IA y ejecuta el bucle principal de juego (Game Loop).
 * * @return int Código de salida del programa (0 para éxito, -1 en caso de error en inicialización).
 */
int main() {
  // ==========================================
  // REGISTRO DE SISTEMAS
  // ==========================================
  registry.AddSystem<ECS::SteeringSystem>();

  // Registramos nuestros 3 nuevos sistemas de IA antes del Render y la UI
  registry.AddSystem<ECS::WanderSystem>();
  registry.AddSystem<ECS::PursuitSystem>();
  registry.AddSystem<ECS::ObstacleAvoidanceSystem>();

  // El sistema que calcula la física y mueve las entidades
  // [CORREGIDO: Se añadió el prefijo ECS::]
  registry.AddSystem<ECS::KinematicSystem>();

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
  // CREACIÓN DE ENTIDADES (ESCENARIO DE PRUEBA)
  // ==========================================

  // 1. EL OBSTÁCULO CENTRAL (Ladrillos)
  // Lo ponemos justo al centro (400, 300) para que sea inevitable cruzarlo
  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE,
    sf::Color(100, 250, 50), "Textures/Bricks.png"));
  // Le damos un radio de colisión un poco más grande (80.f) para obligar a esquivar
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 80.0f });

  // 2. EL TRIÁNGULO VERDE (Deambula y Esquiva)
  ECS::EntityID agent = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ 100.f, 300.f });
  registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green));

  auto& kinematic = registry.AddComponent<ECS::Kinematic>(agent);
  kinematic.maxSpeed = 180.f;
  kinematic.maxForce = 200.f;
  kinematic.velocity = sf::Vector2f{ 100.f, -50.f }; // ¡Importante: velocidad inicial para romper la inercia!

  // Le quitamos Steering estático y le ponemos Wander libre
  auto& wander = registry.AddComponent<ECS::Wander>(agent);
  wander.circleDistance = 50.f;
  wander.circleRadius = 40.f;
  wander.maxJitter = 1.5f;

  // Le enseñamos a esquivar el círculo central
  auto& agentAvoidance = registry.AddComponent<ECS::ObstacleAvoidance>(agent);
  agentAvoidance.maxSeeAhead = 150.f;   // Ve el peligro a 150 píxeles de distancia
  agentAvoidance.avoidanceForce = 400.f; // Fuerza de desvío alta

  // 3. EL TRIÁNGULO CELESTE (Cazador Predictivo y Esquivador)
  ECS::EntityID tri = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 700.f, 500.f }, 45.f);
  registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

  auto& kinTri = registry.AddComponent<ECS::Kinematic>(tri);
  kinTri.maxSpeed = 160.f;
  kinTri.maxForce = 180.f;
  kinTri.velocity = sf::Vector2f{ -50.f, 0.f }; // Velocidad inicial

  auto& pursuit = registry.AddComponent<ECS::Pursuit>(tri);
  pursuit.targetEntity = agent; // Persigue automáticamente al triángulo verde

  // También esquiva el círculo central si se le atraviesa en la caza
  auto& triAvoidance = registry.AddComponent<ECS::ObstacleAvoidance>(tri);
  triAvoidance.maxSeeAhead = 150.f;
  triAvoidance.avoidanceForce = 400.f;


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