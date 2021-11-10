#include <iostream>
#include <stdio.h>
#include <iomanip>

using namespace std;

class Simulation;

class Animal{
protected:
    int x;                  //Координаты
    int y;
    int way;              //Направление пути
    int delay;              //"Постоянство зверя"
    int current_delay;
    int age;                   //Возраст
    int step;
//    int deltaForField;
public:
    Animal (int step) : age(0), step(step), current_delay(0) {
        cin >> x;
        cin >> y;
        cin >> way;
        cin >> delay;
    }

    Animal (int x, int y, int way, int delay, int step) :
            x(x), y(y), way(way), delay(delay), age(0), step(step), current_delay(0) {}

    void fixed (int N, int M) {
        if (x < 0) x = N + x;
        if (x >= N) x = x - N;
        if (y < 0) y = M + y;
        if (y >= M) y = y - M;
    }
    void change_delay () {
        if (current_delay == delay) {
            way++;
            if (way > 3) way -= 4;
            current_delay = 0;
        }
    }

    void moving (int N, int M) {
        switch (way) {
            case 0:
                y -= step;
                break;
            case 1:
                x += step;
                break;
            case 2:
                y += step;
                break;
            case 3:
                x -= step;
                break;
        }
        fixed (N, M);
        current_delay++;
    }

    int getX () {
        return x;
    }

    int getY () {
        return y;
    }
    void aging () {
        age++;
    }

    int getAge () {
        return age;
    }

//    int get_deltaForField() {
//        return deltaForField;
//    }

    virtual int deltaForField () = 0;
    virtual bool death () = 0;
    virtual Animal* multiply () = 0;
    virtual bool kus (Animal*) = 0;
};


class Rabbit : public Animal {
public:
    Rabbit () : Animal (1) {}

    Rabbit (int x, int y, int Way, int Delay) : Animal (x, y, Way, Delay, 1) {}

    int deltaForField() {
        return 1;
    }

    Rabbit* multiply () {
        if (age == 5 || age == 10) {
            return new Rabbit(x, y, way, delay);
        } else return NULL;
    }

   bool kus (Animal* ani) {
       return 0;
//       "Я кролик, есть я не умею, но разработчик сказал, что добавит мне нож"
    }

   bool death() {
        return (age == 10);
    }
};

class Wolf : public Animal {
protected:
    int satiety;                //Сытность
public:
    Wolf () : Animal(2), satiety(0) {}

    Wolf (int x, int y, int Way, int Delay) : Animal(x, y, Way, Delay, 2), satiety(0) {}

    int deltaForField() {
        return -1;
    }

    Animal* multiply () {
        if (satiety >= 2) {;
            satiety = 0;
            return new Wolf(x, y, way, delay);
        } else return NULL;
    }

    bool kus (Animal* ani) {
        if (ani->getX() == x && ani->getY() == y && ani->deltaForField() == 1) {
            satiety++;
            return true;
        } else return false;
    }

    bool death() {
        return age == 15;
    }

};

class Hyena : public Wolf {
public:
    Hyena () : Wolf () {}

    bool kus (Animal* ani) {
        if (ani->getX() == x && ani->getY() == y && satiety < 2) {
            satiety++;
            return true;
        } else return false;
    }

};

class Simulation {
private:
    int N;                      //Размерности поля по X и Y соответсвенно
    int M;
    int T;                      //Кол-во ходов
    int** field;
    Animal** Animals;
    int size_array_animal;
//    int number_rabbit;
//    int number_wolf;
//    int number_hyena;
    int number_animal;

//    void check_population (int i, int delta) {
//        if (dynamic_cast<Rabbit *>(Animals[i])) {
//            number_rabbit += delta;
//        } else if (dynamic_cast<Wolf *>(Animals[i])) {
//            number_wolf += delta;
//        } else if (dynamic_cast<Hyena *>(Animals[i])) {
//            number_hyena += delta;
//        }
//    }

//    int number_animal () {
//        return number_rabbit + number_wolf + number_hyena;
//    }

    void fixed_array_Animals (int n) {
        for (int i = n; i < number_animal; i++) {
            Animals[i] = Animals[i + 1];
        }
    }

    void allocate_memory () {
        field = new int *[N];
        for (int i = 0; i < N; i++) {
            field[i] = new int [M];
            for (int j = 0; j < M; j++) {
                field[i][j] = 0;
            }
        }
    }

    void overexposure_memory () {
        if (number_animal >= size_array_animal) {
            Animal** new_Animals = new Animal *[size_array_animal * 2];
            for (int i = 0; i < size_array_animal; i++) {
                new_Animals[i] = Animals[i];
            }
            delete[] Animals;
            Animals = new_Animals;
            size_array_animal *= 2;
        }
    }

public:
    Simulation () {
        cin >> N;
        cin >> M;
        cin >> T;

        int number_rabbit;
        int number_wolf;
        int number_hyena;

        cin >> number_rabbit;
        cin >> number_wolf;
        cin >> number_hyena;

        number_animal = number_rabbit + number_hyena + number_wolf;
        size_array_animal = number_animal;

        allocate_memory();

        int i = 0;
        Animals = new Animal *[size_array_animal * 2];
        for (; i < number_rabbit; i++) {
            Animals[i] = new Rabbit ();
        }
        for (; i < number_animal; i++) {
            Animals[i] = new Wolf ();
        }
    }

    void print_Simulation () {
        for (int j = 0; j < N; j++) {
            for (int i = 0; i < M; i++) {
                if (field[i][j]) {
                    cout /*<< setw(3)*/ << field[i][j];
                } else {
                    cout /*<< setw(3)*/ << '#';
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    void death_Animal (int i) {
        number_animal--;
//        check_population(i, -1);
        delete Animals[i];
        fixed_array_Animals(i);
    }

//    friend ostream& operator <<(ostream& os, const Simulation& Sim) {
//        for (int j = 0; j < N; j++) {
//            for (int i = 0; i < M; i++) {
//                if (field[i][j]) {
//                    cout << setw(3) << field[i][j];
//                } else {
//                    cout << setw(3) << '#';
//                }
//            }
//            cout << endl;
//        }
//        cout << endl;
//    }

    void turn () {
        for (int i = 0; i < number_animal; i++) {
            Animals[i]->moving(N, M);
            Animals[i]->change_delay();
        }

        for (int i = 0; i < number_animal; i++) {
            if (Animals[i]->deltaForField() == -1) {
                for (int j = 0; j < number_animal; j++) {
                    if (i != j) {
                        if (Animals[i]->kus(Animals[j])) {
                            death_Animal(j);
                            j--;
                            i--;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < number_animal; i++) {         //Сказано, что старение происходит после питания
            Animals[i]->aging();
        }

        for (int i = 0; i < number_animal; i++) {
            overexposure_memory ();
            Animals[number_animal] = Animals[i]->multiply();
            if (Animals[number_animal]) {
                number_animal++;
//                check_population(number_animal, 1);
            }
            if (Animals[i]->death()) {
                death_Animal(i);
                i--;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                field[i][j] = 0;
            }
        }

        for (int g = 0; g < number_animal; g++) {
            if (Animals[g]->deltaForField() == 1) {
                field[Animals[g]->getX()][Animals[g]->getY()]++;
            } else if (Animals[g]->deltaForField() == -1) {
                field[Animals[g]->getX()][Animals[g]->getY()]--;
            }
        }
    }

    void run_Simulation() {
        for (int i = 0; i < T; i++) {
            turn();
//            print_Simulation();
        }
    }

    ~Simulation() {
        for (int i = 0; i < number_animal; i++) {
            delete Animals[i];
        }
        delete[] Animals;

        for (int i = 0; i < N; i++) {
            delete field[i];
        }
        delete[] field;
    }
};


int main() {
    Simulation Game = Simulation ();
//    Game.print_Simulation();
    Game.run_Simulation();
    Game.print_Simulation();
    return 0;
}
