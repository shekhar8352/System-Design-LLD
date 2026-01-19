#pragma once
#include <iostream>
using namespace std;

class BankOperations {
protected:
    double balance;

public:
    BankOperations(double initialBalance = 0) {
        if (initialBalance >= 0) {
            balance = initialBalance;
        } else {
            balance = 0;
            cout << "Initial balance cannot be negative. Set to 0." << endl;
        }
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Amount deposited successfully. New Balance: " << balance << endl;
        } else {
            cout << "Invalid amount" << endl;
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "Amount withdrawn successfully. Remaining Balance: " << balance << endl;
        } else {
            cout << "Invalid amount or insufficient balance" << endl;
        }
    }

    void checkBalance() {    
        cout << "Current Balance: " << balance << endl;
    }
};