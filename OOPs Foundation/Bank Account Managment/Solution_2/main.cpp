#include <iostream>
#include <string>

#include "bankAccount.cpp"
using namespace std;

int main(){
    BankAccount account("Sudhanshu Shekhar", "123456789", 5000);
    account.checkBalance();
    account.deposit(2000);
    account.withdraw(1000);
    account.withdraw(7000);
    account.checkBalance();
    return 0;
}