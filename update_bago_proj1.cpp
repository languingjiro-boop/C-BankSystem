#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <Windows.h>
using namespace std;

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

struct AccInfo
{
    int accNumber;
    string accName;
    string birthday;
    string contactNum;
    double balance;
    string pinCode;
};

struct Node
{
    Node *next;
    AccInfo data;
    Node(AccInfo x)
    {
        data = x;
        next = NULL;
    }
};

class ATMsimulator
{
private:
    Node *head;

public:
    ATMsimulator() : head(NULL) {}
    ~ATMsimulator()
    {
        Node *p;
        while (head != NULL)
        {
            p = head;
            head = head->next;
            delete p;
        }
    }

    bool locateACC(int AccNUM); // meron na
    void add(AccInfo x);        // meron na
    void registerAccount();     // meron na
    void save();                // meron na
    void load();                // meron na
    void saveToUsb();
    void loadFromUsb();

    void Transaction();           // kulang pa ng functions
    bool login(Node *&accRef);    // meron na
    void withdraw(Node *acc);     // meron na
    void deposit(Node *acc);      // meron na
    void checkBalance(Node *acc); // meron na
    void changePin(Node *acc);    // meron na
    int generateAccNumber();      // meron na
    string findFlashDrive();

    /*
    **NEED BAGONG FUNCTION:
        1.hide ng pincode
        2.transfer funds
        3.SAVING ACCNUM AND PINCODE TO USB DRIVE
        4.READING FROM USB DRIVE
    */
};

int menu();

int main()
{
    srand(time(NULL));
    ATMsimulator atm;
    atm.load();
    while (1)
    {
        switch (menu())
        {
        case 1:
            atm.registerAccount();
            break;
        case 2:
            atm.Transaction();
            break;

        case 3:
            atm.save();
            cout << "\nTHANK YOU FOR USING RoRo BANK\n";
            cout << "HAVE A NICE DAY!\n";
            system("pause");
            exit(0);
            break;
        default:
            cout << RED << "Invalid input. Choose from 1 - 3\n"
                 << RESET;
            system("pause");
            break;
        }
    }

    return 0;
}

int menu()
{
    int c;
    system("cls");
    cout << "===================================================\n";
    cout << "              WELCOME TO RoRo BANK\n";
    cout << "===================================================\n";
    cout << "[1] Registration\n";
    cout << "[2] Transaction\n";
    cout << "[3] Exit\n";
    cout << "===================================================\n";
    cout << GREEN << "Enter choice: " << RESET;
    while (!(cin >> c))
    {
        cout << RED << "Invalid input. Enter a number [1-3]: " << RESET;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return c;
}

void ATMsimulator::registerAccount()
{
    AccInfo accIn;
    system("cls");
    cout << "===================================================\n";
    cout << "              RoRo BANK REGISTRATION\n";
    cout << "===================================================\n";
    cout << GREEN << "Input Name: " << RESET;
    cin.ignore(1000, '\n');
    getline(cin, accIn.accName);
    while (accIn.accName.empty())
    {
        cout << RED << "Name cannot be blank. Input Name: " << RESET;
        getline(cin, accIn.accName);
    }

    cout << GREEN << "Input Birthday (Ex. 01/21/2000): " << RESET;
    getline(cin, accIn.birthday);
    while (accIn.birthday.empty())
    {
        cout << RED << "Birthday cannot be blank. Input Birthday: " << RESET;
        getline(cin, accIn.birthday);
    }

    cout << GREEN << "Input Contact Number (Ex. 09123456789): " << RESET;
    getline(cin, accIn.contactNum);
    while (accIn.contactNum.empty())
    {
        cout << RED << "Contact Number cannot be blank. Input Contact Number: " << RESET;
        getline(cin, accIn.contactNum);
    }

    cout << GREEN << "Please make an initial deposit (Minimum is PHP 5000): PHP " << RESET;
    while (!(cin >> accIn.balance) || accIn.balance < 5000)
    {
        cout << RED << "You cannot make an account with less than PHP 5000 balance.\nPlease deposit an amount of PHP 5000 or more: PHP " << RESET;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    accIn.accNumber = generateAccNumber();
    cout << GREEN << "Your Account Number: " << accIn.accNumber << RESET << endl;

    cout << GREEN << "Enter Pin Code (Min. 4 digits & Max. 6 digits): " << RESET;
    getline(cin, accIn.pinCode);
    while (accIn.pinCode.empty() || (accIn.pinCode.length() < 4 || accIn.pinCode.length() > 6))
    {
        cout << RED << "Pin cannot be blank, less than 4 digits and more than 6 digits. Enter Pin Code: " << RESET;
        getline(cin, accIn.pinCode);
    }
    add(accIn);
    cout << GREEN << "ACCOUNT CREATION COMPLETE\n"
         << RESET;
    save();
    system("pause");
}

int ATMsimulator::generateAccNumber()
{
    int AccNm;
    do
    {
        AccNm = 10000 + rand() % 90000;
    } while (locateACC(AccNm));
    return AccNm;
}

bool ATMsimulator::locateACC(int AccNUM)
{
    Node *p = head;
    while (p != NULL)
    {
        if (AccNUM == p->data.accNumber)
            return true;
        p = p->next;
    }
    return false;
}

void ATMsimulator::add(AccInfo x)
{
    Node *p, *q, *newNode;
    p = head;
    q = NULL;
    newNode = new Node(x);
    while (p != NULL && x.accNumber > p->data.accNumber)
    {
        q = p;
        p = p->next;
    }
    if (q == NULL)
        head = newNode;
    else
        q->next = newNode;
    newNode->next = p;
}

bool ATMsimulator::login(Node *&accRef)
{
    int accNum;
    string pin;

    string drive = findFlashDrive();
    if (drive.empty())
    {
        cout << RED << "Please insert card.\n"
             << RESET;
        system("pause");
        return false;
    }

    // loadFromUsb();
    cout << GREEN << "\nEnter Account Number: " << RESET;
    while (!(cin >> accNum))
    {
        cout << RED << "Invalid input. Enter a number: " << RESET;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    cout << GREEN << "Enter Pin Code: " << RESET;
    getline(cin, pin);

    Node *p = head;
    while (p != NULL)
    {
        if (p->data.accNumber == accNum && p->data.pinCode == pin)
        {
            cout << GREEN << "\nLogin successful! Welcome, " << RESET << p->data.accName << endl;
            system("pause");
            accRef = p;
            return true;
        }
        p = p->next;
    }
    cout << RED << "\nWrong Account Number / Wrong Pin Code. Please try again.\n"
         << RESET;
    system("pause");
    return false;
}

void ATMsimulator::Transaction()
{
    Node *acc = NULL;
    if (!login(acc))
    {
        return;
    }

    int choice;
    while (1)
    {
        system("cls");
        cout << "===================================================\n";
        cout << "              RoRo BANK TRANSACTION\n";
        cout << "===================================================\n";
        cout << "[1] Deposit\n";
        cout << "[2] Withdraw\n";
        cout << "[3] Check Balance\n";
        cout << "[4] Fund Transfer\n";
        cout << "[5] Change Pin Code\n";
        cout << "[6] Exit\n";
        cout << "===================================================\n";
        cout << GREEN << "Enter choice: " << RESET;
        while (!(cin >> choice))
        {
            cout << RED << "Invalid input. Enter a number [1-6]: " << RESET;
            cin.clear();
            cin.ignore(1000, '\n');
        }

        switch (choice)
        {
        case 1:
            deposit(acc);
            break;
        case 2:
            withdraw(acc);
            break;
        case 3:
            checkBalance(acc);
            break;
        case 4:
            // Fund Transfer function
            break;
        case 5:
            changePin(acc);
            break;
        case 6:
            save();
            return; // Exit the transaction menu
        default:
            cout << RED << "Invalid choice. Please try again.\n"
                 << RESET;
            system("pause");
            break;
        }
    }
}

void ATMsimulator::deposit(Node *acc)
{
    double amount;
    system("cls");
    cout << "===================================================\n";
    cout << "                    DEPOSIT\n";
    cout << "===================================================\n";
    cout << GREEN << "Enter amount to deposit: PHP " << RESET;
    while (!(cin >> amount) || amount < 1000)
    {
        cout << RED << "Invalid amount. Minimum deposit is PHP 1000\n"
             << RESET;
        cout << GREEN << "Enter amount: PHP " << RESET;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    acc->data.balance += amount;

    cout << fixed << setprecision(2);
    cout << GREEN << "\nDeposit successful!\n";
    cout << "New balance: PHP " << RESET << acc->data.balance << endl;
    save();
    system("pause");
}

void ATMsimulator::withdraw(Node *acc)
{
    double amount;

    system("cls");
    cout << "===================================================\n";
    cout << "                    WITHDRAW\n";
    cout << "===================================================\n";
    if (acc->data.balance - 5000 < 1000)
    {
        cout << RED << "Insufficient available balance.\n"
             << "You must maintain PHP 5000 and the minimum withdrawal is PHP 1000.\n"
             << RESET;
        system("pause");
        return;
    }

    cout << GREEN << "Enter amount to withdraw: PHP " << RESET;

    while (!(cin >> amount) || amount < 1000 || amount > acc->data.balance || (acc->data.balance - amount) < 5000)
    {
        cout << RED << "Invalid amount!\nMinimum withdraw amount is PHP 1000 or amount is greater than balance of PHP " << fixed << setprecision(2) << RESET << acc->data.balance << endl;
        cout << RED << "Also you must maintain your balance at PHP 5000\n"
             << RESET;
        cout << GREEN << "Enter amount: PHP " << RESET;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    acc->data.balance -= amount;

    cout << fixed << setprecision(2);
    cout << GREEN << "\nWithdraw successful!\n"
         << RESET;
    cout << GREEN << "You withdrew PHP " << RESET << amount;
    cout << GREEN << "\nNew balance: PHP " << RESET << acc->data.balance << endl;
    save();
    system("pause");
}

void ATMsimulator::checkBalance(Node *acc)
{
    system("cls");
    cout << "===================================================\n";
    cout << "                 BALANCE INQUIRY\n";
    cout << "===================================================\n";
    cout << "Account Number : " << acc->data.accNumber << endl;
    cout << "Account Name   : " << acc->data.accName << endl;
    cout << fixed << setprecision(2);
    cout << "Current Balance: PHP " << acc->data.balance << endl;
    cout << "===================================================\n";
    system("pause");
}

void ATMsimulator::changePin(Node *acc)
{
    string newPin;
    string oldPin;
    int tries = 5;
    bool match = false;
    system("cls");
    cout << "===================================================\n";
    cout << "                    CHANGE PIN\n";
    cout << "===================================================\n";
    cin.ignore(1000, '\n');
    while (tries > 0 && !match)
    {
        cout << "\nEnter Old Pin Code: ";
        getline(cin, oldPin);
        if (oldPin == acc->data.pinCode)
            match = true;
        else
        {
            tries--;
            if (tries > 0)
                cout << "Pin do not match!\nAttempts left: " << tries;
        }
    }
    if (!match)
    {
        cout << "UNABLE TO MATCH PIN CODE FOR 5 ATTEMPTS!\n";
        cout << "Please try again later\n";
        system("pause");
        return;
    }

    cout << "Enter New Pin Code: ";
    while (!(getline(cin, newPin)) || newPin.length() < 4 || newPin.length() > 6 || newPin == oldPin)
    {
        cout << "Pin cannot be blank, same as old pin, less than 4 digits and more than 6 digits.\nEnter New Pin Code: ";
    }

    acc->data.pinCode = newPin;
    cout << "\nPin change successful!\n";
    save();
    system("pause");
}

void ATMsimulator::save()
{
    Node *p = head;
    ofstream file("List_Accounts.csv");
    if (!file)
    {
        cout << "File Error.\n";
        return;
    }
    else
    {
        file << fixed << setprecision(2);
        while (p != NULL)
        {
            file << p->data.accName << ","
                 << p->data.accNumber << ","
                 << p->data.balance << ","
                 << p->data.birthday << ","
                 << p->data.contactNum << ","
                 << p->data.pinCode << "\n";
            p = p->next;
        }
    }
    file.close();
}

void ATMsimulator::load()
{
    ifstream file("List_Accounts.csv");
    if (!file)
    {
        cout << "File Error.\n";
        return;
    }
    else
    {
        string line;
        AccInfo accLoad;
        string numberString;
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            getline(ss, accLoad.accName, ',');

            getline(ss, numberString, ',');
            accLoad.accNumber = stoi(numberString);
            getline(ss, numberString, ',');
            accLoad.balance = stod(numberString);

            getline(ss, accLoad.birthday, ',');
            getline(ss, accLoad.contactNum, ',');
            getline(ss, accLoad.pinCode, ',');
            add(accLoad);
        }
    }
    file.close();
}
/**
void ATMsimulator::saveToUsb()
{
    Node *p = head;
    string path = findFlashDrive();
    ofstream file(path + "pin.code");
    if (!file)
    {
        cout << "File Error.\n";
        return;
    }
    else
    {
        file << fixed << setprecision(2);
        while (p != NULL)
        {
            file << p->data.accNumber << "," << p->data.pinCode << "\n";
            p = p->next;
        }
    }
    file.close();
}

void ATMsimulator::loadFromUsb()
{
    string path = findFlashDrive();
    ifstream file(path + "pin.code");
    if (!file)
    {
        cout << "File Error.\n";
        return;
    }
    else
    {
        string line;
        AccInfo accLoad;
        string numberString;
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            getline(ss, numberString, ',');
            accLoad.accNumber = stoi(numberString);
            getline(ss, accLoad.pinCode, ',');
            add(accLoad);
        }
    }
    file.close();
}*/

string ATMsimulator::findFlashDrive()
{
    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; letter++)
    {
        if (drives & (1 << (letter - 'A')))
        {
            string root = string(1, letter) + ":\\";
            if (GetDriveTypeA(root.c_str()) == DRIVE_REMOVABLE)
                return root;
        }
    }
    return "";
}
