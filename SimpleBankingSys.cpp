#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>  // For isdigit function

using namespace std;

// Function to validate if a string is a valid PIN (exactly 5 digits)
bool isValidPIN(const string& pin) {
    if (pin.length() != 5) return false;
    for (size_t i = 0; i < pin.length(); ++i) {
        if (!isdigit(pin[i])) return false;
    }
    return true;
}

// Class representing a bank account
class Account {
public:
    string name;  // Account holder's name
    double balance;  // Account balance
    string pin;  // Account PIN
    bool isLocked;  // Account lock status

    // Constructor to initialize account with name, balance, and PIN
    Account(string n, double b, string p) : name(n), balance(b), pin(p), isLocked(false) {}

    // Function to deposit money into the account
    void deposit(double amount) {
        if (isLocked) {
            cout << "Account is locked. Please visit customer support.\n";
            return;
        }
        balance += amount;
        cout << "Deposited: $" << amount << endl;
    }

    // Function to withdraw money from the account
    void withdraw(double amount) {
        if (isLocked) {
            cout << "Account is locked. Please visit customer support.\n";
            return;
        }
        if (amount > balance - 15 && balance > 15) {  // Check if balance would drop below $15 unless closing account
            cout << "Insufficient funds or withdrawal would drop balance below $15!" << endl;
        } else {
            balance -= amount;
            cout << "Withdrew: $" << amount << endl;
        }
    }

    // Function to display account details
    void display() const {
        if (isLocked) {
            cout << "Account is locked. Please visit customer support.\n";
            return;
        }
        cout << "Account: " << name << ", Balance: $" << balance << endl;
    }

    // Function to check if the provided PIN is correct
    bool checkPin(const string& inputPin) const {
        return pin == inputPin;
    }

    // Function to lock the account
    void lock() {
        isLocked = true;
    }

    // Function to save account details to a file
    void saveToFile() const {
        ofstream file("accounts.txt", ios::app);
        file << name << " " << balance << " " << pin << " " << isLocked << endl;
    }

    // Static function to load accounts from a file
    static vector<Account> loadFromFile() {
        vector<Account> accounts;
        ifstream file("accounts.txt");
        string name, pin;
        double balance;
        bool isLocked;

        while (file >> name >> balance >> pin >> isLocked) {
            accounts.push_back(Account(name, balance, pin));
            accounts.back().isLocked = isLocked;
        }

        return accounts;
    }

    // Static function to write all accounts to the file
    static void saveAllAccounts(const vector<Account>& accounts) {
        ofstream file("accounts.txt");
        for (size_t i = 0; i < accounts.size(); ++i) {
            file << accounts[i].name << " " << accounts[i].balance << " " << accounts[i].pin << " " << accounts[i].isLocked << endl;
        }
    }
};

// Function to find an account by name
Account* findAccount(vector<Account>& accounts, const string& name) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].name == name) {
            return &accounts[i];
        }
    }
    return NULL;  // Return NULL if account not found
}

// Function to delete all accounts by clearing the file
void deleteAllAccounts() {
    ofstream file("accounts.txt", ios::trunc);
    file.close();
    cout << "All accounts have been deleted.\n";
}

// Function to delete an account from the list and file
void deleteAccount(vector<Account>& accounts, const string& name) {
    ofstream file("accounts.txt", ios::trunc);
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].name == name) {
            accounts.erase(accounts.begin() + i);
            cout << "Account closed successfully.\n";
            break;
        }
    }
    Account::saveAllAccounts(accounts);  // Save updated account details to file
}

// Function to withdraw all funds from an account with PIN validation and maximum attempts
void withdrawAllFunds(Account* acc, vector<Account>& accounts) {
    string inputPin;
    int attempts = 0;

    while (attempts < 3) {
        cout << "Enter PIN: ";
        cin >> inputPin;

        if (acc->checkPin(inputPin)) {
            double totalBalance = acc->balance;
            if (totalBalance > 15) {
                acc->balance = 0;  // Set balance to zero
                cout << "Total amount withdrawn: $" << totalBalance << endl;
            } else {
                cout << "Balance too low to withdraw all funds. Please use regular withdrawal.\n";
            }
            return;  // Exit the function after successful withdrawal
        } else {
            cout << "Wrong PIN. Please try again.\n";
            attempts++;
        }
    }

    cout << "Too many incorrect attempts. Your account has been locked. Please visit customer support.\n";
    acc->lock();  // Lock the account after three failed attempts
    Account::saveAllAccounts(accounts);  // Save updated account details to file
}

int main() {
    vector<Account> accounts = Account::loadFromFile();  // Load existing accounts from file
    int choice;  // User choice
    string name;  // Account holder's name
    double amount;  // Amount to deposit or withdraw
    string pin;  // Account PIN

    while (true) {  // Loop to keep the program running
        cout << "1. Create Account\n2. Deposit\n3. Withdraw\n4. Check Balance\n5. View All Accounts\n6. Close Account\n7. Delete All Accounts\n8. Exit\n";
        cin >> choice;  // Get user choice

        switch (choice) {
            case 1: {  // Create a new account
                cout << "Enter account name: ";
                cin >> name;
                cout << "Enter initial deposit (minimum $15): ";
                cin >> amount;
                if (amount < 15) {
                    cout << "Initial deposit must be at least $15.\n";
                } else {
                    while (true) {
                        cout << "Set a PIN for your account (5 digits): ";
                        cin >> pin;
                        if (isValidPIN(pin)) {
                            break;
                        } else {
                            cout << "Invalid PIN. Please enter exactly 5 digits.\n";
                        }
                    }
                    accounts.push_back(Account(name, amount, pin));  // Add new account to the list
                    accounts.back().saveToFile();  // Save new account to file
                }
                break;
            }

            case 2: {  // Deposit money into an account
                cout << "Enter account name: ";
                cin >> name;
                cout << "Enter PIN: ";
                cin >> pin;
                Account* acc = findAccount(accounts, name);
                if (acc) {
                    if (acc->checkPin(pin)) {
                        double depositAmount;
                        cout << "Enter deposit amount: ";
                        cin >> depositAmount;
                        acc->deposit(depositAmount);
                        Account::saveAllAccounts(accounts);  // Save updated account details to file
                    } else {
                        cout << "Wrong PIN. Please try again.\n";
                    }
                } else {
                    cout << "No such account present, please retry.\n";
                }
                break;
            }

            case 3: {  // Withdraw money from an account
                cout << "Enter account name: ";
                cin >> name;
                cout << "Enter PIN: ";
                cin >> pin;
                Account* acc = findAccount(accounts, name);
                if (acc) {
                    if (acc->checkPin(pin)) {
                        double withdrawAmount;
                        cout << "Enter withdrawal amount: ";
                        cin >> withdrawAmount;
                        acc->withdraw(withdrawAmount);
                        Account::saveAllAccounts(accounts);  // Save updated account details to file
                    } else {
                        cout << "Wrong PIN. Please try again.\n";
                    }
                } else {
                    cout << "No such account present, please retry.\n";
                }
                break;
            }

            case 4: {  // Check account balance
                cout << "Enter account name: ";
                cin >> name;
                cout << "Enter PIN: ";
                cin >> pin;
                Account* acc = findAccount(accounts, name);
                if (acc) {
                    if (acc->checkPin(pin)) {
                        acc->display();
                    } else {
                        cout << "Wrong PIN. Please try again.\n";
                    }
                } else {
                    cout << "No such account present, please retry.\n";
                }
                break;
            }

            case 5: {  // View all accounts and their balances
                for (size_t i = 0; i < accounts.size(); ++i) {
                    accounts[i].display();
                }
                break;
            }

            case 6: {  // Close an account
                cout << "Enter account name to close: ";
                cin >> name;
                Account* acc = findAccount(accounts, name);
                if (acc) {
                    char confirmation;
                    cout << "Do you want to withdraw all funds and close the account? (y/n): ";
                    cin >> confirmation;
                    if (confirmation == 'y' || confirmation == 'Y') {
                        withdrawAllFunds(acc, accounts);  // Withdraw all funds
                        if (!acc->isLocked) {  // Check if account is locked before deleting
                            deleteAccount(accounts, name);  // Delete the account from the list and file
                        } else {
                            cout << "Account closure canceled due to account lock.\n";
                        }
                    } else {
                        cout << "Account closure canceled.\n";
                    }
                } else {
                    cout << "No such account present, please retry.\n";
                }
                break;
            }

            case 7: {  // Delete all accounts
                cout << "Are you sure you want to delete all accounts? (y/n): ";
                char confirmation;
                cin >> confirmation;
                if (confirmation == 'y' || confirmation == 'Y') {
                    deleteAllAccounts();  // Clear the file
                    accounts.clear();  // Clear the accounts vector
                    cout << "All accounts have been deleted.\n";
                } else {
                    cout << "Deletion canceled.\n";
                }
                break;
            }

            case 8:  // Exit the program
                return 0;

            default:  // Invalid choice
                cout << "Invalid choice! Try again.\n";
                break;
        }
    }

    return 0;
}

