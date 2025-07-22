#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Node {
    string name;
    Node* parent;
    vector<Node*> children;
    Node(const string& n) : name(n), parent(nullptr) {}
};

// 트리에서 이름으로 노드를 찾는 함수
Node* findNode(Node* root, const string& name) {
    if (!root) return nullptr;
    if (root->name == name) return root;
    for (Node* child : root->children) {
        Node* res = findNode(child, name);
        if (res) return res;
    }
    return nullptr;
}

// anc가 n의 직계 조상인지 확인하는 함수
bool isDescendant(Node* anc, Node* n) {
    Node* cur = n;
    while (cur->parent) {
        if (cur->parent == anc) return true;
        cur = cur->parent;
    }
    return false;
}

int main() {
    // 1) 조선왕조.txt 읽어서 트리 구성
    ifstream fin("조선왕조.txt");
    string line;
    Node* root = nullptr;
    vector<Node*> kings;  // 순서대로 저장

    while (getline(fin, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        vector<string> tokens;
        string tok;
        while (iss >> tok) tokens.push_back(tok);

        if (tokens.size() == 1) {
            // 태조 (최상위)
            root = new Node(tokens[0]);
            kings.push_back(root);
        }
        else if (tokens.size() == 2) {
            // "자식 부모" 형식
            string childName = tokens[0];
            string parentName = tokens[1];
            Node* parent = findNode(root, parentName);
            Node* child = new Node(childName);
            child->parent = parent;
            parent->children.push_back(child);
            kings.push_back(child);
        }
    }
    fin.close();

    // 2) 퀴즈.txt 읽고 질문마다 정답 출력
    ifstream qin("퀴즈.txt");
    string question;
    while (getline(qin, question)) {
        if (question.empty()) continue;

        // 1. 조선의 왕을 순서대로 출력
        if (question == "조선의 왕을 순서대로 출력하시오.") {
            for (Node* n : kings) cout << n->name << " ";
            cout << '\n';

            // 2. 조선의 왕을 역순으로 출력
        }
        else if (question == "조선의 왕을 역순으로 출력하시오.") {
            for (int i = (int)kings.size() - 1; i >= 0; --i) cout << kings[i]->name << " ";
            cout << '\n';

            // 3. 왕의 총 수
        }
        else if (question == "조선의 왕은 모두 몇 명인가?") {
            cout << kings.size() << '\n';

            // 4. 특정 왕의 후손 출력
        }
        else if (question.find("조선의 왕 중에서") == 0
            && question.find("의 후손은 누구누구인가?") != string::npos) {
            // "조선의 왕 중에서 {이름}의 후손은 누구누구인가?"
            string p1 = "조선의 왕 중에서";
            string p2 = "의 후손은 누구누구인가?";
            size_t a = question.find(p1) + p1.size();
            size_t b = question.find(p2);
            string ancName = question.substr(a, b - a);
            if (!ancName.empty() && ancName[0] == ' ') ancName.erase(0, 1);
            Node* anc = findNode(root, ancName);
            for (Node* n : kings) {
                if (isDescendant(anc, n)) cout << n->name << " ";
            }
            cout << '\n';

            // 5. 자식 노드가 없는(직계 후손이 왕이 되지 못한) 왕
        }
        else if (question == "직계 후손이 왕이 되지 못한 왕은 누구누구인가?") {
            for (Node* n : kings) {
                if (n->children.empty()) cout << n->name << " ";
            }
            cout << '\n';

            // 6. 자식이 가장 많은 왕
        }
        else if (question == "직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?") {
            size_t maxCnt = 0;
            for (Node* n : kings) maxCnt = max(maxCnt, n->children.size());
            for (Node* n : kings) {
                if (n->children.size() == maxCnt) cout << n->name << " ";
            }
            cout << '\n';

            // 7. 형제로 왕이 된 사람
        }
        else if (question.find("의 형제로 조선의 왕이 된 사람은 누구인가?") != string::npos) {
            string p = "의 형제로 조선의 왕이 된 사람은 누구인가?";
            size_t b = question.find(p);
            string name = question.substr(0, b);
            Node* cur = findNode(root, name);
            if (cur && cur->parent) {
                for (Node* sib : cur->parent->children) {
                    if (sib != cur) cout << sib->name << " ";
                }
            }
            cout << '\n';

            // 8. 특정 왕의 직계 선조 출력
        }
        else if (question.find("의 직계 선조를 모두 출력하시오.") != string::npos) {
            string p = "의 직계 선조를 모두 출력하시오.";
            size_t b = question.find(p);
            string name = question.substr(0, b);
            Node* cur = findNode(root, name);
            while (cur->parent) {
                cout << cur->parent->name << " ";
                cur = cur->parent;
            }
            cout << '\n';

            // 9. 직계 후손이 N명 이상인 왕의 수
        }
        else if (question.find("직계 후손이 ") != string::npos
            && question.find("명 이상 왕이 된 왕은 몇 명인가?") != string::npos) {
            string p1 = "직계 후손이 ";
            string p2 = "명 이상 왕이 된 왕은 몇 명인가?";
            size_t a = question.find(p1) + p1.size();
            size_t c = question.find("명 이상", a);
            int thr = stoi(question.substr(a, c - a));
            int cnt = 0;
            for (Node* n : kings) if ((int)n->children.size() >= thr) cnt++;
            cout << cnt << '\n';

            //10. 대후손 계산
        }
        else if (question.find("은 ") != string::npos
            && question.find("의 몇 대 후손인가?") != string::npos) {
            string p1 = "은 ";
            string p2 = "의 몇 대 후손인가?";
            size_t a = question.find(p1);
            string desc = question.substr(0, a);
            Node* dnode = findNode(root, desc);
            size_t b = question.find(p2, a + p1.size());
            string anc = question.substr(a + p1.size(), b - (a + p1.size()));
            Node* anode = findNode(root, anc);
            int gen = 0;
            Node* cur = dnode;
            while (cur->parent && cur->parent != anode) {
                gen++; cur = cur->parent;
            }
            if (cur->parent == anode) gen++;
            cout << gen << '\n';
        }
    }
    qin.close();
    return 0;
}
