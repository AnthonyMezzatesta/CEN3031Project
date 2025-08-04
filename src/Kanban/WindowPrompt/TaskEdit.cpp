#include "TaskEdit.h"
#include <iostream>
#include <ctime>

TaskEdit::TaskEdit() 
    : task_(nullptr), editMode_(false), currentField_(FIELD_NONE), 
      currentDateSegment_(DATE_YEAR), priorityInput_(Task::Priority::None) {
}

TaskEdit::~TaskEdit() {
    // No dynamic allocation to clean up
}

void TaskEdit::SetTask(Task* task) {
    task_ = task;
    if (task_) {
        InitializeInputFields();
    }
}

void TaskEdit::EnterEditMode() {
    if (!task_) return;
    
    editMode_ = true;
    currentField_ = FIELD_NAME;
    currentDateSegment_ = DATE_YEAR;
    InitializeInputFields();
}

void TaskEdit::ExitEditMode() {
    editMode_ = false;
    currentField_ = FIELD_NONE;
}

void TaskEdit::SetCurrentField(EditField field) {
    currentField_ = field;
}

void TaskEdit::CycleFields() {
    switch (currentField_) {
        case FIELD_NONE:
            currentField_ = FIELD_NAME;
            break;
        case FIELD_NAME:
            currentField_ = FIELD_DESCRIPTION;
            break;
        case FIELD_DESCRIPTION:
            currentField_ = FIELD_DEADLINE;
            break;
        case FIELD_DEADLINE:
            currentField_ = FIELD_PRIORITY;
            break;
        case FIELD_PRIORITY:
            currentField_ = FIELD_NAME;
            break;
    }
}

void TaskEdit::SetCurrentDateSegment(DateSegment segment) {
    currentDateSegment_ = segment;
}

void TaskEdit::CycleDateSegments(bool forward) {
    if (forward) {
        currentDateSegment_ = static_cast<DateSegment>(
            (currentDateSegment_ + 1) % DATE_SEGMENTS_COUNT
        );
    } else {
        currentDateSegment_ = static_cast<DateSegment>(
            (currentDateSegment_ + DATE_SEGMENTS_COUNT - 1) % DATE_SEGMENTS_COUNT
        );
    }
}

void TaskEdit::HandleTextInput(char inputChar) {
    if (!editMode_) return;

    switch (currentField_) {
        case FIELD_NAME:
            if (inputChar >= 32) { // Printable characters
                nameInput_ += inputChar;
            }
            break;
            
        case FIELD_DESCRIPTION:
            if (inputChar >= 32) { // Printable characters
                descriptionInput_ += inputChar;
            }
            break;

        case FIELD_DEADLINE:
            // Initialize deadlineStr from the current value if it's empty
            if (deadlineInputStr_.empty()) {
                deadlineInputStr_ = Task::formatDeadline(deadlineInput_);
            }
            
            if ((inputChar >= '0' && inputChar <= '9') || 
                inputChar == '-' || inputChar == ' ' || inputChar == ':') {
                deadlineInputStr_ += inputChar;
            }
            
            // Try to parse back to a time_point
            try {
                deadlineInput_ = Task::parseDeadline(deadlineInputStr_);
            } catch (...) {
                // Keep the old value if parsing fails
            }
            break;
            
        case FIELD_PRIORITY:
            // Priority is handled with arrow keys, not text input
            break;
            
        case FIELD_NONE:
            break;
    }
}

void TaskEdit::HandleKeyPress(sf::Keyboard::Key key) {
    if (!editMode_) return;

    switch (key) {
        case sf::Keyboard::Tab:
            CycleFields();
            break;
            
        case sf::Keyboard::Left:
            if (currentField_ == FIELD_DEADLINE) {
                CycleDateSegments(false);
            } else if (currentField_ == FIELD_PRIORITY) {
                CyclePriority(false);
            }
            break;
            
        case sf::Keyboard::Right:
            if (currentField_ == FIELD_DEADLINE) {
                CycleDateSegments(true);
            } else if (currentField_ == FIELD_PRIORITY) {
                CyclePriority(true);
            }
            break;
            
        case sf::Keyboard::Up:
            if (currentField_ == FIELD_PRIORITY) {
                CyclePriority(true);
            } else if (currentField_ == FIELD_DEADLINE) {
                ModifyDateSegment(1);
            }
            break;
            
        case sf::Keyboard::Down:
            if (currentField_ == FIELD_PRIORITY) {
                CyclePriority(false);
            } else if (currentField_ == FIELD_DEADLINE) {
                ModifyDateSegment(-1);
            }
            break;
            
        default:
            break;
    }
}

void TaskEdit::HandleBackspace() {
    if (!editMode_) return;

    switch (currentField_) {
        case FIELD_NAME:
            if (!nameInput_.empty())
                nameInput_.pop_back();
            break;
            
        case FIELD_DESCRIPTION:
            if (!descriptionInput_.empty())
                descriptionInput_.pop_back();
            break;
            
        case FIELD_DEADLINE:
            if (!deadlineInputStr_.empty())
                deadlineInputStr_.pop_back();
            break;
            
        case FIELD_PRIORITY:
        case FIELD_NONE:
            break;
    }
}

void TaskEdit::CyclePriority(bool up) {
    if (up) {
        switch (priorityInput_) {
            case Task::Priority::None:
                priorityInput_ = Task::Priority::Low;
                break;
            case Task::Priority::Low:
                priorityInput_ = Task::Priority::Medium;
                break;
            case Task::Priority::Medium:
                priorityInput_ = Task::Priority::High;
                break;
            case Task::Priority::High:
                priorityInput_ = Task::Priority::None;
                break;
        }
    } else {
        switch (priorityInput_) {
            case Task::Priority::None:
                priorityInput_ = Task::Priority::High;
                break;
            case Task::Priority::Low:
                priorityInput_ = Task::Priority::None;
                break;
            case Task::Priority::Medium:
                priorityInput_ = Task::Priority::Low;
                break;
            case Task::Priority::High:
                priorityInput_ = Task::Priority::Medium;
                break;
        }
    }
}

void TaskEdit::ModifyDateSegment(int direction) {
    auto timePoint = deadlineInput_;
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* tm = std::localtime(&time);
    
    switch (currentDateSegment_) {
        case DATE_YEAR:
            tm->tm_year += direction;
            break;
        case DATE_MONTH:
            tm->tm_mon = (tm->tm_mon + direction + 12) % 12;
            break;
        case DATE_DAY:
            tm->tm_mday = ((tm->tm_mday - 1 + direction + 31) % 31) + 1;
            break;
        case DATE_HOUR:
            tm->tm_hour = (tm->tm_hour + direction + 24) % 24;
            break;
        case DATE_MINUTE:
            tm->tm_min = (tm->tm_min + direction + 60) % 60;
            break;
        case DATE_SECOND:
            tm->tm_sec = (tm->tm_sec + direction + 60) % 60;
            break;
    }
    
    std::time_t newTime = std::mktime(tm);
    deadlineInput_ = std::chrono::system_clock::from_time_t(newTime);
    deadlineInputStr_ = Task::formatDeadline(deadlineInput_);
}

bool TaskEdit::SaveChanges(TaskManager& taskManager) {
    if (!editMode_ || !task_) return false;
    
    // Update the task object with input values
    task_->setName(nameInput_);
    task_->setDescription(descriptionInput_);
    task_->setDeadline(deadlineInput_);
    task_->setPriority(priorityInput_);
    
    // Create a copy for updating the database
    Task updatedTask = *task_;

    if (updatedTask.getId().has_value()) {
        return taskManager.updateTask(updatedTask);
    }
    
    return false;
}

void TaskEdit::InitializeInputFields() {
    if (!task_) return;
    
    nameInput_ = task_->getName();
    descriptionInput_ = task_->getDescription();
    deadlineInput_ = task_->getDeadline();
    priorityInput_ = task_->getPriority();
    deadlineInputStr_ = ""; // Reset string representation
}