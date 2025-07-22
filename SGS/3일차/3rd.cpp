#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <clocale>

using namespace std;

// 집합을 문자열로 변환하여 출력하는 함수
void print_set(const set<int>& s) {
    if (s.empty()) {
        cout << endl;
        return;
    }
    auto it = s.begin();
    cout << *it;
    ++it;
    while (it != s.end()) {
        cout << ", " << *it;
        ++it;
    }
    cout << endl;
}

// 쉼표로 구분된 문자열을 정수 집합으로 파싱하는 함수
set<int> parse_set(const string& line) {
    set<int> s;
    stringstream ss(line);
    string item;
    while (getline(ss, item, ',')) {
        try {
            s.insert(stoi(item));
        } catch (const invalid_argument& e) {
            // stoi 변환 실패는 무시
        }
    }
    return s;
}

int main() {
    setlocale(LC_ALL, "Korean");
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    int n;
    inputFile >> n;
    string line;
    getline(inputFile, line); 

    for (int i = 0; i < n; ++i) {
        string line1, line2;
        if (!getline(inputFile, line1) || !getline(inputFile, line2)) {
            break;
        }

        set<int> set1 = parse_set(line1);
        set<int> set2 = parse_set(line2);

        set<int> union_set;
        set_union(set1.begin(), set1.end(), set2.begin(), set2.end(),
                  inserter(union_set, union_set.begin()));

        set<int> intersection_set;
        set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                         inserter(intersection_set, intersection_set.begin()));

        set<int> difference_set;
        set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                       inserter(difference_set, difference_set.begin()));

        cout << "== " << i + 1 << " 번째 쌍의 결과 ==" << endl;
        cout << "합집합: ";
        print_set(union_set);
        cout << "교집합: ";
        print_set(intersection_set);
        cout << "차집합: ";
        print_set(difference_set);
        
        if (i < n - 1) {
            cout << endl;
        }
    }

    inputFile.close();
    return 0;
}