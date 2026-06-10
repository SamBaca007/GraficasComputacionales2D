#pragma once

/**
 * @file Prerequisites.h
 * @brief Cabecera de prerrequisitos del proyecto.
 *
 * Incluye las bibliotecas estándar y de terceros utilizadas
 * de forma común en el motor, además de definir macros de
 * utilidad y enumeraciones globales compartidas.
 */

// ============================================================================
// Bibliotecas estándar
// ============================================================================

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

// ============================================================================
// Bibliotecas de terceros
// ============================================================================

#include <SFML/Graphics.hpp>

// ============================================================================
// Macros
// ============================================================================

/**
 * @def SAFE_PTR_RELEASE(x)
 * @brief Libera de forma segura la memoria asociada a un puntero.
 *
 * Elimina el objeto apuntado y asigna nullptr al puntero para
 * evitar referencias colgantes.
 *
 * @param x Puntero a liberar.
 */
#define SAFE_PTR_RELEASE(x) if(x != nullptr) { delete x; x = nullptr; }

/**
 * @def MESSAGE(classObj, method, state)
 * @brief Muestra un mensaje informativo relacionado con la creación
 *        o estado de un recurso.
 *
 * El mensaje se envía a la salida de error estándar.
 *
 * @param classObj Nombre de la clase que genera el mensaje.
 * @param method Nombre del método que genera el mensaje.
 * @param state Estado o información asociada al recurso.
 */
#define MESSAGE(classObj, method, state)                      \
{                                                             \
    std::ostringstream os_;                                   \
    os_ << classObj << "::" << method << " : "                \
        << "[CREATION OF RESOURCE" << ": " << state "] \n";   \
    std::cerr << os_.str();                                   \
}

/**
 * @def ERROR(classObj, method, errorMSG)
 * @brief Muestra un mensaje de error y finaliza la ejecución.
 *
 * El mensaje se envía a la salida de error estándar y posteriormente
 * se termina el programa mediante exit(1).
 *
 * @param classObj Nombre de la clase que genera el error.
 * @param method Nombre del método donde ocurrió el error.
 * @param errorMSG Descripción del error detectado.
 */
#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    std::ostringstream os_;                                   \
    os_ << "ERROR : " << classObj << "::" << method << " : "  \
        << "  Error in data from params [" << errorMSG"] \n"; \
    std::cerr << os_.str();                                   \
    exit(1);                                                  \
}

// ============================================================================
// Enumeraciones
// ============================================================================

/**
 * @enum ShapeType
 * @brief Identifica los distintos tipos de figuras geométricas
 *        soportadas por el sistema.
 */
enum ShapeType {

    /** No existe ninguna figura asociada. */
    EMPTY = 0,

    /** Figura circular. */
    CIRCLE = 1,

    /** Figura rectangular. */
    RECTANGLE = 2,

    /** Figura triangular. */
    TRIANGLE = 3,

    /** Polígono genérico. */
    POLYGON = 4,

    /** Segmento de línea. */
    LINE = 5
};