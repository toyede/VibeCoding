#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

// 트리 노드 구조체
struct KingNode {
    string name;
    vector<KingNode*> children;
    KingNode* parent;

    KingNode(string kingName) : name(kingName), parent(nullptr) {}
};

class JoseonDynastyTree {
private:
    KingNode* root;
    map<string, KingNode*> kingMap; // 왕 이름으로 노드 찾기
    vector<string> allKings; // 모든 왕들의 순서 저장

public:
    JoseonDynastyTree() : root(nullptr) {}

    // 파일에서 데이터 읽기 (UTF-8 인코딩)
    void loadFromFile(const string& filename) {
        // UTF-8 파일 읽기를 위한 설정
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "파일을 열 수 없습니다: " << filename << endl;
            return;
        }

        // 파일 전체를 string으로 읽기
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        // BOM 제거 (UTF-8 BOM이 있는 경우)
        if (content.length() >= 3 &&
            (unsigned char)content[0] == 0xEF &&
            (unsigned char)content[1] == 0xBB &&
            (unsigned char)content[2] == 0xBF) {
            content = content.substr(3);
        }

        // 줄 단위로 분리
        istringstream contentStream(content);
        string line;

        while (getline(contentStream, line)) {
            if (line.empty()) continue;

            // 캐리지 리턴 제거
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            istringstream iss(line);
            string king, parent;
            iss >> king;

            // 왕 노드 생성 또는 가져오기
            if (kingMap.find(king) == kingMap.end()) {
                kingMap[king] = new KingNode(king);
                allKings.push_back(king);
            }

            // 부모가 있는 경우
            if (iss >> parent) {
                if (kingMap.find(parent) == kingMap.end()) {
                    kingMap[parent] = new KingNode(parent);
                }
                kingMap[king]->parent = kingMap[parent];
                kingMap[parent]->children.push_back(kingMap[king]);
            }
            else {
                // 루트 노드 (태조)
                root = kingMap[king];
            }
        }
    }

    // 트리 구조 출력
    void printTreeStructure() {
        cout << "=== Joseon Dynasty Family Tree Structure ===" << endl;
        if (root) {
            printNode(root, "", true);
        }
        cout << endl;
    }

    // 재귀적으로 트리 노드 출력
    void printNode(KingNode* node, string prefix, bool isLast) {
        if (!node) return;

        cout << prefix;
        cout << (isLast ? "└── " : "├── ");
        cout << node->name << endl;

        for (size_t i = 0; i < node->children.size(); i++) {
            bool childIsLast = (i == node->children.size() - 1);
            string newPrefix = prefix + (isLast ? "    " : "│   ");
            printNode(node->children[i], newPrefix, childIsLast);
        }
    }

    // 원본 파일 내용 출력 (UTF-8)
    void printOriginalFile(const string& filename) {
        cout << "=== " << filename << " 파일 내용 ===" << endl;

        ifstream file(filename, ios::binary);
        if (file.is_open()) {
            // 파일 전체를 string으로 읽기
            string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();

            // BOM 제거 (UTF-8 BOM이 있는 경우)
            if (content.length() >= 3 &&
                (unsigned char)content[0] == 0xEF &&
                (unsigned char)content[1] == 0xBB &&
                (unsigned char)content[2] == 0xBF) {
                content = content.substr(3);
            }

            cout << content;
            if (!content.empty() && content.back() != '\n') {
                cout << endl;
            }
        }
        cout << endl;
    }

    // Quiz 파일 내용 출력 (UTF-8)
    void printQuizFile(const string& filename) {
        cout << "=== " << filename << " 파일 내용 ===" << endl;

        ifstream file(filename, ios::binary);
        if (file.is_open()) {
            // 파일 전체를 string으로 읽기
            string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();

            // BOM 제거 (UTF-8 BOM이 있는 경우)
            if (content.length() >= 3 &&
                (unsigned char)content[0] == 0xEF &&
                (unsigned char)content[1] == 0xBB &&
                (unsigned char)content[2] == 0xBF) {
                content = content.substr(3);
            }

            cout << content;
            if (!content.empty() && content.back() != '\n') {
                cout << endl;
            }
        }
        cout << "=====================================" << endl << endl;
    }

    // 1. 조선의 왕을 순서대로 출력
    void printKingsInOrder() {
        cout << "1. 조선의 왕을 순서대로 출력하시오." << endl;
        for (size_t i = 0; i < allKings.size(); i++) {
            cout << allKings[i];
            if (i < allKings.size() - 1) cout << ", ";
        }
        cout << endl << endl;
    }

    // 2. 조선의 왕을 역순으로 출력
    void printKingsReverse() {
        cout << "2. 조선의 왕을 역순으로 출력하시오." << endl;
        for (int i = allKings.size() - 1; i >= 0; i--) {
            cout << allKings[i];
            if (i > 0) cout << ", ";
        }
        cout << endl << endl;
    }

    // 3. 조선의 왕은 모두 몇 명인가?
    void printTotalKings() {
        cout << "3. 조선의 왕은 모두 몇 명인가?" << endl;
        cout << allKings.size() << "명" << endl << endl;
    }

    // 4. 인조의 후손 찾기
    void printInjoDescendants() {
        cout << "4. 조선의 왕 중에서 인조의 후손은 누구누구인가?" << endl;
        vector<string> descendants;
        if (kingMap.find("인조") != kingMap.end()) {
            findDescendants(kingMap["인조"], descendants);
        }

        for (size_t i = 0; i < descendants.size(); i++) {
            cout << descendants[i];
            if (i < descendants.size() - 1) cout << ", ";
        }
        cout << endl << endl;
    }

    // 후손 찾기 헬퍼 함수
    void findDescendants(KingNode* node, vector<string>& descendants) {
        for (KingNode* child : node->children) {
            descendants.push_back(child->name);
            findDescendants(child, descendants);
        }
    }

    // 5. 직계 후손이 왕이 되지 못한 왕
    void printKingsWithoutDirectSuccessor() {
        cout << "5. 직계 후손이 왕이 되지 못한 왕은 누구누구인가?" << endl;
        vector<string> kingsWithoutSuccessor;

        for (const auto& pair : kingMap) {
            if (pair.second->children.empty()) {
                kingsWithoutSuccessor.push_back(pair.first);
            }
        }

        for (size_t i = 0; i < kingsWithoutSuccessor.size(); i++) {
            cout << kingsWithoutSuccessor[i];
            if (i < kingsWithoutSuccessor.size() - 1) cout << ", ";
        }
        cout << endl << endl;
    }

    // 6. 직계 후손이 왕이 된 수가 가장 많은 왕
    void printKingWithMostSuccessors() {
        cout << "6. 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?" << endl;
        string maxKing = "";
        int maxCount = 0;

        for (const auto& pair : kingMap) {
            vector<string> descendants;
            findDescendants(pair.second, descendants);
            if ((int)descendants.size() > maxCount) {
                maxCount = descendants.size();
                maxKing = pair.first;
            }
        }

        cout << maxKing << " (" << maxCount << "명의 후손)" << endl << endl;
    }

    // 7. 정종의 형제로 조선의 왕이 된 사람
    void printJeongjongBrothers() {
        cout << "7. 정종의 형제로 조선의 왕이 된 사람은 누구인가?" << endl;
        if (kingMap.find("정종") != kingMap.end() && kingMap["정종"]->parent) {
            KingNode* parent = kingMap["정종"]->parent;
            for (KingNode* sibling : parent->children) {
                if (sibling->name != "정종") {
                    cout << sibling->name << " ";
                }
            }
        }
        cout << endl << endl;
    }

    // 8. 순종의 직계 선조 출력
    void printSunjongAncestors() {
        cout << "8. 순종의 직계 선조를 모두 출력하시오." << endl;
        vector<string> ancestors;
        if (kingMap.find("순종") != kingMap.end()) {
            KingNode* current = kingMap["순종"]->parent;
            while (current) {
                ancestors.push_back(current->name);
                current = current->parent;
            }
        }

        reverse(ancestors.begin(), ancestors.end());
        for (size_t i = 0; i < ancestors.size(); i++) {
            cout << ancestors[i];
            if (i < ancestors.size() - 1) cout << " → ";
        }
        cout << endl << endl;
    }

    // 9. 직계 후손이 2명 이상 왕이 된 왕의 수
    void printKingsWithMultipleSuccessors() {
        cout << "9. 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가?" << endl;
        int count = 0;

        for (const auto& pair : kingMap) {
            vector<string> descendants;
            findDescendants(pair.second, descendants);
            if (descendants.size() >= 2) {
                count++;
            }
        }

        cout << count << "명" << endl << endl;
    }

    // 10. 예종은 태종의 몇 대 후손인가?
    void printGenerationsBetween() {
        cout << "10. 예종은 태종의 몇 대 후손인가?" << endl;
        int generations = findGenerationsBetween("예종", "태종");
        if (generations > 0) {
            cout << generations << "대 후손" << endl;
        }
        else {
            cout << "직계 관계가 아닙니다." << endl;
        }
        cout << endl;
    }

    // 두 왕 사이의 세대 수 계산
    int findGenerationsBetween(const string& descendant, const string& ancestor) {
        if (kingMap.find(descendant) == kingMap.end() ||
            kingMap.find(ancestor) == kingMap.end()) {
            return -1;
        }

        KingNode* current = kingMap[descendant];
        int generations = 0;

        while (current->parent) {
            current = current->parent;
            generations++;
            if (current->name == ancestor) {
                return generations;
            }
        }

        return -1; // 직계 관계 아님
    }

    // 모든 퀴즈 실행
    void runAllQuizzes() {
        printKingsInOrder();
        printKingsReverse();
        printTotalKings();
        printInjoDescendants();
        printKingsWithoutDirectSuccessor();
        printKingWithMostSuccessors();
        printJeongjongBrothers();
        printSunjongAncestors();
        printKingsWithMultipleSuccessors();
        printGenerationsBetween();
    }

    // 소멸자
    ~JoseonDynastyTree() {
        for (auto& pair : kingMap) {
            delete pair.second;
        }
    }
};

int main() {
    // 한국어 출력을 위한 콘솔 설정
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 로케일 설정 추가
    locale::global(locale(""));

    JoseonDynastyTree tree;

    // 데이터 로드
    tree.loadFromFile("조선왕조.txt");

    // 트리 구조 출력
    tree.printTreeStructure();

    // 원본 파일 내용 출력
    tree.printOriginalFile("조선왕조.txt");

    // Quiz 파일 내용 출력
    tree.printQuizFile("Quiz.txt");

    // 모든 퀴즈 실행
    tree.runAllQuizzes();

    return 0;
}