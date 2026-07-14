/**
 * @file Prerequisites.h
 * @brief Archivo de cabecera principal que contiene las dependencias estándar,
 *        bibliotecas de terceros, macros de utilidad y definiciones globales.
 */

#pragma once

 // Librerias STD
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <map>
#include <fstream> 
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <limits>
#include <cassert>
#include <utility>
#include <tuple>
#include <cstddef>
#include <queue>

// Third Parties
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

/**
 * @brief Macro para liberar de forma segura la memoria de un puntero.
 *        Elimina el objeto y asigna el puntero a nullptr para evitar punteros colgantes.
 *
 * @param x El puntero que se desea liberar.
 */
 // MACRO for safe release of resources
#define SAFE_PTR_RELEASE(x) if(x != nullptr) { delete x; x = nullptr; }

/**
 * @brief Macro para registrar mensajes en la consola de errores estándar (std::cerr).
 *        Útil para realizar un seguimiento de la creación de recursos y el flujo del programa.
 *
 * @param classObj Nombre de la clase u objeto que genera el mensaje.
 * @param method Nombre del método desde donde se llama a la macro.
 * @param state Estado o mensaje específico a registrar.
 */
#define MESSAGE(classObj, method, state)                      \
{                                                             \
    std::ostringstream os_;                                   \
    os_ << classObj << "::" << method << " : "                \
        << "[CREATION OF RESOURCE" << ": " << state "] \n";\
    std::cerr << os_.str();                                   \
}

 /**
  * @brief Macro para registrar errores fatales y detener la ejecución del programa.
  *        Imprime los detalles del error en la salida de error estándar y finaliza con exit(1).
  *
  * @param classObj Nombre de la clase u objeto donde ocurrió el error.
  * @param method Nombre del método donde ocurrió el error.
  * @param errorMSG Mensaje descriptivo indicando el motivo del error.
  */
#define ERROR(classObj, method, errorMSG)                         \
{                                                                 \
    std::ostringstream os_;                                       \
    os_ << "ERROR : " << classObj << "::" << method << " : "      \
        << "  Error in data from params [" << errorMSG"] \n"; \
    std::cerr << os_.str();                                       \
    exit(1);                                                      \
}

  // ENUMS
  /**
   * @brief Tipos de formas geométricas disponibles.
   *        Utilizado para identificar el tipo de figura base a renderizar o procesar.
   */
enum
  ShapeType {
  /** @brief Representa la ausencia de forma o un estado no inicializado. */
  EMPTY = 0,
  /** @brief Representa una forma circular. */
  CIRCLE = 1,
  /** @brief Representa una forma rectangular. */
  RECTANGLE = 2,
  /** @brief Representa una forma triangular. */
  TRIANGLE = 3,
  /** @brief Representa un polígono de múltiples lados. */
  POLYGON = 4,
  /** @brief Representa una línea simple. */
  LINE = 5
};