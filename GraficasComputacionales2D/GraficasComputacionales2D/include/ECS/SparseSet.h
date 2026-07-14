/**
 * @file SparseSet.h
 * @brief Definición de la clase SparseSet, la estructura de datos base para la gestión
 *        eficiente de componentes en el ECS.
 */

#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"

 /**
	* @namespace ECS
	* @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
	*/
namespace ECS {

	/**
	 * @class SparseSet
	 * @brief Estructura de datos que mapea identificadores esparcidos (entidades) a un
	 *        arreglo denso, garantizando iteraciones continuas en memoria y búsquedas en O(1).
	 */
	class
		SparseSet {
	public:
		/**
		 * @brief Constructor por defecto.
		 */
		SparseSet() = default;

		/**
		 * @brief Destructor virtual por defecto.
		 */
		virtual ~SparseSet() = default;

		// Consultas

		/**
		 * @brief Comprueba si una entidad específica forma parte de este conjunto.
		 *
		 * @param entity El identificador de la entidad a buscar.
		 * @return true Si la entidad está registrada en el SparseSet.
		 * @return false Si la entidad no existe o su índice está fuera de rango.
		 */
		[[nodiscard]] bool Contains(EntityID entity) const noexcept
		{
			const EntityIndex idx = GetEntityIndex(entity);
			if (idx >= m_sparse.size()) return false;
			const EntityIndex denseIdx = m_sparse[idx];
			return denseIdx < m_dense.size() && m_dense[denseIdx] == entity;
		}

		/**
		 * @brief Obtiene la cantidad de entidades actualmente almacenadas en el conjunto.
		 *
		 * @return size_t Número de entidades válidas.
		 */
		[[nodiscard]] size_t Size()  const noexcept { return m_dense.size(); }

		/**
		 * @brief Verifica si el conjunto está vacío.
		 *
		 * @return true Si no hay entidades almacenadas.
		 * @return false Si hay al menos una entidad en el conjunto.
		 */
		[[nodiscard]] bool   Empty() const noexcept { return m_dense.empty(); }

		/**
		 * @brief Obtiene una referencia de solo lectura al arreglo denso de entidades.
		 *
		 * @details Útil para iterar linealmente sobre todas las entidades válidas en caché.
		 *
		 * @return const std::vector<EntityID>& Arreglo denso de identificadores de entidad.
		 */
		[[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept
		{
			return m_dense;
		}

		// Eliminación (swap-with-last)

		/**
		 * @brief Elimina una entidad del conjunto mediante la técnica 'swap-with-last'.
		 *
		 * @details IMPORTANTE: Las subclases DEBEN llamar a este método base DESPUÉS de
		 *          sincronizar sus propios arreglos (ver ComponentPool::Remove).
		 *          La técnica consiste en mover el último elemento del arreglo denso a la
		 *          posición eliminada, evitando huecos y manteniendo la memoria contigua.
		 *
		 * @param entity El identificador de la entidad a eliminar.
		 */
		virtual void Remove(EntityID entity)
		{
			if (!Contains(entity)) return;

			const EntityIndex sparseIdx = GetEntityIndex(entity);
			const EntityIndex denseIdx = m_sparse[sparseIdx];
			const EntityID    last = m_dense.back();

			// Mueve el último elemento al hueco
			m_dense[denseIdx] = last;
			m_sparse[GetEntityIndex(last)] = denseIdx;
			m_dense.pop_back();

			// Invalida la entrada eliminada
			m_sparse[sparseIdx] = INVALID;
		}

		/**
		 * @brief Limpia por completo tanto el arreglo disperso como el denso.
		 */
		virtual void Clear()
		{
			m_sparse.clear();
			m_dense.clear();
		}

	protected:
		/**
		 * @brief Registra una entidad en el arreglo denso y reserva espacio en el disperso.
		 *
		 * @param entity El identificador de la entidad a insertar.
		 * @return EntityIndex El índice que se le asignó dentro del arreglo denso.
		 */
		 // Reserva espacio en m_sparse y registra la entidad en m_dense.
		 // Devuelve el denseIndex asignado.
		EntityIndex InsertEntity(EntityID entity)
		{
			const EntityIndex sparseIdx = GetEntityIndex(entity);
			const EntityIndex denseIdx = static_cast<EntityIndex>(m_dense.size());

			if (sparseIdx >= m_sparse.size())
				m_sparse.resize(sparseIdx + 1, INVALID);

			assert(m_sparse[sparseIdx] == INVALID && "La entidad ya está en el set");

			m_sparse[sparseIdx] = denseIdx;
			m_dense.push_back(entity);
			return denseIdx;
		}

	protected:
		/** @brief Valor constante utilizado para marcar un índice inválido o vacío. */
		static constexpr EntityIndex INVALID = std::numeric_limits<EntityIndex>::max();

		/** @brief Arreglo disperso (sparse). Mapea el índice de la entidad al índice en el arreglo denso. */
		std::vector<EntityIndex> m_sparse;   // sparse[entityIndex] -> dense index

		/** @brief Arreglo denso (dense). Almacena los identificadores completos de las entidades de forma contigua. */
		std::vector<EntityID>    m_dense;    // dense[i] -> EntityID
	};
} // Namespace ECS