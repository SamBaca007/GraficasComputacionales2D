#include <Prerequisites.h>
#include "Core/Window.h"

Window* g_window = nullptr;
sf::CircleShape shape(50.f);

int main()
{
  // create the window
  g_window = new Window(800, 600, "My window");
  // sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");
  // set the color shape to green
  shape.setFillColor(sf::Color(100, 250, 50));

  // run the program as long as the window is open
  while (g_window->isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop
    while (const std::optional event = g_window->m_window->pollEvent())
    {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        g_window->close();
        g_window->destroy();
    }

    // clear the window with black color
    g_window->clear(sf::Color::Black);

    // draw everything here...
    g_window->draw(shape);

    // end the current frame
    g_window->display();
  }
}