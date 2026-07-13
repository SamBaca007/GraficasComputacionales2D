#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Steering.h"
#include "ECS/Components/Wander.h"
#include "ECS/Components/Pursuit.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include <imgui.h>

namespace ECS {
  class UISystem final : public System {
  public:
    UISystem() = default;

    void OnUpdate(Registry& registry, float deltaTime) override {
      // Al primer frame, cuando ImGui ya tiene un contexto válido, aplicamos el estilo
      if (!m_styleApplied) {
        ApplyUnityBlueStyle();
        m_styleApplied = true;
      }

      DrawOutliner(registry);
      DrawDetails(registry);
    }

    static void ApplyUnityBlueStyle() {
      ImGuiStyle& style = ImGui::GetStyle();

      style.WindowRounding = 4.0f;
      style.FrameRounding = 3.0f;
      style.GrabRounding = 3.0f;
      style.PopupRounding = 3.0f;
      style.ScrollbarRounding = 3.0f;
      style.FramePadding = ImVec2(4.0f, 3.0f);
      style.ItemSpacing = ImVec2(8.0f, 4.0f);
      style.WindowPadding = ImVec2(8.0f, 8.0f);

      ImVec4* colors = style.Colors;

      colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
      colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.57f, 1.00f);
      colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
      colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
      colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
      colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
      colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
      colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.32f, 1.00f);
      colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.32f, 0.42f, 1.00f);
      colors[ImGuiCol_Header] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f);
      colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.48f, 0.75f, 1.00f);
      colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.55f, 0.85f, 1.00f);
      colors[ImGuiCol_Button] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f);
      colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.48f, 0.75f, 1.00f);
      colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.55f, 0.85f, 1.00f);
      colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
      colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f);
      colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
      colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
      colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
      colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.85f, 1.00f, 1.00f);
      colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
      colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.75f, 1.00f, 0.78f);
      colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
    }

    void DrawOutliner(Registry& registry) {
      ImGui::Begin("Entities");
      {
        registry.GetView<ECS::Transform>().Each([this]
        (ECS::EntityID id, ECS::Transform& /*transform*/) {
            const std::string label = "Entity " + std::to_string(id);
            const bool isSelected = (id == selectedEntity);

            if (ImGui::Selectable(label.c_str(), isSelected)) {
              selectedEntity = id;
            }
          });
      }
      ImGui::End();
    }

    void DrawDetails(Registry& registry) {
      ImGui::Begin("Inspector");
      {
        if (selectedEntity != ECS::NULL_ENTITY) {
          std::string title = "Entity " + std::to_string(selectedEntity);
          ImGui::Text("%s", title.c_str());
          ImGui::Separator();

          // ==========================================
          // Transform Component (Core - No se remueve)
          // ==========================================
          if (registry.HasComponent<ECS::Transform>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& transform = registry.GetComponent<ECS::Transform>(selectedEntity);

              float pos[2] = { transform.position.x, transform.position.y };
              if (ImGui::DragFloat2("Position", pos, 1.f)) {
                transform.position.x = pos[0];
                transform.position.y = pos[1];
              }

              ImGui::DragFloat("Rotation", &transform.rotation, 1.f);

              float scl[2] = { transform.scale.x, transform.scale.y };
              if (ImGui::DragFloat2("Scale", scl, 0.05f)) {
                transform.scale.x = scl[0];
                transform.scale.y = scl[1];
              }
            }
          }
          // ==========================================
          // Render Component (Core - No se remueve)
          // ==========================================
          if (registry.HasComponent<ECS::Render>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& render = registry.GetComponent<ECS::Render>(selectedEntity);

              ImGui::Checkbox("Visible", &render.visible);

              float colorArr[4] = {
                  render.fillColor.r / 255.f,
                  render.fillColor.g / 255.f,
                  render.fillColor.b / 255.f,
                  render.fillColor.a / 255.f
              };

              if (ImGui::ColorEdit4("Fill Color", colorArr)) {
                render.fillColor.r = static_cast<unsigned char>(colorArr[0] * 255.f);
                render.fillColor.g = static_cast<unsigned char>(colorArr[1] * 255.f);
                render.fillColor.b = static_cast<unsigned char>(colorArr[2] * 255.f);
                render.fillColor.a = static_cast<unsigned char>(colorArr[3] * 255.f);

                if (render.shape) {
                  render.shape->setFillColor(render.fillColor);
                }
              }
            }
          }
          // ==========================================
          // Camera Component
          // ==========================================
          if (registry.HasComponent<ECS::Camera>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& cam = registry.GetComponent<ECS::Camera>(selectedEntity);

              ImGui::Checkbox("Active", &cam.active);
              ImGui::DragFloat("Zoom", &cam.zoom, 0.01f, 0.05f, 10.f);
              ImGui::DragFloat("Follow Speed", &cam.followSpeed, 0.1f, 0.f, 50.f);

              if (cam.followTarget == ECS::NULL_ENTITY) {
                ImGui::Text("Follow Target: (ninguno)");
              }
              else {
                ImGui::Text("Follow Target: %llu",
                  static_cast<unsigned long long>(cam.followTarget));
              }

              ImGui::Spacing();
              if (ImGui::Button("Remover Camera", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::Camera>(selectedEntity);
              }
            }
          }
          // ==========================================
          // Kinematic Component
          // ==========================================
          if (registry.HasComponent<ECS::Kinematic>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Kinematic", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& kinematic = registry.GetComponent<ECS::Kinematic>(selectedEntity);

              float vel[2] = { kinematic.velocity.x, kinematic.velocity.y };
              if (ImGui::DragFloat2("Velocity", vel, 1.f)) {
                kinematic.velocity.x = vel[0];
                kinematic.velocity.y = vel[1];
              }

              float acc[2] = { kinematic.acceleration.x, kinematic.acceleration.y };
              if (ImGui::DragFloat2("Acceleration", acc, 1.f)) {
                kinematic.acceleration.x = acc[0];
                kinematic.acceleration.y = acc[1];
              }

              ImGui::DragFloat("Max Speed", &kinematic.maxSpeed, 1.f, 0.f, 1000.f);
              ImGui::DragFloat("Max Force", &kinematic.maxForce, 1.f, 0.f, 1000.f);

              ImGui::Spacing();
              if (ImGui::Button("Remover Kinematic", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::Kinematic>(selectedEntity);
              }
            }
          }

          // ==========================================
          // Steering Component
          // ==========================================
          if (registry.HasComponent<ECS::Steering>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Steering", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& steering = registry.GetComponent<ECS::Steering>(selectedEntity);

              const char* behaviorNames[] = { "None", "Seek", "Flee", "Arrive",
                "Wander", "Pursuit", "Obstacle Avoidance" };
              int currentBehavior = static_cast<int>(steering.currentBehavior);
              if (ImGui::Combo("Behavior", &currentBehavior, behaviorNames,
                IM_ARRAYSIZE(behaviorNames))) {
                steering.currentBehavior = static_cast
                  <ECS::SteeringBehaviorType>(currentBehavior);
              }

              float targetPos[2] = { steering.target.x, steering.target.y };
              if (ImGui::DragFloat2("Target Pos", targetPos, 1.f)) {
                steering.target.x = targetPos[0];
                steering.target.y = targetPos[1];
              }

              std::string currentTargetLabel =
                (steering.targetEntity == ECS::NULL_ENTITY) ?
                "Ninguno (Usar posicion estatica)" : "Entity " +
                std::to_string(steering.targetEntity);

              if (ImGui::BeginCombo("Target Entity", currentTargetLabel.c_str())) {
                bool isNoneSelected = (steering.targetEntity == ECS::NULL_ENTITY);
                if (ImGui::Selectable
                ("Ninguno (Usar posicion estatica)", isNoneSelected)) {
                  steering.targetEntity = ECS::NULL_ENTITY;
                }

                registry.GetView<ECS::Transform>().Each([&steering, this]
                (ECS::EntityID id, ECS::Transform&) {
                    if (id == selectedEntity) return;

                    std::string entityLabel = "Entity " + std::to_string(id);
                    bool isSelected = (steering.targetEntity == id);

                    if (ImGui::Selectable(entityLabel.c_str(), isSelected)) {
                      steering.targetEntity = id;
                    }
                    if (isSelected) {
                      ImGui::SetItemDefaultFocus();
                    }
                  });
                ImGui::EndCombo();
              }

              if (steering.currentBehavior == ECS::SteeringBehaviorType::Arrive) {
                ImGui::DragFloat("Slowing Radius",
                  &steering.slowingRadius, 1.f, 0.f, 1000.f);
              }

              ImGui::Spacing();
              if (ImGui::Button("Remover Steering", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::Steering>(selectedEntity);
              }
            }
          }

          // ==========================================
          // Wander Component (NUEVO)
          // ==========================================
          if (registry.HasComponent<ECS::Wander>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Wander", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& wander = registry.GetComponent<ECS::Wander>(selectedEntity);

              ImGui::DragFloat("Distancia Circulo", &wander.circleDistance,
                1.0f, 10.0f, 500.0f, "%.1f");
              ImGui::DragFloat("Radio Circulo", &wander.circleRadius,
                1.0f, 5.0f, 300.0f, "%.1f");
              ImGui::DragFloat("Max Jitter", &wander.maxJitter,
                0.01f, 0.0f, 5.0f, "%.2f");
              ImGui::TextDisabled("Angulo Actual: %.2f rad", wander.wanderAngle);

              ImGui::Spacing();
              if (ImGui::Button("Remover Wander", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::Wander>(selectedEntity);
              }
            }
          }

          // ==========================================
          // Pursuit Component (NUEVO)
          // ==========================================
          if (registry.HasComponent<ECS::Pursuit>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Pursuit", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& pursuit = registry.GetComponent<ECS::Pursuit>(selectedEntity);

              std::string currentTargetLabel
                = (pursuit.targetEntity == ECS::NULL_ENTITY) ?
                "Ninguno" : "Entity " + std::to_string(pursuit.targetEntity);

              if (ImGui::BeginCombo("Target Entity", currentTargetLabel.c_str())) {
                bool isNoneSelected = (pursuit.targetEntity == ECS::NULL_ENTITY);
                if (ImGui::Selectable("Ninguno", isNoneSelected)) {
                  pursuit.targetEntity = ECS::NULL_ENTITY;
                }

                registry.GetView<ECS::Transform>().Each([&pursuit, this](ECS::EntityID id,
                  ECS::Transform&) {
                  if (id == selectedEntity) return;

                  std::string entityLabel = "Entity " + std::to_string(id);
                  bool isSelected = (pursuit.targetEntity == id);

                  if (ImGui::Selectable(entityLabel.c_str(), isSelected)) {
                    pursuit.targetEntity = id;
                  }
                  if (isSelected) ImGui::SetItemDefaultFocus();
                  });
                ImGui::EndCombo();
              }

              ImGui::Spacing();
              if (ImGui::Button("Remover Pursuit", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::Pursuit>(selectedEntity);
              }
            }
          }

          // ==========================================
          // Obstacle Avoidance Component (NUEVO)
          // ==========================================
          if (registry.HasComponent<ECS::ObstacleAvoidance>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Obstacle Avoidance",
              ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& avoidance = registry.GetComponent<ECS::ObstacleAvoidance>
                (selectedEntity);

              ImGui::DragFloat("Vision (Rayo)", &avoidance.maxSeeAhead,
                1.0f, 10.0f, 300.0f, "%.1f");
              ImGui::DragFloat("Fuerza Evasion", &avoidance.avoidanceForce,
                5.0f, 10.0f, 500.0f, "%.1f");

              ImGui::Spacing();
              if (ImGui::Button("Remover Obstacle Avoidance", ImVec2(-1, 0))) {
                registry.RemoveComponent<ECS::ObstacleAvoidance>(selectedEntity);
              }
            }
          }

          // ==========================================
          // Acciones de Entidad (Añadir Componentes)
          // ==========================================
          ImGui::Spacing();
          ImGui::Separator();
          ImGui::TextDisabled("Agregar Componentes:");

          if (!registry.HasComponent<ECS::Camera>(selectedEntity)) {
            if (ImGui::Button("Add Camera")) {
              auto& c = registry.AddComponent<ECS::Camera>(selectedEntity);
              c.zoom = 1.0f;
              c.followSpeed = 5.0f;
            }
            ImGui::SameLine();
          }

          if (!registry.HasComponent<ECS::Kinematic>(selectedEntity)) {
            if (ImGui::Button("Add Kinematic")) {
              auto& k = registry.AddComponent<ECS::Kinematic>(selectedEntity);
              k.maxSpeed = 250.f;
              k.maxForce = 200.f;
            }
            ImGui::SameLine();
          }

          if (!registry.HasComponent<ECS::Steering>(selectedEntity)) {
            if (ImGui::Button("Add Steering")) {
              auto& s = registry.AddComponent<ECS::Steering>(selectedEntity);
              s.currentBehavior = ECS::SteeringBehaviorType::None;
            }
          }

          // --- BOTONES PARA NUEVOS COMPONENTES ---
          ImGui::SameLine();

          if (!registry.HasComponent<ECS::Wander>(selectedEntity)) {
            if (ImGui::Button("Add Wander")) {
              registry.AddComponent<ECS::Wander>(selectedEntity, ECS::Wander{});
            }
            ImGui::SameLine();
          }

          if (!registry.HasComponent<ECS::Pursuit>(selectedEntity)) {
            if (ImGui::Button("Add Pursuit")) {
              registry.AddComponent<ECS::Pursuit>(selectedEntity, ECS::Pursuit{});
            }
            ImGui::SameLine();
          }

          if (!registry.HasComponent<ECS::ObstacleAvoidance>(selectedEntity)) {
            if (ImGui::Button("Add Avoidance")) {
              registry.AddComponent<ECS::ObstacleAvoidance>(selectedEntity,
                ECS::ObstacleAvoidance{});
            }
          }

        }
        else {
          ImGui::TextDisabled("Selecciona una entidad en el outliner.");
        }
      }
      ImGui::End();
    }

  private:
    ECS::EntityID selectedEntity = ECS::NULL_ENTITY;
    bool m_styleApplied = false;
  };
} // Namespace ECS