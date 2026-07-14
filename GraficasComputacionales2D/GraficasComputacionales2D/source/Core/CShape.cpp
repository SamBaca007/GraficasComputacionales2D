/**
 * @file CShape.cpp
 * @brief Implementación de la clase CShape.
 *        Contiene la lógica de inicialización y renderizado para las distintas
 *        formas geométricas construidas sobre SFML.
 */

#include "Core/CShape.h"
#include "Core/Window.h"

 /**
	* @brief Constructor parametrizado.
	*        Inicializa el tipo de forma y llama al método de fábrica para instanciarla.
	*/
CShape::CShape(ShapeType shapeType) : m_shapeType(shapeType), m_shape(createShape(shapeType)) {
}

/**
 * @brief Dibuja la figura instanciada en la ventana proporcionada.
 *        Solo intenta dibujar si el puntero inteligente contiene una forma válida.
 */
void
CShape::draw(Window& window) {
	if (m_shape) {
		window.draw(*m_shape);
	}
}

/**
 * @brief Obtiene el puntero crudo a la figura subyacente de SFML.
 */
sf::Shape* CShape::getShape()
{
	return m_shape.get();
}

/**
 * @brief Crea y configura la figura geométrica de SFML correspondiente al tipo solicitado.
 *
 * @details Dependiendo del enum ShapeType, instancia la figura con propiedades
 *          predeterminadas de tamaño, color blanco y una posición inicial predefinida.
 *          - CIRCLE: Círculo de radio 50 en (100, 100).
 *          - RECTANGLE: Rectángulo de 100x50 en (200, 200).
 *          - TRIANGLE: Polígono convexo de 3 lados en (300, 300).
 *          - POLYGON: Polígono irregular de 5 lados en (400, 400).
 *          - LINE: Rectángulo muy delgado (200x1) en (500, 500) para simular una línea.
 *          - EMPTY: Retorna un puntero nulo.
 */
std::unique_ptr<sf::Shape>
CShape::createShape(ShapeType shapeType) {

	switch (shapeType)
	{
	case EMPTY:
	{
		return nullptr;
	}
	case CIRCLE:
	{
		auto circle = std::make_unique<sf::CircleShape>(50.f);
		circle->setFillColor(sf::Color::White);
		circle->setPosition({ 100.0f, 100.0f });
		return circle;
	}
	case RECTANGLE:
	{
		auto rectangle = std::make_unique<sf::RectangleShape>(sf::Vector2f(100.f, 50.f));
		rectangle->setFillColor(sf::Color::White);
		rectangle->setPosition({ 200.0f, 200.0f });
		return rectangle;
	}
	case TRIANGLE:
	{
		auto triangle = std::make_unique<sf::ConvexShape>(3);
		triangle->setPoint(0, sf::Vector2f(0.f, 0.f));
		triangle->setPoint(1, sf::Vector2f(100.f, 0.f));
		triangle->setPoint(2, sf::Vector2f(50.f, 100.f));
		triangle->setFillColor(sf::Color::White);
		triangle->setPosition({ 300.0f, 300.0f });
		return triangle;
	}
	case POLYGON:
	{
		auto polygon = std::make_unique<sf::ConvexShape>(5);
		polygon->setPoint(0, sf::Vector2f(0.f, 0.f));
		polygon->setPoint(1, sf::Vector2f(100.f, 0.f));
		polygon->setPoint(2, sf::Vector2f(120.f, 50.f));
		polygon->setPoint(3, sf::Vector2f(50.f, 100.f));
		polygon->setPoint(4, sf::Vector2f(-20.f, 50.f));
		polygon->setFillColor(sf::Color::White);
		polygon->setPosition({ 400.0f, 400.0f });
		return polygon;
	}
	case LINE:
	{
		auto line = std::make_unique<sf::RectangleShape>(sf::Vector2f(200.f, 1.0f));
		line->setFillColor(sf::Color::White);
		line->setPosition({ 500.0f, 500.0f });
		return line;
	}
	default:
		return nullptr;
	}
}