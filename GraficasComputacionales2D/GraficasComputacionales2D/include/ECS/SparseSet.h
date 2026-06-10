#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"

namespace
  ECS {

  /**
   * @class SparseSet
   * @brief Implementa una estructura Sparse Set para almacenar entidades.
   *
   * Un Sparse Set permite inserciones, búsquedas y eliminaciones
   * en tiempo constante O(1) mediante el uso de dos vectores:
   *
   * - m_sparse: relaciona el índice de una entidad con su posición
   *   dentro del vector denso.
   * - m_dense: almacena las entidades activas de forma contigua.
   *
   * Esta estructura sirve como base para los pools de componentes,
   * permitiendo iterar eficientemente sobre las entidades que
   * poseen un determinado componente.
   */
  class
    SparseSet {
  public:

    /**
     * @brief Constructor por defecto.
     */
    SparseSet();

    /**
     * @brief Destructor virtual por defecto.
     */
    virtual
      ~SparseSet() = default;

    // =========================================================
    // Consultas
    // =========================================================

    /**
     * @brief Comprueba si una entidad pertenece al conjunto.
     *
     * La operación se realiza en tiempo constante mediante
     * la correspondencia entre los vectores sparse y dense.
     *
     * @param entity Entidad a buscar.
     *
     * @return true si la entidad está almacenada.
     * @return false en caso contrario.
     */
    [[nodiscard]] bool
      Contains(EntityID entity) const noexcept
    {
      const EntityIndex idx = GetEntityIndex(entity);

      if (idx >= m_sparse.size())
        return false;

      const EntityIndex denseIdx = m_sparse[idx];

      return denseIdx < m_dense.size() &&
             m_dense[denseIdx] == entity;
    }

    /**
     * @brief Obtiene el número de entidades almacenadas.
     *
     * @return Cantidad de elementos del conjunto.
     */
    [[nodiscard]] size_t Size() const noexcept {
      return m_dense.size();
    }

    /**
     * @brief Indica si el conjunto está vacío.
     *
     * @return true si no contiene entidades.
     * @return false en caso contrario.
     */
    [[nodiscard]] bool
      Empty() const noexcept {
      return m_dense.empty();
    }

    /**
     * @brief Obtiene todas las entidades almacenadas.
     *
     * Las entidades se devuelven en el mismo orden en el que
     * se encuentran dentro del vector denso.
     *
     * @return Referencia constante al vector de entidades.
     */
    [[nodiscard]] const std::vector<EntityID>&
      GetEntities() const noexcept
    {
      return m_dense;
    }

    // =========================================================
    // Eliminación
    // =========================================================

    /**
     * @brief Elimina una entidad del conjunto.
     *
     * Utiliza la técnica "swap-with-last", intercambiando
     * la entidad a eliminar con el último elemento del vector
     * denso para mantener una eliminación en tiempo constante.
     *
     * Las clases derivadas deben sincronizar primero sus
     * estructuras de datos internas y posteriormente invocar
     * esta implementación base.
     *
     * @param entity Entidad a eliminar.
     */
    virtual void
      Remove(EntityID entity) {
      if (!Contains(entity))
        return;

      const EntityIndex sparseIdx = GetEntityIndex(entity);
      const EntityIndex denseIdx = m_sparse[sparseIdx];
      const EntityID last = m_dense.back();

      // Mueve el último elemento al hueco liberado.
      m_dense[denseIdx] = last;
      m_sparse[GetEntityIndex(last)] = denseIdx;

      m_dense.pop_back();

      // Marca la entrada como inválida.
      m_sparse[sparseIdx] = INVALID;
    }

    /**
     * @brief Elimina todas las entidades almacenadas.
     *
     * Vacía tanto el vector sparse como el vector dense.
     */
    virtual void
      Clear()
    {
      m_sparse.clear();
      m_dense.clear();
    }

  protected:

    /**
     * @brief Inserta una entidad en el conjunto.
     *
     * Reserva espacio en el vector sparse si es necesario,
     * registra la correspondencia sparse-dense y añade
     * la entidad al final del vector denso.
     *
     * @param entity Entidad a insertar.
     *
     * @return Índice asignado dentro del vector denso.
     */
    EntityIndex
      InsertEntity(EntityID entity)
    {
      const EntityIndex sparseIdx = GetEntityIndex(entity);
      const EntityIndex denseIdx =
        static_cast<EntityIndex>(m_dense.size());

      if (sparseIdx >= m_sparse.size())
        m_sparse.resize(sparseIdx + 1, INVALID);

      assert(
        m_sparse[sparseIdx] == INVALID &&
        "La entidad ya está en el set."
      );

      m_sparse[sparseIdx] = denseIdx;
      m_dense.push_back(entity);

      return denseIdx;
    }

  protected:

    /**
     * @brief Valor utilizado para representar una posición inválida.
     */
    static constexpr
      EntityIndex INVALID =
      std::numeric_limits<EntityIndex>::max();

    /**
     * @brief Mapeo de índices de entidad a índices densos.
     *
     * sparse[entityIndex] -> denseIndex
     */
    std::vector<EntityIndex> m_sparse;

    /**
     * @brief Almacenamiento contiguo de entidades activas.
     *
     * dense[i] -> EntityID
     */
    std::vector<EntityID> m_dense;
  };

} // namespace ECS