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

  // 0. LA PISTA DE CARRERAS (Circuito de prueba)
  ECS::EntityID track = registry.CreateEntity();
  ECS::Path trackPath;
  trackPath.radius = 80.0f;
  // Un circuito rectangular simple alrededor de tus entidades actuales
  trackPath.points = {
      { 850.0f, 700.0f }, // 1. Línea de meta (recta derecha)
      { 850.0f, 300.0f }, // 2. Final de la recta derecha
      { 750.0f, 150.0f }, // 3. Curva superior derecha (entrada)
      { 650.0f, 100.0f }, // 4. Curva superior derecha (salida)
      { 250.0f, 100.0f }, // 5. Final recta superior
      { 150.0f, 150.0f }, // 6. Curva superior izquierda (entrada)
      { 100.0f, 250.0f }, // 7. Curva superior izquierda (salida)
      { 100.0f, 650.0f }, // 8. Final recta izquierda
      { 150.0f, 750.0f }, // 9. Curva inferior izquierda
      { 350.0f, 850.0f }, // 10. Recta inferior antes del zig-zag
      { 500.0f, 600.0f }, // 11. Zig-zag (subida)
      { 650.0f, 600.0f }, // 12. Zig-zag (bajada)
      { 750.0f, 850.0f }  // 13. Horquilla final antes de la meta
  };
  registry.AddComponent<ECS::Path>(track, trackPath);
  
  // 1. EL OBSTÁCULO CENTRAL (Ladrillos)
  // Lo ponemos justo al centro (400, 300) para que sea inevitable cruzarlo
  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE,
    sf::Color(100, 250, 50), "Textures/Bricks.png"));
  // Le damos un radio de colisión un poco más grande (80.f) para obligar a esquivar
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 80.0f });

  // 2. EL TRIÁNGULO VERDE (Piloto de Carreras)
  ECS::EntityID agent = registry.CreateEntity();

  // Lo colocamos exactamente en el primer punto de la pista (Línea de meta)
  registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ 850.f, 700.f }, -90.f);
  registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green));

  auto& kinematic = registry.AddComponent<ECS::Kinematic>(agent);
  kinematic.maxSpeed = 200.f; // Velocidad máxima del kart
  kinematic.maxForce = 250.f; // Capacidad de giro/aceleración
  kinematic.velocity = sf::Vector2f{ 0.f, -100.f }; // Le damos un empujón inicial hacia arriba

  // Agregamos el comportamiento de seguir la pista
  auto& pathFollow = registry.AddComponent<ECS::PathFollow>(agent);
  pathFollow.pathEntity = track; // Le pasamos el ID de la pista que creamos en el paso anterior

  // NOTA: Si en tu implementación de PathFollowingSystem usas variables como 
  // "lookAheadDistance" o "predictTime", configúralas aquí:
  // pathFollow.predictTime = 0.5f;

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