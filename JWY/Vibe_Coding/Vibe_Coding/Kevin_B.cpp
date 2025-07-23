#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>

using namespace std;

class KevinBaconGame {
private:
    vector<vector<int>> graph;
    int totalNodes;
    int minNode, maxNode;

public:
    KevinBaconGame() : totalNodes(0), minNode(INT_MAX), maxNode(0) {}

    // 파일에서 그래프 로드
    bool loadGraph(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "파일을 열 수 없습니다: " << filename << endl;
            return false;
        }

        string line;
        getline(file, line);
        totalNodes = stoi(line);

        graph.resize(totalNodes + 1); // 1번부터 사용

        while (getline(file, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            int node;
            iss >> node;

            minNode = min(minNode, node);
            maxNode = max(maxNode, node);

            int neighbor;
            while (iss >> neighbor) {
                // 양방향 그래프로 저장
                graph[node].push_back(neighbor);
                graph[neighbor].push_back(node);
                minNode = min(minNode, neighbor);
                maxNode = max(maxNode, neighbor);
            }
        }

        file.close();
        return true;
    }

    // 노드 유효성 검사
    bool isValidNode(int node) {
        return node >= minNode && node <= maxNode && node < graph.size();
    }

    // BFS로 두 노드 간 최단 거리 계산 (경로도 함께 반환)
    pair<int, vector<int>> findDistanceWithPath(int start, int end) {
        if (start == end) return { 0, {start} };

        vector<int> distance(graph.size(), -1);
        vector<int> parent(graph.size(), -1);
        queue<int> q;

        q.push(start);
        distance[start] = 0;

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            for (int neighbor : graph[current]) {
                if (distance[neighbor] == -1) {
                    distance[neighbor] = distance[current] + 1;
                    parent[neighbor] = current;
                    q.push(neighbor);

                    if (neighbor == end) {
                        // 경로 복원
                        vector<int> path;
                        int node = end;
                        while (node != -1) {
                            path.push_back(node);
                            node = parent[node];
                        }
                        reverse(path.begin(), path.end());
                        return { distance[neighbor], path };
                    }
                }
            }
        }

        return { -1, {} }; // 연결되지 않음
    }

    // 기존 함수와의 호환성을 위한 래퍼
    int findDistance(int start, int end) {
        return findDistanceWithPath(start, end).first;
    }

    // BFS로 K단계 내 도달 가능한 모든 노드 찾기
    set<int> getReachableNodes(int start, int k) {
        set<int> reachable;
        vector<int> distance(graph.size(), -1);
        queue<int> q;

        q.push(start);
        distance[start] = 0;
        reachable.insert(start);

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            if (distance[current] >= k) continue;

            for (int neighbor : graph[current]) {
                if (distance[neighbor] == -1) {
                    distance[neighbor] = distance[current] + 1;
                    q.push(neighbor);
                    reachable.insert(neighbor);
                }
            }
        }

        return reachable;
    }

    // 새로 커버할 수 있는 노드 수 계산
    int countNewReachable(int node, int k, const set<int>& covered) {
        set<int> reachable = getReachableNodes(node, k);
        int count = 0;

        for (int n : reachable) {
            if (covered.find(n) == covered.end()) {
                count++;
            }
        }

        return count;
    }

    // 그리디 방식으로 K단계 내 모든 노드에 연락할 최소 노드 집합 찾기
    vector<int> greedyDomination(int k) {
        vector<int> result;
        set<int> covered;
        vector<pair<int, int>> selectionProcess; // (선택된 노드, 새로 커버한 노드 수)

        // 모든 유효한 노드 찾기
        set<int> allNodes;
        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i) && !graph[i].empty()) {
                allNodes.insert(i);
            }
        }

        while (covered.size() < allNodes.size()) {
            int bestNode = -1;
            int maxNewCover = 0;

            for (int node : allNodes) {
                int newCoverCount = countNewReachable(node, k, covered);
                if (newCoverCount > maxNewCover) {
                    maxNewCover = newCoverCount;
                    bestNode = node;
                }
            }

            if (bestNode == -1 || maxNewCover == 0) break;

            result.push_back(bestNode);
            selectionProcess.push_back({ bestNode, maxNewCover });

            // bestNode에서 k단계 내 도달 가능한 모든 노드를 covered에 추가
            set<int> reachable = getReachableNodes(bestNode, k);
            for (int node : reachable) {
                covered.insert(node);
            }
        }

        // 선택 과정 출력
        cout << "선택 과정:" << endl;
        for (auto& process : selectionProcess) {
            cout << "  - 노드 " << process.first << ": " << process.second << "개 노드 커버" << endl;
        }
        cout << "총 커버율: " << allNodes.size() << "개 중 " << covered.size() << "개 노드 도달 가능" << endl;

        return result;
    }

    // Lone Wolf 찾기
    vector<int> findLoneWolves() {
        vector<int> loneWolves;
        set<int> connectedNodes;

        // 모든 연결된 노드들을 찾기
        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i)) {
                if (!graph[i].empty()) {
                    connectedNodes.insert(i); // 나가는 간선이 있는 노드
                    for (int neighbor : graph[i]) {
                        connectedNodes.insert(neighbor); // 들어오는 간선이 있는 노드
                    }
                }
            }
        }

        // 전체 노드 중 연결되지 않은 노드 찾기
        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i) && connectedNodes.find(i) == connectedNodes.end()) {
                loneWolves.push_back(i);
            }
        }

        return loneWolves;
    }

    // DFS로 연결 컴포넌트 개수 계산
    int countConnectedComponents() {
        vector<bool> visited(graph.size(), false);
        int components = 0;

        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i) && !visited[i]) {
                dfs(i, visited);
                components++;
            }
        }

        return components;
    }

    void dfs(int node, vector<bool>& visited) {
        visited[node] = true;

        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                dfs(neighbor, visited);
            }
        }
    }

    // 메인 인터페이스
    void run() {
        cout << "=== 케빈 베이컨 게임 ===" << endl;
        cout << "노드 범위: " << minNode << "~" << maxNode << endl << endl;

        while (true) {
            cout << "질문을 입력하세요:" << endl;
            cout << "1. 두 노드 간 거리: A B 입력" << endl;
            cout << "2. K단계 이내 연락하려면 누구에게 연락해야 할까?: K 입력" << endl;
            cout << "3. Lone Wolf 찾기: 별도 입력 없음" << endl;
            cout << "4. 그룹 개수 확인: 별도 입력 없음" << endl;
            cout << "5. 종료: exit" << endl << endl;

            cout << "질문: ";
            string input;
            getline(cin, input);

            if (input == "exit" || input == "5") {
                cout << "프로그램을 종료합니다." << endl;
                break;
            }

            int choice = 0;
            try {
                choice = stoi(input);
            }
            catch (...) {
                cout << "잘못된 입력입니다." << endl << endl;
                continue;
            }

            switch (choice) {
            case 1: {
                int nodeA, nodeB;
                cout << "노드 A: ";
                cin >> nodeA;
                cout << "노드 B: ";
                cin >> nodeB;
                cin.ignore(); // 버퍼 클리어

                if (!isValidNode(nodeA) || !isValidNode(nodeB)) {
                    cout << "존재하지 않는 노드입니다." << endl << endl;
                    break;
                }

                auto result = findDistanceWithPath(nodeA, nodeB);
                int distance = result.first;
                vector<int> path = result.second;

                if (distance == -1) {
                    cout << "결과: 두 노드는 연결되지 않았습니다." << endl << endl;
                }
                else {
                    cout << "결과: 거리는 " << distance << "입니다." << endl;
                    cout << "경로: ";
                    for (size_t i = 0; i < path.size(); i++) {
                        cout << path[i];
                        if (i < path.size() - 1) cout << " → ";
                    }
                    cout << endl << endl;
                }
                break;
            }

            case 2: {
                int k;
                cout << "K 단계: ";
                cin >> k;
                cin.ignore(); // 버퍼 클리어

                if (k <= 0) {
                    cout << "K는 양수여야 합니다." << endl << endl;
                    break;
                }

                cout << "계산 중..." << endl;
                vector<int> result = greedyDomination(k);

                cout << "결과: ";
                if (result.empty()) {
                    cout << "연락할 노드를 찾을 수 없습니다." << endl;
                }
                else {
                    cout << "노드 ";
                    for (size_t i = 0; i < result.size(); i++) {
                        cout << result[i];
                        if (i < result.size() - 1) cout << ", ";
                    }
                    cout << "에게 연락하세요." << endl;
                    cout << "방법: 그리디 방식 (단계별 최대 커버 노드 선택)" << endl;
                }
                cout << endl;
                break;
            }

            case 3: {
                vector<int> loneWolves = findLoneWolves();
                cout << "결과: ";
                if (loneWolves.empty()) {
                    cout << "Lone Wolf가 없습니다." << endl;
                }
                else {
                    cout << "Lone Wolf: ";
                    for (size_t i = 0; i < loneWolves.size(); i++) {
                        cout << loneWolves[i];
                        if (i < loneWolves.size() - 1) cout << ", ";
                    }
                    cout << endl;
                }
                cout << endl;
                break;
            }

            case 4: {
                int components = countConnectedComponents();
                cout << "결과: " << components << "개의 그룹이 있습니다." << endl << endl;
                break;
            }

            default:
                cout << "잘못된 선택입니다." << endl << endl;
                break;
            }
        }
    }
};

int main() {
    KevinBaconGame game;

    // kb.txt 파일 로드
    string filename = "kb.txt";
    if (!game.loadGraph(filename)) {
        cout << "파일 로드에 실패했습니다. " << filename << " 파일이 존재하는지 확인하세요." << endl;
        return 1;
    }

    game.run();

    return 0;
}