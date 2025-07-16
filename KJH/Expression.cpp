#include <iostream> // 표준 입출력 스트림 객체(예: std::cout, std::cin)를 제공합니다.
#include <string>   // 문자열 작업을 위한 std::string 클래스를 제공합니다.
#include <stack>    // LIFO 자료 구조인 std::stack 컨테이너를 제공합니다. 표현식 변환 및 평가에 사용됩니다.
#include <sstream>  // 인-메모리 문자열 스트림 조작을 위한 std::stringstream을 제공합니다. 문자열을 구문 분석하거나 결합하는 데 사용됩니다.
#include <stdexcept>// 표준 예외 클래스(예: std::runtime_error)를 제공합니다. 실행 중 발생하는 오류를 처리하는 데 사용됩니다.
#include <cctype>   // 문자 처리 함수(예: isdigit, isspace, isalpha)를 제공합니다. 문자를 분류하는 데 사용됩니다.
#include <cmath>    // 수학 함수(예: pow, log10)를 제공합니다. 지수 및 로그 계산에 사용됩니다.

// 연산자의 우선순위를 반환하는 함수입니다.
// 이 함수는 중위 표기식을 후위 표기식으로 변환할 때 연산 순서를 결정합니다.
int getPrecedence(char op)
{
    // 덧셈과 뺄셈은 가장 낮은 우선순위(1)를 가집니다.
    if (op == '+' || op == '-')
    {
        return 1;
    }
    // 곱셈과 나눗셈은 중간 우선순위(2)를 가집니다.
    if (op == '*' || op == '/')
    {
        return 2;
    }
    // 거듭제곱(^)은 가장 높은 우선순위(3)를 가집니다.
    if (op == '^')
    {
        return 3;
    }
    // 다른 모든 문자의 경우 연산자로 간주되지 않으므로 0을 반환합니다.
    return 0;
}

// 두 피연산자(a, b)와 연산자(op)를 받아 계산 결과를 반환하는 함수입니다.
// 이 함수는 후위 표기식을 평가할 때 실제 계산을 수행하는 데 사용됩니다.
double applyOp(double a, double b, char op)
{
    // 연산자 유형에 따라 계산을 수행합니다.
    switch (op)
    {
        case '+': return a + b; // 덧셈
        case '-': return a - b; // 뺄셈
        case '*': return a * b; // 곱셈
        case '/':
            // 0으로 나누는 오류를 방지합니다.
            // 0으로 나누려고 하면 runtime_error를 발생시킵니다.
            if (b == 0)
            {
                throw std::runtime_error("오류: 0으로 나눌 수 없습니다.");
            }
            return a / b; // 나눗셈
        case '^': return pow(a, b); // 거듭제곱 (a의 b 제곱). cmath 라이브러리의 pow 함수를 사용합니다.
    }
    return 0; // 연산자가 인식되지 않으면 0을 반환합니다 (실제로는 오류 처리로 걸러집니다).
}

// 중위 표기식 문자열(예: "2 + 3 * 4")을 후위 표기식 문자열(예: "2 3 4 * +")로 변환합니다.
// 이 변환은 컴퓨터가 표현식을 더 쉽게 평가할 수 있도록 만드는 중요한 단계입니다.
// 숫자와 연산자를 처리하며, 숫자는 출력 스트림에 직접 추가합니다.
std::string infixToPostfix(const std::string& infix)
{
    std::stack<char> ops; // 연산자를 저장하는 스택입니다.
    std::stringstream postfix; // 결과 후위 표기식을 만드는 문자열 스트림입니다.
    bool expectOperand = true; // 다음에 피연산자(true) 또는 연산자(false)가 올 것으로 예상되는지를 나타내는 플래그입니다.

    // 입력된 중위 표기식의 각 문자를 처리합니다.
    for (int i = 0; i < infix.length(); ++i)
    {
        char token = infix[i]; // 현재 처리 중인 문자(토큰)입니다.

        // 공백 문자를 무시하고 다음 문자로 이동합니다.
        if (isspace(token))
        {
            continue;
        }

        // 현재 토큰이 숫자이거나 소수점인 경우 (숫자의 일부).
        if (isdigit(token) || (token == '.'))
        {
            // 이 시점에서 피연산자가 예상되지 않으면 오류입니다 (예: "3 4" 또는 "3 + 4 5").
            if (!expectOperand)
            {
                throw std::runtime_error("오류: 피연산자 뒤에 연산자가 와야 합니다.");
            }
            std::string numStr; // 숫자를 저장할 문자열입니다.
            // 현재 위치에서 연속된 모든 숫자(및 소수점)를 읽어 숫자 문자열을 만듭니다.
            while (i < infix.length() && (isdigit(infix[i]) || infix[i] == '.'))
            {
                numStr += infix[i];
                i++;
            }
            i--; // for 루프의 i++가 한 번 더 진행되므로 i를 감소시킵니다.
            postfix << numStr << " "; // 숫자 뒤에 공백을 추가하여 후위 표기식 스트림에 추가합니다.
            expectOperand = false; // 피연산자 뒤에는 연산자가 와야 합니다.
        }
        // 현재 토큰이 알파벳 문자인 경우 (함수의 일부, 여기서는 'log' 처리).
        else if (isalpha(token))
        {
            // 'log' 함수를 확인합니다 (현재 위치에서 3개의 문자).
            if (i + 2 < infix.length() && infix.substr(i, 3) == "log")
            {
                // 피연산자가 예상되지 않으면 오류입니다 (예: "3 log").
                if (!expectOperand)
                {
                    throw std::runtime_error("오류: 함수 앞에 연산자가 와야 합니다.");
                }
                ops.push('l'); // 'log'에 대한 간단한 식별자로 'l'을 푸시합니다.
                i += 2; // 'o'와 'g'를 건너뛰고 다음 토큰으로 이동합니다.
            }
            else
            {
                // 다른 알파벳 문자이거나 완전한 'log' 함수가 아닌 경우.
                throw std::runtime_error("오류: 표현식에 잘못된 문자가 있습니다.");
            }
        }
        // 현재 토큰이 여는 괄호인 경우 ((, {, [).
        else if (token == '(' || token == '{' || token == '[')
        {
            // 피연산자가 예상되지 않으면 암시적 곱셈을 의미합니다 (예: "3(" 또는 ")(").
            if (!expectOperand)
            {
                 // '*' 연산자를 삽입하여 암시적 곱셈을 처리합니다.
                 while (!ops.empty() && ops.top() != '(' && ops.top() != '{' && ops.top() != '[' && getPrecedence(ops.top()) >= getPrecedence('*'))
                 {
                     postfix << ops.top() << " ";
                     ops.pop();
                 }
                 ops.push('*');
            }
            ops.push(token); // 항상 여는 괄호를 스택에 푸시합니다.
            expectOperand = true; // 여는 괄호 뒤에는 피연산자가 와야 합니다.
        }
        // 현재 토큰이 닫는 괄호인 경우 (), }, ]).
        else if (token == ')' || token == '}' || token == ']')
        {
            // 피연산자가 예상되면 오류입니다 (예: "3 + )").
            if (expectOperand)
            {
                throw std::runtime_error("오류: 닫는 괄호 앞에 피연산자가 와야 합니다.");
            }
            // 현재 닫는 괄호에 맞는 여는 괄호를 찾습니다.
            char openParen = (token == ')') ? '(' : ((token == '}') ? '{' : '[');
            // 일치하는 여는 괄호를 찾을 때까지 스택에서 모든 연산자를 팝하여 후위 표기식 스트림으로 이동합니다.
            while (!ops.empty() && ops.top() != '(' && ops.top() != '{' && ops.top() != '[')
            {
                postfix << ops.top() << " ";
                ops.pop();
            }
            // 스택이 비어 있거나 스택의 맨 위가 일치하는 여는 괄호가 아니면 불일치 오류입니다.
            // (예: "(]" 또는 "{)")
            if (ops.empty() || ops.top() != openParen)
            {
                throw std::runtime_error("오류: 괄호가 일치하지 않습니다.");
            }
            ops.pop(); // 스택에서 일치하는 여는 괄호를 제거합니다.
            // 괄호 앞에 'log' 함수가 있었으면 'log' 연산자를 후위 표기식에 추가합니다.
            if (!ops.empty() && ops.top() == 'l')
            {
                postfix << ops.top() << " ";
                ops.pop();
            }
            expectOperand = false; // 닫는 괄호 뒤에는 연산자가 와야 합니다.
        }
        // 현재 토큰이 연산자인 경우 (+, -, *, /, ^).
        else if (token == '+' || token == '-' || token == '*' || token == '/' || token == '^')
        {
            // 피연산자가 예상되면 오류입니다.
            if (expectOperand)
            {
                 // 단항 연산자를 처리합니다 (예: "-3 + 4" 또는 "+3 - 4").
                 // '-' 또는 '+'가 표현식의 시작, 여는 괄호 뒤 또는 공백 뒤에 나타나면 단항 연산자로 간주됩니다.
                if ((token == '-' || token == '+') && (i == 0 || infix[i-1] == '(' || isspace(infix[i-1])))
                {
                    // 단항 '-' 또는 '+'를 처리하기 위해 '0'을 앞에 추가합니다 (예: -3 -> 0 - 3, +3 -> 0 + 3).
                    postfix << "0 ";
                }
                else
                {
                    throw std::runtime_error("오류: 연산자 앞에 피연산자가 와야 합니다.");
                }
            }

            // '**'를 '^'로 변환합니다.
            if (token == '*' && i + 1 < infix.length() && infix[i+1] == '*') {
                token = '^';
                i++;
            }

            // 스택이 비어 있지 않고, 맨 위가 여는 괄호가 아니며,
            // 스택 맨 위의 연산자 우선순위가 현재 연산자의 우선순위보다 크거나 같으면,
            // 스택에서 연산자를 팝하여 후위 표기식 스트림으로 이동합니다.
            // 이것은 연산자 우선순위 규칙에 기반합니다.
            while (!ops.empty() && ops.top() != '(' && ops.top() != '{' && ops.top() != '[' && getPrecedence(ops.top()) >= getPrecedence(token))
            {
                postfix << ops.top() << " ";
                ops.pop();
            }
            ops.push(token); // 현재 연산자를 스택에 푸시합니다.
            expectOperand = true; // 연산자 뒤에는 피연산자가 와야 합니다.
        }
        else
        {
            // 인식할 수 없는 문자가 발견되면 오류를 발생시킵니다.
            throw std::runtime_error("오류: 표현식에 잘못된 문자가 있습니다.");
        }
    }

    // 전체 표현식을 처리한 후 피연산자가 예상되면 오류입니다 (예: "3 +").
    if (expectOperand)
    {
        throw std::runtime_error("오류: 표현식이 연산자로 끝났습니다.");
    }

    // 스택에 남아있는 모든 연산자를 후위 표기식 스트림으로 이동합니다.
    while (!ops.empty())
    {
        // 스택에 여는 괄호가 남아 있으면 괄호 불일치 오류입니다.
        if (ops.top() == '(' || ops.top() == '{' || ops.top() == '[')
        {
            throw std::runtime_error("오류: 괄호가 일치하지 않습니다.");
        }
        postfix << ops.top() << " ";
        ops.pop();
    }

    return postfix.str(); // 완성된 후위 표기식 문자열을 반환합니다.
}

// 후위 표기식 문자열(예: "2 3 4 * +")을 평가하고 결과를 반환합니다.
// 이 함수는 숫자와 연산자를 처리하며, 스택을 사용하여 계산을 수행합니다. 연산자를 만나면 스택에서 상위 두 값을 가져와 결과를 계산합니다.
double evaluatePostfix(const std::string& postfix)
{
    std::stack<double> values; // 숫자 값을 저장하는 스택입니다.
    std::stringstream ss(postfix); // 후위 표기식 문자열에서 읽기 위한 문자열 스트림입니다.
    std::string token; // 스트림에서 읽은 각 토큰(숫자 또는 연산자)을 저장하는 문자열입니다.

    // 후위 표기식의 각 토큰을 처리합니다.
    while (ss >> token)
    {
        // 토큰이 숫자인 경우 (숫자 또는 소수점으로 시작).
        if (isdigit(token[0]) || (token.length() > 1 && token[0] == '.'))
        {
            values.push(stod(token)); // 문자열을 double로 변환하여 스택에 푸시합니다 (stod는 string to double).
        }
        // 토큰이 'l'인 경우 (로그 연산).
        else if (token == "l")
        {
            // 로그 연산을 수행하려면 스택이 비어 있지 않아야 합니다.
            if (values.empty())
            {
                throw std::runtime_error("오류: 로그 연산에 필요한 피연산자가 충분하지 않습니다.");
            }
            double val = values.top(); // 스택에서 최상위 값을 가져옵니다.
            values.pop(); // 스택에서 값을 제거합니다.
            // 로그의 인수는 양수여야 합니다.
            if (val <= 0)
            {
                throw std::runtime_error("오류: 음수 또는 0의 로그는 정의되지 않았습니다.");
            }
            values.push(log10(val)); // log10 함수를 사용하여 밑이 10인 로그를 계산하고 결과를 스택에 푸시합니다.
        }
        // 토큰이 연산자인 경우.
        else
        {
            // 연산을 수행하려면 스택에 최소 두 개의 피연산자가 필요합니다.
            if (values.size() < 2)
            {
                throw std::runtime_error("오류: 잘못된 표현식입니다 (피연산자가 충분하지 않음).");
            }
            double val2 = values.top(); // 스택에서 두 번째 피연산자를 가져옵니다.
            values.pop(); // 스택에서 두 번째 피연산자를 제거합니다.
            double val1 = values.top(); // 스택에서 첫 번째 피연산자를 가져옵니다.
            values.pop(); // 스택에서 첫 번째 피연산자를 제거합니다.
            values.push(applyOp(val1, val2, token[0])); // applyOp 함수를 사용하여 결과를 계산하고 스택에 푸시합니다.
        }
    }

    // 모든 토큰을 처리한 후 스택에 최종 결과만 남아 있어야 합니다.
    if (values.size() != 1)
    {
        throw std::runtime_error("오류: 잘못된 표현식입니다 (피연산자가 너무 많음).");
    }

    return values.top(); // 최종 계산 결과를 반환합니다.
}

// 프로그램이 시작되는 main 함수입니다.
int main()
{
    std::string infixExpression; // 사용자가 입력한 중위 표기식을 저장하는 문자열 변수입니다.

    // 사용자가 'exit'를 입력할 때까지 계산 과정을 반복합니다.
    while (true)
    {
        // 사용자에게 중위 표기식 입력을 요청합니다.
        std::cout << "중위 표기식을 입력하세요 (종료하려면 'exit' 입력): ";
        // 사용자로부터 한 줄 전체 입력을 읽습니다.
        std::getline(std::cin, infixExpression);

        // 사용자가 'exit'를 입력하면 루프를 종료합니다.
        if (infixExpression == "exit")
        {
            break;
        }

        // 입력 문자열이 비어 있으면 루프의 다음 반복으로 계속합니다.
        if (infixExpression.empty())
        {
            continue;
        }

        try
        {
            // 1. 입력된 중위 표기식을 후위 표기식으로 변환합니다.
            std::string postfixExpression = infixToPostfix(infixExpression);
            std::cout << "후위 표기식: " << postfixExpression << std::endl;

            // 2. 변환된 후위 표기식을 평가하고 결과를 얻습니다.
            double result = evaluatePostfix(postfixExpression);
            std::cout << "계산 결과: " << result << std::endl;

        }
        // 표현식 변환 또는 평가 중 발생하는 runtime_error 예외를 잡아서 처리합니다.
        catch (const std::runtime_error& e)
        {
            // 오류 메시지를 출력합니다.
            std::cerr << e.what() << std::endl;
            std::cerr << "다시 시도해주세요." << std::endl << std::endl;
        }
    }

    return 0; // 프로그램이 성공적으로 종료되었음을 나타냅니다.
}