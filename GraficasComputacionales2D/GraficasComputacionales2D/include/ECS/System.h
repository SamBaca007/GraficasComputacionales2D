#pragma once

/**
 * @file System.h
 * @brief Define la clase base para todos los sistemas del motor ECS.
 */

namespace ECS {

  /**
   * @class Registry
   * @brief Declaración adelantada del registro ECS.
   */
  class Registry;

  /**
   * @class System
   * @brief Clase base abstracta para todos los sistemas del motor.
   *
   * Un sistema contiene únicamente lógica de comportamiento.
   * Los datos se almacenan exclusivamente en componentes.
   *
   * Ciclo de vida:
   * - OnStart(): llamado una vez al registrar el sistema.
   * - OnUpdate(): llamado cada frame.
   * - OnDestroy(): llamado al destruir o desregistrar el sistema.
   */
  class System {
  public:

    /**
     * @brief Destructor virtual por defecto.
     */
    virtual ~System() = default;

    /**
     * @brief Inicializa el sistema.
     *
     * Puede utilizarse para reservar recursos,
     * registrar eventos o realizar configuraciones iniciales.
     *
     * @param registry Referencia al registro ECS.
     */
    virtual void OnStart(Registry& registry) {}

    /**
     * @brief Ejecuta la lógica principal del sistema.
     *
     * Este método se invoca una vez por frame.
     *
     * @param registry Referencia al registro ECS.
     * @param deltaTime Tiempo transcurrido desde el frame anterior.
     */
    virtual void OnUpdate(Registry& registry, float deltaTime) = 0;

    /**
     * @brief Libera los recursos utilizados por el sistema.
     *
     * Se llama antes de destruir o desregistrar el sistema.
     *
     * @param registry Referencia al registro ECS.
     */
    virtual void OnDestroy(Registry& registry) {}

    /**
     * @brief Activa o desactiva el sistema.
     *
     * @param enabled Estado deseado del sistema.
     */
    void SetEnabled(bool enabled) noexcept { m_enabled = enabled; }

    /**
     * @brief Indica si el sistema está activo.
     *
     * @return true si el sistema está habilitado.
     * @return false si el sistema está deshabilitado.
     */
    [[nodiscard]] bool IsEnabled() const noexcept { return m_enabled; }

  private:

    /**
     * @brief Estado de activación del sistema.
     */
    bool m_enabled = true;
  };

} // namespace ECS