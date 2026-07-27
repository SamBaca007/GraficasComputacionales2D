#pragma once
#include "Core/Window.h"
#include "ECS/Components/Path.h"
#include <SFML/Graphics/VertexArray.hpp>

namespace ECS {
  class PathRenderSystem {
  public:
    static void Render(Window& window, const Path& path) {
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