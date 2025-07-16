#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <utility>

using namespace std;

// ��ȣ���� Ȯ�� (�Ұ�ȣ, �߰�ȣ, ���ȣ)
bool isOpenBracket(char c) {
    return c == '(' || c == '{' || c == '[';
}

bool isCloseBracket(char c) {
    return c == ')' || c == '}' || c == ']';
}

// ��Ī�Ǵ� ��ȣ���� Ȯ��
bool isMatchingBracket(char open, char close) {
    return (open == '(' && close == ')') ||
        (open == '{' && close == '}') ||
        (open == '[' && close == ']');
}

class ExpressionCalculator {
private:
    // ������ �켱���� ��ȯ
    int getPrecedence(char op) {
        switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':  // �ŵ����� ������
            return 3;
        default:
            return -1;
        }
    }

    // ���������� Ȯ��
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }

    // ���ڿ����� ���� ���� (����, ��� ����)
    bool isNumber(const string& str) {
        if (str.empty()) return false;

        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            if (str.length() == 1) return false;
            start = 1;
        }

        bool hasDecimal = false;
        for (size_t i = start; i < str.length(); i++) {
            if (str[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
            }
            else if (!isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }

    // ���� ǥ����� ��ū���� �и�
    vector<string> tokenize(const string& expression) {
        vector<string> tokens;
        string current = "";

        for (size_t i = 0; i < expression.length(); i++) {
            char c = expression[i];

            // ���� ����
            if (isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current = "";
                }
                continue;
            }

            // ��ȣ�� �������� ���
            if (isOpenBracket(c) || isCloseBracket(c) || isOperator(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current = "";
                }

                // ��ȣ ó��: +, - �� ���� �տ� ���� ��� (���/����)
                if (c == '-' || c == '+') {
                    if (tokens.empty() || isOpenBracket(tokens.back()[0]) ||
                        (tokens.back().length() == 1 && isOperator(tokens.back()[0]))) {
                        current = c; // + �Ǵ� - ��ȣ�� ó��
                        continue;
                    }
                }

                tokens.push_back(string(1, c));
            }
            // �������� �ʴ� ���� üũ
            else if (!isdigit(c) && c != '.') {
                tokens.push_back("INVALID:" + string(1, c));
            }
            else {
                current += c;
            }
        }

        if (!current.empty()) {
            tokens.push_back(current);
        }

        return tokens;
    }

    // ������ ������ �ʿ��� �������� �˻�
    bool hasImplicitMultiplication(const string& expression) {
        for (size_t i = 0; i < expression.length() - 1; i++) {
            char current = expression[i];
            char next = expression[i + 1];

            // ���� ������ ���� ��ȣ�� ���� ���: 3(, 3{, 3[
            if (isdigit(current) && isOpenBracket(next)) {
                return true;
            }
            // �ݴ� ��ȣ ������ ���ڰ� ���� ���: )3, }3, ]3
            if (isCloseBracket(current) && isdigit(next)) {
                return true;
            }
        }
        return false;
    }

    // ������ ������ ��������� ��ȯ
    string addImplicitMultiplication(const string& expression) {
        string result = "";

        for (size_t i = 0; i < expression.length(); i++) {
            char current = expression[i];
            result += current;

            // ���� ���ڰ� �ְ�, ������ ������ �ʿ��� ���
            if (i < expression.length() - 1) {
                char next = expression[i + 1];

                // ���� ������ ���� ��ȣ�� ���� ���: 3( �� 3*(
                if ((isdigit(current) || isCloseBracket(current)) && isOpenBracket(next)) {
                    result += "*";
                }
                // �ݴ� ��ȣ ������ ���ڰ� ���� ���: )3 �� )*3
                else if (isCloseBracket(current) && (isdigit(next) || next == '-')) {
                    // ������ -�� ���, �� ������ �������� Ȯ��
                    if (next == '-' && i + 2 < expression.length() && isdigit(expression[i + 2])) {
                        result += "*";
                    }
                    else if (isdigit(next)) {
                        result += "*";
                    }
                }
            }
        }

        return result;
    }

    // ���� ǥ����� �⺻ ������ ��ȿ���� �˻�
    bool isValidInfixStructure(const vector<string>& tokens) {
        if (tokens.empty()) return false;

        bool expectOperand = true; // ó������ �ǿ����ڸ� ���

        for (const string& token : tokens) {
            if (token.substr(0, 8) == "INVALID:") {
                return false;
            }

            if (isNumber(token)) {
                if (!expectOperand) return false; // �����ڸ� ����ߴµ� ���ڰ� ��
                expectOperand = false;
            }
            else if (isOpenBracket(token[0])) {
                if (!expectOperand) return false; // �����ڸ� ����ߴµ� ���� ��ȣ�� ��
                expectOperand = true;
            }
            else if (isCloseBracket(token[0])) {
                if (expectOperand) return false; // �ǿ����ڸ� ����ߴµ� �ݴ� ��ȣ�� ��
                expectOperand = false;
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                if (expectOperand) return false; // �ǿ����ڸ� ����ߴµ� �����ڰ� ��
                expectOperand = true;
            }
            else {
                return false; // �� �� ���� ��ū
            }
        }

        return !expectOperand; // ���������� �ǿ����ڷ� ������ ��
    }

public:
    // ���� ǥ����� ���� ǥ������� ��ȯ
    pair<string, string> infixToPostfix(const string& infix) {
        vector<string> tokens = tokenize(infix);

        // �⺻ ���� �˻�
        if (!isValidInfixStructure(tokens)) {
            return make_pair("", "�߸��� �����Դϴ�.");
        }

        stack<string> operators;
        vector<string> postfix;
        stack<char> bracketStack; // ��ȣ ��Ī Ȯ�ο�

        for (const string& token : tokens) {
            // �������� �ʴ� ���� �˻�
            if (token.substr(0, 8) == "INVALID:") {
                return make_pair("", "�������� �ʴ� ������");
            }

            if (isNumber(token)) {
                postfix.push_back(token);
            }
            else if (isOpenBracket(token[0])) {
                operators.push(token);
                bracketStack.push(token[0]);
            }
            else if (isCloseBracket(token[0])) {
                bool foundOpenBracket = false;
                while (!operators.empty()) {
                    if (isOpenBracket(operators.top()[0])) {
                        char openBracket = operators.top()[0];
                        operators.pop();

                        if (!bracketStack.empty() && isMatchingBracket(bracketStack.top(), token[0])) {
                            bracketStack.pop();
                            foundOpenBracket = true;
                            break;
                        }
                        else {
                            return make_pair("", "��ȣ Ÿ���� ��ġ���� �ʽ��ϴ�.");
                        }
                    }
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                if (!foundOpenBracket) {
                    return make_pair("", "��ȣ�� ������ �ʾҽ��ϴ�.");
                }
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                // �ŵ������� ����ռ� (right associative)
                while (!operators.empty() && !isOpenBracket(operators.top()[0]) &&
                    ((token[0] != '^' && getPrecedence(operators.top()[0]) >= getPrecedence(token[0])) ||
                        (token[0] == '^' && getPrecedence(operators.top()[0]) > getPrecedence(token[0])))) {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
            else {
                return make_pair("", "�߸��� ������ �Ǵ� ���� ����");
            }
        }

        while (!operators.empty()) {
            if (isOpenBracket(operators.top()[0])) {
                return make_pair("", "��ȣ�� ������ �ʾҽ��ϴ�.");
            }
            postfix.push_back(operators.top());
            operators.pop();
        }

        // ���� ǥ��� ���ڿ� ����
        string postfixStr = "";
        for (size_t i = 0; i < postfix.size(); i++) {
            if (i > 0) postfixStr += " ";
            postfixStr += postfix[i];
        }

        return make_pair(postfixStr, "");
    }

    // ���� ǥ��� ���
    pair<double, string> evaluatePostfix(const string& postfix) {
        if (postfix.empty()) {
            return make_pair(0, "�߸��� ������ �Ǵ� ���� ����");
        }

        istringstream iss(postfix);
        stack<double> operands;
        string token;

        while (iss >> token) {
            if (isNumber(token)) {
                try {
                    operands.push(stod(token));
                }
                catch (...) {
                    return make_pair(0, "�߸��� ������ �Ǵ� ���� ����");
                }
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                if (operands.size() < 2) {
                    return make_pair(0, "�߸��� ������ �Ǵ� ���� ����");
                }

                double b = operands.top(); operands.pop();
                double a = operands.top(); operands.pop();

                double result = 0;
                switch (token[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/':
                    if (b == 0) {
                        return make_pair(0, "��� �Ұ�");
                    }
                    result = a / b;
                    break;
                case '^':
                    result = pow(a, b);
                    break;
                }
                operands.push(result);
            }
            else {
                return make_pair(0, "�߸��� ������ �Ǵ� ���� ����");
            }
        }

        if (operands.size() != 1) {
            return make_pair(0, "�߸��� ������ �Ǵ� ���� ����");
        }

        return make_pair(operands.top(), "");
    }

    // �Է��� �߸��� �������� �̸� �˻�
    bool isInvalidInput(const string& input) {
        // �� �Է�
        if (input.empty()) {
            return true;
        }

        // ���ӵ� ������ �˻� (**, ++, --, //, ��)
        for (size_t i = 0; i < input.length() - 1; i++) {
            if (isOperator(input[i]) && isOperator(input[i + 1])) {
                // ������ �ƴ� ��츸 ����
                if (!(input[i] == '*' && input[i + 1] == '*')) {
                    // **�� ��� (�ŵ��������� ó������ ������ �׽�Ʈ���̽��� ����)
                }
            }
        }

        return false;
    }

    void run() {
        cout << "=== ���� ����� ===" << endl;
        cout << "���� ǥ�� ������ �Է��ϼ���." << endl;
        cout << "�����ϴ� ������: +, -, *, /, ^(�ŵ�����)" << endl;
        cout << "�����ϴ� ��ȣ: ( ), [ ], { } (��� ������ �켱����)" << endl;
        cout << "����: 3 + 2 * 5, [1 + 2] * 3, 3 * -4, 4^2, 2{3+1}" << endl;
        cout << "�����Ϸ��� 'exit' �Ǵ� 'quit'�� �Է��ϼ���." << endl;
        cout << endl;

        while (true) {
            string input;
            cout << "������ �Է��ϼ���: ";
            getline(cin, input);

            // ���� ����
            if (input == "exit" || input == "quit") {
                cout << "���α׷��� �����մϴ�." << endl;
                break;
            }

            // �� �Է� ó��
            if (input.empty()) {
                cout << "���� ������ �Է��ϼ���." << endl;
                continue;
            }

            // Ư�� ���̽�: **�� ^�� ��ȯ (�ŵ�����)
            string processedInput = input;
            size_t pos = 0;
            while ((pos = processedInput.find("**", pos)) != string::npos) {
                processedInput.replace(pos, 2, "^");
                pos += 1;
            }

            // ������ ������ �ִ��� Ȯ��
            bool hasImplicit = hasImplicitMultiplication(processedInput);

            // ������ ���� ó��
            if (hasImplicit) {
                processedInput = addImplicitMultiplication(processedInput);
            }

            // ���� ǥ����� ���� ǥ������� ��ȯ
            pair<string, string> convertResult = infixToPostfix(processedInput);
            string postfix = convertResult.first;
            string convertError = convertResult.second;

            if (!convertError.empty()) {
                cout << "��ȯ ����: " << convertError << endl;
                cout << "ó���� �Է�: " << processedInput << endl;
                cout << "���� ������ �Է��ϼ���." << endl;
                continue;
            }

            cout << "���� ǥ���: " << postfix << endl;

            // ���� ǥ��� ���
            pair<double, string> evalResult = evaluatePostfix(postfix);
            double result = evalResult.first;
            string evalError = evalResult.second;

            if (!evalError.empty()) {
                cout << "����: " << evalError << endl;
                cout << "���� ������ �Է��ϼ���." << endl;
                continue;
            }

            // ��� ��� (������ ������, �Ǽ��� �Ҽ��� ǥ��)
            if (result == floor(result)) {
                cout << "���: " << static_cast<int>(result) << endl;
            }
            else {
                cout << "���: " << result << endl;
            }

            cout << "���� ������ �Է��ϼ���." << endl;
        }
    }
};

int main() {
    ExpressionCalculator calc;
    calc.run();
    return 0;
}