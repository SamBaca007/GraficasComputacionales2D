#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/System.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <cassert>

// ¡MUY IMPORTANTE: NO incluyas View.h aquí arriba!

namespace ECS {

  // 1. Promesa al compilador: "View existirá, confía en mí"
  template<typename... Components>
  class View;

  class Registry {
  public:
    EntityID CreateEntity() {
      EntityIndex idx;
      if (!m_freeList.empty()) {
        idx = m_freeList.front();
        m_freeList.pop();
      }
      else {
        idx = static_cast<EntityIndex>(m_versions.size());
        m_versions.push_back(0);
        m_entities.push_back(NULL_ENTITY);
      }
      EntityID id = MakeEntityID(idx, m_versions[idx]);
      m_entities[idx] = id;
      return id;
    }

    void DestroyEntity(EntityID entity) {
      assert(IsAlive(entity) && "DestroyEntity: entidad invalida");
      for (auto& [typeID, pool] : m_componentPools)
        pool->RemoveEntity(entity);
      const EntityIndex idx = GetEntityIndex(entity);
      ++m_versions[idx];
      m_entities[idx] = NULL_ENTITY;
      m_freeList.push(idx);
    }

    [[nodiscard]] bool IsAlive(EntityID entity) const noexcept {
      const EntityIndex idx = GetEntityIndex(entity);
      return idx < m_entities.size() && m_entities[idx] == entity;
    }

    [[nodiscard]] std::size_t EntityCount() const noexcept {
      return m_entities.size() - m_freeList.size();
    }

    [[nodiscard]] const std::vector<EntityID>& GetAllEntities() const noexcept {
      return m_entities;
    }

    template<typename T, typename... Args> T& AddComponent(EntityID entity, Args&&... args) {
      assert(IsAlive(entity) && "AddComponent: entidad invalida");
      return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
    }

    template<typename T> void RemoveComponent(EntityID entity) {
      if (auto* pool = GetPool<T>())
        pool->Remove(entity);
    }

    template<typename T>
    T& SetComponent(EntityID entity, T value) {
      assert(IsAlive(entity) && "SetComponent: entidad invalida");
      auto* pool = GetOrCreatePool<T>();
      if (pool->Contains(entity)) {
        pool->Get(entity) = std::move(value);
        return pool->Get(entity);
      }
      return pool->Add(entity, std::move(value));
    }

    template<typename T>
    [[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
      const auto* pool = GetPoolConst<T>();
      return pool && pool->Contains(entity);
    }

    template<typename T>
    [[nodiscard]] T& GetComponent(EntityID entity) {
      assert(IsAlive(entity));
      auto* pool = GetPool<T>();
      assert(pool && "GetComponent: pool no existe");
      return pool->Get(entity);
    }

    template<typename T>
    [[nodiscard]] const T& GetComponent(EntityID entity) const {
      assert(IsAlive(entity));
      const auto* pool = GetPoolConst<T>();
      assert(pool && "GetComponent: pool no existe");
      return pool->Get(entity);
    }

    template<typename T>
    [[nodiscard]] T* TryGetComponent(EntityID entity) noexcept {
      auto* pool = GetPool<T>();
      return pool ? pool->TryGet(entity) : nullptr;
    }

    // 2. SOLO DECLARAMOS GetView aquí adentro. No lo implementamos aún.
    template<typename... Components>
    [[nodiscard]] View<Components...> GetView();

    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) {
      static_assert(std::is_base_of_v<System, T>, "T debe derivar de ECS::System");
      auto system = std::make_unique<T>(std::forward<Args>(args)...);
      T& ref = *system;
      system->OnStart(*this);
      m_systems.push_back(std::move(system));
      return ref;
    }

    void UpdateSystems(float deltaTime) {
      for (auto& system : m_systems)
        if (system->IsEnabled())
          system->OnUpdate(*this, deltaTime);
    }

    void RemoveAllSystems() {
      for (auto& system : m_systems)
        system->OnDestroy(*this);
      m_systems.clear();
    }

    void Clear() {
      RemoveAllSystems();
      for (auto& [typeID, pool] : m_componentPools)
        pool->Clear();
      m_entities.clear();
      m_versions.clear();
      while (!m_freeList.empty()) m_freeList.pop();
    }

    [[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>& GetPools() const noexcept {
      return m_componentPools;
    }

  private:
    template<typename T>
    ComponentPool<T>* GetOrCreatePool() {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      if (it == m_componentPools.end()) {
        auto [newIt, ok] = m_componentPools.emplace(typeID, std::make_unique<ComponentPool<T>>());
        return static_cast<ComponentPool<T>*>(newIt->second.get());
      }
      return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    ComponentPool<T>* GetPool() noexcept {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      return (it != m_componentPools.end()) ? static_cast<ComponentPool<T>*>(it->second.get()) : nullptr;
    }

    template<typename T>
    const ComponentPool<T>* GetPoolConst() const noexcept {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      return (it != m_componentPools.end()) ? static_cast<const ComponentPool<T>*>(it->second.get()) : nullptr;
    }

  private:
    std::vector<EntityID>      m_entities;
    std::vector<EntityVersion> m_versions;
    std::queue<EntityIndex>    m_freeList;
    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;
    std::vector<std::unique_ptr<System>> m_systems;
  };

} // namespace ECS

// =====================================================================
// 3. AHORA SÍ: Incluimos View.h hasta el final, cuando Registry ya está
// 100% definido y construido. ¡Esto destruye la dependencia circular!
// =====================================================================
#include "ECS/View.h"

namespace ECS {
  // 4. Implementamos GetView aquí abajo. Como View ya se incluyó,
  // el compilador sabe exactamente cómo construirlo.
  template<typename... Components>
  inline View<Components...> Registry::GetView() {
    return View<Components...>(GetOrCreatePool<Components>()...);
  }
}