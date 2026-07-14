/**
 * @file RenderSystem.h
 * @brief Implementación del RenderSystem para el Entity Component System.
 *
 * @details Este sistema se encarga de tomar los datos de los componentes Transform
 *          y Render de las entidades, y dibujarlos en la ventana principal.
 *          Muestra un claro ejemplo de cómo un sistema depende de otros componentes
 *          para realizar su lógica funcional.
 */

#pragma once

#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/View.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "Core/Window.h"
#include <SFML/Graphics.hpp>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class RenderSystem
   * @brief Sistema encargado de renderizar las entidades en pantalla.
   *
   * @details Hereda de ECS::System y se declara como final. Su única responsabilidad
   *          es iterar sobre las entidades que poseen tanto un componente Transform
   *          como un componente Render, y dibujarlas en la ventana.
   */
  class RenderSystem final : public System {
  public:
    /**
     * @brief Constructor del sistema de renderizado.
     *
     * @param window Referencia a la ventana (Window) donde se dibujarán las formas.
     */
    explicit RenderSystem(Window& window) noexcept
      : m_window(window) {
    }

    /**
     * @brief Lógica de renderizado frame a frame.
     *
     * @details Consulta el registro (Registry) en busca de todas las entidades que
     *          tengan los componentes Transform y Render. Si la entidad tiene una forma
     *          asignada y está marcada como visible, sincroniza la posición, rotación
     *          y escala del componente Transform hacia la forma de SFML antes de dibujarla.
     *
     * @param registry Referencia al registro principal del ECS.
     * @param deltaTime Tiempo transcurrido desde el último frame (no utilizado en este sistema).
     */
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
    /** @brief Referencia a la ventana de renderizado principal. */
    Window& m_window;
  };
} // Namespace ECS