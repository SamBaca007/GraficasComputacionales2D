#pragma once
#include "Core/Window.h"
#include "ECS/Components/Path.h"
#include <SFML/Graphics/VertexArray.hpp>

namespace ECS {
  class PathRenderSystem {
  public:
    // 1. Variable estática para controlar la visibilidad (apagada por defecto).
    // Nota: El uso de 'inline static' requiere C++17 o superior.
    inline static bool s_showDebugPath = false;

    // 2. Método para alternar la visibilidad desde tu gestor de eventos
    static void ToggleDebugPath() {
      s_showDebugPath = !s_showDebugPath;
    }

    static void Render(Window& window, const Path& path) {
      // 3. Condición de salida temprana: si no queremos depurar, no dibujamos.
      if (!s_showDebugPath) return;

      if (path.points.empty()) return;

      // Usamos LineStrip para conectar los puntos automáticamente
      sf::VertexArray trackLines(sf::PrimitiveType::LineStrip, path.points.size() + 1);

      for (size_t i = 0; i < path.points.size(); ++i) {
        trackLines[i].position = path.points[i];
        trackLines[i].color = sf::Color::Yellow; // Color de depuración
      }

      // Cerramos el circuito conectando el último punto con el primero
      trackLines[path.points.size()].position = path.points[0];
      trackLines[path.points.size()].color = sf::Color::Yellow;

      // Dibujamos usando tu clase Window
      window.draw(trackLines);
    }
  };
} // Namespace ECS