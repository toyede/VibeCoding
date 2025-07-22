/**
 * @file Maze.cpp
 * @brief 스택(Stack) 자료구조를 이용한 미로 찾기 게임입니다.
 * @details
 *  - 7x7 크기의 미로가 랜덤으로 생성됩니다.
 *  - 미로의 벽(1)과 길(0), 시작점(S)과 도착점(E)은 실행할 때마다 랜덤으로 설정됩니다.
 *  - 깊이 우선 탐색(DFS) 알고리즘을 스택으로 구현하여 경로를 찾습니다.
 *  - 탐색 과정은 콘솔 화면에 시각적으로 표시됩니다.
 */

// C++ 표준 라이브러리 및 Windows 관련 헤더 파일을 포함합니다.
#include <iostream>  // 입출력을 위한 라이브러리 (cout, endl 등)
#include <vector>    // 동적 배열(미로)을 사용하기 위한 라이브러리
#include <stack>     // 스택 자료구조를 사용하기 위한 라이브러리
#include <cstdlib>   // 랜덤 함수(rand, srand)를 사용하기 위한 라이브러리
#include <ctime>     // 시간(time) 함수를 사용하기 위한 라이브러리 (랜덤 시드 초기화용)
#include <windows.h> // Windows 전용 함수(Sleep, system)를 사용하기 위한 헤더

// 미로의 크기를 7x7로 정의합니다. 상수를 사용하면 나중에 크기를 변경하기 용이합니다.
const int MAZE_SIZE = 7;

/**
 * @brief 미로 내의 한 점(좌표)을 나타내는 구조체입니다.
 * @details y와 x, 두 개의 정수 멤버 변수를 가집니다.
 */
struct Point
{
    int y, x; // y: 행(row), x: 열(column)

    // 기본 생성자: Point 객체가 생성될 때 y와 x를 0으로 초기화합니다.
    Point() : y(0), x(0) {}

    // 인자를 받는 생성자: y와 x 값을 받아 객체를 초기화합니다.
    Point(int y_val, int x_val) : y(y_val), x(x_val) {}

    // 연산자 오버로딩: 두 Point 객체가 같은지 비교할 수 있도록 '==' 연산자의 동작을 정의합니다.
    bool operator==(const Point& other) const
    {
        return y == other.y && x == other.x;
    }
};

/**
 * @brief 현재 미로의 상태를 콘솔 화면에 출력하는 함수입니다.
 * @param maze 현재 탐색 상태가 기록된 2차원 char 벡터 (방문 'V', 막다른 길 'X' 등)
 * @param start 시작점 좌표
 * @param exit 도착점 좌표
 */
void printMaze(const std::vector<std::vector<char>>& maze, const Point& start, const Point& exit)
{
    system("cls"); // 콘솔 화면을 깨끗하게 지웁니다. (Windows 명령어)

    // 2중 for문을 이용해 미로의 모든 칸을 순회합니다.
    for (int i = 0; i < MAZE_SIZE; ++i)
    {
        for (int j = 0; j < MAZE_SIZE; ++j)
        {
            // 현재 좌표가 시작점(S)이나 도착점(E)이면 해당 문자를 출력합니다.
            if (i == start.y && j == start.x)
            {
                std::cout << "S ";
            }
            else if (i == exit.y && j == exit.x)
            {
                std::cout << "E ";
            }
            else // 그 외의 경우는 displayMaze에 저장된 문자를 출력합니다.
            {
                std::cout << maze[i][j] << " ";
            }
        }
        std::cout << std::endl; // 한 행의 출력이 끝나면 줄을 바꿉니다.
    }
    // 미로 아래에 범례를 출력하여 각 문자의 의미를 설명합니다.
    std::cout << "\nS: Start, E: Exit, V: Visited, X: Dead End, 1: Wall" << std::endl;
}

// 프로그램의 시작점인 main 함수입니다.
int main()
{
    // 랜덤 시드 초기화: 매번 다른 미로를 생성하기 위해 현재 시간을 시드값으로 사용합니다.
    srand(static_cast<unsigned int>(time(0)));

    // --- 1. 미로 초기화 --- //
    // maze: 실제 벽(1)과 길(0)의 정보를 저장하는 2차원 배열
    std::vector<std::vector<int>> maze(MAZE_SIZE, std::vector<int>(MAZE_SIZE));
    // displayMaze: 사용자에게 보여줄 미로 상태를 저장하는 2차원 배열 (문자로 표현)
    std::vector<std::vector<char>> displayMaze(MAZE_SIZE, std::vector<char>(MAZE_SIZE, '0'));

    // --- 2. 미로 랜덤 생성 --- //
    for (int i = 0; i < MAZE_SIZE; ++i)
    {
        for (int j = 0; j < MAZE_SIZE; ++j)
        {
            // 30%의 확률로 벽(1)을 생성합니다.
            if (rand() % 10 < 3)
            {
                maze[i][j] = 1;
                displayMaze[i][j] = '1';
            }
            else // 70%의 확률로 길(0)을 생성합니다.
            {
                maze[i][j] = 0;
                displayMaze[i][j] = '0';
            }
        }
    }

    // --- 3. 시작점(S)과 도착점(E) 랜덤 설정 --- //
    Point start, exit;
    int side = rand() % 4; // 0: 위, 1: 오른쪽, 2: 아래, 3: 왼쪽 가장자리

    // side 값에 따라 미로의 가장자리 중 한 곳에 시작점을 설정합니다.
    if (side == 0) start = Point(0, rand() % MAZE_SIZE);
    else if (side == 1) start = Point(rand() % MAZE_SIZE, MAZE_SIZE - 1);
    else if (side == 2) start = Point(MAZE_SIZE - 1, rand() % MAZE_SIZE);
    else start = Point(rand() % MAZE_SIZE, 0);

    // 도착점은 시작점과 다른 위치에 설정될 때까지 반복해서 랜덤으로 생성합니다.
    do
    {
        side = rand() % 4;
        if (side == 0) exit = Point(0, rand() % MAZE_SIZE);
        else if (side == 1) exit = Point(rand() % MAZE_SIZE, MAZE_SIZE - 1);
        else if (side == 2) exit = Point(MAZE_SIZE - 1, rand() % MAZE_SIZE);
        else exit = Point(rand() % MAZE_SIZE, 0);
    } while (start == exit);

    // 시작점과 도착점은 항상 갈 수 있는 길(0)이어야 하므로 값을 0으로 강제 설정합니다.
    maze[start.y][start.x] = 0;
    displayMaze[start.y][start.x] = '0';
    maze[exit.y][exit.x] = 0;
    displayMaze[exit.y][exit.x] = '0';

    // --- 4. 스택을 이용한 미로 탐색 (DFS) --- //
    std::stack<Point> pathStack; // 지나온 경로를 저장할 스택
    Point currentPos = start;    // 현재 위치를 시작점으로 초기화

    // 이동할 방향을 정의합니다. (상, 하, 좌, 우)
    int dy[] = {-1, 1, 0, 0}; // y좌표 변화량
    int dx[] = {0, 0, -1, 1}; // x좌표 변화량

    // 무한 루프를 돌며 미로를 탐색합니다. (탈출 조건은 내부에서 처리)
    while (true)
    {
        // 현재 미로 상태를 출력하고, 현재 위치 정보를 보여줍니다.
        printMaze(displayMaze, start, exit);
        std::cout << "Current Position: (" << currentPos.y << ", " << currentPos.x << ")" << std::endl;
        Sleep(200); // 0.2초간 실행을 멈춰서 탐색 과정을 시각적으로 보여줍니다.

        // **탈출 조건 1: 현재 위치가 도착점일 경우**
        if (currentPos == exit)
        {
            displayMaze[currentPos.y][currentPos.x] = 'V'; // 마지막 위치도 방문 처리
            printMaze(displayMaze, start, exit); // 최종 미로 상태 출력
            std::cout << "\n*** Maze Solved! ***" << std::endl;
            break; // 루프를 탈출하여 프로그램을 종료합니다.
        }

        // 현재 위치를 방문했다는 의미로 'V'로 표시합니다.
        displayMaze[currentPos.y][currentPos.x] = 'V';

        bool moved = false; // 이번 루프에서 이동에 성공했는지 여부를 저장하는 변수

        // 4방향(상,하,좌,우)에 대해 이동 가능한지 탐색합니다.
        for (int i = 0; i < 4; ++i)
        {
            Point nextPos(currentPos.y + dy[i], currentPos.x + dx[i]);

            // **이동 조건 확인**
            // 1. 다음 위치가 미로 범위 안에 있는지?
            if (nextPos.y >= 0 && nextPos.y < MAZE_SIZE &&
                nextPos.x >= 0 && nextPos.x < MAZE_SIZE &&
                // 2. 다음 위치가 벽(1)이 아닌 길(0)인지?
                maze[nextPos.y][nextPos.x] == 0 &&
                // 3. 아직 방문하지 않은 곳인지? ('V'나 'X'가 아닌지)
                displayMaze[nextPos.y][nextPos.x] != 'V' &&
                displayMaze[nextPos.y][nextPos.x] != 'X')
            {
                // **이동 처리**
                // 현재 위치를 스택에 push하여 되돌아올 경로를 저장합니다.
                pathStack.push(currentPos);
                // 다음 위치로 현재 위치를 업데이트합니다.
                currentPos = nextPos;
                moved = true; // 이동에 성공했음을 표시
                break; // 4방향 탐색을 중단하고 while 루프의 처음으로 돌아갑니다.
            }
        }

        // **후퇴 처리 (Backtracking)**
        // 만약 4방향 모두 탐색했지만 이동할 곳이 없었다면 (moved가 false이면)
        if (!moved)
        {
            // 현재 위치는 더 이상 탐색할 가치가 없는 막다른 길이므로 'X'로 표시합니다.
            displayMaze[currentPos.y][currentPos.x] = 'X';

            // **탈출 조건 2: 스택이 비어있을 경우**
            // 스택이 비었다는 것은 되돌아갈 길이 없다는 의미이므로, 길 찾기에 실패한 것입니다.
            if (pathStack.empty())
            {
                printMaze(displayMaze, start, exit); // 최종 미로 상태 출력
                std::cout << "\n*** Failed to find a path. The stack is empty. ***" << std::endl;
                break; // 루프를 탈출하여 프로그램을 종료합니다.
            }

            // 스택에서 가장 최근에 저장된 위치(바로 이전 위치)를 꺼내(pop) 현재 위치로 설정합니다.
            // 이를 통해 막다른 길에서 한 칸 뒤로 되돌아가는(backtracking) 효과를 냅니다.
            currentPos = pathStack.top();
            pathStack.pop();
        }
    }

    return 0; // 프로그램 정상 종료
}
