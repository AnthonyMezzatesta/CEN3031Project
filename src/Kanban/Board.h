#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "../../include/Task.h"
#include "WindowPromptManager.h"
class WindowPromptManager;
class Column;
using std::vector;
using std::string;

namespace Kanban
{
    class Board
    {
        sf::View boardView;
        const static int colPerScreen = 3;
        vector<Column*> columns;
    public:
        Board(const sf::RenderTarget& target);
        ~Board();
        bool AddColumn(const string& name, const sf::RenderTarget& target, WindowPromptManager& windowPromptManager);
        bool AddTaskToColumn(string colName, Task& task);
        void MoveView(sf::Keyboard::Key key, const float deltaTime);

        void DrawBoard(sf::RenderWindow& window);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
    };
}