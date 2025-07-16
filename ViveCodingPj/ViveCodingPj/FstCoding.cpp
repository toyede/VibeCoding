#include <iostream>
using namespace std;

int main() {
    int num1, num2, result;

    cout << "첫 번째 숫자를 입력하세요: ";
    cin >> num1;

    cout << "두 번째 숫자를 입력하세요: ";
    cin >> num2;

    // 디버깅 포인트를 여기에 설정하면 좋습니다
    result = num1 + num2;

    cout << num1 << " + " << num2 << " = " << result << endl;

    return 0;
}