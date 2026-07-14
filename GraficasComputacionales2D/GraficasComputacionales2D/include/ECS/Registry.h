/**
 * @file Registry.h
 * @brief Definición de la clase Registry, el núcleo del Entity Component System (ECS).
 *
 * @details Gestiona el ciclo de vida de las entidades, coordina el almacenamiento de
 *          componentes en pools tipados y controla la ejecución de los sistemas.
 */

#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/View.h"
#include "ECS/System.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class Registry
   * @brief Administrador central del ECS. Controla la creación/destrucción de entidades,
   *        la asignación de componentes y la actualización de los sistemas.
   */
  class
    Registry {
  public:
    /**
     * @brief Crea una nueva entidad en el registro.
     *
     * @details Reutiliza índices de entidades previamente destruidas (si las hay en la lista libre)
     *          para optimizar la memoria. Si no hay índices libres, expande los arreglos internos.
     *
     * @return EntityID El identificador único de la nueva entidad creada.
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
        m_entities.push_back(NULL_ENTITY);   // placeholder
      }

      EntityID id = MakeEntityID(idx, m_versions[idx]);
      m_entities[idx] = id;
      return id;
    }

    /**
     * @brief Destruye una entidad, invalidando su ID y eliminando todos sus componentes.
     *
     * @details Incrementa la versión interna asociada a este índice. Como resultado,
     *          cualquier variable externa que guarde el viejo EntityID ya no pasará
     *          la validación de IsAlive(). El índice se recicla en la lista libre.
     *
     * @param entity El identificador de la entidad a destruir.
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
     * @brief Verifica si una entidad existe y sigue siendo válida en el registro.
     *
     * @param entity El identificador de la entidad a verificar.
     * @return true Si la entidad está viva y su versión coincide.
     * @return false Si la entidad fue destruida o el ID es inválido.
     */
    [[nodiscard]] bool
      IsAlive(EntityID entity) const noexcept {
      const EntityIndex idx = GetEntityIndex(entity);
      return idx < m_entities.size() && m_entities[idx] == entity;
    }

    /**
     * @brief Obtiene la cantidad de entidades activas (vivas) actualmente.
     *
     * @return std::size_t Número de entidades vivas.
     */
    [[nodiscard]] std::size_t
      EntityCount() const noexcept {
      return m_entities.size() - m_freeList.size();
    }

    /**
     * @brief Obtiene todas las ranuras de entidades, incluyendo los huecos libres.
     *
     * @details Útil para el Serializer; se recomienda filtrar con IsAlive al iterar.
     *
     * @return const std::vector<EntityID>& Arreglo completo con los IDs de entidades (y NULL_ENTITY).
     */
     // Todas las ranuras (incluye NULL_ENTITY para los huecos libres).
     // Útil para el Serializer; filtra con IsAlive.
    [[nodiscard]] const std::vector<EntityID>&
      GetAllEntities() const noexcept {
      return m_entities;
    }

    //  Componentes

    /**
     * @brief Añade un nuevo componente a una entidad y devuelve una referencia al mismo.
     *
     * @details Acepta argumentos de construcción directos (perfect-forward) que se pasan
     *          al constructor del componente tipo T, evitando copias innecesarias.
     *
     * @tparam T Tipo de componente a añadir.
     * @tparam Args Paquete de tipos de los argumentos del constructor.
     * @param entity La entidad que recibirá el componente.
     * @param args Argumentos reenviados al constructor del componente T.
     * @return T& Referencia al componente recién creado en la entidad.
     */
     // Añade un componente a la entidad y devuelve su referencia.
     // Acepta argumentos de construcción directos (perfect-forward).
    template<typename T, typename... Args> T&
      AddComponent(EntityID entity, Args&&... args) {
      assert(IsAlive(entity) && "AddComponent: entidad inválida");
      return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Elimina un componente específico de una entidad (no-op si no lo tiene).
     *
     * @tparam T Tipo de componente a eliminar.
     * @param entity La entidad a procesar.
     */
     // Elimina el componente T de la entidad (no-op si no lo tiene).
    template<typename T> void
      RemoveComponent(EntityID entity) {
      if (auto* pool = GetPool<T>())
        pool->Remove(entity);
    }

    /**
     * @brief Reemplaza un componente existente o lo añade si la entidad no lo tiene.
     *
     * @tparam T Tipo del componente.
     * @param entity La entidad objetivo.
     * @param value El valor o instancia del componente a asignar.
     * @return T& Referencia al componente actualizado.
     */
     // Reemplaza el componente (o lo añade si no existía).
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
     * @brief Verifica si una entidad posee un componente específico.
     *
     * @tparam T Tipo del componente a verificar.
     * @param entity La entidad a consultar.
     * @return true Si la entidad tiene el componente asignado.
     * @return false Si la entidad no tiene el componente.
     */
    template<typename T>
    [[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
      const auto* pool = GetPoolConst<T>();
      return pool && pool->Contains(entity);
    }

    /**
     * @brief Obtiene una referencia modificable al componente de una entidad.
     *
     * @details Acceso garantizado: Lanza un assert si la entidad no existe o si
     *          no posee el componente solicitado.
     *
     * @tparam T Tipo del componente a obtener.
     * @param entity La entidad poseedora del componente.
     * @return T& Referencia al componente.
     */
     // Acceso garantizado (assert si no existe).
    template<typename T>
    [[nodiscard]] T& GetComponent(EntityID entity) {
      assert(IsAlive(entity));
      auto* pool = GetPool<T>();
      assert(pool && "GetComponent: pool no existe para este tipo");
      return pool->Get(entity);
    }

    /**
     * @brief Obtiene una referencia de solo lectura al componente de una entidad.
     *
     * @tparam T Tipo del componente a obtener.
     * @param entity La entidad poseedora del componente.
     * @return const T& Referencia constante al componente.
     */
    template<typename T>
    [[nodiscard]] const T& GetComponent(EntityID entity) const
    {
      assert(IsAlive(entity));
      const auto* pool = GetPoolConst<T>();
      assert(pool && "GetComponent: pool no existe para este tipo");
      return pool->Get(entity);
    }

    /**
     * @brief Acceso seguro a un componente; devuelve nullptr si no existe.
     *
     * @tparam T Tipo del componente a buscar.
     * @param entity La entidad a consultar.
     * @return T* Puntero al componente si existe, de lo contrario nullptr.
     */
     // Acceso seguro: devuelve nullptr si la entidad no tiene el componente.
    template<typename T>
    [[nodiscard]] T* TryGetComponent(EntityID entity) noexcept
    {
      auto* pool = GetPool<T>();
      return pool ? pool->TryGet(entity) : nullptr;
    }

    //  Views (queries multi-componente)

    /**
     * @brief Crea una vista (View) para iterar eficientemente sobre entidades que
     *        compartan un conjunto específico de componentes.
     *
     * @details Ejemplo de uso: `auto view = registry.GetView<Transform, Velocity>();`
     *
     * @tparam Components Paquete de tipos de los componentes requeridos en la vista.
     * @return View<Components...> Objeto vista para iterar las entidades que cumplen la firma.
     */
     // Ejemplo: registry.GetView<Transform, Velocity>()
    template<typename... Components>
    [[nodiscard]] View<Components...> GetView() {
      return View<Components...>(GetOrCreatePool<Components>()...);
    }

    //  Sistemas

    /**
     * @brief Instancia y registra un nuevo sistema (System) en el registro.
     *
     * @details Construye el sistema, reenvía los argumentos al constructor y llama
     *          automáticamente al método `OnStart()` del sistema.
     *
     * @tparam T Tipo de la clase Sistema (debe derivar de ECS::System).
     * @tparam Args Paquete de argumentos para el constructor del sistema.
     * @param args Argumentos a reenviar al sistema.
     * @return T& Referencia al sistema recién creado.
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
     * @brief Actualiza todos los sistemas habilitados, pasando el delta time actual.
     *
     * @param deltaTime Tiempo transcurrido desde el último frame.
     */
    void UpdateSystems(float deltaTime)
    {
      for (auto& system : m_systems)
        if (system->IsEnabled())
          system->OnUpdate(*this, deltaTime);
    }

    /**
     * @brief Destruye y remueve todos los sistemas del registro.
     *
     * @details Llama al método `OnDestroy()` de cada sistema antes de borrarlos de memoria.
     */
    void RemoveAllSystems()
    {
      for (auto& system : m_systems)
        system->OnDestroy(*this);
      m_systems.clear();
    }

    //  Utilidades

    /**
     * @brief Limpia por completo el registro: destruye sistemas, componentes y entidades.
     */
     // Destruye todo: entidades, componentes y sistemas.
    void
      Clear() {
      RemoveAllSystems();
      for (auto& [typeID, pool] : m_componentPools)
        pool->Clear();
      m_entities.clear();
      m_versions.clear();
      while (!m_freeList.empty()) m_freeList.pop();
    }

    /**
     * @brief Obtiene el mapa interno de pools (sin tipar).
     *
     * @details Especialmente útil para iterar y serializar componentes genéricamente
     *          sin conocerlos en tiempo de compilación.
     *
     * @return const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>&
     *         Mapa de pools indexados por ID de tipo de componente.
     */
     // Acceso a pools sin tipo (para el Serializer)
    [[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>&
      GetPools() const noexcept { return m_componentPools; }

  private:
    // Helpers privados

    /**
     * @brief Obtiene el pool asociado a un componente, o lo crea si no existe.
     *
     * @tparam T Tipo del componente.
     * @return ComponentPool<T>* Puntero al pool tipado.
     */
    template<typename T>
    ComponentPool<T>* GetOrCreatePool() {
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
     * @brief Intenta obtener el pool de un componente, sin crearlo si falta.
     *
     * @tparam T Tipo del componente.
     * @return ComponentPool<T>* Puntero al pool tipado, o nullptr si no existe.
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
     * @brief Intenta obtener el pool constante de un componente, sin crearlo si falta.
     *
     * @tparam T Tipo del componente.
     * @return const ComponentPool<T>* Puntero constante al pool tipado, o nullptr.
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
    /** @brief Arreglo con los IDs de entidades actuales (índice directo y versión empaquetada). */
    std::vector<EntityID>      m_entities;

    /** @brief Arreglo paralelo que rastrea la versión de vida para cada índice. */
    std::vector<EntityVersion> m_versions;

    /** @brief Cola con los índices de las entidades destruidas, listos para ser reciclados. */
    std::queue<EntityIndex>    m_freeList;

    // Componentes
    /** @brief Mapa no ordenado que almacena los pools de componentes usando el IComponentPool base. */
    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;

    // Sistemas
    /** @brief Arreglo lineal que almacena y gestiona la vida de los sistemas. */
    std::vector<std::unique_ptr<System>> m_systems;
  };
} // Namespace ECS