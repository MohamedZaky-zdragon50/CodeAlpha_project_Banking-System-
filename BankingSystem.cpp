#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Account
{
private:
    void recordTransaction(const string &transaction)
    {
        transactionHistory.push_back(transaction);
    }

public:
    float balance;
    int accountNumber;
    vector<string> transactionHistory;

    Account(int number) : accountNumber(number), balance(0.0)
    {
        loadFromFile();
    }

    void deposit(float amount)
    {
        balance += amount;
        recordTransaction("Deposited $" + to_string(amount));
        saveToFile();
    }

    bool withdraw(float amount)
    {
        if (amount <= balance)
        {
            balance -= amount;
            recordTransaction("Withdrew $" + to_string(amount));
            saveToFile();
            return true;
        }
        else
        {
            cout << "Insufficient balance.\n";
            return false;
        }
    }

    void display() const
    {
        cout << "The Account Number is: " << accountNumber << endl;
        cout << "The Balance Of Your Account is: " << balance << endl;
    }

    void saveToFile() const
    {
        ofstream file(to_string(accountNumber) + ".txt");
        if (!file.is_open())
        {
            cout << "Error Opening File For Writing.\n";
            return;
        }
        file << balance << "\n";
        for (const auto &transaction : transactionHistory)
        {
            file << transaction << "\n";
        }
    }

    void loadFromFile()
    {
        ifstream file(to_string(accountNumber) + ".txt");
        if (!file.is_open())
        {
            cout << "Error opening File for Reading.\n";
            return;
        }
        file >> balance;
        file.ignore(); // Ignore the newline after balance
        transactionHistory.clear();
        string line;
        while (getline(file, line))
        {
            transactionHistory.push_back(line);
        }
    }
};

class Customer
{
public:
    string name;
    vector<Account> accounts;

    Customer(string customerName) : name(customerName) {}

    bool accountExists(int accountNumber)
    {
        for (const auto &acc : accounts)
        {
            if (accountNumber == acc.accountNumber)
            {
                return true;
            }
        }
        return false;
    }

    void addAccount(int accountNumber)
    {
        if (accountExists(accountNumber))
        {
            cout << "The Account Number Is: " << accountNumber << " Already Exists!\n";
            return;
        }
        else
        {
            accounts.emplace_back(accountNumber);
        }
    }

    void displayAccounts()
    {
        for (const auto &acc : accounts)
        {
            acc.display();
        }
    }

    void saveToFile() const
    {
        for (const auto &acc : accounts)
        {
            acc.saveToFile();
        }
    }

    void loadFromFile()
    {
        for (auto &acc : accounts)
        {
            acc.loadFromFile();
        }
    }
};

class Transaction 
{
public:
    static void deposit(Account &account, float amount)
    {
        account.deposit(amount);
        cout << "The Amount Deposited to the Account Number " << account.accountNumber << " is: $" << amount << "\n";
    }

    static bool withdraw(Account &account, float amount)
    {
        bool success = account.withdraw(amount);
        if (success)
        {
            cout << "The Amount Withdrawn from the Account Number " << account.accountNumber << " is: $" << amount << "\n";
        }
        return success;
    }

    static void transfer(Account &toAccount, Account &fromAccount, float amount)
    {
        if (fromAccount.withdraw(amount))
        {
            toAccount.deposit(amount);
            cout << "The Amount Transferred Is: $" << amount << "\n" 
                 << "From Account Number " << fromAccount.accountNumber << "\n" 
                 << "To Account Number " << toAccount.accountNumber << "\n";
        }
    }
};

class BankingService 
{
public:
    static void createAccount(Customer &customer, int accountNumber)
    {
        if (customer.accountExists(accountNumber))
        {
            cout << "The Account Number: " << accountNumber << " Already Exists!\n";
            return;
        }
        customer.addAccount(accountNumber);
        cout << "Created Account Number " << accountNumber << " for Customer " << customer.name << "\n";
        customer.saveToFile(); 
    }

    static void viewAccountInfo(Customer &customer)
    {
        customer.displayAccounts();
    }
};

int main()
{
    cout << "Welcome to our Banking System. We're here to meet your banking needs with ease and security.\n";
    string customername;
    cout << "Enter Your Name: ";
    getline(cin, customername);

    Customer customer(customername);
    customer.loadFromFile();

    if (customer.accounts.empty())
    {
        cout << "No Account Found For " << customername << ".\n";
        cout << "Enter Your Choice:\n";
        cout << "1. Create a new account\n";
        cout << "2. Exit\n";
        int choice;
        cin >> choice;
        cin.ignore(); // Clear newline 

        if (choice == 1)
        {
            int accountnumber;
            cout << "Enter Your Account Number: ";
            cin >> accountnumber;
            BankingService::createAccount(customer, accountnumber);
        }
        else
        {
            cout << "Exiting...\n";
            cout << "Thank you for using our Banking System. Have a great day!\n";
            return 0;
        }
    }

    while (true)
    {
        cout << "Please select the operation you would like to perform:\n";
        cout << "1. Deposit\n";
        cout << "2. Withdraw\n";
        cout << "3. Transfer\n";
        cout << "4. View Account Information\n";
        cout << "5. Exit\n";
        int option;
        cin >> option;
        cin.ignore(); // Clear newline 

        if (option == 5)
        {
            cout << "Exiting...\n";
            cout << "Thank you for using our Banking System. Have a great day!\n";
            break;
        }

        switch (option)
        {
            case 1:
            {
                int accountNumber;
                float amount;
                cout << "Enter Account Number To Deposit Into: ";
                cin >> accountNumber;
                cout << "Enter Amount To Deposit: ";
                cin >> amount;
                for (auto &acc : customer.accounts)
                {
                    if (acc.accountNumber == accountNumber)
                    {
                        Transaction::deposit(acc, amount);
                        customer.saveToFile();
                        break;
                    }
                }
                break;
            }

            case 2:
            {
                int accountNumber;
                float amount;
                cout << "Enter Account Number To Withdraw From: ";
                cin >> accountNumber;
                cout << "Enter The Amount To Withdraw: ";
                cin >> amount;
                for (auto &acc : customer.accounts)
                {
                    if (acc.accountNumber == accountNumber)
                    {
                        if (Transaction::withdraw(acc, amount))
                        {
                            customer.saveToFile();
                        }
                        break;
                    }
                }
                break;
            }

case 3:
{
    int fromAccountNumber, toAccountNumber;
    float amount;
    cout << "Enter Account Number To Transfer From: ";
    cin >> fromAccountNumber;
    cout << "Enter Account Number To Transfer To: ";
    cin >> toAccountNumber;
    cout << "Enter The Amount To Transfer: ";
    cin >> amount;

    Account *fromAcc = nullptr;
    Account *toAcc = nullptr;

    // البحث عن الحسابين
    for (auto &acc : customer.accounts)
    {
        if (acc.accountNumber == fromAccountNumber)
        {
            fromAcc = &acc;
            cout << "Found source account: " << fromAccountNumber << "\n";
        }
        if (acc.accountNumber == toAccountNumber)
        {
            toAcc = &acc;
            cout << "Found destination account: " << toAccountNumber << "\n";
        }
    }

    // التحقق من وجود كلا الحسابين
    if (fromAcc && toAcc)
    {
        if (Transaction::withdraw(*fromAcc, amount))
        {
            Transaction::deposit(*toAcc, amount);
            customer.saveToFile();
            cout << "Transfer successful.\n";
        }
        else
        {
            cout << "Insufficient balance in the source account.\n";
        }
    }
    else
    {
        if (!fromAcc) cout << "Source account not found: " << fromAccountNumber << "\n";
        if (!toAcc) cout << "Destination account not found: " << toAccountNumber << "\n";
        cout << "Invalid Account Numbers.\n";
    }

    break;
}


            case 4:
                BankingService::viewAccountInfo(customer);
                break;

            default:
                cout << "Invalid option, please try again.\n";
        }
    }

    return 0;
}
