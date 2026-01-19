#include <iostream>
#include <string>
using namespace std;

class BankOperations{
    private:
    double balance;
    public:
    void deposit(double amount){
        if(amount > 0){
            balance += amount;
            cout << "Amount deposited successfully" << endl;
        }else{
            cout << "Invalid amount" << endl;
        }
    }

    void withdraw(double amount){
        if(amount > 0 && amount <= balance){
            balance -= amount;
            cout << "Amount withdrawn successfully" << endl;
        }else{
            cout << "Invalid amount or insufficient balance" << endl;
        }
    }

    void checkBalance(){    
        cout << "Balance checked successfully" << endl;
    }
};