# Installation Instructions

- Requirements: Git, CMake, SQLite3
    - Installation Guides
        - Git: https://git-scm.com/downloads
        - CMake: https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html
        - SQLite3: https://www.w3resource.com/sqlite/snippets/install-sqlite3-guide.php
- Note: if you're using Linux, visit the following link and run the given commands under the 'Requirements' section in a terminal:
    - https://www.sfml-dev.org/tutorials/3.0/getting-started/cmake/#requirements
- Pull the main branch from the remote repository to your local machine
- In the root directory of the project, run the following in the command prompt:

```bash
cmake -B build
cmake --build build
cd build/bin/
# run the SFML executable
./SFML_Main
# run the test cases for TaskManager and UserManager
./RunTests 
```


# CyclePlanner Controls and Navigation

### Kanban Board
- Adding a Column
    - Click the large grey box with a centered plus icon on the far-right of the board to add a column.
- Column Settings
    - Click on the icon with the three dots in the upper-right of any column to show the settings of the prompt. Click on the rename button to rename the column, the delete button to delete the column, or click anywhere outside of the prompt to close the prompt.
    - Renaming a Column
        - After selecting the 'Rename' button, you may enter a new name for the column by typing on your keyboard. Pressing the enter key, or clicking anywhere on the screen, will save the current name and exit renaming mode.
- Adding a Task to a Column
    - Click on the plus icon in the upper-right of a column to show a prompt with available tasks. Click on any task to add it to the selected column. Click anywhere outside of the prompt to close it.
- Removing a Task from a Column
    - Click on the dash icon in the upper-right of a task to remove it from the column.

### Viewing and Modifying Task Details
- Click on any task within a column on the Kanban board, or in the left-hand side pane in the Task Configuration screen, to view its details and a prompt will appear.
- Editing Task Details
    - Click on the edit button or press the 'E' key to enter edit mode.
    - With the title or description selected, you can make edits by typing on your keyboard.
    - With the deadline selected, use the arrow keys to modify the data and time.
    - With the priority selected, use the arrow keys to change its value.
- Saving changes in Edit Mode
    - Pressing the return/enter key or clicking the save button will save any changes made.
- Canceling changes in Edit Mode
    - Click on the cancel button or click anywhere outside of the prompt to cancel any changes made.

### Window Resizing
- Click on the icon with the three dots in the upper-right of the window to view the available window resolutions.
- Select a resolution and click the save button to change the window resolution. Toggle the 'Windowed' option to change the window mode from windowed or fullscreen.
- Pressing the cancel button or clicking anywhere outside of the prompt will close it.

### Viewing Reminders
- Click the bell icon in the upper-right of the window to view the reminders of tasks with upcoming deadlines.
- Click anywhere outside of the pop-up to close it.

### Changing Screens
- Click on the plus icon in the upper-right of the window to show the prompt for changing screens.
- Select an option from the prompt to switch to that screen. To note, only the board and task configuration are selectable at this time, as user login is currently unavailable.
- Click anywhere outside of the pop-up to close it.

### Task Configuration
- Click on the 'Create' button to create a new task
- While a task is selected...
    - click on the 'Delete' button to delete the selected task.
    - click on the 'View Details' button to view the task details (see "Viewing and Modifying Task Details" for instructions on how to use the prompt)