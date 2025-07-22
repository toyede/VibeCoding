// 필요한 라이브러리들을 포함시킵니다.
#include <iostream>  // 표준 입출력(cin, cout)을 위한 라이브러리
#include <fstream>   // 파일 입출력(ifstream, ofstream)을 위한 라이브러리
#include <vector>    // 동적 배열인 벡터(vector)를 사용하기 위한 라이브러리
#include <sstream>   // 문자열을 스트림처럼 다루기 위한 라이브러리 (문자열 파싱에 유용)

// 희소 행렬 데이터가 담긴 텍스트 파일을 읽어서,
// 일반적인 2차원 벡터(Dense Matrix) 형태로 변환해주는 함수입니다.
std::vector<std::vector<int>> readSparseMatrix(const std::string& filename)
{
    // ifstream: 파일을 읽기 위한 객체입니다. filename으로 주어진 이름의 파일을 엽니다.
    std::ifstream file(filename);

    // 파일이 제대로 열렸는지 확인합니다.
    // 만약 파일이 없거나 열 수 없는 경우, 에러 메시지를 출력하고 빈 행렬을 반환합니다.
    if (!file.is_open())
    {
        std::cerr << "오류: " << filename << " 파일을 열 수 없습니다." << std::endl;
        return {}; // 비어있는 벡터를 반환하여 오류가 났음을 알립니다.
    }

    std::string line; // 파일에서 한 줄씩 읽어올 문자열 변수

    // 1. 파일의 첫 번째 줄을 읽습니다. (예: "3 3 4")
    //    첫 줄에는 행렬의 크기(행, 열)와 0이 아닌 원소의 개수 정보가 들어있습니다.
    std::getline(file, line);

    // 2. 읽어온 한 줄(line)을 stringstream 객체에 넣어서,
    //    공백으로 구분된 숫자들을 쉽게 추출할 수 있도록 합니다.
    std::stringstream ss(line);
    int rows, cols, nonZeroCount; // 행, 열, 0이 아닌 원소 수를 저장할 변수
    ss >> rows >> cols >> nonZeroCount; // stringstream에서 차례대로 값을 추출하여 변수에 저장

    // 3. 주어진 행(rows)과 열(cols) 크기에 맞춰 2차원 벡터(행렬)를 생성합니다.
    //    모든 원소의 초기값은 0으로 설정합니다.
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));

    // 4. 파일의 나머지 줄들을 읽으면서 0이 아닌 원소들을 행렬에 배치합니다.
    int r, c, value; // 행(row), 열(column), 값(value)을 저장할 변수
    // 파일의 끝까지 "행 열 값" 형식으로 한 줄씩 읽어들입니다.
    while (file >> r >> c >> value)
    {
        // 행과 열의 인덱스가 유효한 범위 내에 있는지 확인합니다.
        if (r < rows && c < cols)
        {
            // 해당 위치에 원소 값을 저장합니다.
            matrix[r][c] = value;
        }
    }

    // 파일 사용이 끝났으므로 닫아줍니다.
    file.close();

    // 완성된 2차원 행렬을 반환합니다.
    return matrix;
}

// 2차원 벡터(행렬)의 내용을 화면에 예쁘게 출력해주는 함수입니다.
void printMatrix(const std::vector<std::vector<int>>& matrix)
{
    // 행렬이 비어있는지 확인합니다.
    if (matrix.empty())
    {
        std::cout << "행렬이 비어있습니다." << std::endl;
        return; // 함수 종료
    }

    // 바깥쪽 for문: 행렬의 각 행(row)에 대해 반복합니다.
    for (const auto& row : matrix)
    {
        // 안쪽 for문: 현재 행(row)에 있는 각 원소(val)에 대해 반복합니다.
        for (int val : row)
        {
            // 각 원소 값을 출력하고, 탭(	)으로 간격을 줍니다.
            std::cout << val << "	";
        }
        // 한 행의 출력이 끝나면 줄바꿈을 합니다.
        std::cout << std::endl;
    }
}

// 프로그램의 시작점인 main 함수입니다.
int main()
{
    // 1. readSparseMatrix 함수를 호출하여 matA.txt와 matB.txt 파일로부터
    //    두 개의 행렬(matA, matB)을 생성합니다.
    // 'auto' 키워드는 변수의 타입을 컴파일러가 자동으로 추론하게 합니다.
    // 여기서는 std::vector<std::vector<int>> 타입이 됩니다.
    auto matA = readSparseMatrix("matA.txt");
    auto matB = readSparseMatrix("matB.txt");

    // 2. 파일 읽기 중 오류가 발생했는지 확인합니다.
    //    만약 행렬 중 하나라도 비어있다면, 프로그램에 문제가 있는 것이므로 종료합니다.
    if (matA.empty() || matB.empty())
    {
        return 1; // 0이 아닌 값을 반환하여 프로그램이 비정상적으로 종료되었음을 알립니다.
    }

    // 3. 행렬 곱셈이 가능한 조건인지 확인합니다.
    //    (A의 열 개수)와 (B의 행 개수)가 같아야만 곱셈이 가능합니다.
    //    matA[0].size()는 A의 첫 번째 행의 원소 개수, 즉 열의 개수를 의미합니다.
    //    matB.size()는 B의 행의 개수를 의미합니다.
    if (matA.empty() || matA[0].empty() || matB.empty() || matA[0].size() != matB.size())
    {
        std::cerr << "오류: 행렬 곱셈을 할 수 없는 크기입니다." << std::endl;
        std::cerr << "A 행렬의 열: " << (matA.empty() ? 0 : matA[0].size()) << ", B 행렬의 행: " << matB.size() << std::endl;
        return 1; // 오류 메시지를 출력하고 비정상 종료합니다.
    }

    // 4. 행렬 곱셈에 필요한 크기 정보를 변수에 저장합니다.
    int rowsA = matA.size();      // A의 행 개수
    int colsA = matA[0].size();   // A의 열 개수
    int colsB = matB[0].size();   // B의 열 개수

    // 5. 곱셈 결과를 저장할 2차원 벡터(result)를 생성합니다.
    //    결과 행렬의 크기는 (A의 행 개수) x (B의 열 개수)가 됩니다.
    //    모든 원소는 0으로 초기화합니다.
    std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));

    // 6. 행렬 곱셈을 수행합니다. (3중 for문 사용)
    // i: 결과 행렬의 행 인덱스
    for (int i = 0; i < rowsA; ++i)
    {
        // j: 결과 행렬의 열 인덱스
        for (int j = 0; j < colsB; ++j)
        {
            // k: 계산 과정에 사용되는 인덱스
            for (int k = 0; k < colsA; ++k)
            {
                // result[i][j] = (A의 i번째 행) * (B의 j번째 열)
                result[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    // 7. 최종 결과를 화면에 출력합니다.
    std::cout << "행렬 A와 B의 곱셈 결과:" << std::endl;
    printMatrix(result); // 위에서 만든 printMatrix 함수를 사용합니다.

    // 프로그램이 정상적으로 종료되었음을 운영체제에 알립니다.
    return 0;
}
