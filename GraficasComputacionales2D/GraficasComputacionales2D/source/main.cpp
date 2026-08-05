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
  // VARIABLES DE CALIBRACIÓN MÁGICA
  // ==========================================
  float offsetX = 0.f; // Regresamos a 0. Ajusta solo si queda ligeramente movida.
  float offsetY = 0.f;

  // ==========================================
  // CIRCUITO DE LUIGI (Corregido para alinear Meta con inicio lógico)
  // ==========================================
  ECS::EntityID track = registry.CreateEntity();

  ECS::Path trackPath;
  trackPath.radius = 100.0f; // Margen de error para los karts

  // REORGANIZADO: Ahora la línea de meta es el punto 0 real del array.
  trackPath.points = {
    // 1. LÍNEA DE META Y CURVA 1
    { -615.0f,  555.0f }, // [0] LA META ESTÁ AQUÍ
    { -512.0f,  138.0f },
    { -171.0f,  -31.0f },
    {  186.0f, -244.0f },
    {  260.0f, -555.0f },
    {  317.0f, -946.0f },

    // 2. HORQUILLA SUPERIOR (El bucle de arriba)
    {  469.0f, -1013.0f },
    {  692.0f, -816.0f },
    {  680.0f, -581.0f },
    {  593.0f, -220.0f },

    // 3. CHICANE CENTRAL
    {  454.0f,   50.0f },
    {  608.0f,  250.0f },
    {  840.0f,   89.0f },
    { 1057.0f, -150.0f },

    // 4. SECTOR DERECHO
    { 1270.0f, -268.0f },
    { 1498.0f, -145.0f },
    { 1838.0f,   31.0f },
    { 2005.0f,  224.0f }, // Curva extrema derecha
    { 1897.0f,  412.0f },
    { 1625.0f,  421.0f },
    { 1326.0f,  415.0f },

    // 5. DIAGONAL Y BUCLE INFERIOR DERECHO
    { 1073.0f,  461.0f },
    { 1032.0f,  593.0f },
    { 1210.0f,  810.0f }, // Bajando en diagonal
    { 1810.0f, 1383.0f },
    { 2017.0f, 1640.0f }, // Ápex exterior inferior derecho
    { 1944.0f, 1834.0f },
    { 1647.0f, 1841.0f },
    { 1311.0f, 1533.0f },

    // 6. HORQUILLA INFERIOR Y TRANSICIÓN
    { 1143.0f, 1414.0f },
    { 1034.0f, 1211.0f },
    {  807.0f, 1115.0f },
    {  446.0f, 1031.0f },

    // 7. BUCLE INTERIOR IZQUIERDO
    {  264.0f,  741.0f },
    {  118.0f,  576.0f }, // Ápex superior de la U interior
    {  -59.0f,  603.0f },
    { -113.0f,  808.0f },

    // 8. DESCENSO Y CURVA FINAL HACIA META
    { -109.0f, 1383.0f },
    { -177.0f, 1602.0f },
    { -360.0f, 1678.0f }, // Ápex curva final
    { -547.0f, 1597.0f },
    { -606.0f, 1454.0f }, // Conexión de regreso a la recta principal

    // 9. LA RECTA FINAL DE ABAJO (Donde spawnean los karts)
    { -615.0f, 1250.0f }
  };

  registry.AddComponent<ECS::Path>(track, trackPath);

  // ==========================================
  // 1. EL OBSTÁCULO CENTRAL (Ladrillos) - ¡El que había desaparecido!
  // ==========================================
  ECS::EntityID circle = registry.CreateEntity();
  // Aplicamos el offset para que se mueva junto con la pista
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 120.f, 285.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50), "Textures/Bricks.png", 80.f, 80.f));
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 120.0f });


  // ==========================================
  // LA PARRILLA DE SALIDA (Coordenadas Absolutas)
  // ==========================================

  // 2. EL KART VERDE (El Estándar - Pole Position)
  ECS::EntityID agent = registry.CreateEntity();
  // Posición central, al inicio de la recta
  registry.AddComponent<ECS::Transform>(agent, sf::Vector2f{ -615.f, 1100.f }, -90.f);
  registry.AddComponent<ECS::Render>(agent, ECS::Render::Make(TRIANGLE, sf::Color::Green, "", 35.f, 35.f));

  auto& kinGreen = registry.AddComponent<ECS::Kinematic>(agent);
  kinGreen.maxSpeed = 202.f;       // Equilibrio de velocidad punta
  kinGreen.maxForce = 300.f;       // Equilibrio de agarre en curvas
  kinGreen.accelerationRate = 80.f; // Aceleración estándar
  kinGreen.velocity = sf::Vector2f{ 0.f, -100.f }; // Inician con un ligero impulso hacia arriba

  auto& pathFollowGreen = registry.AddComponent<ECS::PathFollow>(agent);
  pathFollowGreen.pathEntity = track;

  auto& avoidanceGreen = registry.AddComponent<ECS::ObstacleAvoidance>(agent);
  avoidanceGreen.maxSeeAhead = 150.f;
  avoidanceGreen.avoidanceForce = 450.f;


  // 3. EL KART AZUL (El Bólido Pesado - Segunda Fila, Izquierda)
  ECS::EntityID blueKart = registry.CreateEntity();
  // Desplazado a la izquierda (-665) y un poco más atrás (1250)
  registry.AddComponent<ECS::Transform>(blueKart, sf::Vector2f{ -665.f, 1150.f }, -90.f);
  registry.AddComponent<ECS::Render>(blueKart, ECS::Render::Make(TRIANGLE, sf::Color::Cyan, "", 35.f, 35.f));

  auto& kinBlue = registry.AddComponent<ECS::Kinematic>(blueKart);
  kinBlue.maxSpeed = 225.f; // Mayor velocidad punta
  kinBlue.maxForce = 25.f;  // Menor capacidad de giro
  kinBlue.accelerationRate = 45.f; // Aceleración muy lenta
  kinBlue.velocity = sf::Vector2f{ 0.f, -100.f };

  auto& pathFollowBlue = registry.AddComponent<ECS::PathFollow>(blueKart);
  pathFollowBlue.pathEntity = track;

  auto& avoidanceBlue = registry.AddComponent<ECS::ObstacleAvoidance>(blueKart);
  avoidanceBlue.maxSeeAhead = 200.f;
  avoidanceBlue.avoidanceForce = 450.f;


  // 4. EL KART ROJO (El Ligero y Técnico - Segunda Fila, Derecha)
  ECS::EntityID redKart = registry.CreateEntity();
  // Desplazado a la derecha (-565) y un poco más atrás (1250)
  registry.AddComponent<ECS::Transform>(redKart, sf::Vector2f{ -565.f, 1150.f }, -90.f);
  registry.AddComponent<ECS::Render>(redKart, ECS::Render::Make(TRIANGLE, sf::Color::Red, "", 35.f, 35.f));

  auto& kinRed = registry.AddComponent<ECS::Kinematic>(redKart);
  kinRed.maxSpeed = 185.f; // Menor velocidad punta
  kinRed.maxForce = 600.f; // Capacidad de giro brutal
  kinRed.accelerationRate = 165.f; // Aceleración inmediata
  kinRed.velocity = sf::Vector2f{ 0.f, -100.f };

  auto& pathFollowRed = registry.AddComponent<ECS::PathFollow>(redKart);
  pathFollowRed.pathEntity = track;

  auto& avoidanceRed = registry.AddComponent<ECS::ObstacleAvoidance>(redKart);
  avoidanceRed.maxSeeAhead = 100.f;
  avoidanceRed.avoidanceForce = 450.f;

  // ==========================================
  // -1. EL FONDO DEL MAPA (¡Debe ser la última entidad dibujable en crearse!)
  // ==========================================
  ECS::EntityID background = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(background, sf::Vector2f{ 500.f, 500.f });
  registry.AddComponent<ECS::Render>(background,
    ECS::Render::Make(RECTANGLE, sf::Color::White, "Textures/pistamariokart.png", 4000.f, 4000.f));


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
      // --- NUEVO: Alternar la línea de depuración con F3 ---
      else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F3) {
          ECS::PathRenderSystem::ToggleDebugPath();
        }
      }
      // ------------------------------------------------------
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

    // ==========================================
    // VENTANA DE LEADERBOARD (Posiciones)
    // ==========================================
    ImGui::Begin("Leaderboard en Vivo");
    {
      struct KartData {
        std::string name;
        sf::Color color;
        float speed;
        size_t currentWaypoint;
        int currentLap;
        float distanceToNext; // VARIABLE DE DESEMPATE ANTIPARPADEO
      };
      std::vector<KartData> leaderboard;

      // ¡NUEVO! Añadimos Transform para poder medir la distancia real
      registry.GetView<ECS::Transform, ECS::Render, ECS::Kinematic, ECS::PathFollow>().Each(
        [&](ECS::EntityID, ECS::Transform& transform, ECS::Render& render, ECS::Kinematic& kin, ECS::PathFollow& follow) {

          std::string kartName = "Piloto Desconocido";
          if (render.fillColor == sf::Color::Green) kartName = "Kart Verde (Estandar)";
          else if (render.fillColor == sf::Color::Cyan) kartName = "Kart Azul (Pesado)";
          else if (render.fillColor == sf::Color::Red) kartName = "Kart Rojo (Ligero)";

          float currentSpeed = std::sqrt(kin.velocity.x * kin.velocity.x + kin.velocity.y * kin.velocity.y);

          // ¡LA MAGIA DEL DESEMPATE! Calculamos la distancia hacia el SIGUIENTE nodo
          size_t nextWaypoint = (follow.currentSegment + 1) % trackPath.points.size();
          sf::Vector2f targetPos = trackPath.points[nextWaypoint];
          float dx = targetPos.x - transform.position.x;
          float dy = targetPos.y - transform.position.y;
          float dist = std::sqrt(dx * dx + dy * dy);

          leaderboard.push_back({ kartName, render.fillColor, currentSpeed, follow.currentSegment, follow.currentLap, dist });
        });

      // Ordenamiento Absoluto (Vuelta > Segmento > Distancia)
      std::sort(leaderboard.begin(), leaderboard.end(), [](const KartData& a, const KartData& b) {
        // 1. Quien va en una vuelta mayor, gana
        if (a.currentLap != b.currentLap) {
          return a.currentLap > b.currentLap;
        }
        // 2. Si están en la misma vuelta, el que va en un segmento más adelantado gana
        if (a.currentWaypoint != b.currentWaypoint) {
          return a.currentWaypoint > b.currentWaypoint;
        }
        // 3. DESEMPATE FINAL: Si están peleando palmo a palmo en el mismo segmento, 
        // el que esté más cerca (menor distancia) del siguiente punto gana.
        return a.distanceToNext < b.distanceToNext;
        });

      if (ImGui::BeginTable("TablaPosiciones", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Piloto");
        ImGui::TableSetupColumn("Vuelta", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Velocidad", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < leaderboard.size(); ++i) {
          ImGui::TableNextRow();

          // 1. Posición
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%zu", i + 1);

          // 2. Piloto (Color)
          ImGui::TableSetColumnIndex(1);
          ImVec4 imColor = ImVec4(leaderboard[i].color.r / 255.f, leaderboard[i].color.g / 255.f, leaderboard[i].color.b / 255.f, 1.0f);
          ImGui::TextColored(imColor, "%s", leaderboard[i].name.c_str());

          // 3. Vuelta actual
          ImGui::TableSetColumnIndex(2);
          int displayLap = std::max(1, leaderboard[i].currentLap + 1);
          ImGui::Text("%d", displayLap);

          // 4. Velocidad
          ImGui::TableSetColumnIndex(3);
          ImGui::Text("%.1f px/s", leaderboard[i].speed);
        }
        ImGui::EndTable();
      }
    }
    ImGui::End();

    g_window.clear(sf::Color::Black);

    registry.UpdateSystems(dt);

    // Dibujar la pista de depuración
    registry.GetView<ECS::Path>().Each([&](ECS::EntityID /*entity*/,
      const ECS::Path& path) { ECS::PathRenderSystem::Render(g_window, path);
      });

    ImGui::SFML::Render(*g_window.m_window);
    g_window.display();

    // APLICAR CAMBIOS GRÁFICOS DE FORMA SEGURA
    if (msaaChanged) {
      ImGui::SFML::Shutdown();
      g_window.setAntialiasingLevel(pendingMSAA);
      ImGui::SFML::Init(*g_window.m_window);
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      deltaClock.restart();
      msaaChanged = false;
    }
  }
  destroy();

  return 0;
}