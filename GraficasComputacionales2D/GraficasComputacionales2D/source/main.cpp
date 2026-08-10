/**
 * @file main.cpp
 * @brief Punto de entrada principal del motor "Surreal Engine 2D" y escenario de pruebas para la IA.
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
#include "ECS/Components/DRS.h"
#include "ECS/Systems/DRSSystem.h"
#include <iomanip>
#include <sstream>

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
  registry.AddSystem<ECS::WanderSystem>();
  registry.AddSystem<ECS::PursuitSystem>();
  registry.AddSystem<ECS::ObstacleAvoidanceSystem>();
  registry.AddSystem<ECS::PathFollowingSystem>();
  registry.AddSystem<ECS::DRSSystem>();
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
  // CIRCUITO DE LUIGI
  // ==========================================
  ECS::EntityID track = registry.CreateEntity();
  ECS::Path trackPath;
  trackPath.radius = 100.0f;

  trackPath.points = {
    { -615.0f,  555.0f }, { -512.0f,  138.0f }, { -171.0f,  -31.0f },
    {  186.0f, -244.0f }, {  260.0f, -555.0f }, {  317.0f, -946.0f },
    {  469.0f, -1013.0f }, {  692.0f, -816.0f }, {  680.0f, -581.0f },
    {  593.0f, -220.0f }, {  454.0f,   50.0f }, {  608.0f,  250.0f },
    {  840.0f,   89.0f }, { 1057.0f, -150.0f }, { 1270.0f, -268.0f },
    { 1498.0f, -145.0f }, { 1838.0f,   31.0f }, { 2005.0f,  224.0f },
    { 1897.0f,  412.0f }, { 1625.0f,  421.0f }, { 1326.0f,  415.0f },
    { 1073.0f,  461.0f }, { 1032.0f,  593.0f }, { 1210.0f,  810.0f },
    { 1810.0f, 1383.0f }, { 2017.0f, 1640.0f }, { 1944.0f, 1834.0f },
    { 1647.0f, 1841.0f }, { 1311.0f, 1533.0f }, { 1143.0f, 1414.0f },
    { 1034.0f, 1211.0f }, {  807.0f, 1115.0f }, {  446.0f, 1031.0f },
    {  264.0f,  741.0f }, {  118.0f,  576.0f }, {  -59.0f,  603.0f },
    { -113.0f,  808.0f }, { -109.0f, 1383.0f }, { -177.0f, 1602.0f },
    { -360.0f, 1678.0f }, { -547.0f, 1597.0f }, { -606.0f, 1454.0f },
    { -615.0f, 1250.0f }
  };

  registry.AddComponent<ECS::Path>(track, trackPath);

  // ==========================================
  // OBSTÁCULOS Y KARTS
  // ==========================================
  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 120.f, 285.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50), "Textures/Bricks.png", 80.f, 80.f));
  registry.AddComponent<ECS::Obstacle>(circle, ECS::Obstacle{ 120.0f });

  // 2. KART VERDE (Estándar)
  ECS::EntityID greenKart = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(greenKart, sf::Vector2f{ -615.f, 1100.f }, -90.f);
  registry.AddComponent<ECS::Render>(greenKart, ECS::Render::Make(RECTANGLE, sf::Color::White, "Textures/GreenKart.png", 100.f, 100.f));
  auto& kinGreen = registry.AddComponent<ECS::Kinematic>(greenKart);
  kinGreen.maxSpeed = 202.f; kinGreen.maxForce = 300.f; kinGreen.accelerationRate = 80.f;
  kinGreen.velocity = sf::Vector2f{ 0.f, -100.f };
  registry.AddComponent<ECS::PathFollow>(greenKart, ECS::PathFollow{ track });
  registry.AddComponent<ECS::ObstacleAvoidance>(greenKart, ECS::ObstacleAvoidance{ 150.f, 450.f });
  registry.AddComponent<ECS::DRS>(greenKart, ECS::DRS{});

  // 3. KART AZUL (Pesado)
  ECS::EntityID blueKart = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(blueKart, sf::Vector2f{ -665.f, 1150.f }, -90.f);
  registry.AddComponent<ECS::Render>(blueKart, ECS::Render::Make(RECTANGLE, sf::Color::White, "Textures/BlueKart.png", 100.f, 100.f));
  auto& kinBlue = registry.AddComponent<ECS::Kinematic>(blueKart);
  kinBlue.maxSpeed = 225.f; kinBlue.maxForce = 25.f; kinBlue.accelerationRate = 45.f;
  kinBlue.velocity = sf::Vector2f{ 0.f, -100.f };
  registry.AddComponent<ECS::PathFollow>(blueKart, ECS::PathFollow{ track });
  registry.AddComponent<ECS::ObstacleAvoidance>(blueKart, ECS::ObstacleAvoidance{ 200.f, 450.f });
  registry.AddComponent<ECS::DRS>(blueKart, ECS::DRS{});

  // 4. KART ROJO (Ligero)
  ECS::EntityID redKart = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(redKart, sf::Vector2f{ -565.f, 1150.f }, -90.f);
  registry.AddComponent<ECS::Render>(redKart, ECS::Render::Make(RECTANGLE, sf::Color::White, "Textures/RedKart.png", 100.f, 100.f));
  auto& kinRed = registry.AddComponent<ECS::Kinematic>(redKart);
  kinRed.maxSpeed = 190.f; kinRed.maxForce = 600.f; kinRed.accelerationRate = 165.f;
  kinRed.velocity = sf::Vector2f{ 0.f, -100.f };
  registry.AddComponent<ECS::PathFollow>(redKart, ECS::PathFollow{ track });
  registry.AddComponent<ECS::ObstacleAvoidance>(redKart, ECS::ObstacleAvoidance{ 100.f, 450.f });
  registry.AddComponent<ECS::DRS>(redKart, ECS::DRS{});

  // MAPA FONDO
  ECS::EntityID background = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(background, sf::Vector2f{ 500.f, 500.f });
  registry.AddComponent<ECS::Render>(background, ECS::Render::Make(RECTANGLE, sf::Color::White, "Textures/pistamariokart.png", 4000.f, 4000.f));

  // CÁMARA
  ECS::EntityID cam = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 350.f, 420.f });
  auto& camComp = registry.AddComponent<ECS::Camera>(cam);
  camComp.followTarget = ECS::NULL_ENTITY; camComp.followSpeed = 5.f; camComp.zoom = 1;

  g_window.handleResize(g_window.m_window->getSize());
  unsigned int pendingMSAA = g_window.getAntialiasingLevel();
  bool msaaChanged = false;

  // ==========================================
  // VARIABLES PARA CRONÓMETROS Y VUELTAS
  // ==========================================
  struct LapTimer {
    float currentLapTime = 0.f;
    float bestLapTime = 0.f;
    int lastKnownLap = 0;
    bool hasCompletedLap = false;
  };
  std::unordered_map<ECS::EntityID, LapTimer> kartTimers;

  // Función Lambda para formatear el tiempo estilo carrera MM:SS.ms
  auto FormatTime = [](float t) -> std::string {
    if (t <= 0.f) return "--:--.--";
    int m = static_cast<int>(t) / 60;
    int s = static_cast<int>(t) % 60;
    int ms = static_cast<int>((t - std::floor(t)) * 100);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d.%02d", m, s, ms);
    return std::string(buf);
    };

  while (g_window.isOpen()) {

    while (const std::optional event = g_window.m_window->pollEvent()) {
      ImGui::SFML::ProcessEvent(*g_window.m_window, *event);
      if (event->is<sf::Event::Closed>()) { g_window.close(); }
      else if (const auto* resized = event->getIf<sf::Event::Resized>()) { g_window.handleResize(resized->size); }
      else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F3) { ECS::PathRenderSystem::ToggleDebugPath(); }
      }
    }

    const sf::Time elapsedTime = deltaClock.restart();
    const float dt = elapsedTime.asSeconds();

    ImGui::SFML::Update(*g_window.m_window, elapsedTime);
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
    ImGui::ShowDemoWindow(&showDemoWindow);

    // Actualizar Cronómetros antes de renderizar la UI
    registry.GetView<ECS::PathFollow>().Each([&](ECS::EntityID entity, ECS::PathFollow& follow) {
      auto& timer = kartTimers[entity];

      if (follow.currentLap > timer.lastKnownLap) {
        if (!timer.hasCompletedLap || timer.currentLapTime < timer.bestLapTime) {
          timer.bestLapTime = timer.currentLapTime;
        }
        timer.hasCompletedLap = true;
        timer.currentLapTime = 0.f;
        timer.lastKnownLap = follow.currentLap;
      }
      else {
        timer.currentLapTime += dt;
      }
      });

    ImGui::Begin("Configuracion de Graficos");
    {
      const char* msaaOptions[] = { "Desactivado (0x)", "2x MSAA", "4x MSAA", "8x MSAA" };
      int currentMSAA = 0;
      unsigned int currentLevel = g_window.getAntialiasingLevel();
      if (currentLevel == 2) currentMSAA = 1; else if (currentLevel == 4) currentMSAA = 2;
      else if (currentLevel == 8) currentMSAA = 3;

      if (ImGui::Combo("Anti-Aliasing", &currentMSAA, msaaOptions, IM_ARRAYSIZE(msaaOptions))) {
        if (currentMSAA == 0) pendingMSAA = 0; else if (currentMSAA == 1) pendingMSAA = 2;
        else if (currentMSAA == 2) pendingMSAA = 4; else if (currentMSAA == 3) pendingMSAA = 8;
        msaaChanged = true;
      }
      ImGui::Text("Estado actual: %ux MSAA", g_window.getAntialiasingLevel());
    }
    ImGui::End();

    // ==========================================
    // VENTANA DE LEADERBOARD (Posiciones y Tiempos)
    // ==========================================
    ImGui::Begin("Leaderboard en Vivo");
    {
      struct KartData {
        std::string name;
        sf::Color color;
        float speed;
        size_t currentWaypoint;
        int currentLap;
        float distanceToNext;
        float currentTime;
        float bestTime;
        bool hasCompletedLap;
      };
      std::vector<KartData> leaderboard;

      registry.GetView<ECS::Transform, ECS::Render, ECS::Kinematic, ECS::PathFollow>().Each(
        [&](ECS::EntityID entity, ECS::Transform& transform, ECS::Render& render,
          ECS::Kinematic& kin, ECS::PathFollow& follow) {

          std::string kartName = "Piloto Desconocido";
          sf::Color uiColor = sf::Color::White;

          // Identificamos a los karts por su maxForce en lugar de su color de Render
          if (kin.maxForce == 300.f) {
            kartName = "Kart Verde (Estandar)";
            uiColor = sf::Color::Green;
          }
          else if (kin.maxForce == 25.f) {
            kartName = "Kart Azul (Pesado)";
            uiColor = sf::Color::Cyan;
          }
          else if (kin.maxForce == 600.f) {
            kartName = "Kart Rojo (Ligero)";
            uiColor = sf::Color::Red;
          }

          float currentSpeed = std::sqrt(kin.velocity.x * kin.velocity.x + kin.velocity.y * kin.velocity.y);

          size_t nextWaypoint = (follow.currentSegment + 1) % trackPath.points.size();
          sf::Vector2f targetPos = trackPath.points[nextWaypoint];
          float dx = targetPos.x - transform.position.x;
          float dy = targetPos.y - transform.position.y;
          float dist = std::sqrt(dx * dx + dy * dy);

          auto& timer = kartTimers[entity];

          leaderboard.push_back({ kartName, uiColor, currentSpeed, follow.currentSegment,
            follow.currentLap, dist, timer.currentLapTime, timer.bestLapTime, timer.hasCompletedLap });
        });

      std::sort(leaderboard.begin(), leaderboard.end(), [](const KartData& a, const KartData& b) {
        if (a.currentLap != b.currentLap) return a.currentLap > b.currentLap;
        if (a.currentWaypoint != b.currentWaypoint) return a.currentWaypoint > b.currentWaypoint;
        return a.distanceToNext < b.distanceToNext;
        });

      if (ImGui::BeginTable("TablaPosiciones", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthFixed, 25.0f);
        ImGui::TableSetupColumn("Piloto");
        ImGui::TableSetupColumn("Vta", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Actual", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Record", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Velocidad", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < leaderboard.size(); ++i) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%zu", i + 1);

          ImGui::TableSetColumnIndex(1);
          ImVec4 imColor = ImVec4(leaderboard[i].color.r / 255.f,
            leaderboard[i].color.g / 255.f,leaderboard[i].color.b / 255.f, 1.0f);
          ImGui::TextColored(imColor, "%s", leaderboard[i].name.c_str());

          ImGui::TableSetColumnIndex(2);
          int displayLap = std::max(1, leaderboard[i].currentLap + 1);
          ImGui::Text("%d", displayLap);

          ImGui::TableSetColumnIndex(3);
          ImGui::Text("%s", FormatTime(leaderboard[i].currentTime).c_str());

          ImGui::TableSetColumnIndex(4);
          if (leaderboard[i].hasCompletedLap) {
            ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "%s",
              FormatTime(leaderboard[i].bestTime).c_str());
          }
          else {
            ImGui::Text("--:--.--");
          }

          ImGui::TableSetColumnIndex(5);
          ImGui::Text("%.1f px/s", leaderboard[i].speed);
        }
        ImGui::EndTable();
      }
    }
    ImGui::End();

    g_window.clear(sf::Color::Black);
    registry.UpdateSystems(dt);

    registry.GetView<ECS::Path>().Each([&](ECS::EntityID /*entity*/,
      const ECS::Path& path) { ECS::PathRenderSystem::Render(g_window, path);
      });

    ImGui::SFML::Render(*g_window.m_window);
    g_window.display();

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