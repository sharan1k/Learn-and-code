#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

class Book {
private:
    std::string title;
    std::string author;
    int currentPageIndex;
 
public:
    Book(const std::string& title, const std::string& author) 
        : title(title), author(author), currentPageIndex(1) {}
 
    std::string getTitle() const {
        return title;
    }
 
    std::string getAuthor() const {
        return author;
    }
 
    void turnPage() {
        currentPageIndex++;
    }
 
    std::string getCurrentPage() const {
        return "Content of page " + std::to_string(currentPageIndex);
    }
};
 
class BookStorage {
public:
    static void saveToFile(const Book& book) {
        std::string filename = "./" + book.getTitle() + " - " + book.getAuthor() + ".txt";
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Title: " << book.getTitle() << "\n";
            file << "Author: " << book.getAuthor() << "\n";
            file << "Current Page: " << book.getCurrentPage() << "\n";
            file.close();
        } else {
            std::cout << "Unable to open file for saving.\n";
        }
    }
};

class LibraryCatalog  {
private:
    std::unordered_map<std::string, std::pair<int, int>> bookLocationMap;

public:
    void registerBookLocation(const std::string& bookName, int shelfNumber, int roomNumber) {
        bookLocationMap [bookName] = {shelfNumber, roomNumber};
    }

    std::string findBookLocation(const std::string& bookName) const {
        auto bookIterator = bookLocationMap.find(bookName);
        if (bookIterator != bookLocationMap.end()) {
            return "Book: " + bookName + " is located at Shelf: " + std::to_string(bookIterator->second.first) +
                   ", Room: " + std::to_string(bookIterator->second.second);
        } else {
            return "Book: " + bookName + " is not found in the library.";
        }
    }
};
 
class Printer {
public:
    virtual void printPage(const std::string& pageContent) const = 0;
};
 
class PlainTextPrinter : public Printer {
public:
    void printPage(const std::string& pageContent) const override {
        std::cout << pageContent << std::endl;
    }
};
 
int main() {
    Book book("The Magic Of Thinking Big", "David");
    PlainTextPrinter textPrinter;
    LibraryCatalog library;
    library.registerBookLocation("The Magic Of Thinking Big",5,1);
    book.turnPage();
    book.turnPage();
    textPrinter.printPage(book.getCurrentPage());
    BookStorage::saveToFile(book);
    std::cout << library.findBookLocation("The Magic Of Thinking Big") << std::endl;
    return 0;
}