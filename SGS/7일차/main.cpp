// 이 파일은 EUC-KR 인코딩으로 저장해야 합니다.
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// 노드 구조체: 왕 이름, 부모, 첫째 자식, 다음 형제 포인터
struct Node {
    string name;
    Node* parent;
    Node* firstChild;
    Node* nextSibling;
};

// 최대 노드 수
const int MAX_NODES = 100;
Node nodes[MAX_NODES];
int nodeCount = 0;

// 이름으로 노드를 찾거나, 없으면 새로 생성
Node* findOrCreate(const string& name) {
    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].name == name) return &nodes[i];
    }
    nodes[nodeCount].name = name;
    nodes[nodeCount].parent = nullptr;
    nodes[nodeCount].firstChild = nullptr;
    nodes[nodeCount].nextSibling = nullptr;
    return &nodes[nodeCount++];
}

// **질문용**: 이름이 이미 있는 노드를 찾고, 없으면 nullptr 리턴
Node* findNode(const string& name) {
    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].name == name) return &nodes[i];
    }
    return nullptr;
}

// 부모-자식 관계를 트리에 추가
void addRelation(const string& childName, const string& parentName) {
    Node* child = findOrCreate(childName);
    Node* parent = findOrCreate(parentName);
    child->parent = parent;
    if (!parent->firstChild) {
        parent->firstChild = child;
    }
    else {
        Node* cur = parent->firstChild;
        while (cur->nextSibling) cur = cur->nextSibling;
        cur->nextSibling = child;
    }
}

// 자식 수 계산
int countChildren(Node* node) {
    int cnt = 0;
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        ++cnt;
    return cnt;
}

// 서브트리 예쁘게 출력 헬퍼 (printTreePretty에서 호출)
void printSubtree(Node* node, const string& prefix, bool isLast) {
    if (!node) return;
    cout << prefix << (isLast ? "└─" : "├─") << node->name << "\n";
    string newPrefix = prefix + (isLast ? "   " : "│  ");
    for (Node* c = node->firstChild; c; c = c->nextSibling) {
        bool last = (c->nextSibling == nullptr);
        printSubtree(c, newPrefix, last);
    }
}

// 트리 전체 예쁘게 출력
void printTreePretty(Node* root) {
    if (!root) return;
    cout << root->name << "\n";
    for (Node* c = root->firstChild; c; c = c->nextSibling) {
        bool last = (c->nextSibling == nullptr);
        printSubtree(c, "", last);
    }
}

// pre-order 순으로 이름을 배열에 채움
void fillOrder(Node* node, string order[], int& idx) {
    if (!node) return;
    order[idx++] = node->name;
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        fillOrder(c, order, idx);
}

// 배열을 역순으로 출력
void printReverse(const string order[], int cnt) {
    for (int i = cnt - 1; i >= 0; --i)
        cout << order[i] << " ";
}

// 주어진 노드의 모든 후손(pre-order) 출력
void printDescendants(Node* node) {
    if (!node) return;
    for (Node* c = node->firstChild; c; c = c->nextSibling) {
        cout << c->name << " ";
        printDescendants(c);
    }
}

// 자식이 없는 노드(직계 후손이 왕이 되지 못한 왕) 출력
void printLeaves(Node* node) {
    if (!node) return;
    if (!node->firstChild) {
        cout << node->name << " ";
    }
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        printLeaves(c);
}

// 특정 노드의 직계 선조를 루트까지 출력
void printAncestors(Node* node) {
    Node* p = node->parent;
    while (p) {
        cout << p->name << " ";
        p = p->parent;
    }
}

// 후손에서 조상까지 세대 거리 계산
int generationDistance(Node* descendant, const string& ancestorName) {
    int dist = 0;
    Node* p = descendant;
    while (p->parent) {
        p = p->parent;
        ++dist;
        if (p->name == ancestorName) return dist;
    }
    return -1;
}

int main() {
    // 1) '조선왕조.txt' 읽어 트리 구성
    ifstream fin("조선왕조.txt");
    string line;
    getline(fin, line);
    findOrCreate(line);  // 루트
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int sp = line.find(' ');
        string child = line.substr(0, sp);
        string parent = line.substr(sp + 1);
        addRelation(child, parent);
    }
    fin.close();

    // 루트 찾기
    Node* root = nullptr;
    for (int i = 0; i < nodeCount; ++i) {
        if (!nodes[i].parent) {
            root = &nodes[i];
            break;
        }
    }

    // 예쁜 트리 출력
    printTreePretty(root);

    // pre-order 이름 배열 준비
    string order[MAX_NODES];
    int idx = 0;
    fillOrder(root, order, idx);

    // 2) '질문.txt' 읽고 동적 처리
    ifstream qfin("질문.txt");
    int Q;
    qfin >> Q;
    getline(qfin, line);  // 개행 제거

    for (int qi = 0; qi < Q; ++qi) {
        getline(qfin, line);
        bool answered = false;

        // 1) 순서대로
        if (line.find("순서대로") != string::npos) {
            for (int i = 0; i < idx; ++i) cout << order[i] << " ";
            answered = true;
        }
        // 2) 역순으로
        else if (line.find("역순") != string::npos) {
            printReverse(order, idx);
            answered = true;
        }
        // 3) 전체 인원
        else if (line.find("모두 몇") != string::npos) {
            cout << nodeCount;
            answered = true;
        }
        // 4) “X의 후손은”
        else if (line.find("의 후손") != string::npos && line.find("몇") == string::npos) {
            size_t pos = line.find("의 후손");
            size_t start = line.rfind(' ', pos) + 1;
            string name = line.substr(start, pos - start);
            Node* nd = findNode(name);
            if (nd && nd->firstChild) {
                printDescendants(nd);
            }
            else {
                cout << "답이 없습니다.";
            }
            answered = true;
        }
        // 5) “되지 못한 왕” (자식 없는 왕)
        else if (line.find("되지 못한") != string::npos) {
            printLeaves(root);
            answered = true;
        }
        // 6) “가장 많은” (자식 수 최대)
        else if (line.find("가장 많은") != string::npos) {
            int maxC = 0;
            string who;
            for (int i = 0; i < nodeCount; ++i) {
                int c = countChildren(&nodes[i]);
                if (c > maxC) {
                    maxC = c;
                    who = nodes[i].name;
                }
            }
            if (maxC > 0) cout << who;
            else          cout << "답이 없습니다.";
            answered = true;
        }
        // 7) “X의 형제”
        else if (line.find("의 형제") != string::npos) {
            size_t pos = line.find("의 형제");
            size_t start = line.rfind(' ', pos) + 1;
            string name = line.substr(start, pos - start);
            Node* nd = findNode(name);
            if (nd && nd->parent) {
                bool any = false;
                for (Node* c = nd->parent->firstChild; c; c = c->nextSibling) {
                    if (c != nd) {
                        cout << c->name << " ";
                        any = true;
                    }
                }
                if (!any) cout << "답이 없습니다.";
            }
            else {
                cout << "답이 없습니다.";
            }
            answered = true;
        }
        // 8) “X의 직계 선조”
        else if (line.find("직계 선조") != string::npos) {
            size_t pos = line.find("의 직계 선조");
            size_t start = line.rfind(' ', pos) + 1;
            string name = line.substr(start, pos - start);
            Node* nd = findNode(name);
            if (nd && nd->parent) {
                printAncestors(nd);
            }
            else {
                cout << "답이 없습니다.";
            }
            answered = true;
        }
        // 9) “직계 후손이 N명 이상 … 몇 명인가”
        else if (line.find("직계 후손이") != string::npos && line.find("몇") != string::npos) {
            // 숫자 추출
            size_t p0 = line.find("직계 후손이") + string("직계 후손이").length();
            while (p0 < line.size() && line[p0] == ' ') ++p0;
            size_t p1 = p0;
            while (p1 < line.size() && line[p1] >= '0' && line[p1] <= '9') ++p1;
            int N = stoi(line.substr(p0, p1 - p0));
            int cnt = 0;
            for (int i = 0; i < nodeCount; ++i) {
                if (countChildren(&nodes[i]) >= N) ++cnt;
            }
            cout << cnt;
            answered = true;
        }
        // 10) “A은 B의 몇 대 후손인가”
        else if (line.find("몇 대 후손") != string::npos) {
            size_t p_eun = line.find("은");
            string desc = line.substr(0, p_eun);
            size_t p_ui = line.find("의", p_eun + 1);
            string anc = line.substr(p_eun + 2, p_ui - (p_eun + 2));
            Node* ndDesc = findNode(desc);
            Node* ndAnc = findNode(anc);
            if (ndDesc && ndAnc) {
                int d = generationDistance(ndDesc, ndAnc->name);
                if (d >= 0) cout << d;
                else        cout << "답이 없습니다.";
            }
            else {
                cout << "답이 없습니다.";
            }
            answered = true;
        }

        if (!answered) {
            cout << "답을 처리할 수 없습니다.";
        }
        cout << "\n";
    }

    return 0;
}