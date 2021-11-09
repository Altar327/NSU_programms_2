#include <iostream>
#include <stdio.h>
#include <iomanip>
//#include "Matrix.cpp"

using namespace std;

template <typename K, typename V>
class Pair {
    K key;
    V value;
public:
    K get_key () {
        return key;
    }

    V get_value () {
        return value;
    }

    Pair(K key, V value) : key(key), value(value) {}
};

template <typename K, typename V>
struct Elem {
    Pair<K, V>* data;
    Elem<K, V>* next;

    bool find_unique_elem(V current_value, K current_key) {
        if (current_key != data->get_key()) {
            if (current_value != data->get_value()) {
                if (next != nullptr) {
                    return next->find_unique_elem(current_value, current_key);
                } else {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    friend ostream& operator << (ostream& os, const Elem<K, V>* elem) {
        cout << elem->data->get_key() << ' ' << elem->data->get_value() << endl;
        if (elem->next != nullptr) {
            cout << elem->next;
        }
        return os;
    }
};

template <typename K, typename V>
class List {
    Elem<K, V>* head;
public:
    List(): head(nullptr) {}

    ~List() {
        Elem<K, V> *current = head;
        while (current != nullptr) {
            Elem<K, V>* temp = head->next;
            delete current;
            current = temp;
        }
    };

    Elem<K, V>* get_head () {
        return head;
    }

    void add_head(K key, V value) {
        Elem<K, V>* e = new Elem<K, V>();
        e->data = new Pair<K, V>(key, value);
        e->next = head;
        head = e;
    }

    void del_from_list(K key) {
        if (key == head->data->get_key()) {
            Elem<K, V> *temp = head->next;
            delete head;
            head = temp;
        } else {
            Elem<K, V> *previous_elem = nullptr;
            Elem<K, V> *current_elem = head;
            while (current_elem != nullptr and current_elem->data->get_key() != key) {
                previous_elem = current_elem;
                current_elem = current_elem->next;
            }
            if (current_elem == nullptr)
                return;

            Elem<K, V> *temp = current_elem->next;
            delete current_elem;

            previous_elem->next = temp;
        }
    }

};

template <typename K, typename V>
class HashMap {
private:
    List<K, V>** table;
    int size;
    int filled_size;

    void allocate_memory () {
        table = new List<K, V>* [size];
        for (int i = 0; i < size; i++) {
            table[i] = new List<K, V>;
        }
    }

    void clear_memory () {
        for (int i = 0; i < size; i++) {
            delete table[i];
        }
        delete table;
    }

    void helper_iterator (int i, int& number_unique_elem, V current_value, K current_key) {
        bool flag = true;
        for (int j = 0; j <= i; j++) {
            if (table[j]->get_head() != nullptr) {
                if (!table[j]->get_head()->find_unique_elem(current_value, current_key)) {
                    flag = false;
                }
            }
        }
        if (flag) {
            number_unique_elem++;
        }
        if (table[i]->get_head()->next != nullptr) {
//            if (number_unique_elem == 0 && filled_size != 0) number_unique_elem++;
            helper_iterator(i, number_unique_elem,
                            table[i]->get_head()->next->data-> get_value(),
                            table[i]->get_head()->next->data->get_key());
        }
    }

public:
    HashMap() : size(100), filled_size(0) {
        allocate_memory();
    }

    HashMap (int size) : size(size), filled_size(0) {
        allocate_memory();
    }

    ~HashMap() {
        clear_memory();
    }

    size_t get_hash(K key) {
        std::hash<K> hash;
        return hash(key) % size;
    }

    void remove_elem (K key) {
        if (table[get_hash(key)]->get_head() != nullptr) {
            table[get_hash(key)]->del_from_list(key);
            filled_size--;
        }
    }

    void add_elem (K key, V value) {
        if (table[get_hash(key)]->get_head() == nullptr) {          //Добовление в таблицу, если коллизии нет
            table[get_hash(key)]->add_head(key, value);
            filled_size++;
            re_hash();
        } else if (table[get_hash(key)]->get_head()->data->get_key() == key) {          //Изменение значения
            remove_elem(key);
            filled_size++;
            table[get_hash(key)]->add_head(key, value);
        } else {                                                       //Добовление, если произошла коллизия
            table[get_hash(key)]->add_head(key, value);
            filled_size++;
            re_hash();
        }
    }

    void re_hash () {
        if (filled_size > (double)(size * 0.6)) {
            List<K, V> **new_table;
            new_table = new List<K, V> *[size * 2];
            for (int i = 0; i < size * 2; i++) {
                new_table[i] = new List<K, V>;
            }

            for (int i = 0; i < size; i++) {
                if (table[i]->get_head() != nullptr) {
                    K current_key = table[i]->get_head()->data->get_key();
                    V current_value = table[i]->get_head()->data->get_value();
                    new_table[get_hash(current_key)]->add_head(current_key, current_value);
                    table[i]->del_from_list(current_key);
                }
            }
            size *= 2;

            clear_memory();
            table = new_table;
        }
    }

    void iterator_print () {
        for (int i = 0; i < size; i++) {
            if (table[i]->get_head() != nullptr) {
                cout << table[i]->get_head();
            }
        }
    }
    
    int iterator_counting_unique_elem() {
        int number_unique_elem = 0;
        for (int i = 0; i < size; i++) {
            if (table[i]->get_head() != nullptr) {
                helper_iterator (i, number_unique_elem,
                                 table[i]->get_head()->data->get_value(),
                                 table[i]->get_head()->data->get_key());
            }
//            if (number_unique_elem == 0 && filled_size != 0) number_unique_elem++;
        }
        return number_unique_elem;
    }

    void print_count_elem () {
        cout << filled_size << ' ' << iterator_counting_unique_elem();
    }
};

template <typename K, typename V>
void map () {
    HashMap<K, V> hash_map;
    int count_command;

    cin >> count_command;

    char command;
    K key;
    V value;

    for (int i = 0; i < count_command; i++) {
        cin >> command;
        if (command == 'A') {
            cin >> key >> value;
            hash_map.add_elem(key, value);
        } else if (command == 'R') {
            cin >> key;
            hash_map.remove_elem(key);
        }
    }

//    hash_map.iterator_print();
    hash_map.print_count_elem();
}


int main() {
    char ch_key, ch_data;

    cin >> ch_key >> ch_data;

    if (ch_key == 'I' && ch_data == 'I') map<int, int>();
    else if (ch_key == 'I' && ch_data == 'D') map<int, double>();
    else if (ch_key == 'I' && ch_data == 'S') map<int, std::string>();
    else if (ch_key == 'D' && ch_data == 'I') map<double, int>();
    else if (ch_key == 'D' && ch_data == 'D') map<double, double>();
    else if (ch_key == 'D' && ch_data == 'S') map<double, std::string>();
    else if (ch_key == 'S' && ch_data == 'I') map<std::string, int>();
    else if (ch_key == 'S' && ch_data == 'D') map<std::string, double>();
    else if (ch_key == 'S' && ch_data == 'S') map<std::string, std::string>();

    return 0;
}
