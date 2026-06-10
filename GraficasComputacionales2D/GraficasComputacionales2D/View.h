#pragma once
#include "ECS/ComponentPool.h"

namespace
  ECS {

  /**
   * @class View
   * @brief Permite realizar consultas sobre entidades que poseen
   *        un conjunto específico de componentes.
   *
   * Una View actúa como un filtro sobre los ComponentPool asociados
   * a los tipos indicados en la plantilla. Sólo las entidades que
   * poseen todos los componentes especificados serán incluidas
   * durante la iteración.
   *
   * Para optimizar el recorrido, la vista utiliza como base el
   * pool con menor cantidad de elementos y verifica la existencia
   * de los demás componentes únicamente para esas entidades.
   *
   * @tparam Components Tipos de componentes requeridos por la vista.
   */
  template<typename... Components>
  class
    View {
  public:

    /**
     * @brief Construye una vista a partir de los pools indicados.
     *
     * Durante la construcción se identifica el pool con menor
     * cantidad de elementos para minimizar el coste de las
     * iteraciones posteriores.
     *
     * @param pools Punteros a los pools de componentes asociados.
     */
    explicit
      View(ComponentPool<Components>*... pools) noexcept
      : m_pools(pools...) {
      FindSmallest();
    }

    // =========================================================
    // Iteración principal
    // =========================================================

    /**
     * @brief Itera sobre todas las entidades válidas de la vista.
     *
     * Ejecuta la función proporcionada para cada entidad que posea
     * todos los componentes requeridos.
     *
     * La firma esperada del callback es:
     * @code
     * void(EntityID, Components&...)
     * @endcode
     *
     * El recorrido se realiza en orden inverso para permitir la
     * eliminación segura de entidades durante la iteración.
     *
     * @tparam Func Tipo de función, lambda o functor.
     * @param func Función a ejecutar para cada entidad encontrada.
     */
    template<typename Func>
    void
      Each(Func&& func) {
      if (!m_smallest)
        return;

      const auto& entities = m_smallest->GetEntities();

      // Recorrido inverso para permitir eliminaciones seguras.
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

    /**
     * @brief Itera únicamente sobre los identificadores de entidad.
     *
     * Resulta útil cuando los componentes serán obtenidos manualmente
     * desde el Registry o cuando únicamente interesa el EntityID.
     *
     * La firma esperada del callback es:
     * @code
     * void(EntityID)
     * @endcode
     *
     * @tparam Func Tipo de función, lambda o functor.
     * @param func Función a ejecutar para cada entidad encontrada.
     */
    template<typename Func>
    void
      EachEntity(Func&& func) {
      if (!m_smallest)
        return;

      const auto& entities = m_smallest->GetEntities();

      for (std::size_t i = entities.size(); i > 0; --i)
      {
        const EntityID entity = entities[i - 1];

        if (AllHave(entity))
          func(entity);
      }
    }

    /**
     * @brief Indica si la vista contiene entidades válidas.
     *
     * @return true si no existen entidades que cumplan los requisitos.
     * @return false en caso contrario.
     */
    [[nodiscard]] bool
      Empty() const noexcept {
      return !m_smallest || m_smallest->Empty();
    }

    /**
     * @brief Obtiene el número de entidades almacenadas en el pool base.
     *
     * Este valor corresponde al tamaño del pool más pequeño utilizado
     * para realizar el filtrado de la vista.
     *
     * @return Número de elementos considerados por la vista.
     */
    [[nodiscard]] std::size_t Size() const noexcept {
      return m_smallest ? m_smallest->Size() : 0;
    }

  private:

    /**
     * @brief Localiza el pool con menor cantidad de elementos.
     *
     * Este pool será utilizado como base para las iteraciones,
     * reduciendo el número de comprobaciones necesarias para
     * verificar la presencia del resto de componentes.
     *
     * @tparam I Índice interno utilizado durante la recursión
     *           en tiempo de compilación.
     */
    template<std::size_t I = 0>
    void
      FindSmallest() noexcept {
      if constexpr (I < sizeof...(Components)) {
        auto* pool = std::get<I>(m_pools);

        if (pool &&
          (!m_smallest || pool->Size() < m_smallest->Size()))
        {
          m_smallest = pool;
        }

        FindSmallest<I + 1>();
      }
    }

    /**
     * @brief Comprueba si una entidad posee todos los componentes
     *        requeridos por la vista.
     *
     * @param entity Entidad a verificar.
     *
     * @return true si la entidad pertenece a todos los pools.
     * @return false en caso contrario.
     */
    [[nodiscard]] bool
      AllHave(EntityID entity) const noexcept
    {
      return std::apply(
        [entity](auto*... pools) noexcept {
          return (... && (pools && pools->Contains(entity)));
        },
        m_pools);
    }

  private:

    /**
     * @brief Tupla que almacena los pools asociados a los tipos
     *        de componentes de la vista.
     */
    std::tuple<ComponentPool<Components>*...> m_pools;

    /**
     * @brief Pool utilizado como base para las iteraciones.
     *
     * Corresponde al pool con menor cantidad de elementos.
     */
    const SparseSet* m_smallest = nullptr;
  };

} // namespace ECS