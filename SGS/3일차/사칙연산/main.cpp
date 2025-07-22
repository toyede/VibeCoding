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

// 함수 선언
string preprocess(const string& expr);
vector<string> infixToPostfix(const string& infix);
double evaluatePostfix(const vector<string>& postfix);
bool isPostfix(const string& expr);
int getPrecedence(const string& op);
double applyOperation(double a, double b, const string& op);
bool isNumber(const string& s);

/**
 * @brief 연산자의 우선순위를 반환합니다.
 * @param op 연산자 문자열
 * @return 우선순위 값 (높을수록 우선순위가 높음)
 */
int getPrecedence(const string& op) {
    if (op == "^") return 3;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

/**
 * @brief 두 피연산자와 연산자를 받아 계산을 수행합니다.
 * @param a 첫 번째 피연산자
 * @param b 두 번째 피연산자
 * @param op 연산자
 * @return 계산 결과
 */
double applyOperation(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("오류: 0으로 나눌 수 없습니다.");
        return a / b;
    }
    if (op == "^") return pow(a, b);
    throw runtime_error("오류: 지원하지 않는 연산자 '" + op + "'.");
}

/**
 * @brief 표현식을 파싱하기 쉽게 전처리합니다.
 *        - `**`를 거듭제곱 연산자인 `^`로 변환합니다.
 *        - `3(4)`나 `(3)4`와 같은 암시적 곱셈을 `3*(4)`, `(3)*4` 형태의 명시적 곱셈으로 바꿉니다.
 * @param expr 원본 표현식 문자열
 * @return 전처리된 표현식 문자열
 */
string preprocess(const string& expr) {
    string temp = expr;
    size_t pos = 0;
    // `**`를 `^`로 변경
    while ((pos = temp.find("**", pos)) != string::npos) {
        temp.replace(pos, 2, "^");
    }

    string result; //반환할 문자열
    result.reserve(temp.length() * 2); //기존 문자열의 두배로 크기 지정
    for (size_t i = 0; i < temp.length(); ++i) {
        result += temp[i];

        size_t next_i = i + 1;
        while (next_i < temp.length() && isspace(temp[next_i])) {
            next_i++;
        }

        if (next_i < temp.length()) {
            char current_char = temp[i];
            char next_char = temp[next_i];

            // 숫자 뒤에 여는 괄호, 닫는 괄호 뒤에 숫자, 괄호 뒤에 괄호가 오는 경우 곱셈(*) 추가
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
 * @brief 중위 표기법 수식을 후위 표기법으로 변환합니다. (샤unting-yard 알고리즘 기반)
 *        단항 연산자(+, -)를 처리하는 기능이 포함되어 있습니다.
 * @param infix 중위 표기법 문자열
 * @return 후위 표기법으로 변환된 토큰 벡터
 */
vector<string> infixToPostfix(const string& infix) {
    vector<string> postfix;
    stack<string> operators;
    bool is_unary = true; // 현재 위치에 단항 연산자가 올 수 있는지 여부

    for (size_t i = 0; i < infix.length(); ++i) {
        char c = infix[i];

        if (isspace(c)) {
            continue;
        }

        if (isdigit(c) || c == '.') { // 숫자인 경우
            string number;
            while (i < infix.length() && (isdigit(infix[i]) || infix[i] == '.')) {
                number += infix[i];
                i++;
            }
            i--;
            postfix.push_back(number);
            is_unary = false;
        } else if (c == '(') { // 여는 괄호
            operators.push("(");
            is_unary = true;
        } else if (c == ')') { // 닫는 괄호
            while (!operators.empty() && operators.top() != "(") {
                postfix.push_back(operators.top());
                operators.pop();
            }
            if (!operators.empty()) operators.pop(); // '(' 제거
            else throw runtime_error("오류: 괄호 쌍이 맞지 않습니다.");
            is_unary = false;
        } else { // 연산자인 경우
            string op(1, c);
            if (is_unary && (c == '-' || c == '+')) { // 단항 연산자 처리
                if (c == '+') { // 단항 '+'는 무시
                    is_unary = true;
                    continue;
                }
                // 단항 '-' 처리
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
            } else { // 이항 연산자 처리
                while (!operators.empty() && operators.top() != "(" && getPrecedence(op) <= getPrecedence(operators.top())) {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(op);
                is_unary = true;
            }
        }
    }

    // 스택에 남은 연산자들을 모두 결과에 추가
    while (!operators.empty()) {
        if (operators.top() == "(") throw runtime_error("오류: 괄호 쌍이 맞지 않습니다.");
        postfix.push_back(operators.top());
        operators.pop();
    }

    return postfix;
}

/**
 * @brief 주어진 문자열이 유효한 숫자인지 확인합니다.
 * @param s 확인할 문자열
 * @return 숫자이면 true, 아니면 false
 */
bool isNumber(const string& s) {
    if (s.empty()) return false;
    try {
        (void)stod(s); // 문자열을 double로 변환 시도. 반환값을 명시적으로 무시하여 C4834 경고를 방지합니다.
        return true;
    } catch (const invalid_argument&) {
        return false;
    } catch (const out_of_range&) {
        return false;
    }
}

/**
 * @brief 후위 표기법으로 표현된 수식을 계산합니다.
 * @param postfix 후위 표기법 토큰 벡터
 * @return 계산 결과
 */
double evaluatePostfix(const vector<string>& postfix) {
    if (postfix.empty()) {
        throw runtime_error("오류: 빈 표현식입니다.");
    }
    stack<double> values;
    for (const string& token : postfix) {
        if (isNumber(token)) { // 토큰이 숫자이면 스택에 push
            values.push(stod(token));
        } else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") { // 토큰이 연산자이면
            if (values.size() < 2) throw runtime_error("오류: 표현식 구조가 잘못되었습니다.");
            double val2 = values.top(); values.pop();
            double val1 = values.top(); values.pop();
            values.push(applyOperation(val1, val2, token)); // 연산 수행 후 결과를 다시 스택에 push
        } else {
            throw runtime_error("오류: 표현식에 잘못된 토큰이 있습니다: " + token);
        }
    }
    if (values.size() != 1) throw runtime_error("오류: 최종 표현식이 잘못되었습니다.");
    return values.top();
}

/**
 * @brief 표현식이 후위 표기법인지 어림짐작(heuristic)하여 확인합니다.
 *        이 함수는 완벽하지 않지만, 일반적인 경우를 빠르고 간단하게 판단하기 위해 사용됩니다.
 * @param expr 확인할 표현식 문자열
 * @return 후위 표기법으로 판단되면 true, 아니면 false
 */
bool isPostfix(const string& expr) {
    // 1. 괄호 확인: 후위 표기법에는 괄호가 없습니다. 괄호가 하나라도 있으면 중위 표기법으로 간주합니다.
    if (expr.find('(') != string::npos || expr.find(')') != string::npos) {
        return false;
    }

    // 2. 토큰 분리: 공백을 기준으로 문자열을 토큰(숫자, 연산자)으로 나눕니다.
    stringstream ss(expr);
    string token;
    vector<string> tokens;
    while (ss >> token) { //stringstreaem의 >>연산자를 사용하여 공백을 기준으로 토큰 분리
        tokens.push_back(token);
    }

    // 후위 표기법은 최소 3개의 토큰(예: 3 4 +)이 필요하지만, 2개(예: 3 4)만 들어올 수도 있으므로 최소 2개로 검사합니다.
    if (tokens.size() < 2) return false;

    // 3. 피연산자와 연산자 개수 세기
    int num_count = 0;
    int op_count = 0;
    for (const auto& t : tokens) {
        if (isNumber(t)) {
            num_count++;
        } else if (t == "**" || t == "^" || t == "*" || t == "/" || t == "+" || t == "-") {
            op_count++;
        }
    }

    // 4. 후위 표기법의 주요 특징을 이용한 휴리스틱(어림짐작) 검사
    
    // 휴리스틱 1: "마지막 토큰은 연산자"이고 "피연산자 수 = 연산자 수 + 1" 규칙을 만족하는가?
    // 이는 후위 표기법의 가장 강력한 특징 중 하나입니다. (예: 3 4 + -> 피연산자 2개, 연산자 1개)
    const string& last_token = tokens.back();
    bool last_is_op = (last_token == "**" || last_token == "^" || last_token == "*" || last_token == "/" || last_token == "+" || last_token == "-");

    if (last_is_op && num_count == op_count + 1) {
        return true;
    }
    
    // 휴리스틱 2: "처음 두 토큰이 모두 숫자인가?"
    // 중위 표기법은 보통 '숫자 연산자 숫자' 순서이지만, 후위 표기법은 '숫자 숫자 연산자' 순서로 시작합니다.
    // 따라서 '3 4 +' 와 같이 처음 두 개가 연속해서 숫자가 나오면 후위 표기법일 가능성이 높습니다.
    if (tokens.size() >= 3 && isNumber(tokens[0]) && isNumber(tokens[1])) {
        return true;
    }

    // 위의 휴리스틱 검사를 모두 통과하지 못하면 중위 표기법으로 간주합니다.
    return false;
}


/**
 * @brief 메인 함수. input.txt 파일에서 한 줄씩 수식을 읽어옵니다.
 *        수식이 중위 표기법인지 후위 표기법인지 판단하여 각각에 맞는 처리를 한 후,
 *        계산 결과를 출력합니다.
 */
int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "오류: input.txt 파일을 열 수 없습니다." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) { //여러 줄의 수식 대응
        if (line.empty() || line.find_first_not_of(" \t\n\r") == string::npos) continue; //빈 줄이면 다음 줄로

        cout << "입력: " << line << endl; //입력받은 수식 출력
        try {
            double result;
            if (isPostfix(line)) { // 후위 표기법 처리, 후위 표기면 후위 연산으로 진행
                stringstream ss(line);
                string token;
                vector<string> tokens;
                while (ss >> token) {
                    if (token == "**") token = "^"; // 후위 표기법의 **도 ^로 변환
                    tokens.push_back(token);
                }
                result = evaluatePostfix(tokens); //후위 연산 진행
            } else { // 중위 표기법 처리
                string processed_line = preprocess(line);
                vector<string> postfix = infixToPostfix(processed_line);
                result = evaluatePostfix(postfix);
            }
            
            // 계산 결과가 정수에 매우 가까우면 정수로 출력
            if (abs(result - round(result)) < 1e-9) {
                 cout << "결과: " << static_cast<long long>(round(result)) << endl;
            } else {
                 cout << "결과: " << result << endl;
            }
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
        cout << "--------------------" << endl;
    }

    inputFile.close();
    return 0;
}
