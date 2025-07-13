#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "Task.h"
#include "Board.h"
using namespace std;

Kanban::Board::Board(const sf::RenderTarget& target) : boardView(target.getDefaultView()) {
    // boardView.setViewport(sf::FloatRect(0.1f, 0.15f, 0.75f, 0.75f));
}

Kanban::Board::~Board() {
    for (unsigned int i = 0; i < columns.size(); i++)
        delete columns[i];
}

bool Kanban::Board::AddColumn(const string& name, const sf::RenderTarget& target,
    WindowPromptManager& windowPromptManager) {
    for (unsigned int i = 0; i < columns.size(); i++)
    {
        if (columns[i]->GetName() == name)
            return false;
    }
    auto targetSize = target.getSize();
    int width =  targetSize.x / (colPerScreen + 1);
    int height = targetSize.y * 0.75f;
    columns.push_back(new Column(name, width, height, windowPromptManager));
    return true;
}

bool Kanban::Board::AddTaskToColumn(string colName, Task& task) {
    for (unsigned int i = 0; i < columns.size(); i++)
    {
        if (columns[i]->GetName() == colName)
            return columns[i]->AddTask(task);
    }
    return false;
}

void Kanban::Board::MoveView(sf::Keyboard::Key key, const float deltaTime) {
    if (key == sf::Keyboard::Left)
    {
        boardView.move(-200 * deltaTime, 0.f);
    }
    else if (key == sf::Keyboard::Right)
    {
        boardView.move(200 * deltaTime, 0.f);
    }
}

void Kanban::Board::DrawBoard(sf::RenderWindow& window)
{
    // columns don't resize based on number, they 'tile'

    window.setView(boardView);

    sf::RectangleShape r(sf::Vector2f(window.getSize().x, window.getSize().y));
    r.setFillColor(sf::Color(160,160,160,255));
    window.draw(r);

    const int colCount = columns.size();
    auto targetSize = window.getSize();
    int width =  targetSize.x / (colPerScreen + 1);
    int height = targetSize.y * 0.75f;
    int xPadding = width / (colPerScreen + 1);
    int yOffset = (targetSize.y - height) / 2;
    for (int i = 0; i < colCount; i++)
    {
        int xOffset = (i+1) * xPadding + width * i;
        columns[i]->Render(sf::Vector2f(xOffset, yOffset), window);
    }

    window.setView(window.getDefaultView());
}

bool Kanban::Board::CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
{
    // auto pixelPos = sf::Mouse::getPosition(target);
    auto mousePos = target.mapPixelToCoords(point, boardView);
    for (unsigned int i = 0; i < columns.size(); i++)
    {
        if (columns[i]->CheckCollision(mousePos))
            return true;
    }
    return false;
}
