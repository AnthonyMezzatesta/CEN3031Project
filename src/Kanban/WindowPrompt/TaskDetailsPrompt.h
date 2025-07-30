#pragma once
#include <SFML/Graphics.hpp>
#include "../Utilities/EventSystem/Subject.h"
#include "WindowPrompt.h"
#include "../Board/Board.h"

class TaskDetailsPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    // enum OptionEnum { Name, Description };
    // static const char* OptionEnumToString(OptionEnum value);
    //
    // class SettingsOption : public Kanban::GUIElement
    // {
    //     OptionEnum type_;
    //     string name_;
    //     void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override;
    //
    // public:
    //     SettingsOption(OptionEnum type);
    //     OptionEnum GetType();
    // };
    // SettingsOption* options_[2];


    sf::Color bgColor_ = sf::Color(128, 128, 128, 255);


public:
    TaskDetailsPrompt(const sf::RenderWindow& target);
    ~TaskDetailsPrompt() {}

    void SetTask(const Task& task) { task_ = task; }
    void Update() override;
    void Deactivate();

    sf::RectangleShape bg_;


    void HandleClick(sf::Vector2i mousePos);

    Task task_;


    // Input fields for task details
    bool editMode_ = false;
    std::string nameInput_;
    std::string descriptionInput_;
    std::chrono::system_clock::time_point deadlineInput_;
    std::string deadlineInputStr_; // String representation of deadline for editing
    Task::Priority priorityInput_ = Task::Priority::None;

    void SaveEdits(TaskManager& taskManager);



    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;

    // Fields for highlighting
    enum EditField {
        FIELD_NONE,
        FIELD_NAME,
        FIELD_DESCRIPTION,
        FIELD_DEADLINE,
        FIELD_PRIORITY
    };
    
    EditField currentField_ = FIELD_NONE;

    // Date editing segments
    enum DateSegment {
        DATE_YEAR,
        DATE_MONTH,
        DATE_DAY,
        DATE_HOUR,
        DATE_MINUTE,
        DATE_SECOND,
        DATE_SEGMENTS_COUNT
    };

    DateSegment currentDateSegment_ = DATE_YEAR;
};