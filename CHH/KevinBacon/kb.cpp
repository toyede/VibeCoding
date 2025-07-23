#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ���� ����Ʈ�� �ϳ��� ��ũ ����ü
struct Node {
    int vertex;
    Node* next;
};

// BFS�� ���� �迭 �� ���� ť
int* distArr;
int* prevArr;
int* queueArr;
int  Qsize, qHead, qTail;

// �׷����� �о� ���� ����Ʈ ����, N�� ��� �� ��ȯ
Node** readGraph(const char* filename, int& N) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "���� ���� ����: " << filename << endl;
        return nullptr;
    }

    infile >> N;
    infile.ignore();  // ù �� ���� �Һ�

    Node** adj = new Node * [N + 1];
    for (int i = 1; i <= N; ++i) adj[i] = nullptr;

    string line;
    int linesRead = 0;
    while (linesRead < N && getline(infile, line)) {
        if (line.empty()) continue;
        int idx = 0;
        // u �Ľ�
        while (idx < (int)line.size() && (line[idx] == ' ' || line[idx] == '\t')) ++idx;
        int start = idx;
        while (idx < (int)line.size() && line[idx] >= '0' && line[idx] <= '9') ++idx;
        int u = 0;
        for (int k = start; k < idx; ++k) u = u * 10 + (line[k] - '0');
        // v�� �Ľ�
        while (idx < (int)line.size()) {
            if (line[idx] < '0' || line[idx] > '9') { ++idx; continue; }
            start = idx;
            while (idx < (int)line.size() && line[idx] >= '0' && line[idx] <= '9') ++idx;
            int v = 0;
            for (int k = start; k < idx; ++k) v = v * 10 + (line[k] - '0');
            // ���� ���� �߰�
            Node* n1 = new Node{ v, adj[u] }; adj[u] = n1;
            Node* n2 = new Node{ u, adj[v] }; adj[v] = n2;
        }
        ++linesRead;
    }
    infile.close();
    return adj;
}

// BFS �迭 �� ť �ʱ�ȭ
void initBFS(int N) {
    distArr = new int[N + 1];
    prevArr = new int[N + 1];
    queueArr = new int[N + 1];
    Qsize = N + 1;
}

// ���� ť ����
void enqueue(int x) { queueArr[qTail] = x; qTail = (qTail + 1) % Qsize; }
int  dequeue() { int x = queueArr[qHead]; qHead = (qHead + 1) % Qsize; return x; }
bool isEmpty() { return qHead == qTail; }

// start�κ��� ��� �������� �ִ� �Ÿ��� ���� ��� ���
void bfs(int start, int N, Node** adj) {
    for (int i = 1; i <= N; ++i) {
        distArr[i] = -1;
        prevArr[i] = -1;
    }
    qHead = qTail = 0;
    distArr[start] = 0;
    prevArr[start] = 0;
    enqueue(start);
    while (!isEmpty()) {
        int u = dequeue();
        for (Node* p = adj[u]; p; p = p->next) {
            int v = p->vertex;
            if (distArr[v] == -1) {
                distArr[v] = distArr[u] + 1;
                prevArr[v] = u;
                enqueue(v);
            }
        }
    }
}

// ������� ���� �׷� �� ���
int countComponents(int N, Node** adj) {
    bool* visited = new bool[N + 1];
    for (int i = 1; i <= N; ++i) visited[i] = false;
    int groups = 0;
    qHead = qTail = 0;
    for (int i = 1; i <= N; ++i) {
        if (!visited[i]) {
            ++groups;
            visited[i] = true;
            enqueue(i);
            while (!isEmpty()) {
                int u = dequeue();
                for (Node* p = adj[u]; p; p = p->next) {
                    int v = p->vertex;
                    if (!visited[v]) { visited[v] = true; enqueue(v); }
                }
            }
        }
    }
    delete[] visited;
    return groups;
}

bool validNumber(int x, int N) {
    return x >= 1 && x <= N;
}

int main() {
    int N;
    Node** adj = readGraph("kb.txt", N);
    if (!adj) return 1;

    initBFS(N);

    // 1) ��� �� ���
    cout << "���� ����� ����� ��: " << N << "��\n";

    // 2) ������� ���� �׷� ��
    int groups = countComponents(N, adj);
    cout << "���� ������� ���� �׷��� ��: " << groups << "��\n\n";

    // 3) �� ��� �Ÿ� ���
    cout << "�� ��� ��ȣ�� �Է��ϼ��� (��: 3 17): ";
    int a, b;
    if (!(cin >> a >> b) || !validNumber(a, N) || !validNumber(b, N)) {
        cerr << "�Է� ����: 1���� " << N << " ������ �ڿ����� �Է����ּ���.\n";
        return 1;
    }
    if (a == b) {
        cout << "�� ��� ��ȣ�� �����Ͽ� �Ÿ��� 0�ܰ��Դϴ�.\n";
    }
    else {
        bfs(a, N, adj);
        if (distArr[b] != -1) {
            cout << a << "�� ���� " << b << "�� ����� �Ÿ��� "
                << distArr[b] << "�ܰ��Դϴ�.\n";
        }
        else {
            cout << a << "�� ���� " << b << "�� ���� ����Ǿ� ���� �ʽ��ϴ�.\n";
        }
    }

    // 4) �ܰ� �� �Է¹޾� k�ܰ� �̳� ��� ��쿡�� �����ϱ� ���� ��� ����
    cout << "\n�ܰ� ���� �Է��ϼ��� (��: 3): ";
    int k;
    if (!(cin >> k) || k < 0) {
        cerr << "�Է� ����: �ܰ�� 0 �̻��� �ڿ������� �մϴ�.\n";
        return 1;
    }

    // k-�̳� reachability ���
    bool** reach = new bool* [N + 1];
    for (int i = 1; i <= N; ++i) {
        reach[i] = new bool[N + 1];
        for (int j = 1; j <= N; ++j) reach[i][j] = false;
        bfs(i, N, adj);
        for (int j = 1; j <= N; ++j) {
            if (distArr[j] != -1 && distArr[j] <= k) reach[i][j] = true;
        }
    }

    bool* covered = new bool[N + 1];
    bool* used = new bool[N + 1];
    int* selected = new int[N + 1];
    for (int i = 1; i <= N; ++i) { covered[i] = false; used[i] = false; }
    int coveredCount = 0;
    int selCount = 0;

    // �׸��� �� Ŀ��
    while (coveredCount < N) {
        int best = 0, bestCover = 0;
        for (int i = 1; i <= N; ++i) {
            if (used[i]) continue;
            int cnt = 0;
            for (int j = 1; j <= N; ++j) {
                if (reach[i][j] && !covered[j]) cnt++;
            }
            if (cnt > bestCover) { bestCover = cnt; best = i; }
        }
        if (bestCover == 0) break;
        used[best] = true;
        selected[selCount++] = best;
        for (int j = 1; j <= N; ++j) {
            if (reach[best][j] && !covered[j]) { covered[j] = true; coveredCount++; }
        }
    }

    cout << "\n�ܰ� " << k << " �̳��� ��� ��쿡�� �����Ϸ��� ���� ���鿡�� �����ϼ���:\n";
    for (int i = 0; i < selCount; ++i) {
        cout << "  - ��� ��ȣ " << selected[i] << "\n";
    }

    return 0;
}
