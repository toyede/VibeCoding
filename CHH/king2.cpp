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

// Ʈ������ �̸����� ��带 ã�� �Լ�
Node* findNode(Node* root, const string& name) {
    if (!root) return nullptr;
    if (root->name == name) return root;
    for (Node* child : root->children) {
        Node* res = findNode(child, name);
        if (res) return res;
    }
    return nullptr;
}

// anc�� n�� ���� �������� Ȯ���ϴ� �Լ�
bool isDescendant(Node* anc, Node* n) {
    Node* cur = n;
    while (cur->parent) {
        if (cur->parent == anc) return true;
        cur = cur->parent;
    }
    return false;
}

int main() {
    // 1) ��������.txt �о Ʈ�� ����
    ifstream fin("��������.txt");
    string line;
    Node* root = nullptr;
    vector<Node*> kings;  // ������� ����

    while (getline(fin, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        vector<string> tokens;
        string tok;
        while (iss >> tok) tokens.push_back(tok);

        if (tokens.size() == 1) {
            // ���� (�ֻ���)
            root = new Node(tokens[0]);
            kings.push_back(root);
        }
        else if (tokens.size() == 2) {
            // "�ڽ� �θ�" ����
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

    // 2) ����.txt �а� �������� ���� ���
    ifstream qin("����.txt");
    string question;
    while (getline(qin, question)) {
        if (question.empty()) continue;

        // 1. ������ ���� ������� ���
        if (question == "������ ���� ������� ����Ͻÿ�.") {
            for (Node* n : kings) cout << n->name << " ";
            cout << '\n';

            // 2. ������ ���� �������� ���
        }
        else if (question == "������ ���� �������� ����Ͻÿ�.") {
            for (int i = (int)kings.size() - 1; i >= 0; --i) cout << kings[i]->name << " ";
            cout << '\n';

            // 3. ���� �� ��
        }
        else if (question == "������ ���� ��� �� ���ΰ�?") {
            cout << kings.size() << '\n';

            // 4. Ư�� ���� �ļ� ���
        }
        else if (question.find("������ �� �߿���") == 0
            && question.find("�� �ļ��� ���������ΰ�?") != string::npos) {
            // "������ �� �߿��� {�̸�}�� �ļ��� ���������ΰ�?"
            string p1 = "������ �� �߿���";
            string p2 = "�� �ļ��� ���������ΰ�?";
            size_t a = question.find(p1) + p1.size();
            size_t b = question.find(p2);
            string ancName = question.substr(a, b - a);
            if (!ancName.empty() && ancName[0] == ' ') ancName.erase(0, 1);
            Node* anc = findNode(root, ancName);
            for (Node* n : kings) {
                if (isDescendant(anc, n)) cout << n->name << " ";
            }
            cout << '\n';

            // 5. �ڽ� ��尡 ����(���� �ļ��� ���� ���� ����) ��
        }
        else if (question == "���� �ļ��� ���� ���� ���� ���� ���������ΰ�?") {
            for (Node* n : kings) {
                if (n->children.empty()) cout << n->name << " ";
            }
            cout << '\n';

            // 6. �ڽ��� ���� ���� ��
        }
        else if (question == "���� �ļ��� ���� �� ���� ���� ���� ���� �����ΰ�?") {
            size_t maxCnt = 0;
            for (Node* n : kings) maxCnt = max(maxCnt, n->children.size());
            for (Node* n : kings) {
                if (n->children.size() == maxCnt) cout << n->name << " ";
            }
            cout << '\n';

            // 7. ������ ���� �� ���
        }
        else if (question.find("�� ������ ������ ���� �� ����� �����ΰ�?") != string::npos) {
            string p = "�� ������ ������ ���� �� ����� �����ΰ�?";
            size_t b = question.find(p);
            string name = question.substr(0, b);
            Node* cur = findNode(root, name);
            if (cur && cur->parent) {
                for (Node* sib : cur->parent->children) {
                    if (sib != cur) cout << sib->name << " ";
                }
            }
            cout << '\n';

            // 8. Ư�� ���� ���� ���� ���
        }
        else if (question.find("�� ���� ������ ��� ����Ͻÿ�.") != string::npos) {
            string p = "�� ���� ������ ��� ����Ͻÿ�.";
            size_t b = question.find(p);
            string name = question.substr(0, b);
            Node* cur = findNode(root, name);
            while (cur->parent) {
                cout << cur->parent->name << " ";
                cur = cur->parent;
            }
            cout << '\n';

            // 9. ���� �ļ��� N�� �̻��� ���� ��
        }
        else if (question.find("���� �ļ��� ") != string::npos
            && question.find("�� �̻� ���� �� ���� �� ���ΰ�?") != string::npos) {
            string p1 = "���� �ļ��� ";
            string p2 = "�� �̻� ���� �� ���� �� ���ΰ�?";
            size_t a = question.find(p1) + p1.size();
            size_t c = question.find("�� �̻�", a);
            int thr = stoi(question.substr(a, c - a));
            int cnt = 0;
            for (Node* n : kings) if ((int)n->children.size() >= thr) cnt++;
            cout << cnt << '\n';

            //10. ���ļ� ���
        }
        else if (question.find("�� ") != string::npos
            && question.find("�� �� �� �ļ��ΰ�?") != string::npos) {
            string p1 = "�� ";
            string p2 = "�� �� �� �ļ��ΰ�?";
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
