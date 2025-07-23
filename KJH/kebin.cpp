#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <limits>

using namespace std;

/**
 * @brief Graph 클래스는 사람 관계 네트워크를 나타냅니다.
 * 
 * 이 클래스는 인접 리스트를 사용하여 그래프를 구현하고,
 * 케빈 베이컨 게임과 관련된 다양한 기능을 제공합니다.
 */
class Graph
{
public:
    // 인접 리스트: 각 사람(노드)에 연결된 다른 사람들의 목록을 저장합니다.
    map<int, set<int>> adj;
    // 전체 사람 목록: 그래프에 존재하는 모든 사람(노드)을 저장합니다.
    set<int> people;

    /**
     * @brief 두 사람 사이에 관계(간선)를 추가합니다.
     * 
     * @param u 첫 번째 사람 (노드)
     * @param v 두 번째 사람 (노드)
     */
    void addEdge(int u, int v)
    {
        adj[u].insert(v);
        adj[v].insert(u);
        people.insert(u);
        people.insert(v);
    }

    /**
     * @brief 파일에서 데이터를 읽어 그래프를 생성합니다.
     * 
     * @param filename 관계 데이터가 담긴 파일 이름
     */
    void buildGraphFromFile(const string& filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error: " << filename << " 파일을 열 수 없습니다." << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            vector<int> group;
            int person;
            while (ss >> person)
            {
                group.push_back(person);
                people.insert(person);
            }

            // 그룹 내 모든 사람들 사이에 양방향 관계를 추가합니다.
            if (group.size() > 1)
            {
                for (size_t i = 0; i < group.size(); ++i)
                {
                    for (size_t j = i + 1; j < group.size(); ++j)
                    {
                        addEdge(group[i], group[j]);
                    }
                }
            }
        }
        file.close();
    }

    /**
     * @brief 너비 우선 탐색(BFS)을 사용하여 두 사람 사이의 최단 거리를 계산합니다.
     * 
     * @param start 시작 사람 (노드)
     * @param end 목표 사람 (노드)
     * @return int 최단 거리 (연결되지 않은 경우 -1)
     */
    int getDistance(int start, int end)
    {
        if (people.find(start) == people.end() || people.find(end) == people.end())
        {
            return -1; // 노드가 존재하지 않음
        }
        if (start == end)
        {
            return 0;
        }

        queue<pair<int, int>> q;
        q.push({start, 0});
        set<int> visited;
        visited.insert(start);

        while (!q.empty())
        {
            int current = q.front().first;
            int dist = q.front().second;
            q.pop();

            if (current == end)
            {
                return dist;
            }

            for (int neighbor : adj[current])
            {
                if (visited.find(neighbor) == visited.end())
                {
                    visited.insert(neighbor);
                    q.push({neighbor, dist + 1});
                }
            }
        }
        return -1; // 경로 없음
    }

    /**
     * @brief 친구가 없는 사람(Lone Wolf) 목록을 찾습니다.
     * 
     * @return vector<int> Lone Wolf 목록
     */
    vector<int> findLoneWolves()
    {
        vector<int> loneWolves;
        for (int person : people)
        {
            if (adj.find(person) == adj.end() || adj[person].empty())
            {
                loneWolves.push_back(person);
            }
        }
        return loneWolves;
    }

    /**
     * @brief 그래프에 존재하는 그룹(연결된 컴포넌트)의 수를 계산합니다.
     * 
     * @return int 그룹의 수
     */
    int countGroups()
    {
        if (people.empty())
        {
            return 0;
        }

        int groupCount = 0;
        set<int> visited;

        for (int person : people)
        {
            if (visited.find(person) == visited.end())
            {
                groupCount++;
                queue<int> q;
                q.push(person);
                visited.insert(person);
                while (!q.empty())
                {
                    int current = q.front();
                    q.pop();
                    for (int neighbor : adj[current])
                    {
                        if (visited.find(neighbor) == visited.end())
                        {
                            visited.insert(neighbor);
                            q.push(neighbor);
                        }
                    }
                }
            }
        }
        return groupCount;
    }
    
    /**
     * @brief 특정 사람이 속한 그룹의 모든 멤버를 찾습니다.
     * 
     * @param start_node 그룹을 찾을 시작 사람 (노드)
     * @return set<int> 그룹 멤버 목록
     */
    set<int> getGroupMembers(int start_node)
    {
        set<int> group_members;
        if (people.find(start_node) == people.end())
        {
            return group_members;
        }

        queue<int> q;
        q.push(start_node);
        group_members.insert(start_node);

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            for (int neighbor : adj[current])
            {
                if (group_members.find(neighbor) == group_members.end())
                {
                    group_members.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
        return group_members;
    }

    /**
     * @brief 3단계 이내에 자신의 그룹 내 모든 사람에게 연락할 수 있는 사람을 찾습니다.
     * 
     * @return vector<int> 3단계 이내 연락 가능한 사람 목록
     */
    vector<int> findThreeStepConnectors()
    {
        vector<int> connectors;
        for (int person : people)
        {
            // Lone wolf는 후보에서 제외합니다.
            if (adj.find(person) == adj.end() || adj[person].empty())
            {
                continue;
            }
            
            set<int> groupMembers = getGroupMembers(person);
            if (groupMembers.size() <= 1) continue;

            int max_dist = 0;
            bool reachable_to_all = true;

            // 그룹 내 모든 멤버까지의 거리를 계산합니다.
            for (int member : groupMembers)
            {
                if (person == member) continue;
                int dist = getDistance(person, member);
                if (dist == -1)
                {
                    reachable_to_all = false;
                    break;
                }
                if (dist > max_dist)
                {
                    max_dist = dist;
                }
            }

            // 최대 거리가 3 이하인 경우, 후보에 추가합니다.
            if (reachable_to_all && max_dist > 0 && max_dist <= 3)
            {
                connectors.push_back(person);
            }
        }
        return connectors;
    }
};

/**
 * @brief 사용자에게 메뉴를 출력합니다.
 */
void printMenu()
{
    cout << "\n--- 케빈 베이컨 게임 ---" << endl;
    cout << "1. 나와 너의 거리는?" << endl;
    cout << "2. Lone wolf는?" << endl;
    cout << "3. 몇 개의 그룹이 존재하는가?" << endl;
    cout << "4. 3단계 이내에 모두에게 연락 하려면 누구에게 연락해야할까?" << endl;
    cout << "5. 종료" << endl;
    cout << "-----------------------" << endl;
    cout << "선택: ";
}

/**
 * @brief 프로그램의 메인 함수입니다.
 */
int main()
{
    Graph g;
    g.buildGraphFromFile("kb.txt");

    int choice;
    while (true)
    {
        printMenu();
        cin >> choice;

        if (cin.fail())
        {
            cout << "잘못된 입력입니다. 숫자를 입력해주세요." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 5)
        {
            cout << "프로그램을 종료합니다." << endl;
            break;
        }

        switch (choice)
        {
            case 1:
            {
                int start, end;
                cout << "두 사람의 번호를 입력하세요 (예: 1 7): ";
                cin >> start >> end;
                 if (cin.fail())
                {
                    cout << "잘못된 입력입니다. 숫자를 입력해주세요." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                int distance = g.getDistance(start, end);
                if (distance != -1)
                {
                    cout << start << "와(과) " << end << "의 거리는 " << distance << "입니다." << endl;
                }
                else
                {
                    cout << start << "와(과) " << end << "는(은) 연결되어 있지 않습니다." << endl;
                }
                break;
            }
            case 2:
            {
                vector<int> loneWolves = g.findLoneWolves();
                if (loneWolves.empty())
                {
                    cout << "Lone wolf가 없습니다." << endl;
                }
                else
                {
                    cout << "Lone wolf: ";
                    for (int p : loneWolves)
                    {
                        cout << p << " ";
                    }
                    cout << endl;
                }
                break;
            }
            case 3:
            {
                cout << "총 그룹의 수: " << g.countGroups() << "개" << endl;
                break;
            }
            case 4:
            {
                vector<int> connectors = g.findThreeStepConnectors();
                if (connectors.empty())
                {
                    cout << "3단계 이내에 모두에게 연락할 수 있는 사람이 없습니다." << endl;
                }
                else
                {
                    cout << "3단계 이내에 모두에게 연락 가능한 사람: ";
                    for (int p : connectors)
                    {
                        cout << p << " ";
                    }
                    cout << endl;
                }
                break;
            }
            default:
                cout << "잘못된 선택입니다. 다시 시도하세요." << endl;
                break;
        }
    }

    return 0;
}