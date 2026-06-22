#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;

void destroy()
{
	ImGui::SFML::Shutdown();
}

int
main() {
	registry.AddSystem<ECS::RenderSystem>(g_window);

	// m_window es un puntero a sf::RenderWindow.
	if (!ImGui::SFML::Init(*g_window.m_window))
	{
		return -1;
	}

	// Habilitar docking.
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

	// run the program as long as the window is open
	while (g_window.isOpen()) {
		// Estructura auxiliar para rastrear las entidades en la UI
		struct UIEntity {
			std::string name;
			ECS::EntityID id;
		};

		// Lista de entidades que aparecerán en la ventana de Jerarquía
		std::vector<UIEntity> sceneEntities = {
				{ "Entity 0", circle },
				{ "Entity 1", tri }
		};

		// Índice para saber cuál entidad está seleccionada actualmente (-1 significa ninguna)
		static int selectedEntityIdx = 0;
		// check all the window's events that were triggered since the last iteration of the loop
		while (const std::optional event = g_window.m_window->pollEvent()) {
			// ImGUi debe recibir todos los eventos de SFML.
			ImGui::SFML::ProcessEvent(*g_window.m_window, *event);
			if (event->is<sf::Event::Closed>()) {
				g_window.close();
			}
		}
		
		const sf::Time elapsedTime = deltaClock.restart();
		const float dt = elapsedTime.asSeconds();

		// Iniciar el frame de ImGui.
		ImGui::SFML::Update(*g_window.m_window, elapsedTime);

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
		ImGui::ShowDemoWindow(&showDemoWindow);

    // =================================================================
// 1. VENTANA DE JERARQUÍA (Entities)
// =================================================================
    ImGui::Begin("Entities");
    {
      for (size_t i = 0; i < sceneEntities.size(); ++i) {
        // Saber si este elemento es el que está seleccionado
        bool isSelected = (selectedEntityIdx == static_cast<int>(i));

        // Creamos un elemento seleccionable en la lista
        if (ImGui::Selectable(sceneEntities[i].name.c_str(), isSelected)) {
          selectedEntityIdx = static_cast<int>(i); // Actualizar selección al hacer clic
        }
      }
    }
    ImGui::End();

    // =================================================================
    // 2. VENTANA DE INSPECTOR
    // =================================================================
    ImGui::Begin("Inspector");
    {
      // Solo dibujamos componentes si hay una entidad seleccionada válida
      if (selectedEntityIdx >= 0 && selectedEntityIdx < static_cast<int>(sceneEntities.size())) {
        ECS::EntityID activeEntity = sceneEntities[selectedEntityIdx].id;

        // Título con el nombre de la entidad seleccionada
        ImGui::Text("%s", sceneEntities[selectedEntityIdx].name.c_str());
        ImGui::Separator();

        // -------------------------------------------------------------
        // SECCIÓN: TRANSFORM COMPONENT
        // -------------------------------------------------------------
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
          auto& transform = registry.GetComponent<ECS::Transform>(activeEntity);

          // Modificar Posición (X, Y)
          float pos[2] = { transform.position.x, transform.position.y };
          if (ImGui::DragFloat2("Position", pos, 1.f)) {
            transform.position.x = pos[0];
            transform.position.y = pos[1];
          }

          // Modificar Rotación (Ángulo flotante)
          // NOTA: Asumo que el miembro se llama 'rotation' dentro de ECS::Transform
          ImGui::DragFloat("Rotation", &transform.rotation, 1.f);

          // Modificar Escala (X, Y)
          // NOTA: Asumo que se llama 'scale' (sf::Vector2f) y tus figuras la implementan
          float scl[2] = { transform.scale.x, transform.scale.y };
          if (ImGui::DragFloat2("Scale", scl, 0.05f)) {
            transform.scale.x = scl[0];
            transform.scale.y = scl[1];
          }
        }

        // -------------------------------------------------------------
        // SECCIÓN: RENDER COMPONENT
        // -------------------------------------------------------------
        if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {
          auto& render = registry.GetComponent<ECS::Render>(activeEntity);

          // Checkbox de Visibilidad
          ImGui::Checkbox("Visible", &render.visible);

          // Color Picker de Relleno
          float colorArr[4] = {
              render.fillColor.r / 255.f,
              render.fillColor.g / 255.f,
              render.fillColor.b / 255.f,
              render.fillColor.a / 255.f
          };

          if (ImGui::ColorEdit4("Fill Color", colorArr)) {
            // 1. Actualizamos el valor en el componente de datos
            render.fillColor.r = static_cast<unsigned char>(colorArr[0] * 255.f);
            render.fillColor.g = static_cast<unsigned char>(colorArr[1] * 255.f);
            render.fillColor.b = static_cast<unsigned char>(colorArr[2] * 255.f);
            render.fillColor.a = static_cast<unsigned char>(colorArr[3] * 255.f);

            // 2. Aplicamos el color directamente al sf::Shape de SFML
            if (render.shape) {
              render.shape->setFillColor(render.fillColor);
            }
          }
        }
      }
      else {
        ImGui::Text("Select an entity from the Hierarchy to inspect.");
      }
    }
    ImGui::End();

		// clear the window with black color
		g_window.clear(sf::Color::Black);

		// draw everything here...
		registry.UpdateSystems(dt);
		// Renderizar ImGui después de la escena.
		ImGui::SFML::Render(*g_window.m_window);

		// end the current frame
		g_window.display();
	}
	destroy();
	return 0;
}