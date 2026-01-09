
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

using namespace std;

/* =========================
   GLOBAL STATE
   ========================= */
double memoryValue = 0.0;
bool scientificMode = false;
vector<string> history;

void showCalculatorArt()
{
    cout << R"(
     _____________________
    |  _________________  |
    | | FUN CALCULATOR | |
    | |_________________| |
    |  ___ ___ ___ ___ ___ |
    | | 7 | 8 | 9 | + | ^ |
    | |___|___|___|___|___|
    | | 4 | 5 | 6 | - | √ |
    | |___|___|___|___|___|
    | | 1 | 2 | 3 | * | ! |
    | |___|___|___|___|___|
    | | 0 | . | = | / | C |
    | |___|___|___|___|___|
    )" << endl;
}

/* =========================
   MEMORY SAVE / LOAD
   ========================= */
void loadMemory()
{
    ifstream file("memory.dat");
    if (file)
        file >> memoryValue;
}

void saveMemory()
{
    ofstream file("memory.dat");
    file << memoryValue;
}

/* =========================
   FACTORIAL
   ========================= */
long long factorial(int n)
{
    if (n < 0) return -1;
    long long r = 1;
    for (int i = 1; i <= n; i++) r *= i;
    return r;
}

/* =========================
   SIMPLE EXPRESSION PARSER
   (Shunting-yard algorithm)
   ========================= */
int precedence(char op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

double apply(double a, double b, char op)
{
    switch (op)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return b == 0 ? 0 : a / b;
    case '^': return pow(a, b);
    }
    return 0;
}

double evaluateExpression(const string& expr)
{
    stack<double> values;
    stack<char> ops;

    for (size_t i = 0; i < expr.length(); i++)
    {
        if (isspace(expr[i])) continue;

        if (isdigit(expr[i]) || expr[i] == '.')
        {
            double val;
            stringstream ss;
            while (i < expr.length() && (isdigit(expr[i]) || expr[i] == '.'))
                ss << expr[i++];
            ss >> val;
            values.push(val);
            i--;
        }
        else
        {
            while (!ops.empty() &&
                   precedence(ops.top()) >= precedence(expr[i]))
            {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(apply(a, b, op));
            }
            ops.push(expr[i]);
        }
    }

    while (!ops.empty())
    {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(apply(a, b, op));
    }

    return values.top();
}

/* =========================
   SCIENTIFIC FUNCTIONS
   ========================= */
double scientificCalc(const string& cmd, double x)
{
    if (cmd == "sin") return sin(x);
    if (cmd == "cos") return cos(x);
    if (cmd == "tan") return tan(x);
    if (cmd == "log") return log(x);
    if (cmd == "exp") return exp(x);
    return 0;
}


int main()
{
    loadMemory();
    showCalculatorArt();

    cout << "Memory loaded: " << memoryValue << "\n\n";

    bool running = true;
    while (running)
    {
        cout << "\n==== MENU ====\n";
        cout << "1. Expression Calculator\n";
        cout << "2. Factorial\n";
        cout << "3. Complex Numbers\n";
        cout << "4. Toggle Scientific Mode\n";
        cout << "5. Show History\n";
        cout << "6. Show Memory\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            cout << "Enter expression (e.g. 3+4*5): ";
            string expr;
            getline(cin, expr);

            double result = evaluateExpression(expr);
            memoryValue = result;

            history.push_back(expr + " = " + to_string(result));
            cout << "Result: " << result << endl;
            break;
        }

        case 2:
        {
            int n;
            cout << "Enter integer: ";
            cin >> n;
            cout << n << "! = " << factorial(n) << endl;
            break;
        }

        case 3:
        {
            double a, b, c, d;
            cout << "Enter z1 (real imag): ";
            cin >> a >> b;
            cout << "Enter z2 (real imag): ";
            cin >> c >> d;

            complex<double> z1(a, b), z2(c, d);
            cout << "z1 + z2 = " << z1 + z2 << endl;
            cout << "z1 * z2 = " << z1 * z2 << endl;
            break;
        }

        case 4:
        {
            scientificMode = !scientificMode;
            cout << "Scientific mode "
                 << (scientificMode ? "ON" : "OFF") << endl;

            if (scientificMode)
            {
                cout << "Command (sin cos tan log exp): ";
                string cmd;
                double x;
                cin >> cmd >> x;
                double r = scientificCalc(cmd, x);
                memoryValue = r;
                cout << "Result: " << r << endl;
            }
            break;
        }

        case 5:
        {
            cout << "\n--- HISTORY ---\n";
            for (auto& h : history)
                cout << h << endl;
            break;
        }

        case 6:
            cout << "Memory = " << memoryValue << endl;
            break;

        case 0:
            saveMemory();
            cout << "Memory saved. Goodbye 👋\n";
            running = false;
            break;
        }
    }

    return 0;
}
