#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

// ---------- Base class ----------
class BankAccount {
protected:
    int accNo;
    string name;
    double balance;
public:
    BankAccount(int no, string n, double bal) : accNo(no), name(n), balance(bal) {}
    virtual ~BankAccount() {}

    int getAccNo() const { return accNo; }
    double getBalance() const { return balance; }

    void deposit(double amt) {
        if (amt <= 0) { cout << "Invalid amount. Deposit must be positive.\n"; return; }
        balance += amt;
        cout << "Deposit Successful\nUpdated Balance: Rs." << balance << "\n";
    }
    virtual bool withdraw(double amt) {
        if (amt <= 0)       { cout << "Invalid amount.\n"; return false; }
        if (amt > balance)  { cout << "Insufficient Balance\nTransaction Failed\n"; return false; }
        balance -= amt;
        cout << "Withdrawal Successful\nUpdated Balance: Rs." << balance << "\n";
        return true;
    }
    virtual string type() const = 0;
    virtual void display() const {
        cout << "Account No : " << accNo << "\nName       : " << name
             << "\nType       : " << type() << "\nBalance    : Rs." << balance << "\n";
    }
    string toFile() const {
        return to_string(accNo) + "," + name + "," + type() + "," + to_string(balance);
    }
};

// ---------- Derived classes ----------
class SavingsAccount : public BankAccount {
public:
    using BankAccount::BankAccount;
    string type() const override { return "Savings"; }
};

class CurrentAccount : public BankAccount {
    static constexpr double MIN_BAL = 1000;
public:
    using BankAccount::BankAccount;
    string type() const override { return "Current"; }
    bool withdraw(double amt) override {           // polymorphic rule
        if (amt > 0 && balance - amt < MIN_BAL) {
            cout << "Minimum balance of Rs." << MIN_BAL << " must be maintained\nTransaction Failed\n";
            return false;
        }
        return BankAccount::withdraw(amt);
    }
};

// ---------- Bank (manages all accounts) ----------
class Bank {
    vector<BankAccount*> accounts;
    BankAccount* find(int no) {
        for (auto a : accounts) if (a->getAccNo() == no) return a;
        return nullptr;
    }
public:
    ~Bank() { for (auto a : accounts) delete a; }

    void create() {
        int no, t; string n; double bal;
        cout << "Enter account number: "; cin >> no;
        if (find(no)) { cout << "Account number already exists.\n"; return; }
        cout << "Enter customer name: "; cin >> n;
        cout << "Account type (1-Savings, 2-Current): "; cin >> t;
        cout << "Enter initial balance: "; cin >> bal;
        if (bal < 0 || (t != 1 && t != 2)) { cout << "Invalid input.\n"; return; }
        if (t == 2 && bal < 1000) { cout << "Current account needs minimum Rs.1000.\n"; return; }
        accounts.push_back(t == 1 ? (BankAccount*)new SavingsAccount(no, n, bal)
                                  : (BankAccount*)new CurrentAccount(no, n, bal));
        cout << "Account created successfully.\n";
    }
    void deposit()  { int no; double a; cout << "Account no: "; cin >> no; BankAccount* x = find(no);
        if (!x) { cout << "Account not found.\n"; return; } cout << "Amount: "; cin >> a; x->deposit(a); }
    void withdraw() { int no; double a; cout << "Account no: "; cin >> no; BankAccount* x = find(no);
        if (!x) { cout << "Account not found.\n"; return; } cout << "Amount: "; cin >> a; x->withdraw(a); }
    void balance()  { int no; cout << "Account no: "; cin >> no; BankAccount* x = find(no);
        if (!x) { cout << "Account not found.\n"; return; } cout << "Current Balance: Rs." << x->getBalance() << "\n"; }
    void display()  { int no; cout << "Account no: "; cin >> no; BankAccount* x = find(no);
        if (!x) { cout << "Account not found.\n"; return; } x->display(); }
    void transfer() {
        int s, d; double a;
        cout << "Source account no: "; cin >> s; cout << "Destination account no: "; cin >> d;
        BankAccount *from = find(s), *to = find(d);
        if (!from || !to || s == d) { cout << "Invalid source/destination account.\n"; return; }
        cout << "Amount: "; cin >> a;
        if (from->withdraw(a)) { to->deposit(a); cout << "Transfer Successful\n"; }
    }
    void save() {
        ofstream f("accounts.txt");
        for (auto a : accounts) f << a->toFile() << "\n";
        cout << "Saved " << accounts.size() << " account(s) to accounts.txt\n";
    }
};

int main() {
    Bank bank; int ch;
    do {
        cout << "\n===== BANK ACCOUNT MANAGEMENT SYSTEM =====\n"
                "1. Create Account\n2. Deposit Money\n3. Withdraw Money\n4. Check Balance\n"
                "5. Display Account\n6. Transfer Money\n7. Save Account Data\n8. Exit\nEnter choice: ";
        if (!(cin >> ch)) break;
        switch (ch) {
            case 1: bank.create();   break;
            case 2: bank.deposit();  break;
            case 3: bank.withdraw(); break;
            case 4: bank.balance();  break;
            case 5: bank.display();  break;
            case 6: bank.transfer(); break;
            case 7: bank.save();     break;
            case 8: cout << "Thank you!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (ch != 8);
}
