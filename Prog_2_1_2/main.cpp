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
public:
    Animal (int step) : age(0), step(step), current_delay(0) {
        cin >> x;
        cin >> y;
        cin >> way;
        cin >> delay;
    }

    Animal (int x, int y, int way, int delay, int step) : x(x), y(y), way(way), delay(delay), age(0), step(step), current_delay(0) {}

    void fixed (int N, int M) {
        if (x < 0) {
            x = N + x;
        }
        if (x >= N) {
            x = x - N;
        }
        if (y < 0) {
            y = M + y;
        }
        if (y >= M) {
            y = y - M;
        }
    }
    void change_delay () {
        if (current_delay == delay) {
            way++;
            if (way > 3) {
                way -= 4;
            }
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

    virtual bool death () = 0;
    virtual Animal* multiply () = 0;
    virtual bool kus (Animal*) = 0;
};


class Rabbit : public Animal {
public:
    Rabbit () : Animal (1) {}

    Rabbit (int x, int y, int Way, int Delay) : Animal (x, y, Way, Delay, 1) {}

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
        if (age == 10) {
            return true;
        } else {
            return false;
        }
    }


};

class Wolf : public Animal {
protected:
    int satiety;                //Сытность
public:
    Wolf () : Animal(2), satiety(0) {}

    Wolf (int x, int y, int Way, int Delay) : Animal(x, y, Way, Delay, 2), satiety(0) {}

    Animal* multiply () {
        if (satiety >= 2) {;
            satiety = 0;
            return new Wolf(x, y, way, delay);
        } else return NULL;
    }

    bool kus (Animal* ani) {
        if (ani->getX() == x && ani->getY() == y && dynamic_cast<Rabbit*>(ani)) {
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
    int number_rabbit;
    int number_wolf;
    int number_hyena;

    void check_population (int i, bool flag) {
        if (flag) {
            if (dynamic_cast<Rabbit *>(Animals[i])) {
                number_rabbit++;
            } else if (dynamic_cast<Wolf *>(Animals[i])) {
                number_wolf++;
            } else if (dynamic_cast<Hyena *>(Animals[i])) {
                number_hyena++;
            }
        } else {
            if (dynamic_cast<Rabbit *>(Animals[i])) {
                number_rabbit--;
            } else if (dynamic_cast<Wolf *>(Animals[i])) {
                number_wolf--;
            } else if (dynamic_cast<Hyena *>(Animals[i])) {
                number_hyena--;
            }
        }
    }

    int number_animal () {
        return number_rabbit + number_wolf + number_hyena;
    }

    void fixed_array_Animals () {
        for (int i = 0; i < number_animal(); i++) {
            Animals[i] = Animals[i + 1];
        }
    }

public:
    Simulation () {
        cin >> N;
        cin >> M;
        cin >> T;
        cin >> number_rabbit;
        cin >> number_wolf;
        cin >> number_hyena;

        field = new int *[N];
        for (int i = 0; i < N; i++) {
            field[i] = new int [M];
            for (int j = 0; j < M; j++) {
                field[i][j] = 0;
            }
        }

        int i = 0;
        Animals = new Animal *[(number_animal()) * 10];
        for (; i < number_rabbit; i++) {
            Animals[i] = new Rabbit ();
//            field[Animals[i]->getX()][Animals[i]->getY()]++;
        }
        for (; i < number_animal(); i++) {
            Animals[i] = new Wolf ();
//            field[Animals[i]->getX()][Animals[i]->getY()]--;
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
        check_population(i, false);
        delete Animals[i];
        fixed_array_Animals();
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
        for (int i = 0; i < number_animal(); i++) {
            Animals[i]->moving(N, M);
            Animals[i]->change_delay();
        }

        for (int i = 0; i < number_animal(); i++) {
            if (dynamic_cast<Wolf *>(Animals[i]) || dynamic_cast<Hyena *>(Animals[i])) {
                for (int j = 0; j < number_animal(); j++) {
                    if (i != j) {
                        if (Animals[i]->kus(Animals[j]) == true) {
                            death_Animal(j);
                            j--;
                            i--;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < number_animal(); i++) {         //Сказано, что старение происходит после питания
            Animals[i]->aging();
        }

        for (int i = 0; i < number_animal(); i++) {
            Animals[number_animal()] = Animals[i]->multiply();
            if (Animals[number_animal()]) {
                check_population(number_animal(), true);
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

        for (int g = 0; g < number_animal(); g++) {
            if (dynamic_cast<Rabbit *>(Animals[g])) {
                field[Animals[g]->getX()][Animals[g]->getY()]++;
            } else if (dynamic_cast<Wolf *>(Animals[g]) || dynamic_cast<Hyena *>(Animals[g])) {
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

};


int main() {
    Simulation Game = Simulation ();
//    Game.print_Simulation();
    Game.run_Simulation();
    Game.print_Simulation();
    return 0;
}
