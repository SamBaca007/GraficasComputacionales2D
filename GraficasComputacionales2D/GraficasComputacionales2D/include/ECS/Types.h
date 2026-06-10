#pragma once
#include "Prerequisites.h"
// =========================================================
// ECS :: Types.h
// Tipos fundamentales de Entity Component System.
//
// EntityID = uint64_t que empaqueta:
//            bits [0..31] -> EntityIndex (posición en el array)
//            bits [32..63] -> EntityVersion (generación; invalida IDs viejos)
//
// Al destruir una entidad su versión sube 1, así cualquier EntityID antiguo
// guardado en otro sitio queda invalidado.
// =========================================================

namespace
  ECS {
  // Tipos Primitvos
  using EntityIndex = uint32_t; // Índice de la entidad en el array
  using EntityVersion = uint32_t; // Versión de la entidad (para invalidar IDs antiguos)
  using EntityID = uint64_t; // ID completo que empaqueta el índice y la versión
  using ComponentTypeID = uint32_t; // ID de tipo para componentes

  // Valor centinela para "ninguna entidad"
  inline constexpr
    EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

  // Empaquetado / Desempaquetado de EntityID

  /**
 * @brief Extrae la parte del índice de un identificador de entidad.
 * * Obtiene los 32 bits menos significativos del EntityID mediante una máscara
 * de bits. El índice suele representar la posición física de la entidad
 * en una estructura de datos.
 * * @param id El identificador único completo de la entidad (64 bits).
 * @return EntityIndex El índice base extraído de la entidad (32 bits).
 */
  [[nodiscard]] inline
    EntityIndex GetEntityIndex(EntityID id) noexcept
  {
    return static_cast<EntityIndex>(id & 0xFFFF'FFFFull);
  }

  /**
 * @brief Extrae la versión o generación de un identificador de entidad.
 * * Obtiene los 32 bits más significativos del EntityID desplazando los bits.
 * La versión se utiliza para distinguir entidades que han reutilizado el
 * mismo índice, evitando así referencias colgantes (dangling pointers).
 * * @param id El identificador único completo de la entidad (64 bits).
 * @return EntityVersion La versión extraída de la entidad (32 bits).
 */
  [[nodiscard]] inline
    EntityVersion GetEntityVersion(EntityID id) noexcept
  {
    return static_cast<EntityVersion>((id >> 32) & 0xFFFF'FFFFull);
  }

  /**
 * @brief Construye un identificador único combinando un índice y una versión.
 * * Empaqueta ambos valores de 32 bits en un solo identificador de 64 bits.
 * La versión se desplaza hacia la mitad superior y se combina con el índice
 * en la mitad inferior mediante un operador OR a nivel de bits.
 * * @param index El índice de la entidad (32 bits).
 * @param version La versión o generación de la entidad (32 bits).
 * @return EntityID El identificador único combinado (64 bits).
 */
  [[nodiscard]] inline
    EntityID MakeEntityID(EntityIndex index,
    EntityVersion version) noexcept
  {
    return (static_cast<EntityID>(version) << 32) | static_cast<EntityID>(index);
  }

  // Generador de IDs de tipo de componente
  // Cada tipo T obtiene un ID único en tiempo de ejecución
  // la primera vez que se llama a GetComponentTypeID<T>().

  /**
 * @brief Genera y devuelve el siguiente identificador único disponible.
 * * Utiliza un contador estático interno. Cada vez que se llama a esta función,
 * devuelve el valor actual del contador y luego lo incrementa. Es el motor
 * subyacente para la asignación dinámica de IDs de los componentes.
 * * @return ComponentTypeID Un identificador único secuencial.
 */
  [[nodiscard]] ComponentTypeID
    NextComponentTypeID() noexcept
  {
    static ComponentTypeID counter = 0;
    return counter++;
  }

  /**
 * @brief Obtiene el identificador único asociado a un tipo de componente específico.
 * * Aprovecha el comportamiento de las plantillas (templates) en C++: la variable
 * estática 'id' se inicializa solo una vez por cada tipo distinto 'T'.
 * La primera vez que se llama para un tipo (ej. NextComponentTypeID<Position>()),
 * obtiene un nuevo ID del contador global. Las llamadas posteriores para ese
 * mismo tipo devolverán siempre el mismo ID guardado.
 * * @tparam T El tipo de componente (struct o class) para el cual se solicita el ID.
 * @return ComponentTypeID El identificador único y constante asignado al tipo T.
 */
  template<typename T>
  [[nodiscard]] ComponentTypeID
    NextComponentTypeID() noexcept
  {
    static const ComponentTypeID id = NextComponentTypeID();
    return id;
  }
}