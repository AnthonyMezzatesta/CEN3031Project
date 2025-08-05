cmake_minimum_required(VERSION 3.15)
project(CyclePlanner VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
#find_package(SQLite3 REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Include directories
include_directories(include)
include_directories(src/Utilities src/Utilities/EventSystem)
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIRS})

# Source files
set(SOURCES
        src/TaskManager/Task.cpp
        src/TaskManager/TaskManager.cpp
        src/TaskManager/User.cpp
        src/TaskManager/UserManager.cpp
        src/TaskManager/Team.cpp
        src/TaskManager/TeamManager.cpp
)

set (SFML_SOURCES
        src/Utilities/Utilities.cpp
        src/Utilities/EventSystem/Subject.h
        src/Utilities/EventSystem/Observer.h
        src/Utilities/WindowResizeHandler.h
        src/ReminderManager/ReminderManager.h
        src/GUIStateMachine/GUIStateMachine.h
        src/GUIStateMachine/GUIState.h
        src/TaskCreationScreen.h
        src/OutsideBoardThing.h
        src/Kanban/Board/Board.cpp
        src/Kanban/Board/Column.cpp
        src/Kanban/GUIElement/GUIElement.cpp
        src/Kanban/GUIElement/Icon.cpp
        src/Kanban/GUIElement/TaskCard.h
        src/Kanban/GUIElement/TaskOption.h
        src/Kanban/GUIElement/TaskCard.cpp
        src/Kanban/GUIElement/AuthenticationManager.h
        src/Kanban/GUIElement/AuthenticationManager.cpp
        src/Kanban/WindowPrompt/TaskEdit.cpp
        src/Kanban/WindowPrompt/TaskEdit.h
        src/Kanban/WindowPrompt/WindowPrompt.h
        src/Kanban/WindowPrompt/WindowPromptManager.h
        src/Kanban/WindowPrompt/WindowPromptManager.cpp
        src/Kanban/WindowPrompt/AddTaskPrompt.cpp
        src/Kanban/WindowPrompt/ColumnSettingsPrompt.cpp
        src/Kanban/WindowPrompt/TaskDetailsPrompt.cpp
        src/Kanban/WindowPrompt/DummyPrompt.h
        src/Kanban/WindowPrompt/ReminderPrompt.h
        src/Kanban/WindowPrompt/WindowResizePrompt.h
        src/Kanban/GUIElement/NamedButton.h
        src/Kanban/GUIElement/ScrollableTexture.cpp
        src/Kanban/GUIElement/ScrollBar.cpp
)

# Executables
add_executable(TaskManager ${SOURCES} src/TaskManager/main.cpp)
add_executable(Test_SFML ${SFML_SOURCES} ${SOURCES} src/Kanban/main.cpp)

# Test files
set(TEST_SOURCES
    tests/test_framework.cpp
    tests/test_task.cpp
    tests/test_user.cpp
    tests/test_task_manager.cpp
    tests/test_user_manager.cpp
    tests/test_team_manager.cpp
    tests/test_team_tasks.cpp
    tests/main.cpp
)

add_executable(RunTests ${SOURCES} ${TEST_SOURCES})

# Link libraries
target_link_libraries(TaskManager ${SQLITE3_LIBRARIES})
target_compile_options(TaskManager PRIVATE ${SQLITE3_CFLAGS_OTHER})
target_link_libraries(Test_SFML ${SQLITE3_LIBRARIES})
target_compile_options(Test_SFML PRIVATE ${SQLITE3_CFLAGS_OTHER})
target_link_libraries(Test_SFML sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
target_link_libraries(RunTests ${SQLITE3_LIBRARIES})
target_compile_options(RunTests PRIVATE ${SQLITE3_CFLAGS_OTHER})

# Set output directory
set_target_properties(TaskManager PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(Test_SFML PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(RunTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create build type options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
