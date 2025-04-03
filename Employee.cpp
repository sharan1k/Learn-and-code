#include <iostream>
#include <string>

class Employee
{
private:
    std::string name;
    int age;
    float salary;

public:
    std::string getName()
    {
        return name;
    }

    void setName(std::string name)
    {
        this->name = name;
    }

    int getAge()
    {
        return age;
    }

    void setAge(int age)
    {
        this->age = age;
    }

    float getSalary()
    {
        return salary;
    }

    void setSalary(float salary)
    {
        this->salary = salary;
    }
};

int main()
{
    Employee employee;

    employee.setName("sharan");
    employee.setAge(22);
    employee.setSalary(5000.0);

    std::cout << "Employee Name: " << employee.getName() << std::endl;
    std::cout << "Employee Age: " << employee.getAge() << std::endl;
    std::cout << "Employee Salary: " << employee.getSalary() << std::endl;

    return 0;
}

/*
The `Employee` class can be considered a data structure because it defines a
template for storing related information, such as an employee's name, age,
and salary. It organizes these attributes into a single unit, encapsulating
employee data in a structured manner. However, unlike simple data structures,
it also includes methods to get and set these values, enhancing its functionality.
*/