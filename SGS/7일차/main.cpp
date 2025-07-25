// �� ������ EUC-KR ���ڵ����� �����ؾ� �մϴ�.
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ��� ����ü: �� �̸�, �θ�, ù° �ڽ�, ���� ���� ������
struct Node {
    string name;
    Node* parent;
    Node* firstChild;
    Node* nextSibling;
};

// �ִ� ��� ��
const int MAX_NODES = 100;
Node nodes[MAX_NODES];
int nodeCount = 0;

// �̸����� ��带 ã�ų�, ������ ���� ����
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

// **������**: �̸��� �̹� �ִ� ��带 ã��, ������ nullptr ����
Node* findNode(const string& name) {
    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].name == name) return &nodes[i];
    }
    return nullptr;
}

// �θ�-�ڽ� ���踦 Ʈ���� �߰�
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

// �ڽ� �� ���
int countChildren(Node* node) {
    int cnt = 0;
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        ++cnt;
    return cnt;
}

// ����Ʈ�� ���ڰ� ��� ���� (printTreePretty���� ȣ��)
void printSubtree(Node* node, const string& prefix, bool isLast) {
    if (!node) return;
    cout << prefix << (isLast ? "����" : "����") << node->name << "\n";
    string newPrefix = prefix + (isLast ? "   " : "��  ");
    for (Node* c = node->firstChild; c; c = c->nextSibling) {
        bool last = (c->nextSibling == nullptr);
        printSubtree(c, newPrefix, last);
    }
}

// Ʈ�� ��ü ���ڰ� ���
void printTreePretty(Node* root) {
    if (!root) return;
    cout << root->name << "\n";
    for (Node* c = root->firstChild; c; c = c->nextSibling) {
        bool last = (c->nextSibling == nullptr);
        printSubtree(c, "", last);
    }
}

// pre-order ������ �̸��� �迭�� ä��
void fillOrder(Node* node, string order[], int& idx) {
    if (!node) return;
    order[idx++] = node->name;
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        fillOrder(c, order, idx);
}

// �迭�� �������� ���
void printReverse(const string order[], int cnt) {
    for (int i = cnt - 1; i >= 0; --i)
        cout << order[i] << " ";
}

// �־��� ����� ��� �ļ�(pre-order) ���
void printDescendants(Node* node) {
    if (!node) return;
    for (Node* c = node->firstChild; c; c = c->nextSibling) {
        cout << c->name << " ";
        printDescendants(c);
    }
}

// �ڽ��� ���� ���(���� �ļ��� ���� ���� ���� ��) ���
void printLeaves(Node* node) {
    if (!node) return;
    if (!node->firstChild) {
        cout << node->name << " ";
    }
    for (Node* c = node->firstChild; c; c = c->nextSibling)
        printLeaves(c);
}

// Ư�� ����� ���� ������ ��Ʈ���� ���
void printAncestors(Node* node) {
    Node* p = node->parent;
    while (p) {
        cout << p->name << " ";
        p = p->parent;
    }
}

// �ļտ��� ������� ���� �Ÿ� ���
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
    // 1) '��������.txt' �о� Ʈ�� ����
    ifstream fin("��������.txt");
    string line;
    getline(fin, line);
    findOrCreate(line);  // ��Ʈ
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int sp = line.find(' ');
        string child = line.substr(0, sp);
        string parent = line.substr(sp + 1);
        addRelation(child, parent);
    }
    fin.close();

    // ��Ʈ ã��
    Node* root = nullptr;
    for (int i = 0; i < nodeCount; ++i) {
        if (!nodes[i].parent) {
            root = &nodes[i];
            break;
        }
    }

    // ���� Ʈ�� ���
    printTreePretty(root);

    // pre-order �̸� �迭 �غ�
    string order[MAX_NODES];
    int idx = 0;
    fillOrder(root, order, idx);

    // 2) '����.txt' �а� ���� ó��
    ifstream qfin("����.txt");
    int Q;
    qfin >> Q;
    getline(qfin, line);  // ���� ����

    for (int qi = 0; qi < Q; ++qi) {
        getline(qfin, line);
        bool answered = false;

        // 1) �������
        if (line.find("�������") != string::npos) {
            for (int i = 0; i < idx; ++i) cout << order[i] << " ";
            answered = true;
        }
        // 2) ��������
        else if (line.find("����") != string::npos) {
            printReverse(order, idx);
            answered = true;
        }
        // 3) ��ü �ο�
        else if (line.find("��� ��") != string::npos) {
            cout << nodeCount;
            answered = true;
        }
        // 4) ��X�� �ļ�����
        else if (line.find("�� �ļ�") != string::npos && line.find("��") == string::npos) {
            size_t pos = line.find("�� �ļ�");
            size_t start = line.rfind(' ', pos) + 1;
            string name = line.substr(start, pos - start);
            Node* nd = findNode(name);
            if (nd && nd->firstChild) {
                printDescendants(nd);
            }
            else {
                cout << "���� �����ϴ�.";
            }
            answered = true;
        }
        // 5) ������ ���� �ա� (�ڽ� ���� ��)
        else if (line.find("���� ����") != string::npos) {
            printLeaves(root);
            answered = true;
        }
        // 6) ������ ������ (�ڽ� �� �ִ�)
        else if (line.find("���� ����") != string::npos) {
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
            else          cout << "���� �����ϴ�.";
            answered = true;
        }
        // 7) ��X�� ������
        else if (line.find("�� ����") != string::npos) {
            size_t pos = line.find("�� ����");
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
                if (!any) cout << "���� �����ϴ�.";
            }
            else {
                cout << "���� �����ϴ�.";
            }
            answered = true;
        }
        // 8) ��X�� ���� ������
        else if (line.find("���� ����") != string::npos) {
            size_t pos = line.find("�� ���� ����");
            size_t start = line.rfind(' ', pos) + 1;
            string name = line.substr(start, pos - start);
            Node* nd = findNode(name);
            if (nd && nd->parent) {
                printAncestors(nd);
            }
            else {
                cout << "���� �����ϴ�.";
            }
            answered = true;
        }
        // 9) ������ �ļ��� N�� �̻� �� �� ���ΰ���
        else if (line.find("���� �ļ���") != string::npos && line.find("��") != string::npos) {
            // ���� ����
            size_t p0 = line.find("���� �ļ���") + string("���� �ļ���").length();
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
        // 10) ��A�� B�� �� �� �ļ��ΰ���
        else if (line.find("�� �� �ļ�") != string::npos) {
            size_t p_eun = line.find("��");
            string desc = line.substr(0, p_eun);
            size_t p_ui = line.find("��", p_eun + 1);
            string anc = line.substr(p_eun + 2, p_ui - (p_eun + 2));
            Node* ndDesc = findNode(desc);
            Node* ndAnc = findNode(anc);
            if (ndDesc && ndAnc) {
                int d = generationDistance(ndDesc, ndAnc->name);
                if (d >= 0) cout << d;
                else        cout << "���� �����ϴ�.";
            }
            else {
                cout << "���� �����ϴ�.";
            }
            answered = true;
        }

        if (!answered) {
            cout << "���� ó���� �� �����ϴ�.";
        }
        cout << "\n";
    }

    return 0;
}