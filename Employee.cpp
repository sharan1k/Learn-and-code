#include <iostream>
#include <string>

class Employee {
private:
    std::string name;
    int age;
    float salary;

public:
    std::string getName() {
        return name;
    }

    void setName(std::string name) {
        this->name = name;
    }

    int getAge() {
        return age;
    }

    void setAge(int age) {
        this->age = age;
    }

    float getSalary() {
        return salary;
    }

    void setSalary(float salary) {
        this->salary = salary;
    }
};

int main() {
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
The employee is considered an object because it is a specific instance of the `Employee` class. 
It stores actual data for the class's attributes like name, age etc. and can perform operations 
using the methods defined in the class. As an object, it brings the class blueprint to life
by holding and managing concrete information.
*/