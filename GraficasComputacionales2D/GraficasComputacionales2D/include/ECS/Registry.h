#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/View.h"
#include "ECS/System.h"
/**
 * @file Registry.h
 * @brief Implementa el registro principal del sistema ECS.
 *
 * La clase Registry es el núcleo de la arquitectura Entity-Component-System.
 * Se encarga de:
 * - Crear y destruir entidades.
 * - Almacenar y gestionar componentes.
 * - Mantener pools de componentes por tipo.
 * - Registrar y actualizar sistemas.
 * - Proporcionar consultas sobre entidades y componentes.
 */
namespace
  ECS {
  /**
   * @class Registry
   * @brief Administrador central del sistema ECS.
   *
   * Registry coordina la creación de entidades, el almacenamiento
   * de componentes y la ejecución de sistemas.
   *
   * Cada entidad se identifica mediante un EntityID único compuesto
   * por un índice y una versión. Esto permite invalidar automáticamente
   * identificadores obsoletos cuando una entidad es destruida.
   */
  class
    Registry {
  public:
    /**
     * @brief Crea una nueva entidad.
     *
     * Si existen índices libres reutiliza uno de ellos; en caso contrario,
     * crea una nueva entrada en los contenedores internos.
     *
     * @return Identificador de la entidad creada.
     */
    EntityID CreateEntity() {
      EntityIndex idx;
      if (!m_freeList.empty()) {
        idx = m_freeList.front();
        m_freeList.pop();
      }
      else {
        idx = static_cast<EntityIndex>(m_versions.size());
        m_versions.push_back(0);
        m_entities.push_back(NULL_ENTITY); // placeholder
      }

      EntityID id = MakeEntityID(idx, m_versions[idx]);
      m_entities[idx] = id;
      return id;
    }
    /**
     * @brief Destruye una entidad existente.
     *
     * Elimina todos los componentes asociados a la entidad,
     * incrementa su versión para invalidar referencias antiguas
     * y libera su índice para futuras reutilizaciones.
     *
     * @param entity Entidad a destruir.
     */
    void
    DestroyEntity(EntityID entity) {
      assert(IsAlive(entity) && "DestroyEntity: entidad inválida o ya destruida");

      // Elimina todos los componentes de esta entidad
      for (auto& [typeID, pool] : m_componentPools)
        pool->RemoveEntity(entity);

      // Incrementa versión -> los IDs viejos quedan inválidos
      const EntityIndex idx = GetEntityIndex(entity);
      ++m_versions[idx];
      m_entities[idx] = NULL_ENTITY;
      m_freeList.push(idx);
    }
    /**
     * @brief Comprueba si una entidad sigue siendo válida.
     *
     * Una entidad es válida si su índice existe y la versión
     * almacenada coincide con la versión actual.
     *
     * @param entity Entidad a verificar.
     * @return true si la entidad existe.
     * @return false en caso contrario.
     */
    [[nodiscard]] bool
      IsAlive(EntityID entity) const noexcept {
      const EntityIndex idx = GetEntityIndex(entity);
      return idx < m_entities.size() && m_entities[idx] == entity;
    }
    /**
     * @brief Obtiene el número de entidades activas.
     *
     * No cuenta las entidades destruidas cuyos índices se encuentran
     * en la lista de reutilización.
     *
     * @return Cantidad de entidades vivas.
     */
    [[nodiscard]] std::size_t
      EntityCount() const noexcept {
      return m_entities.size() - m_freeList.size();
    }
    /**
     * @brief Obtiene el contenedor interno de entidades.
     *
     * Puede contener entradas inválidas representadas mediante
     * NULL_ENTITY.
     *
     * Resulta útil para procesos de serialización o depuración.
     *
     * @return Referencia constante al vector de entidades.
     */
    [[nodiscard]] const std::vector<EntityID>&
      GetEntities() const noexcept {
      return m_entities;
    }

    // Componentes

    /**
     * @brief Añade un componente a una entidad.
     *
     * Si el pool correspondiente aún no existe, se crea automáticamente.
     *
     * @tparam T Tipo del componente.
     * @tparam Args Tipos de los argumentos de construcción.
     *
     * @param entity Entidad propietaria del componente.
     * @param args Argumentos utilizados para construir el componente.
     *
     * @return Referencia al componente creado.
     */
    template<typename T, typename... Args> T&
      AddComponent(EntityID entity, Args&&... args) {
      assert(IsAlive(entity) && "AddComponent: entidad inválida");
      return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
    }
    /**
     * @brief Elimina un componente de una entidad.
     *
     * Si la entidad no posee dicho componente,
     * la operación no produce ningún efecto.
     *
     * @tparam T Tipo del componente.
     * @param entity Entidad objetivo.
     */
    template<typename T> void
      RemoveComponent(EntityID entity) {
      if (auto* pool = GetPool<T>())
        pool->Remove(entity);
    }
    /**
     * @brief Sustituye o crea un componente.
     *
     * Si la entidad ya posee un componente del tipo indicado,
     * sus datos se reemplazan. En caso contrario se crea uno nuevo.
     *
     * @tparam T Tipo del componente.
     * @param entity Entidad propietaria.
     * @param value Nuevo valor del componente.
     *
     * @return Referencia al componente almacenado.
     */
    template<typename T>
    T& SetComponent(EntityID entity, T value) {
      assert(IsAlive(entity) && "SetComponent: entidad inválida");
      auto* pool = GetOrCreatePool<T>();
      if (pool->Contains(entity)) {
        pool->Get(entity) = std::move(value);
        return pool->Get(entity);
      }
      return pool->Add(entity, std::move(value));
    }
    /**
     * @brief Comprueba si una entidad posee un componente.
     *
     * @tparam T Tipo del componente.
     * @param entity Entidad a consultar.
     *
     * @return true si el componente existe.
     * @return false en caso contrario.
     */
    template<typename T>
    [[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
      const auto* pool = GetPoolConst<T>();
      return pool && pool->Contains(entity);
    }

    // Acceso garantizado (assert si no existe)
    /**
     * @brief Obtiene una referencia a un componente existente.
     *
     * Produce una aserción si la entidad no posee dicho componente.
     *
     * @tparam T Tipo del componente.
     * @param entity Entidad propietaria.
     *
     * @return Referencia al componente solicitado.
     */
    template<typename T>
    [[nodiscard]] T& GetComponent(EntityID entity) const
    {
      assert(IsAlive(entity));
      const auto* pool = GetConstPool<T>();
      assert(pool && "GetComponent: pool no existe para este tipo");
      return pool->Get(entity);
    }
    /**
     * @brief Obtiene un componente de forma segura.
     *
     * A diferencia de GetComponent(), no produce una aserción.
     *
     * @tparam T Tipo del componente.
     * @param entity Entidad propietaria.
     *
     * @return Puntero al componente si existe.
     * @return nullptr si no existe.
     */
    template<typename T>
    [[nodiscard]] T* TryGetComponent(EntityID entity) noexcept
    {
      auto* pool = GetPool<T>();
      return pool ? pool->TryGet(entity : nullptr;)
    }

    // Views (queries multi-componente)
    // Ejemplo: registry.GetView<Transform, Velocity>()
    /**
     * @brief Registra un nuevo sistema.
     *
     * El sistema se construye, se inicializa mediante OnStart()
     * y queda registrado para recibir actualizaciones.
     *
     * @tparam T Tipo del sistema.
     * @tparam Args Tipos de los argumentos de construcción.
     *
     * @param args Argumentos enviados al constructor.
     *
     * @return Referencia al sistema creado.
     */
    template<typename T, typename... Args>
    T& AddSystem(Args&&... args)
    {
      static_assert(std::is_base_of_v<System, T>, "T debe derivar de ECS::System");
      auto system = std::make_unique<T>(std::forward<Args>(args)...);
      T& ref = *system;
      system->OnStart(*this);
      m_systems.push_back(std::move(system));
      return ref;
    }
    /**
     * @brief Actualiza todos los sistemas habilitados.
     *
     * Invoca OnUpdate() sobre cada sistema registrado.
     *
     * @param deltaTime Tiempo transcurrido desde el último frame.
     */
    void
      UpdateSystems(float deltaTime)
    {
      for (auto& system : m_systems)
        if (system->IsEnabled())
          system->OnUpdate(*this, deltaTime);
    }
    /**
     * @brief Elimina todos los sistemas registrados.
     *
     * Antes de eliminarlos invoca OnDestroy() para permitir
     * la liberación de recursos.
     */
    void
      RemoveAllSystems()
    {
      for (auto& system : m_systems)
        system->OnDestroy(*this);
      m_systems.clear();
    }

  private:
    // Helpers privados
    /**
     * @brief Obtiene o crea el pool correspondiente a un tipo de componente.
     *
     * @tparam T Tipo del componente.
     *
     * @return Puntero al pool solicitado.
     */
    template<typename T>
    ComponentPool<T> GetOrCreatePool()
    {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      if (it == m_componentPools.end())
      {
        auto [newIt, ok] = m_componentPools.emplace(
          typeID, std::make_unique<ComponentPool<T>>());
        return static_cast<ComponentPool<T>*>(newIt->second.get());
      }
      return static_cast<ComponentPool<T>*>(it->second.get());
    }
    /**
     * @brief Obtiene un pool existente.
     *
     * @tparam T Tipo del componente.
     *
     * @return Puntero al pool o nullptr si no existe.
     */
    template<typename T>
    ComponentPool<T>* GetPool() noexcept {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      return (it != m_componentPools.end())
        ? static_cast<ComponentPool<T>*>(it->second.get())
        : nullptr;
    }
    /**
     * @brief Obtiene un pool existente en modo solo lectura.
     *
     * @tparam T Tipo del componente.
     *
     * @return Puntero constante al pool o nullptr si no existe.
     */
    template<typename T>
    const ComponentPool<T>* GetPoolConst() const noexcept {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      return (it != m_componentPools.end())
        ? static_cast<const ComponentPool<T>*>(it->second.get())
        : nullptr;
    }

  private:
    // Entidades
    std::vector<EntityID> m_entities;
    std::vector<EntityVersion> m_versions;
    std::queue<EntityIndex> m_freeList;

    // Componentes
    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;

    // Sistemas
    std::vector<std::unique_ptr<System>> m_systems;
  };
}