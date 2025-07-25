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

class Graph
{
public:
    map<int, set<int>> adj;
    set<int> people;

    void addEdge(int u, int v)
    {
        adj[u].insert(v);
        adj[v].insert(u);
        people.insert(u);
        people.insert(v);
    }

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
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            stringstream ss(line);
            vector<int> group;
            int person;
            while (ss >> person)
            {
                group.push_back(person);
                people.insert(person);
            }

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

    int getDistance(int start, int end)
    {
        if (people.find(start) == people.end() || people.find(end) == people.end())
        {
            return -1;
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
        return -1;
    }

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

    vector<int> findThreeStepConnectors()
    {
        vector<int> connectors;
        for (int person : people)
        {
            if (adj.find(person) == adj.end() || adj[person].empty())
            {
                continue;
            }

            set<int> groupMembers = getGroupMembers(person);
            if (groupMembers.size() <= 1) continue;

            int max_dist = 0;
            bool reachable_to_all = true;

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

            if (reachable_to_all && max_dist > 0 && max_dist <= 3)
            {
                connectors.push_back(person);
            }
        }
        return connectors;
    }
};

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

int main()
{
    Graph g;
    g.buildGraphFromFile("C:/ViveCoding/KJH/kb.txt");

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
