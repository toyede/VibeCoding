#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

// Ʈ�� ��� Ŭ����
class TreeNode {
public:
    string name;
    TreeNode* parent;
    vector<TreeNode*> children;

    TreeNode(string n) : name(n), parent(nullptr) {}

    void addChild(TreeNode* child) {
        children.push_back(child);
        child->parent = this;
    }
};

class JoseonDynastyTree {
private:
    TreeNode* root;
    map<string, TreeNode*> nodeMap;
    vector<string> kingOrder;
    set<string> validKings;
    vector<string> quizQuestions;

    // �������� ������ (�ϵ��ڵ� - ����)
    vector<pair<string, string>> joseonData = {
        {"����", ""},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"���걺", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"���ر�", "����"},
        {"����", "����"},
        {"ȿ��", "����"},
        {"����", "ȿ��"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"����", "����"},
        {"ö��", "����"},
        {"����", "����"},
        {"����", "����"}
    };

public:
    JoseonDynastyTree() : root(nullptr) {}

    ~JoseonDynastyTree() {
        deleteTree(root);
    }

    void deleteTree(TreeNode* node) {
        if (node == nullptr) return;
        for (TreeNode* child : node->children) {
            deleteTree(child);
        }
        delete node;
    }

    // �ϵ��ڵ��� �����ͷ� Ʈ�� ����
    void buildTree() {
        for (const auto& pair : joseonData) {
            string king = pair.first;
            string parent = pair.second;

            // ��ȿ�� �� �̸� ��Ͽ� �߰�
            validKings.insert(king);

            if (parent.empty()) {
                // ��Ʈ ��� (����)
                root = new TreeNode(king);
                nodeMap[king] = root;
                kingOrder.push_back(king);
            }
            else {
                // �θ� ��尡 ������ ����
                if (nodeMap.find(parent) == nodeMap.end()) {
                    nodeMap[parent] = new TreeNode(parent);
                }

                // �ڽ� ��� ���� �� ����
                TreeNode* childNode = new TreeNode(king);
                nodeMap[king] = childNode;
                nodeMap[parent]->addChild(childNode);
                kingOrder.push_back(king);
            }
        }
    }

    // Quiz.txt ���� �б� (ANSI ���ڵ�)
    void loadQuizFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Quiz.txt ������ �� �� �����ϴ�." << endl;
            return;
        }

        string line;
        bool firstLine = true;

        while (getline(file, line)) {
            if (line.empty()) continue;

            if (firstLine) {
                firstLine = false;
                continue; // ù ���� ���� �����̹Ƿ� ��ŵ
            }

            quizQuestions.push_back(line);
        }
        file.close();
    }

    // ���ڿ����� �� �̸� ���� �Լ�
    string extractKingFromQuestion(const string& question) {
        // ������ ��� �� �̸��� ���� ������ ���� (�� �̸����� �˻�)
        vector<string> sortedKings(validKings.begin(), validKings.end());
        sort(sortedKings.begin(), sortedKings.end(), [](const string& a, const string& b) {
            return a.length() > b.length();
        });

        for (const string& king : sortedKings) {
            if (question.find(king) != string::npos) {
                return king;
            }
        }
        return "";
    }

    // �� �� �̸� ���� (�� �� �ļ� ������)
    pair<string, string> extractTwoKingsFromQuestion(const string& question) {
        size_t eunPos = question.find("�� ");
        size_t uiPos = question.find("�� �� ��");

        if (eunPos == string::npos || uiPos == string::npos) {
            return make_pair("", "");
        }

        string descendantPart = question.substr(0, eunPos);
        string ancestorPart = question.substr(eunPos + 2, uiPos - (eunPos + 2));

        string descendant = "";
        string ancestor = "";

        for (const string& king : validKings) {
            if (descendantPart.find(king) != string::npos) {
                descendant = king;
            }
            if (ancestorPart.find(king) != string::npos) {
                ancestor = king;
            }
        }

        return make_pair(descendant, ancestor);
    }

    // Ʈ�� ���� ���
    void printTree() {
        cout << "�������� Ʈ�� ����:" << endl;
        printTreeHelper(root, "", true);
        cout << endl;
    }

    void printTreeHelper(TreeNode* node, string prefix, bool isLast) {
        if (node == nullptr) return;

        cout << prefix;
        cout << (isLast ? "������ " : "������ ");
        cout << node->name << endl;

        for (size_t i = 0; i < node->children.size(); i++) {
            bool isChildLast = (i == node->children.size() - 1);
            string newPrefix = prefix + (isLast ? "    " : "��   ");
            printTreeHelper(node->children[i], newPrefix, isChildLast);
        }
    }

    // �������� ������ ���
    void printJoseonData() {
        cout << "��������.txt ����:" << endl;
        for (const auto& pair : joseonData) {
            if (pair.second.empty()) {
                cout << pair.first << endl;
            }
            else {
                cout << pair.first << " " << pair.second << endl;
            }
        }
        cout << endl;
    }

    // Quiz.txt ���� ���� ���
    void printQuizFile(const string& filename) {
        cout << "Quiz.txt ����:" << endl;
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        }
        else {
            cout << "������ �� �� �����ϴ�." << endl;
        }
        cout << endl;
    }

    // ���� �ذ� �Լ���
    void solveKingsInOrder() {
        cout << "��: ";
        for (const string& king : kingOrder) {
            cout << king << " ";
        }
        cout << endl;
    }

    void solveKingsInReverse() {
        cout << "��: ";
        for (int i = kingOrder.size() - 1; i >= 0; i--) {
            cout << kingOrder[i] << " ";
        }
        cout << endl;
    }

    void solveTotalKings() {
        cout << "��: " << kingOrder.size() << "��" << endl;
    }

    void solveDescendants(const string& kingName) {
        cout << "��: ";
        if (nodeMap.find(kingName) != nodeMap.end()) {
            vector<string> descendants;
            getDescendants(nodeMap[kingName], descendants);
            if (descendants.empty()) {
                cout << "�ļ��� �����ϴ�.";
            }
            else {
                for (const string& desc : descendants) {
                    cout << desc << " ";
                }
            }
        }
        else {
            cout << "�ش� ���� ã�� �� �����ϴ�.";
        }
        cout << endl;
    }

    void getDescendants(TreeNode* node, vector<string>& descendants) {
        for (TreeNode* child : node->children) {
            descendants.push_back(child->name);
            getDescendants(child, descendants);
        }
    }

    void solveKingsWithoutDirectSuccessors() {
        cout << "��: ";
        for (const auto& pair : nodeMap) {
            if (pair.second->children.empty()) {
                cout << pair.first << " ";
            }
        }
        cout << endl;
    }

    void solveKingWithMostDescendants() {
        string maxKing;
        int maxCount = 0;

        for (const auto& pair : nodeMap) {
            vector<string> descendants;
            getDescendants(pair.second, descendants);
            if (descendants.size() > maxCount) {
                maxCount = descendants.size();
                maxKing = pair.first;
            }
        }

        cout << "��: " << maxKing << " (" << maxCount << "��)" << endl;
    }

    void solveSiblings(const string& kingName) {
        cout << "��: ";
        if (nodeMap.find(kingName) != nodeMap.end() && nodeMap[kingName]->parent != nullptr) {
            TreeNode* parent = nodeMap[kingName]->parent;
            bool hasSiblings = false;
            for (TreeNode* sibling : parent->children) {
                if (sibling->name != kingName) {
                    cout << sibling->name << " ";
                    hasSiblings = true;
                }
            }
            if (!hasSiblings) {
                cout << "������ �����ϴ�.";
            }
        }
        else {
            cout << "�ش� ���� ã�� �� ���ų� �θ� �����ϴ�.";
        }
        cout << endl;
    }

    void solveAncestors(const string& kingName) {
        cout << "��: ";
        if (nodeMap.find(kingName) != nodeMap.end()) {
            vector<string> ancestors;
            TreeNode* current = nodeMap[kingName]->parent;
            while (current != nullptr) {
                ancestors.push_back(current->name);
                current = current->parent;
            }
            reverse(ancestors.begin(), ancestors.end());
            if (ancestors.empty()) {
                cout << "������ �����ϴ�.";
            }
            else {
                for (const string& ancestor : ancestors) {
                    cout << ancestor << " ";
                }
            }
        }
        else {
            cout << "�ش� ���� ã�� �� �����ϴ�.";
        }
        cout << endl;
    }

    void solveKingsWithMultipleSuccessors() {
        int count = 0;
        for (const auto& pair : nodeMap) {
            if (pair.second->children.size() >= 2) {
                count++;
            }
        }
        cout << "��: " << count << "��" << endl;
    }

    void solveGenerationDifference(const string& descendant, const string& ancestor) {
        cout << "��: ";
        if (nodeMap.find(ancestor) != nodeMap.end() && nodeMap.find(descendant) != nodeMap.end()) {
            int generation = getGenerationDifference(nodeMap[ancestor], nodeMap[descendant]);
            if (generation > 0) {
                cout << generation << "�� �ļ�";
            }
            else {
                cout << "�ļ� ���谡 �ƴ�";
            }
        }
        else {
            cout << "�ش� ���� ã�� �� �����ϴ�.";
        }
        cout << endl;
    }

    int getGenerationDifference(TreeNode* ancestor, TreeNode* descendant) {
        return getGenerationHelper(ancestor, descendant, 0);
    }

    int getGenerationHelper(TreeNode* current, TreeNode* target, int depth) {
        if (current == target) return depth;

        for (TreeNode* child : current->children) {
            int result = getGenerationHelper(child, target, depth + 1);
            if (result > 0) return result;
        }
        return 0;
    }

    // Quiz.txt���� ���� �������� ������ �ذ�
    void solveQuiz() {
        int questionNum = 1;

        for (const string& question : quizQuestions) {
            cout << questionNum << ". " << question << endl;

            // ���� �������� �б� ó�� (���� ���� �ؽ�Ʈ ���)
            if (question.find("������� ����Ͻÿ�") != string::npos) {
                solveKingsInOrder();
            }
            else if (question.find("�������� ����Ͻÿ�") != string::npos) {
                solveKingsInReverse();
            }
            else if (question.find("��� �� ���ΰ�") != string::npos) {
                solveTotalKings();
            }
            else if (question.find("�� �ļ���") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveDescendants(kingName);
                }
                else {
                    cout << "��: �� �̸��� ã�� �� �����ϴ�." << endl;
                }
            }
            else if (question.find("���� �ļ��� ���� ���� ����") != string::npos) {
                solveKingsWithoutDirectSuccessors();
            }
            else if (question.find("���� �ļ��� ���� �� ���� ���� ����") != string::npos) {
                solveKingWithMostDescendants();
            }
            else if (question.find("�� ������") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveSiblings(kingName);
                }
                else {
                    cout << "��: �� �̸��� ã�� �� �����ϴ�." << endl;
                }
            }
            else if (question.find("�� ���� ������") != string::npos) {
                string kingName = extractKingFromQuestion(question);
                if (!kingName.empty()) {
                    solveAncestors(kingName);
                }
                else {
                    cout << "��: �� �̸��� ã�� �� �����ϴ�." << endl;
                }
            }
            else if (question.find("���� �ļ��� 2�� �̻�") != string::npos) {
                solveKingsWithMultipleSuccessors();
            }
            else if (question.find("�� �� �ļ��ΰ�") != string::npos) {
                pair<string, string> kings = extractTwoKingsFromQuestion(question);
                if (!kings.first.empty() && !kings.second.empty()) {
                    solveGenerationDifference(kings.first, kings.second);
                }
                else {
                    cout << "��: �� �̸��� ã�� �� �����ϴ�." << endl;
                }
            }
            else {
                cout << "��: �� �� ���� ���� �����Դϴ�." << endl;
            }

            cout << endl;
            questionNum++;
        }
    }
};

int main() {
    JoseonDynastyTree tree;

    // �������� �����ͷ� Ʈ�� ����
    tree.buildTree();

    // Quiz.txt���� ���� ���� �б�
    tree.loadQuizFromFile("Quiz.txt");

    // ���� ���� ���
    tree.printQuizFile("Quiz.txt");
    cout << "====================================" << endl;

    tree.printJoseonData();
    cout << "====================================" << endl;

    // Ʈ�� ���� ���
    tree.printTree();
    cout << "====================================" << endl;

    // Quiz.txt���� ���� �������� ������ �ذ�
    tree.solveQuiz();

    return 0;
}