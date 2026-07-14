/**
 * @file ComponentPool.h
 * @brief Implementación de las clases IComponentPool y ComponentPool para la gestión
 *        de múltiples pools de componentes en el ECS.
 */

#pragma once
#include "Prerequisites.h"
#include "SparseSet.h"

namespace
	ECS {

	/**
	 * @class IComponentPool
	 * @brief Interfaz polimórfica sin tipo para que Registry pueda gestionar pools heterogéneos.
	 *
	 * @details Al heredar de SparseSet, permite manejar la lógica de entidades escasas (sparse)
	 *          mientras se oculta el tipo específico del componente.
	 */
	class
		IComponentPool : public SparseSet {
	public:
		/**
		 * @brief Destructor virtual por defecto.
		 */
		virtual
			~IComponentPool() = default;

		/**
		 * @brief Elimina el componente asociado a una entidad si existe.
		 *
		 * @param entity El identificador de la entidad a procesar.
		 */
		virtual void
			RemoveEntity(EntityID entity) = 0;

		/**
		 * @brief Obtiene un puntero sin tipo (void*) al componente.
		 *
		 * @details Útil para sistemas genéricos o para el Serializer que necesitan
		 *          acceder a los datos sin conocer el tipo en tiempo de compilación.
		 *
		 * @param entity El identificador de la entidad.
		 * @return void* Puntero al componente, o nullptr si no existe.
		 */
		virtual void*
			GetRaw(EntityID entity) noexcept = 0;
	};

	/**
	 * @class ComponentPool
	 * @brief Almacena componentes de un tipo específico T en un arreglo denso (dense array)
	 *        paralelo al de SparseSet.
	 *
	 * @details El método Remove utiliza un enfoque 'swap-with-last' (intercambio con el último)
	 *          igual que el SparseSet para mantener los dos arreglos sincronizados eficientemente
	 *          sin dejar huecos en la memoria.
	 *
	 * @tparam T El tipo de componente que este pool almacenará y gestionará.
	 */
	template<typename T>
	class
		ComponentPool final : public IComponentPool {
	public:
		/**
		 * @brief Añade un nuevo componente a la entidad especificada.
		 *
		 * @tparam Args Paquete de parámetros para el constructor del componente.
		 * @param entity La entidad a la que se le añadirá el componente.
		 * @param args Argumentos que se reenviarán (forward) al constructor del componente T.
		 * @return T& Referencia al componente recién creado.
		 */
		template<typename... Args> T&
			Add(EntityID entity, Args&&... args) {
			assert(!Contains(entity) && "La entidad ya tiene este componente");
			InsertEntity(entity); // registra en sparse/dense
			m_components.emplace_back(std::forward<Args>(args)...);
			return m_components.back();
		}

		/**
		 * @brief Obtiene una referencia modificable al componente de la entidad.
		 *
		 * @param entity La entidad de la cual obtener el componente.
		 * @return T& Referencia al componente.
		 */
		[[nodiscard]] T&
			Get(EntityID entity) noexcept {
			assert(Contains(entity) && "La entidad no tiene este componente");
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		/**
		 * @brief Obtiene una referencia de solo lectura al componente de la entidad.
		 *
		 * @param entity La entidad de la cual obtener el componente.
		 * @return const T& Referencia constante al componente.
		 */
		[[nodiscard]] const T&
			Get(EntityID entity) const noexcept {
			assert(Contains(entity) && "La entidad no tiene este componente");
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		/**
		 * @brief Intenta obtener un puntero al componente de la entidad.
		 *
		 * @param entity La entidad a consultar.
		 * @return T* Puntero al componente si existe, o nullptr si la entidad no lo tiene.
		 */
		[[nodiscard]] T*
			TryGet(EntityID entity) noexcept {
			if (!Contains(entity)) return nullptr;
			return &m_components[m_sparse[GetEntityIndex(entity)]];
		}

		/**
		 * @brief Elimina el componente asociado a una entidad usando swap-with-last.
		 *
		 * @details IMPORTANTE: Primero sincronizamos m_components y luego llamamos a
		 *          SparseSet::Remove para que sincronice m_dense. Ambos intercambios (swap)
		 *          usan el mismo denseIdx, así quedan alineados.
		 *
		 * @param entity La entidad cuyo componente será eliminado.
		 */
		void
			Remove(EntityID entity) override {
			if (!Contains(entity)) return;

			const EntityIndex denseIdx = m_sparse[GetEntityIndex(entity)];

			// Mueve el último componente al hueco
			m_components[denseIdx] = std::move(m_components.back());
			m_components.pop_back();

			// Sincroniza sparse/dense (base class)
			SparseSet::Remove(entity);
		}

		/**
		 * @brief Implementación de la interfaz IComponentPool para eliminar una entidad.
		 *        Llama internamente a Remove().
		 *
		 * @param entity La entidad a procesar.
		 */
		void
			RemoveEntity(EntityID entity) override { Remove(entity); }

		/**
		 * @brief Implementación de la interfaz IComponentPool para obtener el componente sin tipo.
		 *
		 * @param entity La entidad a consultar.
		 * @return void* Puntero al componente sin tipo, o nullptr si no existe.
		 */
		void*
			GetRaw(EntityID entity) noexcept override { return TryGet(entity); }

		/**
		 * @brief Obtiene el arreglo subyacente que contiene todos los componentes.
		 *
		 * @details Acceso masivo (útil para el Serializer o para los Sistemas que iteran).
		 *
		 * @return std::vector<T>& Referencia al vector denso de componentes.
		 */
		[[nodiscard]] std::vector<T>&
			GetComponents() noexcept { return m_components; }

		/**
		 * @brief Obtiene el arreglo subyacente constante que contiene todos los componentes.
		 *
		 * @return const std::vector<T>& Referencia constante al vector denso de componentes.
		 */
		[[nodiscard]] const std::vector<T>&
			GetComponents() const noexcept { return m_components; }

		/**
		 * @brief Limpia por completo el pool, eliminando todos los componentes y limpiando
		 *        los arreglos del SparseSet.
		 */
		void
			Clear() override {
			SparseSet::Clear();
			m_components.clear();
		}

	private:
		/** @brief Arreglo denso que almacena los componentes, paralelo al arreglo m_dense de SparseSet. */
		std::vector<T> m_components;
	};
} // Namespace ECS