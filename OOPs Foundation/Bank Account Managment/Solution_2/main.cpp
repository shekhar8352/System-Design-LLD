#include <iostream>
#include "bankAccount.cpp" // Including the source file directly

using namespace std;

int main(){
    // Create an object of BankAccount
    BankAccount account("Sudhanshu Shekhar", "123456789", 5000);
    
    // Perform operations
    account.checkBalance(); // Should verify initial balance
    
    account.deposit(2000);
    account.checkBalance(); // Should be 7000
    
    account.withdraw(1000); // Should be 6000
    
    account.withdraw(7000); // Should fail (insufficient funds)
    
    account.checkBalance(); // Final check

    return 0;
}