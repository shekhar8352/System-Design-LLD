#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// 1. Define Book first so others can use it
class Book {
public:
    string title;
    string author;
    string id;
    bool isAvailable;

    Book(string title, string author, string id) {
        this->title = title;
        this->author = author;
        this->id = id;
        this->isAvailable = true;
    }

    // Overloading == operator to compare books easily
    bool operator==(const Book& other) const {
        return this->id == other.id;
    }
};

// 2. Define User next
class User {
public:
    string name;
    string id;
    vector<Book> borrowedBooks;

    User(string name, string id) {
        this->name = name;
        this->id = id;
    }
};

// 3. Define Library (The Manager)
class Library {
private:
    vector<Book> books;
    vector<User> users;

public:
    void addBook(Book book) {
        books.push_back(book);
    }

    void addUser(User user) {
        users.push_back(user);
    }

    void availableBooks() {
        cout << "Available Books:" << endl;
        for (const auto& book : books) {
            if (book.isAvailable) {
                cout << "- " << book.title << endl;
            }
        }
    }

    // 4. Update Borrow Logic: We need to know WHO lies borrowing WHAT
    void borrowBook(User& user, string bookId) {
        bool found = false;
        for (auto& book : books) {
            if (book.id == bookId) {
                found = true;
                if (book.isAvailable) {
                     if (user.borrowedBooks.size() < 3) {
                        book.isAvailable = false;
                        user.borrowedBooks.push_back(book);
                        cout << user.name << " borrowed " << book.title << endl;
                    } else {
                        cout << "User can borrow at most 3 books" << endl;
                    }
                } else {
                    cout << "Book is already borrowed." << endl;
                }
                break;
            }
        }
        if (!found) cout << "Book not found in library." << endl;
    }

    void returnBook(User& user, string bookId) {
        // 1. Remove from User's list
        bool bookInUser = false;
        for (auto it = user.borrowedBooks.begin(); it != user.borrowedBooks.end(); ++it) {
            if (it->id == bookId) {
                user.borrowedBooks.erase(it);
                bookInUser = true;
                break;
            }
        }

        if (bookInUser) {
            // 2. Mark as available in Library
            for (auto& book : books) {
                if (book.id == bookId) {
                    book.isAvailable = true;
                    cout << user.name << " returned " << book.title << endl;
                    break;
                }
            }
        } else {
            cout << "This user does not have this book." << endl;
        }
    }
};

int main() {
    Library library;
    User sudhanshu("Sudhanshu", "1");
    Book cleanCode("Clean Code", "Robert C. Martin", "101");
    
    library.addBook(cleanCode);
    library.addUser(sudhanshu);
    
    library.availableBooks();
    
    library.borrowBook(sudhanshu, "101");
    
    library.availableBooks(); 
    
    library.returnBook(sudhanshu, "101");
    
    library.availableBooks(); 

    return 0;
}