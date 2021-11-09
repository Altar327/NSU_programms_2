#include <iostream>
#include <stdio.h>
#include <iomanip>

using namespace std;

class Matrix;
class List_SmartArrays;

class SmartArray {
private:
    bool flag;
    int a;
    const Matrix &S;
    bool flag_death;

    friend class Matrix;
public:
    SmartArray(bool flag, int a, Matrix &S) : flag(flag), a(a), S(S), flag_death(false) {}

    void rise_flag () {
        flag_death= true;
    }

    void printArr() const;
    int& operator[] (int b);
    ~SmartArray();
//    SmartArray &operator= (SmartArray &sm);
};

void _swap (int& a, int& b) {
    int c = a;
    a = b;
    b = c;
}

class Node {
    Node *next_node;
    SmartArray *SmartArray_adress;

    friend class List_SmartArrays;
public:
    Node(SmartArray *SmartArray_adress) : next_node(next_node) {
        this->SmartArray_adress = SmartArray_adress;
    }
};

class List_SmartArrays {
    Node *head, *end;
public:
    List_SmartArrays() : head(nullptr), end(nullptr) {}

    void append(SmartArray *SmartArray_adress) {
        Node *newN = new Node(SmartArray_adress);
        if (head == nullptr && end == nullptr) {
            head = newN;
            end = newN;
        } else {
            end->next_node = newN;
            end = end->next_node;
        }
    }

    void delete_ (SmartArray *SmartArray_adress) {
        if (SmartArray_adress == head->SmartArray_adress) {
            Node *temp = head->next_node;
            delete head;
            head = temp;
        } else {
            Node *previousNode = nullptr;
            Node *currentNode = head;
            while (currentNode != nullptr and currentNode->SmartArray_adress != SmartArray_adress) {
                previousNode = currentNode;
                currentNode = currentNode->next_node;
            }
            if (currentNode == nullptr)
                return;

            Node *temp = currentNode->next_node;
            delete currentNode;
            if (previousNode == head) {
                end = head;
            }
            previousNode->next_node = temp;
        }
    }

    void make_inaccessible() {
        Node *current = head;
        while (current != nullptr) {
            current->SmartArray_adress->rise_flag();
            current = current->next_node;
        }
    }

    ~List_SmartArrays() {
        Node *current = head;
        while (current != nullptr) {
            Node *temp = head->next_node;
            delete current;
            current = temp;
        }
    }
};

class Matrix {
    int n;                  //Размерность матрицы
    int **A;                //Сама матрица
    friend class SmartArray;
    List_SmartArrays* head;

    void matrix_allocate_memory () {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
        }
    }

public:
    Matrix() : n(0), A(nullptr), head(nullptr) {}

    Matrix(int N) : n(N), head(nullptr) {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
            for (int j = 0; j < n; j++) {
                cin >> A[i][j];
            }
        }
        head = new List_SmartArrays;
    };

    Matrix& operator=(const Matrix& inst) {
        n = inst.n;
        head->make_inaccessible();
        delete head;
        head = new List_SmartArrays;
        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        delete[] A;

        matrix_allocate_memory ();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = inst.A[i][j];
            }
        }
        return *this;
    };

    Matrix(int N, int k) : n(N), head(nullptr) {
        matrix_allocate_memory ();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j == i) {
                    A[i][j] = k;
                } else {
                    A[i][j] = 0;
                }
            }
        }
        head = new List_SmartArrays;
    };

    int get_n() const {
        return n;
    }

    int get_element(int i, int j) const {
        return A[i][j];
    }

    List_SmartArrays* get_head() const {
        return head;
    }

//    void print_matrix() const {
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//                cout << A[i][j] << ' ';
//            }
//            cout << endl;
//        }
//    };

//    void fout_matrix() const {
//        ofstream name_file;
//        name_file.open("test");
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//                name_file << A[i][j];
//            }
//            name_file << endl;
//        }
//    };

//    void fin_matrix() const {
//        ofstream name_file;
//        name_file.open("test");
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//                name_file >> A[i][j];
//            }
//            name_file >> endl;
////            cout << '\n';
//        }
//    };

//    matrix (const matrix& orig) {
//        this->numerator = orig.numerator;
//
//        printf("copied!");
//    }


    ~Matrix() {
        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        head->make_inaccessible();
        delete head;
        delete[] A;
    };


//    void operator >> (ostream& os) {
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//                cout << A[i][j] << ' ';
//            }
//            cout << endl;
//        }
//    }


    friend ostream& operator <<(ostream& os, const Matrix& m)
    {
        for (int i = 0; i < m.get_n(); i++) {
            for (int j = 0; j < m.get_n(); j++) {
                cout << m.get_element(i, j) << ' ';
            }
            cout << endl;
        }
        return os;
    }

//    friend ostream& operator << (ostream& os, const Matrix& m)            //перегрузка оператора для считывание из файла
//    {
//        ofstream name_file;
//        name_file.open("test");
//        for (int i = 0; i < m.get_n(); i++) {
//            for (int j = 0; j < m.get_n(); j++) {
//                name_file << m.get_element(i, j);
//            }
//            name_file << endl;
//        }
//        name_file.close();
//        return os;
//    }
//
//    friend istream& operator >> (ostream& os, const Matrix& m)            //перегрузка оператора для вывода из файла
//    {
//        ofstream name_file;
//        name_file.open("test");
//        for (int i = 0; i < m.get_n(); i++) {
//            for (int j = 0; j < m.get_n(); j++) {
//                name_file >> m.get_element(i, j);
//            }
//            name_file >> endl;
//        }
//        name_file.close();
//        return os;
//    }

    Matrix operator +(const Matrix &m2) {
        if (n != m2.n) {
            throw "Невозможно сложить матрицы разной размерности";
        }
        Matrix m = Matrix(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                m[i][j] = A[i][j] + m2.A[i][j];
            }
        }
        return m;
    }

    Matrix operator - (const Matrix &m2) {
        if (n != m2.n) {
            throw "Невозможно вычесть матрицы разной размерности";
        }
        Matrix m = Matrix(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                m[i][j] = A[i][j] - m2.get_element(i, j);
            }
        }
        return m;
    }

    Matrix operator* (const Matrix &m2) {
        if (n != m2.n) {
            throw "Я всего лишь машина откуда мне знать как это умножить";
        }
        Matrix m = Matrix(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                m[i][j] = 0;
                for (int f = 0; f < n; f++) {
                    m[i][j] += A[i][f] * m2.A[f][j];
                }
            }
        }
        return m;
    }

    Matrix operator () (const int a, const int b) {             //Взятие минора
        Matrix m = Matrix(n, 0);
        int i_m = 0;
        int j_m = 0;
        for (int i = 0; i < n; i++) {
            if (i != a) {
                for (int j = 0; j < n; j++) {
                    if (j != b) {
                        m[i_m][j_m] = A[i][j];
                        j_m++;
                    }
                }
                j_m = 0;
                i_m++;
            }
        }
        return m;
    }

    SmartArray operator [] (const int a) {              //Взятие строки
        if (n < a) {
            throw "Я всего лишь машина откуда мне знать как это работает";
        }
        SmartArray sm(0, a, *this);
        head->append(&sm);
        return sm;
    }

    SmartArray operator () (const int a) {          //Взятие столбца
        if (n < a) {
            throw "Я всего лишь машина откуда мне знать как это работает";
        }
        SmartArray sm(1, a, *this);
        head->append(&sm);
        return sm;
    }

    Matrix& operator ! () {            //Транспонирование матрицы
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                _swap(A[i][j], A[j][i]);
            }
        }
        return *this;
    }

    Matrix operator == (const Matrix &m2) {
        if (n != m2.get_n()) {
            throw "Я всего лишь машина откуда мне знать как это сравнивать";
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (A[i][j] != m2.A[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    Matrix operator != (const Matrix &m2) {
        if (n != m2.n) {
            cout << "Я всего лишь машина откуда мне знать как это сравнивать";
            throw "Press F to pay respect";

        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (A[i][j] != m2.A[i][j]) {
                    return true;
                }
            }
        }
        return false;
    }
};


int& SmartArray:: operator [] (int b) {
    if (!flag_death) {
        if (b > S.n) {
            throw "Я всего лишь машина откуда мне знать как это работает";
        }
        if (flag) {
            return S.A[b][a];
        } else {
            return S.A[a][b];
        }
    } else {
        throw "Матрица уже мертва";
    }
}

//SmartArray &SmartArray::operator= (SmartArray &sm) {
//    if (!flag_death) {
//        return sm;
//    } else {
//        throw "Матрица уже мертва";
//    }
//}

void SmartArray:: printArr() const {
    if (!flag_death) {
        if (!flag) {
            for (int i = 0; i < S.n; i++) {
                cout << S.A[a][i] << ' ';
            }
        } else {
            for (int i = 0; i < S.n; i++) {
                cout << S.A[i][a] << endl;
            }
        }
    } else {
        throw "Матрица уже мертва";
    }
}

SmartArray :: ~SmartArray() {
    if (!flag_death) {
        S.get_head()->delete_(this);
    } else {
        throw "Матрица уже мертва";
    }
}
