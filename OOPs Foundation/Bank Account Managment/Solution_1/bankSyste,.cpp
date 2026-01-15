#include <iostream>
#include <string>
using namespace std;

class BankAccount{
    private:
    string accountHolderName;
    string accountNumber;
    double balance;

    public:
    BankAccount(string name, string accNum, double initialBalance){
        accountHolderName = name;
        accountNumber = accNum;
        balance = initialBalance;
    }

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
        cout << "Account Number: " << accountNumber << endl;
        cout << "Account Holder Name: " << accountHolderName << endl;
        cout << "Balance: " << balance << endl;
    }
};

int main(){
    BankAccount account("Sudhanshu Shekhar", "123456789", 5000);
    account.checkBalance();
    account.deposit(2000);
    account.withdraw(1000);
    account.withdraw(7000);
    account.checkBalance();
    return 0;
}
