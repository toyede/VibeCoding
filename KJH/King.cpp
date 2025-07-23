/**
 * @file King.cpp
 * @brief 트리(Tree) 자료구조를 이용한 조선 왕조 관계 분석 및 퀴즈 프로그램입니다.
 * @details
 *  - '조선왕조.txt' 파일을 읽어 왕과 부모의 관계를 트리로 구성합니다.
 *  - '퀴즈.txt' 파일을 읽어 트리 데이터를 기반으로 다양한 질문에 답변합니다.
 *  - g++ 환경에 최적화되어 있으며, UTF-8 인코딩을 지원합니다.
 */

// C++ 표준 라이브러리를 포함합니다.
#include <iostream>  // 콘솔 입출력 (cout, cerr)
#include <fstream>   // 파일 입출력 (ifstream)
#include <string>    // 문자열 처리 (string)
#include <vector>    // 동적 배열 (vector)
#include <map>       // 키-값 쌍의 연관 컨테이너 (map)
#include <cstdlib>   // C 표준 라이브러리 (system 함수)

/**
 * @brief 왕 한 명의 정보를 저장하는 트리 노드(Node) 구조체입니다.
 * @details 각 노드는 왕의 이름, 부모 노드를 가리키는 포인터, 여러 자식 노드를
 *          가리키는 포인터들을 가집니다. 이를 통해 왕들 간의 관계가 트리 형태로 연결됩니다.
 */
struct KingNode
{
    std::string name;                // 왕의 이름
    KingNode* parent;                // 부모 왕 노드를 가리키는 포인터
    std::vector<KingNode*> children; // 자식 왕 노드들을 가리키는 포인터의 벡터

    // 생성자: 왕의 이름을 받아 노드를 생성하고, 부모는 없는 상태(nullptr)로 초기화합니다.
    KingNode(const std::string& n) : name(n), parent(nullptr) {}
};

/**
 * @brief 특정 왕의 모든 후손을 찾는 재귀 함수입니다. (깊이 우선 탐색, DFS)
 * @param node 탐색을 시작할 왕의 노드 포인터
 * @param descendants 후손들의 이름이 저장될 벡터 (참조로 전달)
 * @details
 * 
 *  1. 현재 노드의 모든 자식들을 순회합니다.
 *  2. 각 자식의 이름을 `descendants` 벡터에 추가합니다.
 *  3. 그 자식을 새로운 시작점으로 하여 자기 자신(findAllDescendants)을 다시 호출합니다.
 *  4. 자식이 더 이상 없을 때까지 이 과정이 반복되어 모든 후손을 찾게 됩니다.
 */
void findAllDescendants(KingNode* node, std::vector<std::string>& descendants)
{
    if (!node)
    {
        return; // 안전 장치: 노드가 유효하지 않으면 즉시 종료
    }

    for (KingNode* child : node->children)
    {
        descendants.push_back(child->name);
        findAllDescendants(child, descendants);
    }
}

/**
 * @brief 루트로부터의 깊이(세대)를 계산하는 함수입니다.
 * @param node 세대를 계산할 왕의 노드 포인터
 * @return 시조로부터의 세대 수 (깊이)
 * @details
 *  - 현재 노드에서 시작하여 부모 포인터(parent)를 따라 계속 올라갑니다.
 *  - 한 단계 올라갈 때마다 세대 수(depth)를 1씩 증가시킵니다.
 *  - 부모가 더 이상 없는 최상위 노드(시조)에 도달하면 반복을 멈추고 계산된 세대 수를 반환합니다.
 */
int getDepth(KingNode* node)
{
    int depth = 0;
    while (node && node->parent != nullptr)
    {
        node = node->parent;
        depth++;
    }
    return depth;
}

// 프로그램의 메인 실행 함수입니다.
int main()
{
    // Windows 콘솔의 코드 페이지를 UTF-8(65001)로 변경합니다.
    system("chcp 65001 > nul");

    // --- 파일 열기 ---
    std::ifstream dynastyFile("조선왕조.txt");
    std::ifstream quizFile("퀴즈.txt");

    // 파일이 정상적으로 열렸는지 확인합니다.
    if (!dynastyFile.is_open() || !quizFile.is_open())
    {
        std::cerr << "오류: 조선왕조.txt 또는 퀴즈.txt 파일을 열 수 없습니다." << std::endl;
        return 1; // 오류 코드를 반환하며 프로그램 종료
    }

    // --- 트리 데이터 구조 준비 ---
    // `map`을 사용하여 왕의 이름(string)으로 해당 노드 포인터(KingNode*)를 빠르게 찾을 수 있습니다.
    std::map<std::string, KingNode*> kings;

    // `vector`를 사용하여 왕들의 순서를 입력된 순서대로 저장합니다.
    std::vector<std::string> kingOrder;

    std::string line; // 파일에서 한 줄씩 읽어올 문자열 변수

    // 1. `조선왕조.txt` 파일을 읽고 왕들의 관계를 트리로 구성합니다.
    while (std::getline(dynastyFile, line))
    {
        // Windows에서 사용하는 개행문자(CRLF) 중 CR(`\r`)을 제거합니다.
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue; // 빈 줄은 건너뜁니다.
        }

        std::string kingName, parentName;

        // 시조인 "태조"는 부모가 없으므로 특별히 처리합니다.
        if (line == "태조")
        {
            kingName = line;
            parentName = ""; // 부모 없음
        }

        else
        {
            // "정종태조" 같은 문자열에서 자식("정종")과 부모("태조")를 분리합니다.
            // 이미 `kings` 맵에 추가된 왕들의 이름을 기준으로 가장 길게 일치하는 부모를 찾습니다.
            for (const auto& pair : kings)
            {
                const std::string& pName = pair.first;

                // 현재 줄의 뒷부분이 기존 왕의 이름(pName)과 일치하는지 확인합니다.
                if (line.length() > pName.length() && line.rfind(pName) == (line.length() - pName.length()))
                {
                    // 부모 문자열을 변수에 저장
                    parentName = pName;
                    kingName = line.substr(0, line.length() - pName.length());
                    break; // 부모를 찾았으므로 더 이상 탐색할 필요 없음
                }
            }
        }

        // 맵에 아직 없는 새로운 왕이라면, 새 노드를 생성하고 순서 벡터에도 추가합니다.
        if (kings.find(kingName) == kings.end())
        {
            kings[kingName] = new KingNode(kingName);
            kingOrder.push_back(kingName);
        }

        // 부모 이름이 존재하고, 그 부모가 맵에 있다면 부모-자식 관계를 설정합니다.
        if (!parentName.empty() && kings.count(parentName))
        {
            kings[kingName]->parent = kings[parentName]; // 자식의 부모 포인터 설정
            kings[parentName]->children.push_back(kings[kingName]); // 부모의 자식 벡터에 추가
        }
    }

    // 2. `퀴즈.txt` 파일을 읽고 각 질문에 대한 답을 처리합니다.
    int numQuestions;
    quizFile >> numQuestions; // 첫 줄에서 질문의 개수를 읽습니다.
    std::getline(quizFile, line); // 숫자 입력 후 남은 개행 문자를 비워줍니다.

    for (int i = 0; i < numQuestions; ++i)
    {
        std::getline(quizFile, line);
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        std::cout << "\nQ: " << line << std::endl;
        std::cout << "A: ";

        // 질문의 내용에 따라 적절한 답변 로직을 실행합니다.
        if (line.find("순서대로") != std::string::npos)
        {
            for (const auto& name : kingOrder)
            {
                std::cout << name << " ";
            }
            std::cout << std::endl;
        }

        else if (line.find("역순으로") != std::string::npos)
        {
            // rbegin()과 rend()를 사용하여 벡터를 역순으로 순회합니다.
            for (auto it = kingOrder.rbegin(); it != kingOrder.rend(); ++it)
            {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
        }

        else if (line.find("모두몇명") != std::string::npos)
        {
            std::cout << kings.size() << "명" << std::endl;
        }

        else if (line.find("의후손은누구누구인가?") != std::string::npos)
        {
            // "조선의왕중에서[왕이름]의후손은누구누구인가?" 형식의 질문 처리
            size_t posOfEuihuson = line.find("의후손");
            std::string tempStr = line.substr(0, posOfEuihuson);
            size_t posOfEseo = tempStr.find("에서");
            
            std::string targetKingName = tempStr.substr(posOfEseo + std::string("에서").length());
                
            if (kings.count(targetKingName))
            {
                std::vector<std::string> descendants;
                findAllDescendants(kings[targetKingName], descendants);
                for (const auto& name : descendants)
                {
                    std::cout << name << " ";
                }
                std::cout << std::endl;
            }

            else
            {
                std::cout << "해당 왕을 찾을 수 없습니다." << std::endl;
            }
        }

        else if (line.find("직계후손이왕이되지못한왕") != std::string::npos)
        {
            // 자식 노드 벡터(children)가 비어있는 왕을 찾습니다.
            for (const auto& name : kingOrder)
            {
                if (kings[name]->children.empty())
                {
                    std::cout << name << " ";
                }
            }
            std::cout << std::endl;
        }

        else if (line.find("가장많은왕") != std::string::npos)
        {
            std::string maxKing;
            size_t maxChildren = 0;
            for (const auto& pair : kings)
            {
                if (pair.second->children.size() > maxChildren)
                {
                    maxChildren = pair.second->children.size();
                    maxKing = pair.first;
                }
            }
            std::cout << maxKing << " (" << maxChildren << "명)" << std::endl;
        }

        else if (line.find("의형제로조선의왕이된사람은누구인가?") != std::string::npos)
        {
            // "[왕이름]의형제로조선의왕이된사람은누구인가?" 형식의 질문 처리
            size_t posOfEuihyeongje = line.find("의형제");
            std::string targetKingName = line.substr(0, posOfEuihyeongje);

            if (kings.count(targetKingName) && kings[targetKingName]->parent)
            {
                KingNode* parent = kings[targetKingName]->parent;
                for (KingNode* child : parent->children)
                {
                    if (child->name != targetKingName)
                    {
                        std::cout << child->name << " ";
                    }
                }
                std::cout << std::endl;
            }

            else
            {
                std::cout << "해당 왕을 찾을 수 없거나 형제를 찾을 수 없습니다." << std::endl;
            }
        }

        else if (line.find("의직계선조를모두출력하시오.") != std::string::npos)
        {
            // "[왕이름]의직계선조를모두출력하시오." 형식의 질문 처리
            size_t posOfEuiJikgye = line.find("의직계선조");
            std::string targetKingName = line.substr(0, posOfEuiJikgye);

            if (kings.count(targetKingName))
            {
                KingNode* current = kings[targetKingName];
                while (current && current->parent != nullptr)
                {
                    std::cout << current->name << " ";
                    current = current->parent;
                }
                std::cout << std::endl;
            }

            else
            {
                std::cout << "해당 왕을 찾을 수 없습니다." << std::endl;
            }
        }

        else if (line.find("의몇대후손인가?") != std::string::npos)
        {
            // "[왕이름1]은[왕이름2]의몇대후손인가?" 형식의 질문 처리
            size_t posOfEuiMyeotdae = line.find("의몇대후손");

            if (posOfEuiMyeotdae == std::string::npos)
            {
                std::cout << "질문 형식이 올바르지 않습니다. (suffix not found)" << std::endl;
                continue;
            }

            std::string tempPrefix = line.substr(0, posOfEuiMyeotdae);
            size_t posOfEun = tempPrefix.find("은");
            size_t posOfNeun = tempPrefix.find("는");

            size_t splitPos = std::string::npos;
            std::string delimiter = "";

            if (posOfEun != std::string::npos && posOfNeun != std::string::npos)
            {
                // Both found, use the one that appears later
                if (posOfEun > posOfNeun)
                {
                    splitPos = posOfEun;
                    delimiter = "은";
                }

                else
                {
                    splitPos = posOfNeun;
                    delimiter = "는";
                }
            }

            else if (posOfEun != std::string::npos)
            {
                splitPos = posOfEun;
                delimiter = "은";
            }

            else if (posOfNeun != std::string::npos)
            {
                splitPos = posOfNeun;
                delimiter = "는";
            }

            if (splitPos == std::string::npos)
            {
                std::cout << "질문 형식이 올바르지 않습니다. (delimiter '은' or '는' not found)" << std::endl;
                continue;
            }

            std::string king1Name = tempPrefix.substr(0, splitPos);
            std::string king2Name = tempPrefix.substr(splitPos + delimiter.length());

            if (kings.count(king1Name) && kings.count(king2Name))
            {
                int king1Depth = getDepth(kings[king1Name]);
                int king2Depth = getDepth(kings[king2Name]);
                
                if (king1Depth >= king2Depth)
                {
                    std::cout << king1Depth - king2Depth << "대 후손" << std::endl;
                }

                else
                {
                    std::cout << "관계 파악 불가" << std::endl; // 왕1이 왕2보다 선조일 경우
                }
            }

            else
            {
                std::cout << "해당 왕들을 찾을 수 없습니다." << std::endl;
            }
        }

        else if (line.find("2명이상왕이된왕") != std::string::npos)
        {
            int count = 0;
            for (const auto& pair : kings)
            {
                if (pair.second->children.size() >= 2)
                {
                    count++;
                }
            }
            std::cout << count << "명" << std::endl;
        }
    }

    // --- 메모리 해제 ---
    // `new` 키워드로 동적으로 할당했던 모든 KingNode 객체들을 `delete`로 해제합니다.
    // 이를 통해 메모리 누수(memory leak)를 방지합니다.
    for (auto& pair : kings)
    {
        delete pair.second;
    }

    // 사용이 끝난 파일 스트림을 닫습니다.
    dynastyFile.close();
    quizFile.close();

    return 0; // 프로그램 정상 종료
}