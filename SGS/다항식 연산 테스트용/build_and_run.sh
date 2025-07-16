#!/bin/bash
# C++ 코드를 컴파일합니다.
g++ main.cpp -o polynomial_calculator -std=c++11 -static

# 컴파일이 성공했는지 확인합니다.
if [ $? -ne 0 ]; then
    echo "컴파일 중 오류가 발생했습니다."
    exit 1
fi

# 컴파일된 프로그램을 실행합니다.
./polynomial_calculator