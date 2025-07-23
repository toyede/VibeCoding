#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <locale.h>

// 표준 라이브러리의 특정 기능을 사용하기 위해 선언
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::stringstream;

// 전방 선언
struct Node;

// 자식 노드들을 관리하기 위한 연결 리스트 구조체
struct ChildNode {
    Node* king_node;    // 자식 왕 노드를 가리키는 포인터
    ChildNode* next; // 다음 자식 노드를 가리키는 포인터
};

// 왕 한 명의 정보를 담는 트리 노드 구조체
struct Node {
    string name;          // 왕의 이름
    Node* parent;         // 부모 노드를 가리키는 포인터
    ChildNode* children;  // 자식 노드들의 연결 리스트의 시작
    int child_count;      // 자식의 수
};

// 함수 선언
Node* createNode(const string& name);
void addChild(Node* parent, Node* child);
Node* findNode(Node* current, const string& name);
void printTree(Node* current, int depth);
void deleteTree(Node* current);
void answerQuestions(Node* root, const string& filename);
void printAllKings(Node* current);
void printAllKingsReverse(Node* current);
int countKings(Node* current);
void findDescendants(Node* startNode, bool& first);
void findKingsWithNoRoyalHeir(Node* current, bool& first);
Node* findKingWithMostRoyalHeirs(Node* root);
void findBrothers(Node* root, const string& kingName);
void printAncestors(Node* root, const string& kingName);
int countKingsWithMultipleHeirs(Node* root);
int findGenerationGap(Node* root, const string& ancestorName, const string& descendantName);


// 프로그램의 시작점
int main() {
    // 한글 입출력을 위한 로케일 설정
    setlocale(LC_ALL, "ko_KR.UTF-8");

    Node* root = nullptr;
    ifstream file("조선왕조.txt");
    string line;

    if (!file.is_open()) {
        cout << "'조선왕조.txt' 파일을 열 수 없습니다." << endl;
        return 1;
    }

    // '조선왕조.txt' 파일 읽고 트리 구성
    while (getline(file, line)) {
        // UTF-8 BOM(Byte Order Mark) 제거
        if (line.size() >= 3 && line[0] == (char)0xEF && line[1] == (char)0xBB && line[2] == (char)0xBF) {
            line = line.substr(3);
        }
        
        stringstream ss(line);
        string child_name, parent_name;
        ss >> child_name >> parent_name;

        if (parent_name.empty()) { // 부모가 없는 경우, 즉 태조
            if (root == nullptr) {
                root = createNode(child_name);
            }
        } else {
            Node* parent_node = findNode(root, parent_name);
            if (parent_node) {
                Node* child_node = findNode(root, child_name);
                if(child_node == nullptr) {
                    child_node = createNode(child_name);
                }
                addChild(parent_node, child_node);
            }
        }
    }
    file.close();

    // 생성된 트리 구조 출력
    cout << "--- 조선 왕조 트리 구조 ---" << endl;
    printTree(root, 0);
    cout << "--------------------------" << endl << endl;

    // '질문.txt' 파일의 질문에 답변
    answerQuestions(root, "질문.txt");

    // 동적 할당된 메모리 해제
    deleteTree(root);

    return 0;
}

// 새로운 왕 노드를 생성하는 함수
Node* createNode(const string& name) {
    Node* newNode = new Node();
    newNode->name = name;
    newNode->parent = nullptr;
    newNode->children = nullptr;
    newNode->child_count = 0;
    return newNode;
}

// 부모 노드에 자식 노드를 추가하는 함수
void addChild(Node* parent, Node* child) {
    child->parent = parent;
    parent->child_count++;

    ChildNode* newChildNode = new ChildNode();
    newChildNode->king_node = child;
    newChildNode->next = parent->children; // 새로운 자식을 리스트의 맨 앞에 추가
    parent->children = newChildNode;
}

// 트리에서 특정 이름을 가진 노드를 찾는 함수 (전위 순회)
Node* findNode(Node* current, const string& name) {
    if (current == nullptr) {
        return nullptr;
    }
    if (current->name == name) {
        return current;
    }

    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        Node* found = findNode(child_list->king_node, name);
        if (found) {
            return found;
        }
        child_list = child_list->next;
    }
    return nullptr;
}

// 트리의 전체 구조를 재귀적으로 출력하는 함수
void printTree(Node* current, int depth) {
    if (current == nullptr) {
        return;
    }
    for (int i = 0; i < depth; ++i) {
        cout << "  ";
    }
    cout << "-> " << current->name << endl;

    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        printTree(child_list->king_node, depth + 1);
        child_list = child_list->next;
    }
}

// 할당된 트리 메모리를 재귀적으로 해제하는 함수 (후위 순회)
void deleteTree(Node* current) {
    if (current == nullptr) {
        return;
    }
    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        ChildNode* next = child_list->next;
        deleteTree(child_list->king_node);
        delete child_list;
        child_list = next;
    }
    delete current;
}

// '질문.txt'를 읽고 각 질문에 대한 답을 처리하는 함수
void answerQuestions(Node* root, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "'" << filename << "' 파일을 열 수 없습니다." << endl;
        return;
    }

    string line;
    // 첫 줄(질문 개수) 읽기 (BOM 처리 포함)
    getline(file, line);
    if (line.size() >= 3 && line[0] == (char)0xEF && line[1] == (char)0xBB && line[2] == (char)0xBF) {
        line = line.substr(3);
    }
    
    int question_count = 0;
    try {
        question_count = stoi(line);
    } catch (...) {
        // 숫자 변환 실패 시 0으로 유지
    }

    for (int i = 1; i <= question_count; ++i) {
        if (!getline(file, line)) break;
        cout << "질문 " << i << ": " << line << endl;
        cout << "답���: ";

        if (line.find("순서대로 출력") != string::npos) {
            printAllKings(root);
        } else if (line.find("역순으로 출력") != string::npos) {
            printAllKingsReverse(root);
        } else if (line.find("모두 몇 명인가") != string::npos) {
            cout << countKings(root) << "명";
        } else if (line.find("후손은 누구누구인가") != string::npos) {
            size_t pos = line.find("의 후손은");
            if (pos != string::npos) {
                string kingPart = line.substr(0, pos);
                size_t space_pos = kingPart.rfind(' ');
                string kingName = (space_pos != string::npos) ? kingPart.substr(space_pos + 1) : kingPart;
                
                Node* startNode = findNode(root, kingName);
                if (startNode) {
                    bool first = true;
                    findDescendants(startNode, first);
                    if(first) cout << "없음";
                } else {
                    cout << "'" << kingName << "'을(를) 찾을 수 없습니다.";
                }
            }
        } else if (line.find("왕이 되지 못한 왕") != string::npos) {
            bool first = true;
            findKingsWithNoRoyalHeir(root, first);
        } else if (line.find("가장 많은 왕은 누구인가") != string::npos) {
            Node* king = findKingWithMostRoyalHeirs(root);
            if (king) {
                cout << king->name << " (" << king->child_count << "명)";
            }
        } else if (line.find("형제로 조선의 왕이 된 사람") != string::npos) {
            size_t pos = line.find("의 형제로");
            if (pos != string::npos) {
                string kingName = line.substr(0, pos);
                findBrothers(root, kingName);
            }
        } else if (line.find("직계 선조를 모두 출력") != string::npos) {
            size_t pos = line.find("의 직계 선조");
            if (pos != string::npos) {
                string kingName = line.substr(0, pos);
                printAncestors(root, kingName);
            }
        } else if (line.find("2명 이상 왕이 된 왕은 몇 명") != string::npos) {
            cout << countKingsWithMultipleHeirs(root) << "명";
        } else if (line.find("은") != string::npos && line.find("의 몇 대 후손인가") != string::npos) {
            size_t pos_eun = line.find("은 ");
            size_t pos_ui = line.find("의 ");
            if (pos_eun != string::npos && pos_ui != string::npos) {
                string descendantName = line.substr(0, pos_eun);
                string ancestorPart = line.substr(pos_eun + sizeof("은 ") - 1);
                size_t ancestor_end_pos = ancestorPart.find("의 ");
                if (ancestor_end_pos != string::npos) {
                    string ancestorName = ancestorPart.substr(0, ancestor_end_pos);
                    int gap = findGenerationGap(root, ancestorName, descendantName);
                    if (gap > 0) {
                        cout << gap << "대 후손입니다.";
                    } else {
                        cout << "관계를 찾을 수 없습니다.";
                    }
                }
            }
        } else {
            cout << "알 수 없는 질문입니다.";
        }
        cout << endl << endl;
    }
    file.close();
}

// 1. 모든 왕을 순서대로 출력 (전위 순회)
void printAllKings(Node* current) {
    if (current == nullptr) return;
    cout << current->name << " ";
    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        printAllKings(child_list->king_node);
        child_list = child_list->next;
    }
}

// 2. 모든 왕을 역순으로 출력 (후위 순회)
void printAllKingsReverse(Node* current) {
    if (current == nullptr) return;
    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        printAllKingsReverse(child_list->king_node);
        child_list = child_list->next;
    }
    cout << current->name << " ";
}

// 3. 모든 왕의 수를 세는 함수
int countKings(Node* current) {
    if (current == nullptr) return 0;
    int count = 1;
    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        count += countKings(child_list->king_node);
        child_list = child_list->next;
    }
    return count;
}

// 4. 특정 왕의 모든 후손을 출력하는 함수
void findDescendants(Node* startNode, bool& first) {
    if (startNode == nullptr) return;
    ChildNode* child_list = startNode->children;
    while (child_list != nullptr) {
        if (!first) cout << ", ";
        cout << child_list->king_node->name;
        first = false;
        findDescendants(child_list->king_node, first);
        child_list = child_list->next;
    }
}

// 5. 직계 후손이 왕이 되지 못한 왕(자식이 없는 왕)을 찾는 함수
void findKingsWithNoRoyalHeir(Node* current, bool& first) {
    if (current == nullptr) return;
    if (current->child_count == 0) {
        if (!first) cout << ", ";
        cout << current->name;
        first = false;
    }
    ChildNode* child_list = current->children;
    while (child_list != nullptr) {
        findKingsWithNoRoyalHeir(child_list->king_node, first);
        child_list = child_list->next;
    }
}

// 6. 직계 후손(자식)이 가장 많은 왕을 찾는 함수
Node* findKingWithMostRoyalHeirs(Node* root) {
    if (root == nullptr) return nullptr;

    Node* maxKing = root;
    Node* queue[100]; // 간단한 큐 구현
    int front = 0, back = 0;
    queue[back++] = root;

    while (front < back) {
        Node* current = queue[front++];
        if (current->child_count > maxKing->child_count) {
            maxKing = current;
        }
        ChildNode* child_list = current->children;
        while (child_list != nullptr) {
            queue[back++] = child_list->king_node;
            child_list = child_list->next;
        }
    }
    return maxKing;
}

// 7. 특정 왕의 형제를 찾는 함수
void findBrothers(Node* root, const string& kingName) {
    Node* kingNode = findNode(root, kingName);
    if (kingNode == nullptr || kingNode->parent == nullptr) {
        cout << "형제를 찾을 수 없습니다.";
        return;
    }

    Node* parent = kingNode->parent;
    ChildNode* child_list = parent->children;
    bool first = true;
    while (child_list != nullptr) {
        if (child_list->king_node->name != kingName) {
            if (!first) cout << ", ";
            cout << child_list->king_node->name;
            first = false;
        }
        child_list = child_list->next;
    }
    if(first) cout << "없음";
}

// 8. 특정 왕의 모든 직계 선조를 출력하는 함수
void printAncestors(Node* root, const string& kingName) {
    Node* kingNode = findNode(root, kingName);
    if (kingNode == nullptr) {
        cout << "왕을 찾을 수 없습니다.";
        return;
    }
    Node* current = kingNode->parent;
    bool first = true;
    while (current != nullptr) {
        if (!first) cout << ", ";
        cout << current->name;
        first = false;
        current = current->parent;
    }
     if(first) cout << "없음";
}

// 9. 직계 후손이 2명 이상인 왕의 수를 세는 함수
int countKingsWithMultipleHeirs(Node* root) {
    if (root == nullptr) return 0;
    
    int count = 0;
    Node* queue[100];
    int front = 0, back = 0;
    queue[back++] = root;

    while(front < back) {
        Node* current = queue[front++];
        if (current->child_count >= 2) {
            count++;
        }
        ChildNode* child_list = current->children;
        while (child_list != nullptr) {
            queue[back++] = child_list->king_node;
            child_list = child_list->next;
        }
    }
    return count;
}

// 10. 두 왕 사이의 세대 차이를 계산하는 함수
int findGenerationGap(Node* root, const string& ancestorName, const string& descendantName) {
    Node* ancestorNode = findNode(root, ancestorName);
    Node* descendantNode = findNode(root, descendantName);

    if (ancestorNode == nullptr || descendantNode == nullptr) {
        return -1;
    }

    int gap = 0;
    Node* current = descendantNode;
    while (current != nullptr && current != ancestorNode) {
        current = current->parent;
        gap++;
    }

    return (current == ancestorNode) ? gap : -1;
}
