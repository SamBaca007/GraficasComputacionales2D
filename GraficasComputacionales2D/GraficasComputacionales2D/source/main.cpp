#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"
#include <optional>


/**
 * @file main.cpp
 * @brief Punto de entrada de la aplicación.
 */

/**
 * @brief Ventana principal de la aplicación.
 *
 * Se almacena mediante un puntero inteligente para garantizar
 * la liberación automática de recursos.
 */
Window g_window(Window(800, 600, "Labrid Engine"));

/**
 * @brief Figura utilizada durante la demostración.
 *
 * Se inicializa como un círculo y posteriormente se modifica
 * su color antes de comenzar el bucle principal.
 */
// CShape Circle(ShapeType::CIRCLE);
// CShape Circle(ShapeType::LINE);

// Registro: Script encargado de administrar los sistemas y entidades
// Paso 1:
// Paso 2: Creación de entidades

ECS::Registry registry;

/**
 * @brief Libera los recursos globales de la aplicación.
 *
 * Reinicia el puntero inteligente asociado a la ventana,
 * provocando la destrucción de la misma.
 */
void
destroy() {
  // g_window.reset();
}

/**
 * @brief Punto de entrada de la aplicación.
 *
 * Crea una ventana de renderizado, configura una figura
 * geométrica y ejecuta el bucle principal del programa.
 *
 * Durante cada iteración:
 * - Se procesan los eventos de la ventana.
 * - Se limpia la pantalla.
 * - Se renderizan los elementos de la escena.
 * - Se presenta el frame resultante.
 *
 * @return Código de finalización del programa.
 */
int
main()
{
  registry.AddSystem<ECS::RenderSystem>(g_window);

  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50)));

  // run the program as long as the window is open
  while (g_window.isOpen()) {
    while (const std::optional event = g_window.m_window->pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        g_window.close();
      }
    }

    float dt = 1.f / 60.f;

    // 1. Limpiar pantalla
    g_window.clear(sf::Color::Black);

    // 2. ACTUALIZAR SISTEMAS (Aquí es donde RenderSystem dibuja el círculo)
    registry.UpdateSystems(dt);

    // 3. Mostrar en pantalla
    g_window.display();
  }

  return 0;
}