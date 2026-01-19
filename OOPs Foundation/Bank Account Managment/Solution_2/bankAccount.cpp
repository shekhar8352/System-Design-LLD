#include <iostream>
#include <string>

#include "bankOperations.cpp"
using namespace std;

class BankAccount : public BankOperations{
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
};

    
    