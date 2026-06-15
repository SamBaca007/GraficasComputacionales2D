#pragma once

#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/View.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "Core/Window.h"
#include <SFML/Graphics.hpp>

namespace ECS {
  class RenderSystem final : public System {
  public:
    explicit RenderSystem(Window& window) noexcept
      : m_window(window) {
    }

    void OnUpdate(Registry& registry, float /*deltaTime*/) override {
      registry.GetView<Transform, Render>().Each(
        [this](EntityID /*entity*/, Transform& t, Render& r) {
          if (!r.shape || !r.visible) return;

          r.shape->setPosition(t.position);
          r.shape->setRotation(sf::degrees(t.rotation));
          r.shape->setScale(t.scale);
          r.shape->setFillColor(r.fillColor);

          m_window.draw(*r.shape);
        });
    }

  private:
    Window& m_window;
  };
} // Namespace ECS