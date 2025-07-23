#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// 인접 리스트의 하나의 링크 구조체
struct Node {
    int vertex;
    Node* next;
};

// BFS용 전역 배열 및 원형 큐
int* distArr;
int* prevArr;
int* queueArr;
int  Qsize, qHead, qTail;

// 그래프를 읽어 인접 리스트 구성, N에 배우 수 반환
Node** readGraph(const char* filename, int& N) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "파일 열기 실패: " << filename << endl;
        return nullptr;
    }

    infile >> N;
    infile.ignore();  // 첫 줄 개행 소비

    Node** adj = new Node * [N + 1];
    for (int i = 1; i <= N; ++i) adj[i] = nullptr;

    string line;
    int linesRead = 0;
    while (linesRead < N && getline(infile, line)) {
        if (line.empty()) continue;
        int idx = 0;
        // u 파싱
        while (idx < (int)line.size() && (line[idx] == ' ' || line[idx] == '\t')) ++idx;
        int start = idx;
        while (idx < (int)line.size() && line[idx] >= '0' && line[idx] <= '9') ++idx;
        int u = 0;
        for (int k = start; k < idx; ++k) u = u * 10 + (line[k] - '0');
        // v들 파싱
        while (idx < (int)line.size()) {
            if (line[idx] < '0' || line[idx] > '9') { ++idx; continue; }
            start = idx;
            while (idx < (int)line.size() && line[idx] >= '0' && line[idx] <= '9') ++idx;
            int v = 0;
            for (int k = start; k < idx; ++k) v = v * 10 + (line[k] - '0');
            // 무향 간선 추가
            Node* n1 = new Node{ v, adj[u] }; adj[u] = n1;
            Node* n2 = new Node{ u, adj[v] }; adj[v] = n2;
        }
        ++linesRead;
    }
    infile.close();
    return adj;
}

// BFS 배열 및 큐 초기화
void initBFS(int N) {
    distArr = new int[N + 1];
    prevArr = new int[N + 1];
    queueArr = new int[N + 1];
    Qsize = N + 1;
}

// 원형 큐 연산
void enqueue(int x) { queueArr[qTail] = x; qTail = (qTail + 1) % Qsize; }
int  dequeue() { int x = queueArr[qHead]; qHead = (qHead + 1) % Qsize; return x; }
bool isEmpty() { return qHead == qTail; }

// start로부터 모든 정점까지 최단 거리와 이전 노드 기록
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

// 연결되지 않은 그룹 수 계산
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

    // 1) 배우 수 출력
    cout << "현재 저장된 배우의 수: " << N << "명\n";

    // 2) 연결되지 않은 그룹 수
    int groups = countComponents(N, adj);
    cout << "서로 연결되지 않은 그룹의 수: " << groups << "개\n\n";

    // 3) 두 배우 거리 계산
    cout << "두 배우 번호를 입력하세요 (예: 3 17): ";
    int a, b;
    if (!(cin >> a >> b) || !validNumber(a, N) || !validNumber(b, N)) {
        cerr << "입력 오류: 1부터 " << N << " 사이의 자연수를 입력해주세요.\n";
        return 1;
    }
    if (a == b) {
        cout << "두 배우 번호가 동일하여 거리는 0단계입니다.\n";
    }
    else {
        bfs(a, N, adj);
        if (distArr[b] != -1) {
            cout << a << "번 배우와 " << b << "번 배우의 거리는 "
                << distArr[b] << "단계입니다.\n";
        }
        else {
            cout << a << "번 배우와 " << b << "번 배우는 연결되어 있지 않습니다.\n";
        }
    }

    // 4) 단계 수 입력받아 k단계 이내 모든 배우에게 연락하기 위한 배우 선택
    cout << "\n단계 수를 입력하세요 (예: 3): ";
    int k;
    if (!(cin >> k) || k < 0) {
        cerr << "입력 오류: 단계는 0 이상인 자연수여야 합니다.\n";
        return 1;
    }

    // k-이내 reachability 계산
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

    // 그리디 셋 커버
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

    cout << "\n단계 " << k << " 이내에 모든 배우에게 연락하려면 다음 배우들에게 연락하세요:\n";
    for (int i = 0; i < selCount; ++i) {
        cout << "  - 배우 번호 " << selected[i] << "\n";
    }

    return 0;
}
