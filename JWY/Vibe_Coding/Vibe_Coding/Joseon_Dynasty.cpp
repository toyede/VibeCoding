#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

// 트리 노드 클래스
class TreeNode {
public:
    string name;
    TreeNode* parent;
    vector<TreeNode*> children;

    TreeNode(string n) : name(n), parent(nullptr) {}

    void addChild(TreeNode* child) {
        children.push_back(child);
        child->parent = this;
    }
};

class JoseonDynastyTree {
private:
    TreeNode* root;
    map<string, TreeNode*> nodeMap;
    vector<string> kingOrder;
    set<string> validKings;
    vector<string> quizQuestions;

    // 조선왕조 데이터 (하드코딩 - 고정)
    vector<pair<string, string>> joseonData = {
        {"태조", ""},
        {"정종", "태조"},
        {"태종", "태조"},
        {"세종", "태종"},
        {"문종", "세종"},
        {"단종", "문종"},
        {"세조", "세종"},
        {"예종", "세조"},
        {"성종", "세조"},
        {"연산군", "성종"},
        {"중종", "성종"},
        {"인종", "중종"},
        {"명종", "중종"},
        {"선조", "중종"},
        {"광해군", "선조"},
        {"인조", "선조"},
        {"효종", "인조"},
        {"현종", "효종"},
        {"숙종", "현종"},
        {"경종", "숙종"},
        {"영조", "숙종"},
        {"정조", "영조"},
        {"순조", "정조"},
        {"헌종", "순조"},
        {"철종", "영조"},
        {"고종", "인조"},
        {"순종", "고종"}
    };

public:
    JoseonDynastyTree() : root(nullptr) {}

    ~JoseonDynastyTree() {
        deleteTree(root);
    }

    void deleteTree(TreeNode* node) {
        if (node == nullptr) return;
        for (TreeNode* child : node->children) {
            deleteTree(child);
        }
        delete node;
    }

    // 하드코딩된 데이터로 트리 구성
    void buildTree() {
        for (const auto& pair : joseonData) {
            string king = pair.first;
            string parent = pair.second;

            // 유효한 왕 이름 목록에 추가
            validKings.insert(king);

            if (parent.empty()) {
                // 루트 노드 (태조)
                root = new TreeNode(king);
                nodeMap[king] = root;
                kingOrder.push_back(king);
            }
            else {
                // 부모 노드가 없으면 생성
                if (nodeMap.find(parent) == nodeMap.end()) {
                    nodeMap[parent] = new TreeNode(parent);
                }

                // 자식 노드 생성 및 연결
                TreeNode* childNode = new TreeNode(king);
                nodeMap[king] = childNode;
                nodeMap[parent]->addChild(childNode);
                kingOrder.push_back(king);
            }
        }
    }

    // Quiz.txt 파일 읽기 (ANSI 인코딩)
    void loadQuizFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Quiz.txt 파일을 열 수 없습니다." << endl;
            return;
        }

        string line;
        bool firstLine = true;

        while (getline(file, line)) {
            if (line.empty()) continue;

            if (firstLine) {
                firstLine = false;
                continue; // 첫 줄은 문제 개수이므로 스킵
            }

            quizQuestions.push_back(line);
        }
        file.close();
    }

    // 문자열에서 왕 이름 추출 함수
    string extractKingFromQuestion(const string& question) {
        // 가능한 모든 왕 이름을 길이 순으로 정렬 (긴 이름부터 검사)
        vector<string> sortedKings(validKings.begin(), validKings.end());
        sort(sortedKings.begin(), sortedKings.end(), [](const string& a, const string& b) {
            return a.length() > b.length();
        });

        for (const string& king : sortedKings) {
            if (question.find(king) != string::npos) {
                return king;
            }
        }
        return "";
    }

    // 두 왕 이름 추출 (몇 대 후손 문제용)
    pair<string, string> extractTwoKingsFromQuestion(const string& question) {
        size_t eunPos = question.find("은 ");
        size_t uiPos = question.find("의 몇 대");

        if (eunPos == string::npos || uiPos == string::npos) {
            return make_pair("", "");
        }

        string descendantPart = question.substr(0, eunPos);
        string ancestorPart = question.substr(eunPos + 2, uiPos - (eunPos + 2));

        string descendant = "";
        string ancestor = "";

        for (const string& king : validKings) {
            if (descendantPart.find(king) != string::npos) {
                descendant = king;
            }
            if (ancestorPart.find(king) != string::npos) {
                ancestor = king;
            }
        }

        return make_pair(descendant, ancestor);
    }

    // 트리 구조 출력
    void printTree() {
        cout << "조선왕조 트리 구조:" << endl;
        printTreeHelper(root, "", true);
        cout << endl;
    }

    void printTreeHelper(TreeNode* node, string prefix, bool isLast) {
        if (node == nullptr) return;

        cout << prefix;
        cout << (isLast ? "└── " : "├── ");
        cout << node->name << endl;

        for (size_t i = 0; i < node->children.size(); i++) {
            bool isChildLast = (i == node->children.size() - 1);
            string newPrefix = prefix + (isLast ? "    " : "│   ");
            printTreeHelper(node->children[i], newPrefix, isChildLast);
        }
    }

    // 조선왕조 데이터 출력
    void printJoseonData() {
        cout << "조선왕조.txt 내용:" << endl;
        for (const auto& pair : joseonData) {
            if (pair.second.empty()) {
                cout << pair.first << endl;
            }
            else {
                cout << pair.first << " " << pair.second << endl;
            }
        }
        cout << endl;
    }

    // Quiz.txt 파일 내용 출력
    void printQuizFile(const string& filename) {
        cout << "Quiz.txt 내용:" << endl;
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        }
        else {
            cout << "파일을 열 수 없습니다." << endl;
        }
        cout << endl;
    }

    // 문제 해결 함수들
    void solveKingsInOrder() {
        cout << "답: ";
        for (const string& king : kingOrder) {
            cout << king << " ";
        }
        cout << endl;
    }

    void solveKingsInReverse() {
        cout << "답: ";
        for (int i = kingOrder.size() - 1; i >= 0; i--) {
            cout << kingOrder[i] << " ";
        }
        cout << endl;
    }

    void solveTotalKings() {
        cout << "답: " << kingOrder.size() << "명" << endl;
    }

    void solveDescendants(const string& kingName) {
        cout << "답: ";
        if (nodeMap.find(kingName) != nodeMap.end()) {
            vector<string> descendants;
            getDescendants(nodeMap[kingName], descendants);
            if (descendants.empty()) {
                cout << "후손이 없습니다.";
            }
            else {
                for (const string& desc : descendants) {
                    cout << desc << " ";
                }
            }
        }
        else {
            cout << "해당 왕을 찾을 수 없습니다.";
        }
        cout << endl;
    }

    void getDescendants(TreeNode* node, vector<string>& descendants) {
        for (TreeNode* child : node->children) {
            descendants.push_back(child->name);
            getDescendants(child, descendants);
        }
    }

    void solveKingsWithoutDirectSuccessors() {
        cout << "답: ";
        for (const auto& pair : nodeMap) {
            if (pair.second->children.empty()) {
                cout << pair.first << " ";
            }
        }
        cout << endl;
    }

    void solveKingWithMostDescendants() {
        string maxKing;
        int maxCount = 0;

        for (const auto& pair : nodeMap) {
            vector<string> descendants;
            getDescendants(pair.second, descendants);
            if (descendants.size() > maxCount) {
                maxCount = descendants.size();
                maxKing = pair.first;
            }
        }

        cout << "답: " << maxKing << " (" << maxCount << "명)" << endl;
    }

    void solveSiblings(const string& kingName) {
        cout << "답: ";
        if (nodeMap.find(kingName) != nodeMap.end() && nodeMap[kingName]->parent != nullptr) {
            TreeNode* parent = nodeMap[kingName]->parent;
            bool hasSiblings = false;
            for (TreeNode* sibling : parent->children) {
                if (sibling->name != kingName) {
                    cout << sibling->name << " ";
                    hasSiblings = true;
                }
            }
            if (!hasSiblings) {
                cout << "형제가 없습니다.";
            }
        }
        else {
            cout << "해당 왕을 찾을 수 없거나 부모가 없습니다.";
        }
        cout << endl;
    }

    void solveAncestors(const string& kingName) {
        cout << "답: ";
        if (nodeMap.find(kingName) != nodeMap.end()) {
            vector<string> ancestors;
            TreeNode* current = nodeMap[kingName]->parent;
            while (current != nullptr) {
                ancestors.push_back(current->name);
                current = current->parent;
            }
            reverse(ancestors.begin(), ancestors.end());
            if (ancestors.empty()) {
                cout << "선조가 없습니다.";
            }
            else {
                for (const string& ancestor : ancestors) {
                    cout << ancestor << " ";
                }
            }
        }
        else {
            cout << "해당 왕을 찾을 수 없습니다.";
        }
        cout << endl;
    }

    void solveKingsWithMultipleSuccessors() {
        int count = 0;
        for (const auto& pair : nodeMap) {
            if (pair.second->children.size() >= 2) {
                count++;
            }
        }
        cout << "답: " << count << "명" << endl;
    }

    void solveGenerationDifference(const string& descendant, const string& ancestor) {
        cout << "답: ";
        if (nodeMap.find(ancestor) != nodeMap.end() && nodeMap.find(descendant) != nodeMap.end()) {
            int generation = getGenerationDifference(nodeMap[ancestor], nodeMap[descendant]);
            if (generation > 0) {
                cout << generation << "대 후손";
            }
            else {
                cout << "후손 관계가 아님";
            }
        }
        else {
            cout << "해당 왕을 찾을 수 없습니다.";
        }
        cout << endl;
    }

    int getGenerationDifference(TreeNode* ancestor, TreeNode* descendant) {
        return getGenerationHelper(ancestor, descendant, 0);
    }

    int getGenerationHelper(TreeNode* current, TreeNode* target, int depth) {
        if (current == target) return depth;

        for (TreeNode* child : current->children) {
            int result = getGenerationHelper(child, target, depth + 1);
            if (result > 0) return result;
        }
        return 0;
    }

    // Quiz.txt에서 읽은 문제들을 실제로 해결
    void solveQuiz() {
        int questionNum = 1;

        for (const string& question : quizQuestions) {
            cout << questionNum << ". " << question << endl;

            // 문제 유형별로 분기 처리 (실제 문제 텍스트 기반)
            if (question.find("순서대로 출력하시오") != string::npos) {
                solveKingsInOrder();
            }
            else if (question.find("역순으로 출력하시오") != string::npos) {
                solveKingsInReverse();
            }
            else if (question.find("모두 몇 명인가") != string::npos) {
                solveTotalKings();
            }
            else if (question.find("의 후손은") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveDescendants(kingName);
                }
                else {
                    cout << "답: 왕 이름을 찾을 수 없습니다." << endl;
                }
            }
            else if (question.find("직계 후손이 왕이 되지 못한") != string::npos) {
                solveKingsWithoutDirectSuccessors();
            }
            else if (question.find("직계 후손이 왕이 된 수가 가장 많은") != string::npos) {
                solveKingWithMostDescendants();
            }
            else if (question.find("의 형제로") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveSiblings(kingName);
                }
                else {
                    cout << "답: 왕 이름을 찾을 수 없습니다." << endl;
                }
            }
            else if (question.find("의 직계 선조를") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveAncestors(kingName);
                }
                else {
                    cout << "답: 왕 이름을 찾을 수 없습니다." << endl;
                }
            }
            else if (question.find("직계 후손이 2명 이상") != string::npos) {
                solveKingsWithMultipleSuccessors();
            }
            else if (question.find("몇 대 후손인가") != string::npos) {
                pair<string, string> kings = extractTwoKingsFromQuestion(question);
                if (!kings.first.empty() && !kings.second.empty()) {
                    solveGenerationDifference(kings.first, kings.second);
                }
                else {
                    cout << "답: 왕 이름을 찾을 수 없습니다." << endl;
                }
            }
            else {
                cout << "답: 알 수 없는 문제 유형입니다." << endl;
            }

            cout << endl;
            questionNum++;
        }
    }
};

int main() {
    JoseonDynastyTree tree;

    // 조선왕조 데이터로 트리 구성
    tree.buildTree();

    // Quiz.txt에서 실제 문제 읽기
    tree.loadQuizFromFile("Quiz.txt");

    // 파일 내용 출력
    tree.printQuizFile("Quiz.txt");
    cout << "====================================" << endl;

    tree.printJoseonData();
    cout << "====================================" << endl;

    // 트리 구조 출력
    tree.printTree();
    cout << "====================================" << endl;

    // Quiz.txt에서 읽은 문제들을 실제로 해결
    tree.solveQuiz();

    return 0;
}