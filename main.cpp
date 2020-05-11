#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <iostream>

static const int kWidth = 800;
static const int kHeight = 480;
static const int kFPS = 30;
static const sf::Time kUpdateMs = sf::seconds(1.f/static_cast<float>(kFPS));

using sf::RenderWindow,
    sf::VideoMode,
    sf::Event,
    sf::CircleShape,
    sf::RectangleShape,
    sf::Color,
    sf::Vector2f,
    sf::Font,
    sf::Text,
    sf::FloatRect,
    sf::Time,
    sf::Clock;

using Draw = std::function<void(RenderWindow&)>;
using Update = std::function<void(float)>;

using Player = struct Player {
    RectangleShape shape;
    Vector2f vel = {150.f, 150.f};
    int score = 100;
    Text& scoreText;
    Player(float x, float y, Text& score_text):
    scoreText(score_text){
        shape.setPosition(x, y);
        shape.setSize(Vector2f{20.f, 80.f});
    }
    float width() const {
        return shape.getSize().x;
    }
    float height() const {
        return shape.getSize().y;
    }
    void draw(RenderWindow& rw) {
        rw.draw(shape);
    }
    void update(float dt) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            float ny = shape.getPosition().y - vel.y * dt;
            if (ny > 0) {
                shape.setPosition(shape.getPosition().x, ny);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            float ny = shape.getPosition().y + vel.y * dt + shape.getSize().y;
            if (ny < kHeight) {
                shape.setPosition(shape.getPosition().x, shape.getPosition().y + vel.y * dt);
            }
        }
    }
};

using Ball = struct Ball {
    RectangleShape shape;
    Vector2f vel = {200.f, 200.f};
    Ball (float x, float y) {
        shape.setPosition(x, y);
        shape.setSize(Vector2f{15.f, 15.f});
    }
    float width() const {
        return shape.getSize().x;
    }
    float height() const {
        return shape.getSize().y;
    }
    void draw(RenderWindow& rw) {
        rw.draw(shape);
    }
    void update(float dt) {
        Vector2f pos = shape.getPosition();
        pos.x += vel.x * dt;
        pos.y += vel.y * dt;
        if (pos.x + width() > kWidth) {
            pos.x = kWidth - width();
            vel.x *= -1.f;
        } else if (pos.x < 0) {
            pos.x = 0;
            vel.x *= -1.f;
        }
        if (pos.y + height() > kHeight) {
            pos.y = kHeight - height();
            vel.y *= -1.f;
        } else if (pos.y < 0) {
            pos.y = 0;
            vel.y *= -1.f;
        }
        shape.setPosition(pos);
    }
};

float center(float element, float screen) {
    return (screen/2.f) - (element/2.f);
}

void loop(RenderWindow& window,
          Update update = nullptr,
          Draw draw = nullptr) {
    Clock clock;
    Time elapsed = clock.restart();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        elapsed += clock.restart();
        while (elapsed >= kUpdateMs) {
            if (update) update(kUpdateMs.asSeconds());
            elapsed -= kUpdateMs;
        }
        window.clear();
        if (draw) draw(window);
        window.display();
    }
}

void drawWall(RenderWindow& rw,
              RectangleShape& wall,
              float centerXWall) {
    for (int i = 0; i < 48; ++i) {
        wall.setPosition(centerXWall, 10.f * i);
        rw.draw(wall);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "Aula 04 - Pong");
    Font font;
    font.loadFromFile("atari.ttf");
    Text score("", font, 80);
    Player p1(10.f, center(80.f, kHeight), score);
    p1.score = 10;
    Player p2(kWidth - 30.f, center(80.f, kHeight), score);
    p2.score = 100;
    Ball ball(10.f, 10.f);
    RectangleShape wall(Vector2f{5.f, 5.f});
    float centerXWall = center(wall.getSize().x, kWidth); // posição no eixo x dos quadrados que fazem parte do separador de tela
    Update update = [&](float dt) {
        p1.update(dt);
        ball.update(dt);
    };
    Draw draw = [&](RenderWindow& rw) {
        p1.draw(rw);
        p2.draw(rw);
        ball.draw(rw);
        drawWall(rw, wall, centerXWall);
    };
    loop(window, update, draw);
    return 0;
}
