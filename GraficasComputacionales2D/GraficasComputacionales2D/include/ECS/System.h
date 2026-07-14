/**
 * @file System.h
 * @brief Clase base para todos los sistemas del motor en el ECS.
 *
 * @details Un Sistema contiene únicamente LÓGICA, nunca datos.
 *          Los datos viven en los componentes.
 *
 *          Ciclo de vida:
 *            - OnStart   -> llamado una vez al registrar el sistema
 *            - OnUpdate  -> llamado cada frame
 *            - OnDestroy -> llamado al destruir o desregistrar el sistema
 */

#pragma once

namespace ECS {

  /**
   * @brief Declaración adelantada (forward declaration) para evitar inclusión circular.
   */
  class Registry;

  /**
   * @class System
   * @brief Clase base abstracta de la cual deben heredar todos los sistemas del ECS.
   */
  class
    System {
  public:
    /**
     * @brief Destructor virtual por defecto.
     */
    virtual ~System() = default;

    /**
     * @brief Inicialización del sistema.
     *
     * @details Se utiliza para reservar recursos, suscribirse a eventos, etc.
     *
     * @param registry Referencia al registro principal del ECS.
     */
     // Inicialización: reservar recursos, suscribirse a eventos, etc.
    virtual void OnStart(Registry& /*registry*/) {}

    /**
     * @brief Lógica principal del sistema ejecutada frame a frame.
     *
     * @param registry Referencia al registro principal del ECS.
     * @param deltaTime Tiempo transcurrido desde el último frame (Delta Time).
     */
     // Lógica frame-a-frame
    virtual void OnUpdate(Registry& registry, float deltaTime) = 0;

    /**
     * @brief Limpieza de recursos al destruir o desregistrar el sistema.
     *
     * @param registry Referencia al registro principal del ECS.
     */
     // Limpieza al destruir el sistema
    virtual void OnDestroy(Registry& /*registry*/) {}

    /**
     * @brief Activa o desactiva la ejecución del sistema sin destruirlo de la memoria.
     *
     * @param enabled true para activar el sistema, false para pausarlo.
     */
     // Opcional: activa/desactiva el sistema sin destruirlo
    void SetEnabled(bool enabled) noexcept { m_enabled = enabled; }

    /**
     * @brief Verifica si el sistema está actualmente activo y ejecutando su lógica.
     *
     * @return true Si el sistema está activo.
     * @return false Si el sistema está en pausa.
     */
    [[nodiscard]] bool IsEnabled() const noexcept { return m_enabled; }

  private:
    /** @brief Estado actual del sistema (activo por defecto). */
    bool m_enabled = true;
  };

} // Namespace ECS