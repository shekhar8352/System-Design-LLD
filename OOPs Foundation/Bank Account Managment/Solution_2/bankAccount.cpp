#pragma once
#include <string>
#include <iostream>
#include "bankOperations.cpp" // Importing the file directly as requested

using namespace std;

class BankAccount : public BankOperations {
private:
    string accountHolderName;
    string accountNumber;

public:
    // Constructor initializes the parent class (BankOperations)
    BankAccount(string name, string accNum, double initialBalance) 
        : BankOperations(initialBalance) {
        accountHolderName = name;
        accountNumber = accNum;
    }

    // Shadowing the checkBalance method to add more details
    void checkBalance() {
        cout << "-------------------------------" << endl;
        cout << "Account Holder: " << accountHolderName << endl;
        cout << "Account Number: " << accountNumber << endl;
        // Call the base class logic to print the balance
        BankOperations::checkBalance();
        cout << "-------------------------------" << endl;
    }
};