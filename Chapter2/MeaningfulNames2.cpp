#include <iostream> 
#include <vector> 
#include <string> 
#include <numeric> 
#include <iomanip> 

using namespace std; 

struct Subject { 
    string name; 
    int score; 

    Subject(string subjectName, int subjectScore) 
        : name(subjectName), score(subjectScore) {} 
};  

struct Student  { 
    string name; 
    int rollNumber; 
    vector<Subject> subjects; 

    Student(string studentName, int studentRollNumber, vector<Subject> subjectList) 
        : name(studentName), rollNumber(studentRollNumber), subjects(subjectList) {} 
}; 

void displayStudentDetails(int rollNumber, const vector<Student>& studentList) { 
    const Student* foundStudent = nullptr; 

    for (const auto& student : studentList) { 
        if (student.rollNumber == rollNumber) { 
            foundStudent = &student; 
            break; 
        } 
    } 

    if (!foundStudent) { 
        cout << "Not found: " << rollNumber << endl; 
        return; 
    } 

    int totalScore = accumulate(
        foundStudent->subjects.begin(),
        foundStudent->subjects.end(), 
        0, 
        [](int total, const Subject& subject) { 
            return total + subject.score; 
            }
        ); 
        
    double percentage = (static_cast<double>(totalScore) / (foundStudent->subjects.size() * 100)) * 100; 

    cout << "Name: " << foundStudent->name << endl; 
    cout << "Roll: " << foundStudent->rollNumber << endl; 
    cout << "Details:" << endl; 

    for (const auto& subject : foundStudent->subjects) { 
        cout << "  " << subject.name << ": " << subject.score << endl; 
    } 

    cout << "Total: " << totalScore << endl; 
    cout << "Percentage: " << fixed << setprecision(2) << percentage << "%" << endl; 
    cout << "----------------------" << endl; 
} 

int main() { 
    vector<Student> students = { 
        Student("Alice", 101, {Subject("Math", 85), Subject("English", 90), Subject("Science", 78)}), 
        Student("Bob", 102, {Subject("Math", 70), Subject("English", 88), Subject("Science", 92)}), 
        Student("Charlie", 103, {Subject("Math", 95), Subject("English", 80), Subject("Science", 85)}) 
    }; 

    displayStudentDetails(101, students); // Replace 101 with any roll number to test 

    return 0; 
} 