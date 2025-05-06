#include <iostream>
#include <vector>
#include <string>
#include <limits>

enum MenuOption
{
    ADD_TASK = 1,
    REMOVE_TASK,
    MARK_TASK_DONE,
    LIST_TASKS,
    EXIT
};

struct Task
{
    std::string description;
    bool isDone = false;
};

void clearInputStream()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void showMenu()
{
    std::cout << "\n1. Add Task\n"
              << "2. Remove Task\n"
              << "3. Mark All Tasks as Done\n"
              << "4. List Tasks\n"
              << "5. Exit\n"
              << "Enter option: ";
}

MenuOption getUserOption()
{
    int input;
    while (!(std::cin >> input) || input < ADD_TASK || input > EXIT)
    {
        std::cout << "Invalid option. Please enter a number between 1 and 5: ";
        clearInputStream();
    }
    clearInputStream();
    return static_cast<MenuOption>(input);
}

class TaskManager
{
public:
    void run();

private:
    std::vector<Task> tasks;

    void handleOption(MenuOption option);
    void addTask();
    void removeTask();
    void markAllTasksDone();
    void listTasks() const;
    void printTaskList() const;
    bool isTaskListEmpty() const;
};

void TaskManager::run()
{
    while (true)
    {
        showMenu();
        MenuOption option = getUserOption();
        if (option == EXIT)
        {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        handleOption(option);
    }
}

void TaskManager::handleOption(MenuOption option)
{
    switch (option)
    {
    case ADD_TASK:
        addTask();
        break;
    case REMOVE_TASK:
        removeTask();
        break;
    case MARK_TASK_DONE:
        markAllTasksDone();
        break;
    case LIST_TASKS:
        listTasks();
        break;
    default:
        break;
    }
}

void TaskManager::addTask()
{
    std::cout << "Enter task description: ";
    std::string description;
    std::getline(std::cin, description);

    if (description.empty())
    {
        std::cout << "Task cannot be empty." << std::endl;
        return;
    }

    tasks.push_back({description, false});
}

void TaskManager::removeTask()
{
    if (isTaskListEmpty())
    {
        std::cout << "No tasks available." << std::endl;
        return;
    }

    printTaskList();

    std::cout << "Enter task number to remove: ";
    int index;

    while (!(std::cin >> index) || index < 1 || index > static_cast<int>(tasks.size()))
    {
        std::cout << "Invalid task number. Try again: ";
        clearInputStream();
    }
    
    clearInputStream();

    tasks.erase(tasks.begin() + index - 1);
}

void TaskManager::markAllTasksDone()
{
    if (isTaskListEmpty())
    {
        std::cout << "No tasks available." << std::endl;
        return;
    }

    for (Task &task : tasks)
    {
        task.isDone = true;
    }

    std::cout << "All incomplete tasks have been marked as done." << std::endl;
}

void TaskManager::listTasks() const
{
    if (isTaskListEmpty())
    {
        std::cout << "No tasks available." << std::endl;
        return;
    }

    printTaskList();
}

void TaskManager::printTaskList() const
{
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        const Task &task = tasks[i];
        std::cout << i + 1 << ". [" << (task.isDone ? "✔" : " ") << "] " << task.description << std::endl;
    }
}

bool TaskManager::isTaskListEmpty() const
{
    return tasks.empty();
}

int main()
{
    TaskManager manager;
    manager.run();
    return 0;
}
