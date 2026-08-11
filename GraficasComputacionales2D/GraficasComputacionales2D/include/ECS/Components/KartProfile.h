#pragma once
#include <string>
#include <SFML/Graphics/Color.hpp>

namespace ECS {
  struct KartProfile {
    std::string name = "Piloto Desconocido";
    sf::Color uiColor = sf::Color::White;
  };
}