#pragma once
#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>
#include "Task.h"
#include "TaskManager.h"

class TaskEdit {
public:
    // Edit field enumeration
    enum EditField {
        FIELD_NONE,
        FIELD_NAME,
        FIELD_DESCRIPTION,
        FIELD_DEADLINE,
        FIELD_PRIORITY
    };

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

private:
    Task* task_;
    bool editMode_;
    EditField currentField_;
    DateSegment currentDateSegment_;

    // Input fields for task details
    std::string nameInput_;
    std::string descriptionInput_;
    std::chrono::system_clock::time_point deadlineInput_;
    std::string deadlineInputStr_;
    Task::Priority priorityInput_;

public:
    TaskEdit();
    ~TaskEdit();

    // Core functionality
    void SetTask(Task* task);
    void EnterEditMode();
    void ExitEditMode();
    bool IsEditMode() const { return editMode_; }

    // Field management
    void SetCurrentField(EditField field);
    EditField GetCurrentField() const { return currentField_; }
    void CycleFields();

    // Date segment management
    void SetCurrentDateSegment(DateSegment segment);
    DateSegment GetCurrentDateSegment() const { return currentDateSegment_; }
    void CycleDateSegments(bool forward);

    // Input handling
    void HandleTextInput(char inputChar);
    void HandleKeyPress(sf::Keyboard::Key key);
    void HandleBackspace();

    // Priority management
    void CyclePriority(bool up);

    // Date/time management
    void ModifyDateSegment(int direction);

    // Save functionality
    bool SaveChanges(TaskManager& taskManager);

    // Getters for input fields
    const std::string& GetNameInput() const { return nameInput_; }
    const std::string& GetDescriptionInput() const { return descriptionInput_; }
    const std::chrono::system_clock::time_point& GetDeadlineInput() const { return deadlineInput_; }
    const std::string& GetDeadlineInputStr() const { return deadlineInputStr_; }
    Task::Priority GetPriorityInput() const { return priorityInput_; }

    // Initialize input fields from task
    void InitializeInputFields();
};