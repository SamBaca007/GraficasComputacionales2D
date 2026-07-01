#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"

namespace ECS {
  class UISystem final : public System {
  public:
    UISystem() = default;

    void OnUpdate(Registry& registry, float deltaTime) override {
      DrawOutliner(registry);
      DrawDetails(registry);
    }

    void DrawOutliner(Registry& registry) {
      ImGui::Begin("Entities"); // Le ponemos el mismo nombre que tenías en main
      {
        // Iteramos sobre las entidades que tienen Transform
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
        // Validamos que haya una entidad seleccionada
        if (selectedEntity != ECS::NULL_ENTITY) {
          std::string title = "Entity " + std::to_string(selectedEntity);
          ImGui::Text("%s", title.c_str());
          ImGui::Separator();

          // -------------------------------------------------------------
          // SECCIÓN: TRANSFORM COMPONENT
          // -------------------------------------------------------------
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

          // -------------------------------------------------------------
          // SECCIÓN: RENDER COMPONENT
          // -------------------------------------------------------------
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

          // -------------------------------------------------------------
          // SECCIÓN: CAMERA COMPONENT
          // -------------------------------------------------------------
          if (registry.HasComponent<ECS::Camera>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
              auto& cam = registry.GetComponent<ECS::Camera>(selectedEntity);

              // La posición y la rotación de la cámara se editan arriba,
              // en el bloque Transform. Aquí solo lo propio de la cámara.
              ImGui::Checkbox("Active", &cam.active);
              ImGui::DragFloat("Zoom", &cam.zoom, 0.01f, 0.05f, 10.f);
              ImGui::DragFloat("Follow Speed", &cam.followSpeed, 0.1f, 0.f, 50.f);

              // Objetivo a seguir (solo lectura aquí; muestra el ID).
              if (cam.followTarget == ECS::NULL_ENTITY) {
                ImGui::Text("Follow Target: (ninguno)");
              }
              else {
                ImGui::Text("Follow Target: %llu", static_cast<unsigned long long>
                  (cam.followTarget));
              }
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
  };
}