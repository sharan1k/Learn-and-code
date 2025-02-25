#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
 
class Employee {
private:
    int employeeId;
    std::string employeeName;
    std::string employeeDepartment;
    bool working;
 
public:
    Employee(int employeeId, const std::string& name, const std::string& employeeDepartment, bool working)
        : employeeId(employeeId), employeeName(employeeName), employeeDepartment(employeeDepartment), working(working) {}
 
    int getEmployeeId() const { 
        return employeeId; 
    }

    std::string getEmployeeName() const { 
        return employeeName; 
    }

    std::string getEmployeeDepartment() const {
         return employeeDepartment; 
    }

    bool getEmployeeWorkingStatus() const { 
        return working; 
    }

    void setEmployeeWorkingStatus(bool status) {
        working = status;
    }
};
 
class DatabaseOperations  {
public:
    void saveEmployeeToDatabase(const Employee& employee) {
        std::cout << "Saving Employee to Database: " << employee.getEmployeeName() << std::endl;
    }
};
 
class EmployeeReportGenerator {
public:
    void printEmployeeDetailsAsXML(const Employee& employee) {
        std::cout << "Generating XML report for employee " << employee.getEmployeeName() << "\n";
        std::cout << "<Employee>\n\t<ID>" << employee.getEmployeeId() << "</ID>\n\t<Name>" << employee.getEmployeeName()
                  << "</Name>\n\t<Department>" << employee.getEmployeeDepartment() << "</Department>\n</Employee>\n";
    }
 
    void printEmployeeDetailsAsCSV(const Employee& employee) {
        std::cout << "Generating CSV report for employee " << employee.getEmployeeName() << "...\n";
        std::cout << employee.getEmployeeId() << ", " << employee.getEmployeeName() << ", " 
                  << employee.getEmployeeDepartment() << "\n";
    }
};
 
class EmployeeManagement {
public:
    void terminateEmployee(Employee& employee) {
        employee.setEmployeeWorkingStatus(false);
        std::cout << "Employee " << employee.getEmployeeName() << " has been terminated.\n";
    }

    bool checkIfEmployeeIsWorking(const Employee& employee) {
        return employee.getEmployeeWorkingStatus();
    }
};
 
int main() {
    Employee employee(1, "John Doe", "HR", true);

    DatabaseOperations databaseOperations;
    EmployeeReportGenerator reportGenerator;
    EmployeeManagement employeeManagement;
 
    databaseOperations.saveEmployeeToDatabase(employee);
    reportGenerator.printEmployeeDetailsAsXML(employee);
    reportGenerator.printEmployeeDetailsAsCSV(employee);
    employeeManagement.terminateEmployee(employee);
    
    return 0;
}