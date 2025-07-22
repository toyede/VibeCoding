// Compile Command: cl /EHsc /W4 /Fe:calculator.exe main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <cctype>
#include <algorithm>

using namespace std;

// �Լ� ����
string preprocess(const string& expr);
vector<string> infixToPostfix(const string& infix);
double evaluatePostfix(const vector<string>& postfix);
bool isPostfix(const string& expr);
int getPrecedence(const string& op);
double applyOperation(double a, double b, const string& op);
bool isNumber(const string& s);

/**
 * @brief �������� �켱������ ��ȯ�մϴ�.
 * @param op ������ ���ڿ�
 * @return �켱���� �� (�������� �켱������ ����)
 */
int getPrecedence(const string& op) {
    if (op == "^") return 3;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

/**
 * @brief �� �ǿ����ڿ� �����ڸ� �޾� ����� �����մϴ�.
 * @param a ù ��° �ǿ�����
 * @param b �� ��° �ǿ�����
 * @param op ������
 * @return ��� ���
 */
double applyOperation(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("����: 0���� ���� �� �����ϴ�.");
        return a / b;
    }
    if (op == "^") return pow(a, b);
    throw runtime_error("����: �������� �ʴ� ������ '" + op + "'.");
}

/**
 * @brief ǥ������ �Ľ��ϱ� ���� ��ó���մϴ�.
 *        - `**`�� �ŵ����� �������� `^`�� ��ȯ�մϴ�.
 *        - `3(4)`�� `(3)4`�� ���� �Ͻ��� ������ `3*(4)`, `(3)*4` ������ ����� �������� �ٲߴϴ�.
 * @param expr ���� ǥ���� ���ڿ�
 * @return ��ó���� ǥ���� ���ڿ�
 */
string preprocess(const string& expr) {
    string temp = expr;
    size_t pos = 0;
    // `**`�� `^`�� ����
    while ((pos = temp.find("**", pos)) != string::npos) {
        temp.replace(pos, 2, "^");
    }

    string result; //��ȯ�� ���ڿ�
    result.reserve(temp.length() * 2); //���� ���ڿ��� �ι�� ũ�� ����
    for (size_t i = 0; i < temp.length(); ++i) {
        result += temp[i];

        size_t next_i = i + 1;
        while (next_i < temp.length() && isspace(temp[next_i])) {
            next_i++;
        }

        if (next_i < temp.length()) {
            char current_char = temp[i];
            char next_char = temp[next_i];

            // ���� �ڿ� ���� ��ȣ, �ݴ� ��ȣ �ڿ� ����, ��ȣ �ڿ� ��ȣ�� ���� ��� ����(*) �߰�
            if ((isdigit(current_char) && next_char == '(') ||
                (current_char == ')' && isdigit(next_char)) ||
                (current_char == ')' && next_char == '(')) {
                result += '*';
            }
        }
    }
    return result;
}

/**
 * @brief ���� ǥ��� ������ ���� ǥ������� ��ȯ�մϴ�. (��unting-yard �˰��� ���)
 *        ���� ������(+, -)�� ó���ϴ� ����� ���ԵǾ� �ֽ��ϴ�.
 * @param infix ���� ǥ��� ���ڿ�
 * @return ���� ǥ������� ��ȯ�� ��ū ����
 */
vector<string> infixToPostfix(const string& infix) {
    vector<string> postfix;
    stack<string> operators;
    bool is_unary = true; // ���� ��ġ�� ���� �����ڰ� �� �� �ִ��� ����

    for (size_t i = 0; i < infix.length(); ++i) {
        char c = infix[i];

        if (isspace(c)) {
            continue;
        }

        if (isdigit(c) || c == '.') { // ������ ���
            string number;
            while (i < infix.length() && (isdigit(infix[i]) || infix[i] == '.')) {
                number += infix[i];
                i++;
            }
            i--;
            postfix.push_back(number);
            is_unary = false;
        } else if (c == '(') { // ���� ��ȣ
            operators.push("(");
            is_unary = true;
        } else if (c == ')') { // �ݴ� ��ȣ
            while (!operators.empty() && operators.top() != "(") {
                postfix.push_back(operators.top());
                operators.pop();
            }
            if (!operators.empty()) operators.pop(); // '(' ����
            else throw runtime_error("����: ��ȣ ���� ���� �ʽ��ϴ�.");
            is_unary = false;
        } else { // �������� ���
            string op(1, c);
            if (is_unary && (c == '-' || c == '+')) { // ���� ������ ó��
                if (c == '+') { // ���� '+'�� ����
                    is_unary = true;
                    continue;
                }
                // ���� '-' ó��
                string number = "-";
                i++;
                while (i < infix.length() && isspace(infix[i])) i++;
                while (i < infix.length() && (isdigit(infix[i]) || infix[i] == '.')) {
                    number += infix[i];
                    i++;
                }
                i--;
                postfix.push_back(number);
                is_unary = false;
            } else { // ���� ������ ó��
                while (!operators.empty() && operators.top() != "(" && getPrecedence(op) <= getPrecedence(operators.top())) {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(op);
                is_unary = true;
            }
        }
    }

    // ���ÿ� ���� �����ڵ��� ��� ����� �߰�
    while (!operators.empty()) {
        if (operators.top() == "(") throw runtime_error("����: ��ȣ ���� ���� �ʽ��ϴ�.");
        postfix.push_back(operators.top());
        operators.pop();
    }

    return postfix;
}

/**
 * @brief �־��� ���ڿ��� ��ȿ�� �������� Ȯ���մϴ�.
 * @param s Ȯ���� ���ڿ�
 * @return �����̸� true, �ƴϸ� false
 */
bool isNumber(const string& s) {
    if (s.empty()) return false;
    try {
        (void)stod(s); // ���ڿ��� double�� ��ȯ �õ�. ��ȯ���� ��������� �����Ͽ� C4834 ��� �����մϴ�.
        return true;
    } catch (const invalid_argument&) {
        return false;
    } catch (const out_of_range&) {
        return false;
    }
}

/**
 * @brief ���� ǥ������� ǥ���� ������ ����մϴ�.
 * @param postfix ���� ǥ��� ��ū ����
 * @return ��� ���
 */
double evaluatePostfix(const vector<string>& postfix) {
    if (postfix.empty()) {
        throw runtime_error("����: �� ǥ�����Դϴ�.");
    }
    stack<double> values;
    for (const string& token : postfix) {
        if (isNumber(token)) { // ��ū�� �����̸� ���ÿ� push
            values.push(stod(token));
        } else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") { // ��ū�� �������̸�
            if (values.size() < 2) throw runtime_error("����: ǥ���� ������ �߸��Ǿ����ϴ�.");
            double val2 = values.top(); values.pop();
            double val1 = values.top(); values.pop();
            values.push(applyOperation(val1, val2, token)); // ���� ���� �� ����� �ٽ� ���ÿ� push
        } else {
            throw runtime_error("����: ǥ���Ŀ� �߸��� ��ū�� �ֽ��ϴ�: " + token);
        }
    }
    if (values.size() != 1) throw runtime_error("����: ���� ǥ������ �߸��Ǿ����ϴ�.");
    return values.top();
}

/**
 * @brief ǥ������ ���� ǥ������� �����(heuristic)�Ͽ� Ȯ���մϴ�.
 *        �� �Լ��� �Ϻ����� ������, �Ϲ����� ��츦 ������ �����ϰ� �Ǵ��ϱ� ���� ���˴ϴ�.
 * @param expr Ȯ���� ǥ���� ���ڿ�
 * @return ���� ǥ������� �ǴܵǸ� true, �ƴϸ� false
 */
bool isPostfix(const string& expr) {
    // 1. ��ȣ Ȯ��: ���� ǥ������� ��ȣ�� �����ϴ�. ��ȣ�� �ϳ��� ������ ���� ǥ������� �����մϴ�.
    if (expr.find('(') != string::npos || expr.find(')') != string::npos) {
        return false;
    }

    // 2. ��ū �и�: ������ �������� ���ڿ��� ��ū(����, ������)���� �����ϴ�.
    stringstream ss(expr);
    string token;
    vector<string> tokens;
    while (ss >> token) { //stringstreaem�� >>�����ڸ� ����Ͽ� ������ �������� ��ū �и�
        tokens.push_back(token);
    }

    // ���� ǥ����� �ּ� 3���� ��ū(��: 3 4 +)�� �ʿ�������, 2��(��: 3 4)�� ���� ���� �����Ƿ� �ּ� 2���� �˻��մϴ�.
    if (tokens.size() < 2) return false;

    // 3. �ǿ����ڿ� ������ ���� ����
    int num_count = 0;
    int op_count = 0;
    for (const auto& t : tokens) {
        if (isNumber(t)) {
            num_count++;
        } else if (t == "**" || t == "^" || t == "*" || t == "/" || t == "+" || t == "-") {
            op_count++;
        }
    }

    // 4. ���� ǥ����� �ֿ� Ư¡�� �̿��� �޸���ƽ(�����) �˻�
    
    // �޸���ƽ 1: "������ ��ū�� ������"�̰� "�ǿ����� �� = ������ �� + 1" ��Ģ�� �����ϴ°�?
    // �̴� ���� ǥ����� ���� ������ Ư¡ �� �ϳ��Դϴ�. (��: 3 4 + -> �ǿ����� 2��, ������ 1��)
    const string& last_token = tokens.back();
    bool last_is_op = (last_token == "**" || last_token == "^" || last_token == "*" || last_token == "/" || last_token == "+" || last_token == "-");

    if (last_is_op && num_count == op_count + 1) {
        return true;
    }
    
    // �޸���ƽ 2: "ó�� �� ��ū�� ��� �����ΰ�?"
    // ���� ǥ����� ���� '���� ������ ����' ����������, ���� ǥ����� '���� ���� ������' ������ �����մϴ�.
    // ���� '3 4 +' �� ���� ó�� �� ���� �����ؼ� ���ڰ� ������ ���� ǥ����� ���ɼ��� �����ϴ�.
    if (tokens.size() >= 3 && isNumber(tokens[0]) && isNumber(tokens[1])) {
        return true;
    }

    // ���� �޸���ƽ �˻縦 ��� ������� ���ϸ� ���� ǥ������� �����մϴ�.
    return false;
}


/**
 * @brief ���� �Լ�. input.txt ���Ͽ��� �� �پ� ������ �о�ɴϴ�.
 *        ������ ���� ǥ������� ���� ǥ������� �Ǵ��Ͽ� ������ �´� ó���� �� ��,
 *        ��� ����� ����մϴ�.
 */
int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "����: input.txt ������ �� �� �����ϴ�." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) { //���� ���� ���� ����
        if (line.empty() || line.find_first_not_of(" \t\n\r") == string::npos) continue; //�� ���̸� ���� �ٷ�

        cout << "�Է�: " << line << endl; //�Է¹��� ���� ���
        try {
            double result;
            if (isPostfix(line)) { // ���� ǥ��� ó��, ���� ǥ��� ���� �������� ����
                stringstream ss(line);
                string token;
                vector<string> tokens;
                while (ss >> token) {
                    if (token == "**") token = "^"; // ���� ǥ����� **�� ^�� ��ȯ
                    tokens.push_back(token);
                }
                result = evaluatePostfix(tokens); //���� ���� ����
            } else { // ���� ǥ��� ó��
                string processed_line = preprocess(line);
                vector<string> postfix = infixToPostfix(processed_line);
                result = evaluatePostfix(postfix);
            }
            
            // ��� ����� ������ �ſ� ������ ������ ���
            if (abs(result - round(result)) < 1e-9) {
                 cout << "���: " << static_cast<long long>(round(result)) << endl;
            } else {
                 cout << "���: " << result << endl;
            }
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
        cout << "--------------------" << endl;
    }

    inputFile.close();
    return 0;
}
