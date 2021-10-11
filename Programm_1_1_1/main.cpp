#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class matrix;

class SmartArray {
private:
    int flag;
    int a;
    matrix &S;
    friend class matrix;
public:
    SmartArray(int flag, int a, matrix &S) : flag(flag), a(a), S(S) {}

    void printArr() const;
    int& operator[] (int b);
};

void _swap (int& a, int& b) {
    int c = a;
    a = b;
    b = c;
}



class matrix {
    int n;                  //Размерность матрицы
    int **A;                //Сама матрица
    friend class SmartArray;
public:
    matrix() : n(0), A(nullptr) {}

    matrix(int N) : n(N) {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
            for (int j = 0; j < n; j++) {
                cin >> A[i][j];
            }
        }
    };

    matrix& operator=(const matrix& inst) {
        n = inst.n;
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
            for (int j = 0; j < n; j++) {
                A[i][j] = inst.A[i][j];
            }
        }
        return *this;
    };

    matrix(int N, int k) : n(N) {
        A = new int *[n];
        for (int i = 0; i < n; i++) {
            A[i] = new int [n];
            for (int j = 0; j < n; j++) {
                if (j == i) {
                    A[i][j] = k;
                } else {
                    A[i][j] = 0;
                }
            }
        }
    };

    matrix(int N, int **values) : n(N) {
        this->A = values;
    }

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

    int get_count(int i, int j) const {
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


    ~matrix() {
        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        delete[] A;
    };


    matrix operator +(const matrix &m2) {
        if (n != m2.n) {
            cout << "Невозможно сложить матрицы разной размерности";
            return matrix();
        }
        matrix m_sum = matrix(n, 0);
        int **S = new int *[n];
        for (int i = 0; i < n; i++) {
            S[i] = new int [n];
            for (int j = 0; j < n; j++) {
                S[i][j] = A[i][j] + m2.A[i][j];
            }
        }
//        matrix mS (n, S);
//        for (int i = 0; i < n; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
        return matrix(n, S);
    }

    matrix operator - (const matrix &m2) {
        if (n != m2.n) {
            cout << "Невозможно вычесть матрицы разной размерности";
            return matrix();
        }
        int **S = new int *[n];
        for (int i = 0; i < n; i++) {
            S[i] = new int [n];
            for (int j = 0; j < n; j++) {
                S[i][j] = A[i][j] - m2.get_count(i, j);
            }
        }
//        matrix mS (n, S);
//        for (int i = 0; i < n; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
        return matrix(n, S);
    }

    matrix operator* (const matrix &m2) {
        if (n != m2.n) {
            cout << "Я всего лишь машина откуда мне знать как это умножить";
            return matrix();
        }
        int **S = new int *[n];
        for (int i = 0; i < n; i++) {
            S[i] = new int [n];
            for (int j = 0; j < n; j++) {
                S[i][j] = 0;
                for (int f = 0; f < n; f++) {
                    S[i][j] += A[i][f] * m2.A[f][j];
                }
            }
        }
//        matrix mS (n, S);
//        for (int i = 0; i < n; i++) {
//            delete[] S[i];
//        }
//        delete[] S;
//        return mS;
        return matrix(n, S);
    }

    matrix operator () (const int a, const int b) {             //Взятие минора
        int **S = new int *[n - 1];
        int i_m = 0;
        int j_m = 0;
        for (int i = 0; i < n; i++) {
            if (i != a) {
                S[i_m] = new int [n - 1];
                for (int j = 0; j < n; j++) {
                    if (j != b) {
                        S[i_m][j_m] = A[i][j];
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
        return matrix(n - 1, S);
    }

    SmartArray operator [] (const int a) {              //Взятие строки
        if (n < a) {
            cout << "Я всего лишь машина откуда мне знать как это работает";
            throw;
        }
//        int **S = new int *[n];
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < n; j++) {
//
//            }
//        }
        return SmartArray(0, a, *this);
    }

    SmartArray operator () (const int a) {          //Взятие столбца
        if (n < a) {
            cout << "Я всего лишь машина откуда мне знать как это работает";
            throw;
        }
        return SmartArray(1, a, *this);
    }

    matrix& operator ! () {            //Транспонирование матрицы
        //int **S = new int *[n];
        for (int i = 0; i < n; i++) {
            //S = new int *[n];
            for (int j = 0; j < i; j++) {
                _swap(A[i][j], A[j][i]);
            }
        }
        return *this;
    }

    matrix operator == (const matrix &m2) {
        if (n != m2.get_n()) {
            cout << "Я всего лишь машина откуда мне знать как это сравнивать";
            return matrix();
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

    matrix operator != (const matrix &m2) {
        if (n != m2.n) {
            cout << "Я всего лишь машина откуда мне знать как это сравнивать";
            cout << "Press F to pay respect";
            return matrix();
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
        cout << "Я всего лишь машина откуда мне знать как это работает";
        throw;
    }
    return S.A[a][b];
}

void SmartArray:: printArr() const {
    if (flag == 0) {
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
//    matrix A = matrix(n);
//    matrix B = matrix(n);
//    matrix C = matrix(n);
//    matrix D = matrix(n);
    matrix K = matrix(n, k);
//
//    ((A + (B * !C) + K) * (!D)).print_matrix();
//
////    matrix K = matrix(n, k);
////    K(2, 2).print_matrix();
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
K[2].printArr();
    return 0;
}
