#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
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

      // Mantenemos la forma redondeada (Estilo moderno)
      style.WindowRounding = 4.0f;
      style.FrameRounding = 3.0f;
      style.GrabRounding = 3.0f;
      style.PopupRounding = 3.0f;
      style.ScrollbarRounding = 3.0f;
      style.FramePadding = ImVec2(4.0f, 3.0f);
      style.ItemSpacing = ImVec2(8.0f, 4.0f);
      style.WindowPadding = ImVec2(8.0f, 8.0f);

      ImVec4* colors = style.Colors;

      // Textos más brillantes para mayor contraste
      colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
      colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.57f, 1.00f);

      // Fondos principales más oscuros (hace que los paneles resalten)
      colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
      colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
      colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);

      // Bordes sutiles
      colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
      colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

      // Fondos de inputs (DragFloats, Checkboxes) 
      colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
      colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.32f, 1.00f);
      colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.32f, 0.42f, 1.00f);

      // Cabeceras y Botones (El azul vibrante regresa aquí)
      colors[ImGuiCol_Header] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f);
      colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.48f, 0.75f, 1.00f);
      colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.55f, 0.85f, 1.00f);

      colors[ImGuiCol_Button] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f);
      colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.48f, 0.75f, 1.00f);
      colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.55f, 0.85f, 1.00f);

      // Títulos de las ventanas
      colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
      colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.40f, 0.65f, 1.00f); // Se ilumina en azul al seleccionarse
      colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

      // Acentos (Checkboxes, Sliders activos) - Azul muy brillante
      colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
      colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
      colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.85f, 1.00f, 1.00f);

      // Separadores
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

          // Transform Component
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

          // Render Component
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

          // Camera Component
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
                ImGui::Text("Follow Target: %llu", static_cast<unsigned long long>(cam.followTarget));
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

    // AQUÍ ESTÁ LA DECLARACIÓN CLAVE:
    // Al estar dentro de las llaves de la clase y en la zona 'private', 
    // la función OnUpdate podrá leerla y modificarla sin problemas de definición.
    bool m_styleApplied = false;
  };
}