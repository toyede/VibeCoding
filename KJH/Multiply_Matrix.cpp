// �ʿ��� ���̺귯������ ���Խ�ŵ�ϴ�.
#include <iostream>  // ǥ�� �����(cin, cout)�� ���� ���̺귯��
#include <fstream>   // ���� �����(ifstream, ofstream)�� ���� ���̺귯��
#include <vector>    // ���� �迭�� ����(vector)�� ����ϱ� ���� ���̺귯��
#include <sstream>   // ���ڿ��� ��Ʈ��ó�� �ٷ�� ���� ���̺귯�� (���ڿ� �Ľ̿� ����)

// ��� ��� �����Ͱ� ��� �ؽ�Ʈ ������ �о,
// �Ϲ����� 2���� ����(Dense Matrix) ���·� ��ȯ���ִ� �Լ��Դϴ�.
std::vector<std::vector<int>> readSparseMatrix(const std::string& filename)
{
    // ifstream: ������ �б� ���� ��ü�Դϴ�. filename���� �־��� �̸��� ������ ���ϴ�.
    std::ifstream file(filename);

    // ������ ����� ���ȴ��� Ȯ���մϴ�.
    // ���� ������ ���ų� �� �� ���� ���, ���� �޽����� ����ϰ� �� ����� ��ȯ�մϴ�.
    if (!file.is_open())
    {
        std::cerr << "����: " << filename << " ������ �� �� �����ϴ�." << std::endl;
        return {}; // ����ִ� ���͸� ��ȯ�Ͽ� ������ ������ �˸��ϴ�.
    }

    std::string line; // ���Ͽ��� �� �پ� �о�� ���ڿ� ����

    // 1. ������ ù ��° ���� �н��ϴ�. (��: "3 3 4")
    //    ù �ٿ��� ����� ũ��(��, ��)�� 0�� �ƴ� ������ ���� ������ ����ֽ��ϴ�.
    std::getline(file, line);

    // 2. �о�� �� ��(line)�� stringstream ��ü�� �־,
    //    �������� ���е� ���ڵ��� ���� ������ �� �ֵ��� �մϴ�.
    std::stringstream ss(line);
    int rows, cols, nonZeroCount; // ��, ��, 0�� �ƴ� ���� ���� ������ ����
    ss >> rows >> cols >> nonZeroCount; // stringstream���� ���ʴ�� ���� �����Ͽ� ������ ����

    // 3. �־��� ��(rows)�� ��(cols) ũ�⿡ ���� 2���� ����(���)�� �����մϴ�.
    //    ��� ������ �ʱⰪ�� 0���� �����մϴ�.
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));

    // 4. ������ ������ �ٵ��� �����鼭 0�� �ƴ� ���ҵ��� ��Ŀ� ��ġ�մϴ�.
    int r, c, value; // ��(row), ��(column), ��(value)�� ������ ����
    // ������ ������ "�� �� ��" �������� �� �پ� �о���Դϴ�.
    while (file >> r >> c >> value)
    {
        // ��� ���� �ε����� ��ȿ�� ���� ���� �ִ��� Ȯ���մϴ�.
        if (r < rows && c < cols)
        {
            // �ش� ��ġ�� ���� ���� �����մϴ�.
            matrix[r][c] = value;
        }
    }

    // ���� ����� �������Ƿ� �ݾ��ݴϴ�.
    file.close();

    // �ϼ��� 2���� ����� ��ȯ�մϴ�.
    return matrix;
}

// 2���� ����(���)�� ������ ȭ�鿡 ���ڰ� ������ִ� �Լ��Դϴ�.
void printMatrix(const std::vector<std::vector<int>>& matrix)
{
    // ����� ����ִ��� Ȯ���մϴ�.
    if (matrix.empty())
    {
        std::cout << "����� ����ֽ��ϴ�." << std::endl;
        return; // �Լ� ����
    }

    // �ٱ��� for��: ����� �� ��(row)�� ���� �ݺ��մϴ�.
    for (const auto& row : matrix)
    {
        // ���� for��: ���� ��(row)�� �ִ� �� ����(val)�� ���� �ݺ��մϴ�.
        for (int val : row)
        {
            // �� ���� ���� ����ϰ�, ��(	)���� ������ �ݴϴ�.
            std::cout << val << "	";
        }
        // �� ���� ����� ������ �ٹٲ��� �մϴ�.
        std::cout << std::endl;
    }
}

// ���α׷��� �������� main �Լ��Դϴ�.
int main()
{
    // 1. readSparseMatrix �Լ��� ȣ���Ͽ� matA.txt�� matB.txt ���Ϸκ���
    //    �� ���� ���(matA, matB)�� �����մϴ�.
    // 'auto' Ű����� ������ Ÿ���� �����Ϸ��� �ڵ����� �߷��ϰ� �մϴ�.
    // ���⼭�� std::vector<std::vector<int>> Ÿ���� �˴ϴ�.
    auto matA = readSparseMatrix("matA.txt");
    auto matB = readSparseMatrix("matB.txt");

    // 2. ���� �б� �� ������ �߻��ߴ��� Ȯ���մϴ�.
    //    ���� ��� �� �ϳ��� ����ִٸ�, ���α׷��� ������ �ִ� ���̹Ƿ� �����մϴ�.
    if (matA.empty() || matB.empty())
    {
        return 1; // 0�� �ƴ� ���� ��ȯ�Ͽ� ���α׷��� ������������ ����Ǿ����� �˸��ϴ�.
    }

    // 3. ��� ������ ������ �������� Ȯ���մϴ�.
    //    (A�� �� ����)�� (B�� �� ����)�� ���ƾ߸� ������ �����մϴ�.
    //    matA[0].size()�� A�� ù ��° ���� ���� ����, �� ���� ������ �ǹ��մϴ�.
    //    matB.size()�� B�� ���� ������ �ǹ��մϴ�.
    if (matA.empty() || matA[0].empty() || matB.empty() || matA[0].size() != matB.size())
    {
        std::cerr << "����: ��� ������ �� �� ���� ũ���Դϴ�." << std::endl;
        std::cerr << "A ����� ��: " << (matA.empty() ? 0 : matA[0].size()) << ", B ����� ��: " << matB.size() << std::endl;
        return 1; // ���� �޽����� ����ϰ� ������ �����մϴ�.
    }

    // 4. ��� ������ �ʿ��� ũ�� ������ ������ �����մϴ�.
    int rowsA = matA.size();      // A�� �� ����
    int colsA = matA[0].size();   // A�� �� ����
    int colsB = matB[0].size();   // B�� �� ����

    // 5. ���� ����� ������ 2���� ����(result)�� �����մϴ�.
    //    ��� ����� ũ��� (A�� �� ����) x (B�� �� ����)�� �˴ϴ�.
    //    ��� ���Ҵ� 0���� �ʱ�ȭ�մϴ�.
    std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));

    // 6. ��� ������ �����մϴ�. (3�� for�� ���)
    // i: ��� ����� �� �ε���
    for (int i = 0; i < rowsA; ++i)
    {
        // j: ��� ����� �� �ε���
        for (int j = 0; j < colsB; ++j)
        {
            // k: ��� ������ ���Ǵ� �ε���
            for (int k = 0; k < colsA; ++k)
            {
                // result[i][j] = (A�� i��° ��) * (B�� j��° ��)
                result[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    // 7. ���� ����� ȭ�鿡 ����մϴ�.
    std::cout << "��� A�� B�� ���� ���:" << std::endl;
    printMatrix(result); // ������ ���� printMatrix �Լ��� ����մϴ�.

    // ���α׷��� ���������� ����Ǿ����� �ü���� �˸��ϴ�.
    return 0;
}
