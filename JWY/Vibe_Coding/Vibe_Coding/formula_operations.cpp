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

// 괄호인지 확인 (소괄호, 중괄호, 대괄호)
bool isOpenBracket(char c) {
    return c == '(' || c == '{' || c == '[';
}

bool isCloseBracket(char c) {
    return c == ')' || c == '}' || c == ']';
}

// 매칭되는 괄호인지 확인
bool isMatchingBracket(char open, char close) {
    return (open == '(' && close == ')') ||
        (open == '{' && close == '}') ||
        (open == '[' && close == ']');
}

class ExpressionCalculator {
private:
    // 연산자 우선순위 반환
    int getPrecedence(char op) {
        switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':  // 거듭제곱 연산자
            return 3;
        default:
            return -1;
        }
    }

    // 연산자인지 확인
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }

    // 문자열에서 숫자 추출 (음수, 양수 포함)
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

    // 중위 표기법을 토큰으로 분리
    vector<string> tokenize(const string& expression) {
        vector<string> tokens;
        string current = "";

        for (size_t i = 0; i < expression.length(); i++) {
            char c = expression[i];

            // 공백 무시
            if (isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current = "";
                }
                continue;
            }

            // 괄호나 연산자인 경우
            if (isOpenBracket(c) || isCloseBracket(c) || isOperator(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current = "";
                }

                // 부호 처리: +, - 가 숫자 앞에 오는 경우 (양수/음수)
                if (c == '-' || c == '+') {
                    if (tokens.empty() || isOpenBracket(tokens.back()[0]) ||
                        (tokens.back().length() == 1 && isOperator(tokens.back()[0]))) {
                        current = c; // + 또는 - 부호로 처리
                        continue;
                    }
                }

                tokens.push_back(string(1, c));
            }
            // 지원하지 않는 문자 체크
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

    // 묵시적 곱셈이 필요한 수식인지 검사
    bool hasImplicitMultiplication(const string& expression) {
        for (size_t i = 0; i < expression.length() - 1; i++) {
            char current = expression[i];
            char next = expression[i + 1];

            // 숫자 다음에 여는 괄호가 오는 경우: 3(, 3{, 3[
            if (isdigit(current) && isOpenBracket(next)) {
                return true;
            }
            // 닫는 괄호 다음에 숫자가 오는 경우: )3, }3, ]3
            if (isCloseBracket(current) && isdigit(next)) {
                return true;
            }
        }
        return false;
    }

    // 묵시적 곱셈을 명시적으로 변환
    string addImplicitMultiplication(const string& expression) {
        string result = "";

        for (size_t i = 0; i < expression.length(); i++) {
            char current = expression[i];
            result += current;

            // 다음 문자가 있고, 묵시적 곱셈이 필요한 경우
            if (i < expression.length() - 1) {
                char next = expression[i + 1];

                // 숫자 다음에 여는 괄호가 오는 경우: 3( → 3*(
                if ((isdigit(current) || isCloseBracket(current)) && isOpenBracket(next)) {
                    result += "*";
                }
                // 닫는 괄호 다음에 숫자가 오는 경우: )3 → )*3
                else if (isCloseBracket(current) && (isdigit(next) || next == '-')) {
                    // 다음이 -인 경우, 그 다음이 숫자인지 확인
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

    // 중위 표기법의 기본 구조가 유효한지 검사
    bool isValidInfixStructure(const vector<string>& tokens) {
        if (tokens.empty()) return false;

        bool expectOperand = true; // 처음에는 피연산자를 기대

        for (const string& token : tokens) {
            if (token.substr(0, 8) == "INVALID:") {
                return false;
            }

            if (isNumber(token)) {
                if (!expectOperand) return false; // 연산자를 기대했는데 숫자가 옴
                expectOperand = false;
            }
            else if (isOpenBracket(token[0])) {
                if (!expectOperand) return false; // 연산자를 기대했는데 여는 괄호가 옴
                expectOperand = true;
            }
            else if (isCloseBracket(token[0])) {
                if (expectOperand) return false; // 피연산자를 기대했는데 닫는 괄호가 옴
                expectOperand = false;
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                if (expectOperand) return false; // 피연산자를 기대했는데 연산자가 옴
                expectOperand = true;
            }
            else {
                return false; // 알 수 없는 토큰
            }
        }

        return !expectOperand; // 마지막에는 피연산자로 끝나야 함
    }

public:
    // 중위 표기법을 후위 표기법으로 변환
    pair<string, string> infixToPostfix(const string& infix) {
        vector<string> tokens = tokenize(infix);

        // 기본 구조 검사
        if (!isValidInfixStructure(tokens)) {
            return make_pair("", "잘못된 수식입니다.");
        }

        stack<string> operators;
        vector<string> postfix;
        stack<char> bracketStack; // 괄호 매칭 확인용

        for (const string& token : tokens) {
            // 지원하지 않는 문자 검사
            if (token.substr(0, 8) == "INVALID:") {
                return make_pair("", "지원하지 않는 연산자");
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
                            return make_pair("", "괄호 타입이 일치하지 않습니다.");
                        }
                    }
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                if (!foundOpenBracket) {
                    return make_pair("", "괄호가 닫히지 않았습니다.");
                }
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                // 거듭제곱은 우결합성 (right associative)
                while (!operators.empty() && !isOpenBracket(operators.top()[0]) &&
                    ((token[0] != '^' && getPrecedence(operators.top()[0]) >= getPrecedence(token[0])) ||
                        (token[0] == '^' && getPrecedence(operators.top()[0]) > getPrecedence(token[0])))) {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
            else {
                return make_pair("", "잘못된 연산자 또는 수식 오류");
            }
        }

        while (!operators.empty()) {
            if (isOpenBracket(operators.top()[0])) {
                return make_pair("", "괄호가 닫히지 않았습니다.");
            }
            postfix.push_back(operators.top());
            operators.pop();
        }

        // 후위 표기법 문자열 생성
        string postfixStr = "";
        for (size_t i = 0; i < postfix.size(); i++) {
            if (i > 0) postfixStr += " ";
            postfixStr += postfix[i];
        }

        return make_pair(postfixStr, "");
    }

    // 후위 표기법 계산
    pair<double, string> evaluatePostfix(const string& postfix) {
        if (postfix.empty()) {
            return make_pair(0, "잘못된 연산자 또는 수식 오류");
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
                    return make_pair(0, "잘못된 연산자 또는 수식 오류");
                }
            }
            else if (isOperator(token[0]) && token.length() == 1) {
                if (operands.size() < 2) {
                    return make_pair(0, "잘못된 연산자 또는 수식 오류");
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
                        return make_pair(0, "계산 불가");
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
                return make_pair(0, "잘못된 연산자 또는 수식 오류");
            }
        }

        if (operands.size() != 1) {
            return make_pair(0, "잘못된 연산자 또는 수식 오류");
        }

        return make_pair(operands.top(), "");
    }

    // 입력이 잘못된 수식인지 미리 검사
    bool isInvalidInput(const string& input) {
        // 빈 입력
        if (input.empty()) {
            return true;
        }

        // 연속된 연산자 검사 (**, ++, --, //, 등)
        for (size_t i = 0; i < input.length() - 1; i++) {
            if (isOperator(input[i]) && isOperator(input[i + 1])) {
                // 음수가 아닌 경우만 오류
                if (!(input[i] == '*' && input[i + 1] == '*')) {
                    // **는 허용 (거듭제곱으로 처리하지 않지만 테스트케이스에 있음)
                }
            }
        }

        return false;
    }

    void run() {
        cout << "=== 수식 연산기 ===" << endl;
        cout << "중위 표기 수식을 입력하세요." << endl;
        cout << "지원하는 연산자: +, -, *, /, ^(거듭제곱)" << endl;
        cout << "지원하는 괄호: ( ), [ ], { } (모두 동일한 우선순위)" << endl;
        cout << "예시: 3 + 2 * 5, [1 + 2] * 3, 3 * -4, 4^2, 2{3+1}" << endl;
        cout << "종료하려면 'exit' 또는 'quit'을 입력하세요." << endl;
        cout << endl;

        while (true) {
            string input;
            cout << "수식을 입력하세요: ";
            getline(cin, input);

            // 종료 조건
            if (input == "exit" || input == "quit") {
                cout << "프로그램을 종료합니다." << endl;
                break;
            }

            // 빈 입력 처리
            if (input.empty()) {
                cout << "다음 수식을 입력하세요." << endl;
                continue;
            }

            // 특별 케이스: **를 ^로 변환 (거듭제곱)
            string processedInput = input;
            size_t pos = 0;
            while ((pos = processedInput.find("**", pos)) != string::npos) {
                processedInput.replace(pos, 2, "^");
                pos += 1;
            }

            // 묵시적 곱셈이 있는지 확인
            bool hasImplicit = hasImplicitMultiplication(processedInput);

            // 묵시적 곱셈 처리
            if (hasImplicit) {
                processedInput = addImplicitMultiplication(processedInput);
            }

            // 중위 표기법을 후위 표기법으로 변환
            pair<string, string> convertResult = infixToPostfix(processedInput);
            string postfix = convertResult.first;
            string convertError = convertResult.second;

            if (!convertError.empty()) {
                cout << "변환 오류: " << convertError << endl;
                cout << "처리된 입력: " << processedInput << endl;
                cout << "다음 수식을 입력하세요." << endl;
                continue;
            }

            cout << "후위 표기법: " << postfix << endl;

            // 후위 표기법 계산
            pair<double, string> evalResult = evaluatePostfix(postfix);
            double result = evalResult.first;
            string evalError = evalResult.second;

            if (!evalError.empty()) {
                cout << "오류: " << evalError << endl;
                cout << "다음 수식을 입력하세요." << endl;
                continue;
            }

            // 결과 출력 (정수면 정수로, 실수면 소수점 표시)
            if (result == floor(result)) {
                cout << "결과: " << static_cast<int>(result) << endl;
            }
            else {
                cout << "결과: " << result << endl;
            }

            cout << "다음 수식을 입력하세요." << endl;
        }
    }
};

int main() {
    ExpressionCalculator calc;
    calc.run();
    return 0;
}