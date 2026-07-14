/**
 * @file CameraSystem.h
 * @brief Implementación del CameraSystem para el Entity Component System.
 *
 * @details LÓGICA de cámara. Busca la cámara activa (Transform + Cámara),
 *          interpola su posición hacia el objetivo a seguir y aplica la
 *          vista resultante a la ventana.
 *
 *          ORDEN DE EJECUCIÓN: Este sistema debe registrarse ANTES del
 *          RenderSystem, porque el método setView de la ventana afecta a todo
 *          lo que se dibuje DESPUÉS de él.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "Core/Window.h"
#include <cmath>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class CameraSystem
   * @brief Sistema encargado de calcular la posición de la cámara y volcarla a la vista de la ventana.
   */
  class CameraSystem final : public System {
  public:
    /**
     * @brief Constructor del sistema de cámara.
     *
     * @param window Referencia a la ventana de la aplicación donde se aplicará la vista calculada.
     */
    explicit CameraSystem(Window& window) noexcept : m_window(window) {
    }

    /**
     * @brief Actualiza la lógica de la cámara frame a frame.
     *
     * @details Itera sobre las entidades con Transform y Camera. Si la cámara está activa
     *          y tiene un objetivo, utiliza un suavizado exponencial INDEPENDIENTE DEL FRAMERATE:
     *          `t = 1 - e^(-speed * dt)`.
     *          Esto garantiza exactamente el mismo movimiento de seguimiento ya sea a 38, 60
     *          o 144 FPS, a diferencia de un lerp (interpolación lineal) con factor fijo.
     *          Finalmente, vuelca la posición y el zoom de la cámara a la vista de SFML.
     *
     * @param registry Referencia al registro principal del ECS.
     * @param deltaTime Tiempo transcurrido desde el último frame.
     */
    void OnUpdate(Registry& registry, float deltaTime) override {
      registry.GetView<Transform, Camera>().Each([this, &registry, deltaTime]
      (EntityID, Transform& camT, Camera& cam) {
          if (!cam.active) return;

          // Seguimiento interpolado 
          // Suavizado exponencial INDEPENDIENTE DEL FRAMERATE:
          //   t = 1 - e^(-speed * dt)
          // Garantiza el mismo movimiento a 38 o a 144 FPS, a
          // diferencia de un lerp con factor fijo.
          if (cam.followTarget != NULL_ENTITY && registry.IsAlive(cam.followTarget)) {
            if (auto* targetT = registry.TryGetComponent<Transform>(cam.followTarget)) {
              const float t = 1.f - std::exp(-cam.followSpeed * deltaTime);
              camT.position += (targetT->position - camT.position) * t;
            }
          }

          // Vuelca la posición/zoom de la cámara a la vista SFML.
          m_window.applyCameraView(camT.position, cam.zoom, camT.rotation);
        });
    }

  private:
    /** @brief Referencia a la ventana donde se modificará la vista activa (cámara). */
    Window& m_window;
  };

} // Namespace ECS