#include <iostream> // 표준 입출력 스트림을 사용하기 위해 포함합니다. (예: std::cout, std::cin)
#include <string>   // 문자열(std::string)을 다루기 위해 포함합니다.
#include <stack>    // 스택(std::stack) 자료구조를 사용하기 위해 포함합니다. 후위 표기식 변환 및 계산에 필수적입니다.
#include <sstream>  // 문자열 스트림(std::stringstream)을 사용하기 위해 포함합니다. 문자열을 쉽게 파싱하거나 조합할 때 유용합니다.
#include <stdexcept>// 표준 예외 클래스(예: std::runtime_error)를 사용하기 위해 포함합니다. 오류 발생 시 예외를 던질 때 사용합니다.
#include <cctype>   // 문자 분류 함수(예: isdigit, isspace, isalpha)를 사용하기 위해 포함합니다. 문자의 종류를 판별할 때 사용합니다.
#include <vector>   // 벡터(std::vector) 자료구조를 사용하기 위해 포함합니다. (현재 코드에서는 직접 사용되지 않지만, 일반적으로 C++에서 많이 사용됩니다.)
#include <cmath>    // 수학 함수(예: pow, log10)를 사용하기 위해 포함합니다. 제곱이나 로그 계산에 필요합니다.
#include <windows.h> // Windows API 함수(예: SetConsoleOutputCP)를 사용하기 위해 포함합니다. 콘솔의 문자 인코딩 설정을 위해 사용됩니다.

// 연산자 우선순위를 반환하는 함수
// 이 함수는 중위 표기식을 후위 표기식으로 변환할 때 연산자 스택에 쌓을지 여부를 결정하는 데 사용됩니다.
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
    // 제곱(^)은 가장 높은 우선순위(3)를 가집니다.
    if (op == '^')
    {
        return 3;
    }
    // 그 외의 문자는 연산자가 아니므로 우선순위가 없음을 나타내는 0을 반환합니다.
    return 0;
}

// 두 피연산자(a, b)와 연산자(op)를 받아 계산 결과를 반환하는 함수입니다.
// 이 함수는 후위 표기식을 계산할 때 실제 연산을 수행하는 데 사용됩니다.
double applyOp(double a, double b, char op)
{
    // 연산자의 종류에 따라 적절한 계산을 수행합니다.
    switch (op)
    {
        case '+': return a + b; // 덧셈
        case '-': return a - b; // 뺄셈
        case '*': return a * b; // 곱셈
        case '/':
            // 0으로 나누는 경우를 방지하기 위한 예외 처리입니다.
            // 0으로 나누면 수학적으로 정의되지 않으므로 오류를 발생시킵니다.
            if (b == 0)
            {
                throw std::runtime_error("오류: 0으로 나눌 수 없습니다.");
            }
            return a / b; // 나눗셈
        case '^': return pow(a, b); // 제곱 (a의 b승). cmath 라이브러리의 pow 함수를 사용합니다.
    }
    return 0; // 유효하지 않은 연산자가 들어올 경우 0을 반환 (실제로는 예외 처리로 걸러집니다).
}

// 중위 표기식(예: "2 + 3 * 4") 문자열을 후위 표기식(예: "2 3 4 * +")으로 변환하는 함수입니다.
// 이 변환은 수식을 컴퓨터가 쉽게 계산할 수 있는 형태로 만드는 중요한 과정입니다.
// 스택을 사용하여 연산자의 우선순위를 관리하고, 피연산자는 바로 출력 스트림에 추가합니다.
std::string infixToPostfix(const std::string& infix)
{
    std::stack<char> ops; // 연산자를 임시로 저장하는 스택입니다.
    std::stringstream postfix; // 후위 표기식 결과를 문자열 형태로 구축하는 스트림입니다.
    bool expectOperand = true; // 현재 피연산자가 와야 하는지(true) 연산자가 와야 하는지(false)를 나타내는 플래그입니다.

    // 입력된 중위 표기식의 각 문자를 순서대로 처리합니다.
    for (int i = 0; i < infix.length(); ++i)
    {
        char token = infix[i]; // 현재 처리할 문자(토큰)입니다.

        // 공백 문자는 무시하고 다음 문자로 넘어갑니다.
        if (isspace(token))
        {
            continue;
        }

        // 현재 토큰이 숫자이거나 소수점(.)인 경우 (피연산자일 가능성)
        if (isdigit(token) || (token == '.'))
        {
            // 피연산자가 와야 할 차례가 아닌데 숫자가 나오면 오류입니다. (예: "3 4" 또는 "3 + 4 5")
            if (!expectOperand)
            {
                throw std::runtime_error("오류: 피연산자 뒤에 연산자가 와야 합니다.");
            }
            std::string numStr; // 숫자를 저장할 문자열입니다.
            // 현재 위치부터 숫자가 끝날 때까지(숫자 또는 소수점) 문자를 읽어 numStr에 추가합니다.
            while (i < infix.length() && (isdigit(infix[i]) || infix[i] == '.'))
            {
                numStr += infix[i];
                i++;
            }
            i--; // for 루프의 i++ 때문에 한 칸 뒤로 되돌립니다.
            postfix << numStr << " "; // 완성된 숫자를 후위 표기식 스트림에 추가하고 공백으로 구분합니다.
            expectOperand = false; // 숫자를 읽었으므로 다음에는 연산자가 와야 합니다.
        }
        // 현재 토큰이 알파벳인 경우 (함수일 가능성, 여기서는 'log'만 처리)
        else if (isalpha(token))
        {
            // 'log' 함수인지 확인합니다. (현재 위치에서 3글자가 'log'인지)
            if (i + 2 < infix.length() && infix.substr(i, 3) == "log")
            {
                // 피연산자가 와야 할 차례가 아닌데 함수가 나오면 오류입니다. (예: "3 log")
                if (!expectOperand)
                {
                    throw std::runtime_error("오류: 연산자 뒤에 함수가 올 수 없습니다.");
                }
                ops.push('l'); // 'log'를 스택에 'l'로 푸시합니다. (간단한 식별자)
                i += 2; // 'o'와 'g'를 건너뛰어 다음 토큰으로 이동합니다.
            }
            else
            {
                // 'log'가 아닌 다른 알파벳이거나, 'log' 형태가 아니면 인식할 수 없는 문자입니다.
                throw std::runtime_error("오류: 인식할 수 없는 문자입니다.");
            }
        }
        // 현재 토큰이 여는 괄호인 경우 ((, {, [)
        else if (token == '(' || token == '{' || token == '[')
        {
            // 피연산자가 와야 할 차례가 아닌데 여는 괄호가 나오면 오류입니다. (예: "3 (")
            if (!expectOperand)
            {
                 throw std::runtime_error("오류: 연산자 뒤에 여는 괄호가 올 수 없습니다.");
            }
            ops.push(token); // 여는 괄호는 무조건 스택에 푸시합니다.
        }
        // 현재 토큰이 닫는 괄호인 경우 (), }, ])
        else if (token == ')' || token == '}' || token == ']')
        {
            // 피연산자가 와야 할 차례인데 닫는 괄호가 나오면 오류입니다. (예: "3 + )")
            if (expectOperand)
            {
                throw std::runtime_error("오류: 피연산자 뒤에 닫는 괄호가 올 수 없습니다.");
            }
            // 현재 닫는 괄호에 해당하는 여는 괄호를 찾습니다.
            char openParen = (token == ')') ? '(' : ((token == '}') ? '{' : '[');
            // 스택에서 여는 괄호를 만날 때까지 모든 연산자를 후위 표기식 스트림으로 옮깁니다.
            while (!ops.empty() && ops.top() != '(' && ops.top() != '{' && ops.top() != '[')
            {
                postfix << ops.top() << " ";
                ops.pop();
            }
            // 스택이 비어있거나, 스택의 맨 위가 현재 닫는 괄호와 짝이 맞지 않는 여는 괄호인 경우 오류입니다.
            // (예: "(]" 또는 "{)")
            if (ops.empty() || ops.top() != openParen)
            {
                throw std::runtime_error("오류: 괄호 쌍이 맞지 않습니다.");
            }
            ops.pop(); // 스택에서 해당 여는 괄호를 제거합니다.
            // 괄호 안에 log 함수가 있었을 경우, log 함수를 후위 표기식에 추가합니다.
            if (!ops.empty() && ops.top() == 'l')
            { 
                postfix << ops.top() << " ";
                ops.pop();
            }
            expectOperand = false; // 닫는 괄호 뒤에는 연산자가 와야 합니다.
        }
        // 현재 토큰이 연산자(+, -, *, /, ^)인 경우
        else
        { 
            // 피연산자가 와야 할 차례인데 연산자가 나오면 오류입니다.
            if (expectOperand)
            {
                 // 단항 연산자 '-' 처리 (예: "-3 + 4").
                 // 수식의 맨 앞이거나, 여는 괄호 뒤, 또는 공백 뒤에 '-'가 오면 단항 연산자로 간주합니다.
                if (token == '-' && (i == 0 || infix[i-1] == '(' || isspace(infix[i-1])))
                {
                    // 단항 '-'를 처리하기 위해 '0 -' 형태로 변환합니다. (예: -3 -> 0 - 3)
                    postfix << "0 "; 
                }
                else
                {
                    throw std::runtime_error("오류: 연산자 앞에 피연산자가 와야 합니다.");
                }
            }
            
            // 스택이 비어있지 않고, 스택의 맨 위가 여는 괄호가 아니며,
            // 스택의 맨 위 연산자의 우선순위가 현재 연산자의 우선순위보다 높거나 같으면
            // 스택의 연산자를 후위 표기식 스트림으로 옮깁니다.
            // 이는 연산자 우선순위 규칙을 따르기 위함입니다.
            while (!ops.empty() && ops.top() != '(' && ops.top() != '{' && ops.top() != '[' && getPrecedence(ops.top()) >= getPrecedence(token))
            {
                postfix << ops.top() << " ";
                ops.pop();
            }
            ops.push(token); // 현재 연산자를 스택에 푸시합니다.
            expectOperand = true; // 연산자 뒤에는 피연산자가 와야 합니다.
        }
    }

    // 수식의 끝에 도달했을 때 피연산자가 와야 한다면 오류입니다. (예: "3 +")
    if (expectOperand)
    {
        throw std::runtime_error("오류: 수식이 피연산자로 끝나야 합니다.");
    }

    // 스택에 남아있는 모든 연산자를 후위 표기식 스트림으로 옮깁니다.
    while (!ops.empty())
    {
        // 스택에 여는 괄호가 남아있다면 괄호 쌍이 맞지 않는 오류입니다.
        if (ops.top() == '(' || ops.top() == '{' || ops.top() == '[')
        {
            throw std::runtime_error("오류: 괄호 쌍이 맞지 않습니다.");
        }
        postfix << ops.top() << " ";
        ops.pop();
    }

    return postfix.str(); // 완성된 후위 표기식 문자열을 반환합니다.
}

// 후위 표기식 문자열(예: "2 3 4 * +")을 계산하여 결과를 반환하는 함수입니다.
// 이 함수는 스택을 사용하여 피연산자를 저장하고, 연산자를 만나면 스택의 최상위 두 값을 꺼내 계산합니다.
double evaluatePostfix(const std::string& postfix)
{
    std::stack<double> values; // 피연산자 값을 저장하는 스택입니다.
    std::stringstream ss(postfix); // 후위 표기식 문자열을 파싱하기 위한 스트림입니다.
    std::string token; // 스트림에서 읽어들일 각 토큰(숫자 또는 연산자)을 저장할 변수입니다.

    // 후위 표기식의 각 토큰을 순서대로 처리합니다.
    while (ss >> token)
    {
        // 토큰이 숫자로 시작하거나 소수점(.)을 포함하는 경우 (피연산자)
        if (isdigit(token[0]) || (token.length() > 1 && token[0] == '.'))
        {
            values.push(stod(token)); // 문자열을 double 형으로 변환하여 스택에 푸시합니다. (stod는 string to double)
        }
        // 토큰이 'l'인 경우 (log 함수)
        else if (token == "l")
        { 
            // log 함수는 하나의 인수를 필요로 하므로 스택에 값이 있는지 확인합니다.
            if (values.empty())
            {
                throw std::runtime_error("오류: log 함수의 인수가 없습니다.");
            }
            double val = values.top(); // 스택의 맨 위 값을 인수로 가져옵니다.
            values.pop(); // 인수를 스택에서 제거합니다.
            // log 함수의 인수는 양수여야 합니다. (수학적 정의)
            if (val <= 0)
            {
                throw std::runtime_error("오류: log의 인수는 양수여야 합니다.");
            }
            values.push(log10(val)); // log10 함수를 사용하여 상용로그(밑이 10인 로그)를 계산하고 결과를 스택에 푸시합니다.
        }
        // 토큰이 연산자인 경우
        else
        {
            // 연산을 수행하려면 최소 두 개의 피연산자가 스택에 있어야 합니다.
            if (values.size() < 2)
            {
                throw std::runtime_error("오류: 수식이 잘못되었습니다 (피연산자 부족).");
            }
            double val2 = values.top(); // 스택의 맨 위 값 (두 번째 피연산자)을 가져옵니다.
            values.pop(); // 두 번째 피연산자를 스택에서 제거합니다.
            double val1 = values.top(); // 스택의 다음 값 (첫 번째 피연산자)을 가져옵니다.
            values.pop(); // 첫 번째 피연산자를 스택에서 제거합니다.
            values.push(applyOp(val1, val2, token[0])); // applyOp 함수를 사용하여 계산하고 결과를 스택에 푸시합니다.
        }
    }

    // 모든 토큰을 처리한 후 스택에 하나의 값만 남아있어야 합니다. (최종 계산 결과)
    if (values.size() != 1)
    {
        throw std::runtime_error("오류: 수식이 잘못되었습니다 (연산자 부족).");
    }

    return values.top(); // 최종 계산 결과를 반환합니다.
}

// 프로그램의 시작점인 main 함수입니다.
int main()
{
    // Windows 콘솔에서 한글이 깨지지 않고 UTF-8로 올바르게 출력되도록 코드 페이지를 설정합니다.
    // 65001은 UTF-8의 코드 페이지 번호입니다.
    SetConsoleOutputCP(65001);

    std::string infixExpression; // 사용자로부터 입력받을 중위 표기식 문자열을 저장할 변수입니다.

    // 사용자가 'exit'를 입력할 때까지 수식 계산을 반복합니다.
    while (true)
    {
        // 사용자에게 수식 입력을 요청하는 메시지를 출력합니다.
        std::cout << "중위 표기식 수식을 입력하세요 (종료: exit): ";
        // 사용자로부터 한 줄 전체를 입력받습니다.
        std::getline(std::cin, infixExpression);

        // 사용자가 'exit'를 입력했으면 반복을 종료합니다.
        if (infixExpression == "exit")
        {
            break;
        }
        
        // 빈 문자열이 입력되었으면 다음 반복으로 넘어갑니다.
        if (infixExpression.empty())
        {
            continue;
        }

        try
        {
            // 1. 입력받은 중위 표기식을 후위 표기식으로 변환합니다.
            std::string postfixExpression = infixToPostfix(infixExpression);
            std::cout << "후위 표기식 변환: " << postfixExpression << std::endl;

            // 2. 변환된 후위 표기식을 계산하여 결과를 얻습니다.
            double result = evaluatePostfix(postfixExpression);
            std::cout << "계산 결과: " << result << std::endl;

        }
        // 수식 변환 또는 계산 중 오류(std::runtime_error)가 발생하면 이곳에서 처리합니다.
        catch (const std::runtime_error& e)
        {
            // 발생한 오류 메시지를 출력합니다.
            std::cerr << e.what() << std::endl;
            std::cerr << "다시 입력해주세요." << std::endl << std::endl;
        }
    }

    return 0; // 프로그램이 성공적으로 종료되었음을 나타냅니다.
}
