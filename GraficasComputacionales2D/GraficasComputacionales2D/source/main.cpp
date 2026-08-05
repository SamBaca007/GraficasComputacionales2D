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
#include "ECS/Components/Path.h"
#include "ECS/Systems/PathRenderSystem.h"
#include "ECS/Components/PathFollow.h"
#include "ECS/Systems/PathFollowingSystem.h"

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

  registry.AddSystem<ECS::PathFollowingSystem>();
  // El sistema que calcula la física y mueve las entidades
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

  // ==========================================
  // 0. CIRCUITO DE LUIGI (Mario Kart DS)
  // ==========================================
  ECS::EntityID track = registry.CreateEntity();

  ECS::Path trackPath;
  trackPath.radius = 100.0f; // Ancho de pista amplio para permitir rebasados

  trackPath.points = {
    // 1. Recta de Meta (Subiendo por la izquierda)
    { 180.0f, 700.0f },
    { 180.0f, 350.0f },

    // 2. Horquilla Superior (Curva 180° arriba)
    { 200.0f, 160.0f },
    { 320.0f, 80.0f  },
    { 450.0f, 120.0f },
    { 440.0f, 250.0f }, // Salida de la horquilla

    // 3. La "S" Central / Slalom (El territorio del Kart Rojo)
    { 520.0f, 320.0f }, // Giro a la derecha
    { 680.0f, 220.0f }, // Cumbre de la primera 'S'
    { 760.0f, 300.0f }, // Cambio de peso a la izquierda
    { 660.0f, 440.0f }, // Centro del slalom
    { 780.0f, 560.0f }, // Salida disparada a la gran curva

    // 4. Gran Horquilla Inferior Derecha (180° abierta)
    { 880.0f, 700.0f },
    { 780.0f, 860.0f },
    { 620.0f, 840.0f },

    // 5. Diagonal de regreso y la "U" final hacia la meta
    { 450.0f, 620.0f },
    { 320.0f, 500.0f }, // Entrada al rincón interno
    { 320.0f, 720.0f }, // Horquilla de reincorporación
    { 220.0f, 820.0f }  // Entrada de nuevo a la recta principal
  };

  registry.AddComponent<ECS::Path>(track, trackPath);
  
  // 1. EL OBSTÁCULO CENTRAL (Ladrillos)
  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 325.f, 290.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE,
    sf::Color(100, 250, 50), "Textures/Bricks.png"));
  // Le damos un radio de colisión un poco más grande (80.f) para obligar a esquivar
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 80.0f });

  // ==========================================
  // LA PARRILLA DE SALIDA
  // ==========================================

  // 2. EL KART VERDE (El Estándar - Equilibrado)
  ECS::EntityID agent = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ 850.f, 700.f }, 90.f);
  registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green));

  auto& kinGreen = registry.AddComponent<ECS::Kinematic>(agent);
  kinGreen.maxSpeed = 200.f;
  kinGreen.maxForce = 300.f;
  kinGreen.accelerationRate = 85.f; // Aceleración estándar
  kinGreen.velocity = sf::Vector2f{ 0.f, -100.f };

  auto& pathFollowGreen = registry.AddComponent<ECS::PathFollow>(agent);
  pathFollowGreen.pathEntity = track;

  auto& avoidanceGreen = registry.AddComponent<ECS::ObstacleAvoidance>(agent);
  avoidanceGreen.maxSeeAhead = 150.f;
  avoidanceGreen.avoidanceForce = 450.f;


  // 3. EL KART AZUL (El Bólido Pesado - Rápido en rectas, derrapa en curvas)
  ECS::EntityID blueKart = registry.CreateEntity();
  // Lo colocamos un poco más atrás y a la izquierda
  registry.AddComponent<ECS::Transform>(blueKart, sf::Vector2f{ 820.f, 730.f }, 90.f);
  registry.AddComponent<ECS::Render>(blueKart, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

  auto& kinBlue = registry.AddComponent<ECS::Kinematic>(blueKart);
  kinBlue.maxSpeed = 240.f; // Mayor velocidad punta
  kinBlue.maxForce = 25.f; // Menor capacidad de giro (provoca derrape)
  kinBlue.accelerationRate = 45.f; // Aceleración muy lenta (sufre tras frenar en curvas)
  kinBlue.velocity = sf::Vector2f{ 0.f, -100.f };

  auto& pathFollowBlue = registry.AddComponent<ECS::PathFollow>(blueKart);
  pathFollowBlue.pathEntity = track;

  auto& avoidanceBlue = registry.AddComponent<ECS::ObstacleAvoidance>(blueKart);
  avoidanceBlue.maxSeeAhead = 200.f;    // Ve más lejos porque va más rápido
  avoidanceBlue.avoidanceForce = 450.f;


  // 4. EL KART ROJO (El Ligero y Técnico - Lento pero se pega a las curvas)
  ECS::EntityID redKart = registry.CreateEntity();
  // Lo colocamos un poco más atrás y a la derecha
  registry.AddComponent<ECS::Transform>(redKart, sf::Vector2f{ 880.f, 730.f }, 90.f);
  registry.AddComponent<ECS::Render>(redKart, ECS::Render::Make(TRIANGLE, sf::Color::Red));

  auto& kinRed = registry.AddComponent<ECS::Kinematic>(redKart);
  kinRed.maxSpeed = 180.f; // Menor velocidad punta
  kinRed.maxForce = 800.f; // Capacidad de giro brutal
  kinRed.accelerationRate = 180.f; // Aceleración de respuesta inmediata
  kinRed.velocity = sf::Vector2f{ 0.f, -100.f };

  auto& pathFollowRed = registry.AddComponent<ECS::PathFollow>(redKart);
  pathFollowRed.pathEntity = track;

  auto& avoidanceRed = registry.AddComponent<ECS::ObstacleAvoidance>(redKart);
  avoidanceRed.maxSeeAhead = 100.f;    // Reacciona en distancias más cortas
  avoidanceRed.avoidanceForce = 450.f;

  // ==========================================
  // CÁMARA
  // ==========================================
  ECS::EntityID cam = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 350.f, 420.f });
  auto& camComp = registry.AddComponent<ECS::Camera>(cam);
  camComp.followTarget = ECS::NULL_ENTITY;
  camComp.followSpeed = 5.f;
  camComp.zoom = 1;

  g_window.handleResize(g_window.m_window->getSize());

  unsigned int pendingMSAA = g_window.getAntialiasingLevel();
  bool msaaChanged = false;

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

    // ==========================================
    // VENTANA DE CONFIGURACIÓN DE GRÁFICOS (MSAA)
    // ==========================================
    ImGui::Begin("Configuracion de Graficos");
    {
      const char* msaaOptions[] = { "Desactivado (0x)", "2x MSAA", "4x MSAA", "8x MSAA" };
      int currentMSAA = 0;

      unsigned int currentLevel = g_window.getAntialiasingLevel();
      if (currentLevel == 2) currentMSAA = 1;
      else if (currentLevel == 4) currentMSAA = 2;
      else if (currentLevel == 8) currentMSAA = 3;

      if (ImGui::Combo("Anti-Aliasing", &currentMSAA, msaaOptions, IM_ARRAYSIZE(msaaOptions))) {
        if (currentMSAA == 0) pendingMSAA = 0;
        else if (currentMSAA == 1) pendingMSAA = 2;
        else if (currentMSAA == 2) pendingMSAA = 4;
        else if (currentMSAA == 3) pendingMSAA = 8;

        // Solo marcamos que queremos cambiarlo al final del ciclo
        msaaChanged = true;
      }

      ImGui::Text("Estado actual: %ux MSAA", g_window.getAntialiasingLevel());
    }
    ImGui::End();

    g_window.clear(sf::Color::Black);

    registry.UpdateSystems(dt);

    // --- NUEVO: Dibujar la pista de depuración ---
    // Iteramos sobre cualquier entidad que tenga un componente Path (nuestra pista)
    registry.GetView<ECS::Path>().Each([&](ECS::EntityID /*entity*/,
      const ECS::Path& path) { ECS::PathRenderSystem::Render(g_window, path);
      });
    // ---------------------------------------------

    ImGui::SFML::Render(*g_window.m_window);
    g_window.display();

    // --- NUEVO: APLICAR CAMBIOS GRÁFICOS DE FORMA SEGURA ---
    if (msaaChanged) {
      // 1. Apagamos ImGui para que suelte la ventana actual
      ImGui::SFML::Shutdown();

      // 2. Recreamos la ventana (cambia el puntero y el Native Handle)
      g_window.setAntialiasingLevel(pendingMSAA);

      // 3. Volvemos a inicializar ImGui enganchándolo a la nueva ventana
      ImGui::SFML::Init(*g_window.m_window);

      // 4. Restauramos banderas importantes de ImGui (como el Docking)
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

      // 5. Reiniciamos el reloj para evitar un pico gigante de Delta Time por el retraso de recreación
      deltaClock.restart();

      msaaChanged = false;
    }
  }
  destroy();

  return 0;
}