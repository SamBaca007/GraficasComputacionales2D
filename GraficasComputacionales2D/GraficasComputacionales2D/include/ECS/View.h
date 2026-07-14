/**
 * @file View.h
 * @brief Definición de la clase View, utilizada para realizar consultas (queries)
 *        multi-componente eficientes dentro del ECS.
 */

#pragma once
#include "ECS/ComponentPool.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class View
   * @brief Vista que permite iterar sobre todas las entidades que poseen un conjunto
   *        específico de componentes.
   *
   * @details Optimiza la iteración buscando primero el pool con la menor cantidad
   *          de elementos (el "cuello de botella") y verificando las entidades de
   *          ese pool contra el resto, minimizando así las comprobaciones necesarias.
   *
   * @tparam Components Paquete de tipos de los componentes requeridos por esta vista.
   */
  template<typename... Components>
  class View {
  public:
    /**
     * @brief Constructor de la vista.
     *
     * @details Inicializa la tupla de pools y determina cuál de ellos es el más pequeño
     *          para optimizar las iteraciones posteriores.
     *
     * @param pools Punteros a los ComponentPool de cada componente requerido.
     */
    explicit View(ComponentPool<Components>*... pools) noexcept
      : m_pools(pools...) {
      FindSmallest();
    }

    // Iteración principal
    // Callback: void(EntityID, Components&...)

    /**
     * @brief Itera sobre todas las entidades que cumplen con la firma de componentes y
     *        ejecuta un callback para cada una.
     *
     * @details Realiza un recorrido inverso (de fin a inicio). Esto hace que la iteración
     *          sea segura incluso si se eliminan entidades o componentes durante la ejecución
     *          del callback.
     *
     * @tparam Func Tipo del callback o expresión lambda.
     * @param func Función invocable con la firma: void(EntityID, Components&...).
     */
    template<typename Func>
    void Each(Func&& func)
    {
      if (!m_smallest) return;

      const auto& entities = m_smallest->GetEntities();

      // Recorrido inverso -> seguro al eliminar durante la iteración
      for (std::size_t i = entities.size(); i > 0; --i)
      {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity))
        {
          std::apply(
            [&](auto*... pools) {
              func(entity, pools->Get(entity)...);
            },
            m_pools);
        }
      }
    }

    // Iteración solo de entidades
    // Útil cuando solo necesitas el EntityID y accedes a
    // componentes manualmente.

    /**
     * @brief Itera únicamente proporcionando el ID de las entidades que cumplen con la firma.
     *
     * @details Es útil cuando solo necesitas el EntityID y prefieres acceder a los
     *          componentes manualmente o enviarlos a otros sistemas. También utiliza
     *          el recorrido inverso seguro.
     *
     * @tparam Func Tipo del callback o expresión lambda.
     * @param func Función invocable con la firma: void(EntityID).
     */
    template<typename Func>
    void EachEntity(Func&& func)
    {
      if (!m_smallest) return;
      const auto& entities = m_smallest->GetEntities();
      for (std::size_t i = entities.size(); i > 0; --i)
      {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity))
          func(entity);
      }
    }

    /**
     * @brief Verifica si la vista está vacía.
     *
     * @details Se basa en el pool más pequeño; si este está vacío o no existe,
     *          es imposible que haya una entidad con todos los componentes.
     *
     * @return true Si no hay entidades posibles en esta vista.
     * @return false Si hay al menos una entidad potencial.
     */
    [[nodiscard]] bool  Empty() const noexcept { return !m_smallest || m_smallest->Empty(); }

    /**
     * @brief Obtiene la cantidad de entidades en el pool más pequeño.
     *
     * @details Esto representa el límite máximo teórico de entidades que podrían cumplir
     *          con la firma completa de la vista.
     *
     * @return std::size_t Número máximo posible de entidades en la vista.
     */
    [[nodiscard]] std::size_t Size() const noexcept { return m_smallest ? m_smallest->Size() : 0; }


  private:
    /**
     * @brief Encuentra el pool con la menor cantidad de elementos.
     *
     * @details Este método recursivo de plantillas (template metaprogramming) busca
     *          el pool más pequeño en la tupla durante la inicialización, estableciéndolo
     *          como el "cuello de botella" y optimizando así el filtro de iteración.
     *
     * @tparam I Índice actual evaluado en la tupla.
     */
     // Encuentra el pool con menos elementos (mejor filtro)
    template<std::size_t I = 0>
    void FindSmallest() noexcept {
      if constexpr (I < sizeof...(Components)) {
        auto* pool = std::get<I>(m_pools);
        if (pool && (!m_smallest || pool->Size() < m_smallest->Size()))
          m_smallest = pool;
        FindSmallest<I + 1>();
      }
    }

    /**
     * @brief Comprueba si una entidad específica posee todos los componentes de la vista.
     *
     * @param entity El identificador de la entidad a evaluar.
     * @return true Si la entidad existe en todos los pools solicitados.
     * @return false Si falta al menos un componente.
     */
    [[nodiscard]] bool AllHave(EntityID entity) const noexcept
    {
      return std::apply(
        [entity](auto*... pools) noexcept {
          return (... && (pools && pools->Contains(entity)));
        },
        m_pools);
    }

  private:
    /** @brief Tupla que almacena punteros a los pools de cada componente. */
    std::tuple<ComponentPool<Components>*...> m_pools;

    /** @brief Puntero al SparseSet del componente con menor cantidad de entidades. */
    const SparseSet* m_smallest = nullptr;
  };
} // Namespace ECS