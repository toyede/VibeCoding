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

    // ���Ͽ��� �׷��� �ε�
    bool loadGraph(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "������ �� �� �����ϴ�: " << filename << endl;
            return false;
        }

        string line;
        getline(file, line);
        totalNodes = stoi(line);

        graph.resize(totalNodes + 1); // 1������ ���

        while (getline(file, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            int node;
            iss >> node;

            minNode = min(minNode, node);
            maxNode = max(maxNode, node);

            int neighbor;
            while (iss >> neighbor) {
                // ����� �׷����� ����
                graph[node].push_back(neighbor);
                graph[neighbor].push_back(node);
                minNode = min(minNode, neighbor);
                maxNode = max(maxNode, neighbor);
            }
        }

        file.close();
        return true;
    }

    // ��� ��ȿ�� �˻�
    bool isValidNode(int node) {
        return node >= minNode && node <= maxNode && node < graph.size();
    }

    // BFS�� �� ��� �� �ִ� �Ÿ� ��� (��ε� �Բ� ��ȯ)
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
                        // ��� ����
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

        return { -1, {} }; // ������� ����
    }

    // ���� �Լ����� ȣȯ���� ���� ����
    int findDistance(int start, int end) {
        return findDistanceWithPath(start, end).first;
    }

    // BFS�� K�ܰ� �� ���� ������ ��� ��� ã��
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

    // ���� Ŀ���� �� �ִ� ��� �� ���
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

    // �׸��� ������� K�ܰ� �� ��� ��忡 ������ �ּ� ��� ���� ã��
    vector<int> greedyDomination(int k) {
        vector<int> result;
        set<int> covered;
        vector<pair<int, int>> selectionProcess; // (���õ� ���, ���� Ŀ���� ��� ��)

        // ��� ��ȿ�� ��� ã��
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

            // bestNode���� k�ܰ� �� ���� ������ ��� ��带 covered�� �߰�
            set<int> reachable = getReachableNodes(bestNode, k);
            for (int node : reachable) {
                covered.insert(node);
            }
        }

        // ���� ���� ���
        cout << "���� ����:" << endl;
        for (auto& process : selectionProcess) {
            cout << "  - ��� " << process.first << ": " << process.second << "�� ��� Ŀ��" << endl;
        }
        cout << "�� Ŀ����: " << allNodes.size() << "�� �� " << covered.size() << "�� ��� ���� ����" << endl;

        return result;
    }

    // Lone Wolf ã��
    vector<int> findLoneWolves() {
        vector<int> loneWolves;
        set<int> connectedNodes;

        // ��� ����� ������ ã��
        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i)) {
                if (!graph[i].empty()) {
                    connectedNodes.insert(i); // ������ ������ �ִ� ���
                    for (int neighbor : graph[i]) {
                        connectedNodes.insert(neighbor); // ������ ������ �ִ� ���
                    }
                }
            }
        }

        // ��ü ��� �� ������� ���� ��� ã��
        for (int i = minNode; i <= maxNode; i++) {
            if (isValidNode(i) && connectedNodes.find(i) == connectedNodes.end()) {
                loneWolves.push_back(i);
            }
        }

        return loneWolves;
    }

    // DFS�� ���� ������Ʈ ���� ���
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

    // ���� �������̽�
    void run() {
        cout << "=== �ɺ� ������ ���� ===" << endl;
        cout << "��� ����: " << minNode << "~" << maxNode << endl << endl;

        while (true) {
            cout << "������ �Է��ϼ���:" << endl;
            cout << "1. �� ��� �� �Ÿ�: A B �Է�" << endl;
            cout << "2. K�ܰ� �̳� �����Ϸ��� �������� �����ؾ� �ұ�?: K �Է�" << endl;
            cout << "3. Lone Wolf ã��: ���� �Է� ����" << endl;
            cout << "4. �׷� ���� Ȯ��: ���� �Է� ����" << endl;
            cout << "5. ����: exit" << endl << endl;

            cout << "����: ";
            string input;
            getline(cin, input);

            if (input == "exit" || input == "5") {
                cout << "���α׷��� �����մϴ�." << endl;
                break;
            }

            int choice = 0;
            try {
                choice = stoi(input);
            }
            catch (...) {
                cout << "�߸��� �Է��Դϴ�." << endl << endl;
                continue;
            }

            switch (choice) {
            case 1: {
                int nodeA, nodeB;
                cout << "��� A: ";
                cin >> nodeA;
                cout << "��� B: ";
                cin >> nodeB;
                cin.ignore(); // ���� Ŭ����

                if (!isValidNode(nodeA) || !isValidNode(nodeB)) {
                    cout << "�������� �ʴ� ����Դϴ�." << endl << endl;
                    break;
                }

                auto result = findDistanceWithPath(nodeA, nodeB);
                int distance = result.first;
                vector<int> path = result.second;

                if (distance == -1) {
                    cout << "���: �� ���� ������� �ʾҽ��ϴ�." << endl << endl;
                }
                else {
                    cout << "���: �Ÿ��� " << distance << "�Դϴ�." << endl;
                    cout << "���: ";
                    for (size_t i = 0; i < path.size(); i++) {
                        cout << path[i];
                        if (i < path.size() - 1) cout << " �� ";
                    }
                    cout << endl << endl;
                }
                break;
            }

            case 2: {
                int k;
                cout << "K �ܰ�: ";
                cin >> k;
                cin.ignore(); // ���� Ŭ����

                if (k <= 0) {
                    cout << "K�� ������� �մϴ�." << endl << endl;
                    break;
                }

                cout << "��� ��..." << endl;
                vector<int> result = greedyDomination(k);

                cout << "���: ";
                if (result.empty()) {
                    cout << "������ ��带 ã�� �� �����ϴ�." << endl;
                }
                else {
                    cout << "��� ";
                    for (size_t i = 0; i < result.size(); i++) {
                        cout << result[i];
                        if (i < result.size() - 1) cout << ", ";
                    }
                    cout << "���� �����ϼ���." << endl;
                    cout << "���: �׸��� ��� (�ܰ躰 �ִ� Ŀ�� ��� ����)" << endl;
                }
                cout << endl;
                break;
            }

            case 3: {
                vector<int> loneWolves = findLoneWolves();
                cout << "���: ";
                if (loneWolves.empty()) {
                    cout << "Lone Wolf�� �����ϴ�." << endl;
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
                cout << "���: " << components << "���� �׷��� �ֽ��ϴ�." << endl << endl;
                break;
            }

            default:
                cout << "�߸��� �����Դϴ�." << endl << endl;
                break;
            }
        }
    }
};

int main() {
    KevinBaconGame game;

    // kb.txt ���� �ε�
    string filename = "kb.txt";
    if (!game.loadGraph(filename)) {
        cout << "���� �ε忡 �����߽��ϴ�. " << filename << " ������ �����ϴ��� Ȯ���ϼ���." << endl;
        return 1;
    }

    game.run();

    return 0;
}