#include "Core/CShape.h"
#include "Core/Window.h"

/**
 * @file CShape.cpp
 * @brief Implementación de la clase CShape.
 */

/**
 * @brief Construye un componente de figura.
 *
 * Almacena el tipo de figura solicitado y crea la instancia
 * correspondiente mediante el método estático createShape().
 *
 * @param shapeType Tipo de figura geométrica a crear.
 */
CShape::CShape(ShapeType shapeType)
  : m_shapeType(shapeType),
  m_shape(createShape(shapeType))
{
}

/**
 * @brief Dibuja la figura asociada al componente.
 *
 * Si existe una figura válida, ésta se envía al sistema
 * de renderizado de la ventana especificada.
 *
 * @param window Ventana donde se renderizará la figura.
 */
void CShape::draw(Window& window) {

  if (m_shape) {
    window.draw(*m_shape);
  }
}

/**
 * @brief Obtiene acceso a la figura interna.
 *
 * Permite modificar directamente propiedades específicas
 * de SFML como posición, escala, color o rotación.
 *
 * @return Puntero a la figura almacenada.
 */
sf::Shape* CShape::getShape() {

  return m_shape.get();
}

/**
 * @brief Crea una figura geométrica según el tipo indicado.
 *
 * Genera una instancia concreta derivada de sf::Shape y
 * configura algunos valores iniciales como tamaño,
 * posición y color.
 *
 * @param shapeType Tipo de figura a crear.
 *
 * @return Puntero inteligente a la figura creada.
 * @return nullptr si el tipo no es válido.
 */
std::unique_ptr<sf::Shape>
CShape::createShape(ShapeType shapeType) {

  switch (shapeType) {

    /**
     * @brief No crea ninguna figura.
     */
  case EMPTY:
    break;

    /**
     * @brief Crea un círculo blanco.
     */
  case CIRCLE:
  {
    auto circle =
      std::make_unique<sf::CircleShape>(50.f);

    circle->setFillColor(sf::Color::White);
    circle->setPosition({ 100.0f, 100.0f });

    return circle;
  }

  /**
   * @brief Crea un rectángulo blanco.
   */
  case RECTANGLE:
  {
    auto rectangle =
      std::make_unique<sf::RectangleShape>(
        sf::Vector2f(100.f, 50.f)
      );

    rectangle->setFillColor(sf::Color::White);
    rectangle->setPosition({ 100.0f, 100.0f });

    return rectangle;
  }

  /**
   * @brief Crea un triángulo blanco utilizando una figura convexa.
   */
  case TRIANGLE:
  {
    auto triangle =
      std::make_unique<sf::ConvexShape>(3);

    triangle->setPoint(0, sf::Vector2f(0.f, 0.f));
    triangle->setPoint(1, sf::Vector2f(100.f, 0.f));
    triangle->setPoint(2, sf::Vector2f(50.f, 100.f));

    triangle->setFillColor(sf::Color::White);
    triangle->setPosition({ 300.f, 300.f });

    return triangle;
  }

  /**
   * @brief Crea un polígono convexo de cinco vértices.
   */
  case POLYGON:
  {
    auto polygon =
      std::make_unique<sf::ConvexShape>(5);

    polygon->setPoint(0, sf::Vector2f(0.f, 0.f));
    polygon->setPoint(1, sf::Vector2f(100.f, 0.f));
    polygon->setPoint(2, sf::Vector2f(120.f, 50.f));
    polygon->setPoint(3, sf::Vector2f(50.f, 100.f));
    polygon->setPoint(4, sf::Vector2f(-20.f, 50.f));

    polygon->setFillColor(sf::Color::White);
    polygon->setPosition({ 400.f, 400.f });

    return polygon;
  }

  /**
   * @brief Crea una línea horizontal representada mediante
   *        un rectángulo de un píxel de grosor.
   */
  case LINE:
  {
    auto line =
      std::make_unique<sf::RectangleShape>(
        sf::Vector2f(200.f, 1.f)
      );

    line->setFillColor(sf::Color::White);
    line->setPosition({ 500.f, 500.f });

    return line;
  }

  /**
   * @brief Caso por defecto para tipos no reconocidos.
   */
  default:
    return nullptr;
  }

  return nullptr;
}