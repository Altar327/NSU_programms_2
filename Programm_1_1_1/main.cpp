#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class Matrix;

class SmartArray {
private:
    bool flag;
    int a;
    Matrix &S;
    friend class Matrix;
public:
    SmartArray(bool flag, int a, Matrix &S) : flag(flag), a(a), S(S) {}

    void printArr() const;
    int& operator[] (int b);
};

void _swap (int& a, int& b) {
    int c = a;
    a = b;
    b = c;
}



class Matrix {
    int n;                  //Размерность матрицы
    int **A;                //Сама матрица
    friend class SmartArray;

    void matrix_allocate_memory () {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
        }
    }
public:
    Matrix() : n(0), A(nullptr) {}

    Matrix(int N) : n(N) {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
            for (int j = 0; j < n; j++) {
                cin >> A[i][j];
            }
        }
    };

    Matrix& operator=(const Matrix& inst) {
        n = inst.n;

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

    Matrix(int N, int k) : n(N) {
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
    };

//    Matrix(int N, int **values) : n(N) {
//        this->A = values;
//    }

//    matrix(int N1, int N2, int **values) {                //Конструктор матрицы размерности n1 на n2
//        A = new int *[N1];
//        for (int i = 0; i < N1; i++) {
//            A[i] = new int [n];
//            for (int j = 0; j < N2; j++) {
//                cin >> A[i][j];
//            }
//        }
//    };

    int get_n() const {
        return n;
    }

    int get_element(int i, int j) const {
        return A[i][j];
    }

    void print_matrix() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << A[i][j] << ' ';
            }
            cout << endl;
        }
    };

    void fout_matrix() const {
        ofstream name_file;
        name_file.open("test");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                name_file << A[i][j];
            }
            name_file << endl;
//            cout << '\n';
        }
    };

//    void fin_matrix() const {
//        ofstream name_file;
//        name_file.open("test");
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//                name_file << A[i][j];
//            }
//            name_file << endl;
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
        delete[] A;
    };


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
//        matrix mS (n, S);
//        for (int i = 0; i < n; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
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
//        matrix mS (n, S);
//        for (int i = 0; i < n; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
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
//        matrix mS (n - 1, S);
//        for (int i = 0; i < n - 1; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
        return m;
    }

    SmartArray operator [] (const int a) {              //Взятие строки
        if (n < a) {
            throw "Я всего лишь машина откуда мне знать как это работает";
        }
        return SmartArray(0, a, *this);
    }

    SmartArray operator () (const int a) {          //Взятие столбца
        if (n < a) {
            throw "Я всего лишь машина откуда мне знать как это работает";
        }
        return SmartArray(1, a, *this);
    }

    Matrix& operator ! () {            //Транспонирование матрицы
        //int **S = new int *[n];
        for (int i = 0; i < n; i++) {
            //S = new int *[n];
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
                    return 0;
                }
            }
        }
        return 1;
    }

    Matrix operator != (const Matrix &m2) {
        if (n != m2.n) {
            cout << "Я всего лишь машина откуда мне знать как это сравнивать";
            throw "Press F to pay respect";

        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (A[i][j] != m2.A[i][j]) {
                    return 1;
                }
            }
        }
        return 0;
    }

};


int& SmartArray:: operator [] (int b) {
    if (b > S.n) {
        throw "Я всего лишь машина откуда мне знать как это работает";
    }
    if (flag) {
        return S.A[b][a];
    } else {
        return S.A[a][b];
    }
}

void SmartArray:: printArr() const {
    if (!flag) {
        for (int i = 0; i < S.n; i++) {
            cout << S.A[a][i] << ' ';
        }
    } else {
        for (int i = 0; i < S.n; i++) {
            cout << S.A[i][a] << endl;
        }
    }
}



int main() {
    int n, k;
    cin >> n;
    cin >> k;
    Matrix A = Matrix(n);
    Matrix B = Matrix(n);
    Matrix C = Matrix(n);
    Matrix D = Matrix(n);
    Matrix K = Matrix(n, k);
//
    ((A + (B * !C) + K) * (!D)).print_matrix();
//
//    matrix K = matrix(n, k);
//    K(2, 2).print_matrix();
//
//
//    !C;
//    !D;
//    B * C;
//    A + B;
//    A + K;
//    A * D;
//    A.print_matrix();
//delete
//K[2].printArr();
    return 0;
}
