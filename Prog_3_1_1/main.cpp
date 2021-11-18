//#include <iostream>
#include <stdio.h>
#include <iomanip>
#include "Matrix.h"

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

    bool check_list_on_collision (K key) {
        if (key == data->get_key()) {
            return true;
        } else if (next != nullptr) {
            return next->check_list_on_collision(key);
        } else {
            return false;
        }
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

    bool del_from_list(K key) {
        if (key == head->data->get_key()) {
            Elem<K, V> *temp = head->next;
            delete head;
            head = temp;
            return true;
        } else {
            Elem<K, V> *previous_elem = nullptr;
            Elem<K, V> *current_elem = head;
            while (current_elem != nullptr and current_elem->data->get_key() != key) {
                previous_elem = current_elem;
                current_elem = current_elem->next;
            }
            if (current_elem == nullptr)
                return false;

            Elem<K, V> *temp = current_elem->next;
            delete current_elem;

            previous_elem->next = temp;
            return true;
        }
    }

    bool del_all_elem_from_list(K key) {
        if (head != nullptr) {
            if (key == head->data->get_key()) {
                Elem<K, V> *temp = head->next;
                delete head;
                head = temp;
                return true;
            } else {
                Elem<K, V> *previous_elem = nullptr;
                Elem<K, V> *current_elem = head;
                while (current_elem != nullptr and current_elem->data->get_key() != key) {
                    previous_elem = current_elem;
                    current_elem = current_elem->next;
                }
                if (current_elem == nullptr)
                    return false;

                Elem<K, V> *temp = current_elem->next;
                delete current_elem;

                previous_elem->next = temp;
                return true;
            }
        } else {
            return false;
        }
    }
};

template <typename K, typename V>
class HashMap {
protected:
    List<K, V>** table;
    int size;
    int filled_size;
    double flag_for_re_hash;

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
        delete[] table;
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
            helper_iterator(i, number_unique_elem,
                            table[i]->get_head()->next->data-> get_value(),
                            table[i]->get_head()->next->data->get_key());
        }
    }
public:
    HashMap() : size(100), filled_size(0), flag_for_re_hash(0.6) {
        allocate_memory();
    }

    HashMap (int size, double flag_for_re_hash) : size(size), filled_size(0), flag_for_re_hash(flag_for_re_hash) {
        allocate_memory();
    }

    ~HashMap() {
        clear_memory();
    }

    size_t get_hash(K key) {
        std::hash<K> hash;
        return hash(key) % size;
    }

    size_t get_hash(Matrix M) {
        std::hash<K> hash;

        //Код взят из 2-го семестра... не судите строго... пришлось пойти на жертвы....
        int set_Col[M.get_n()];                         //Массив с "пометками", какую строку мы вычеркиваем
        for (int i = 0; i < M.get_n(); i++) {
            set_Col[i] = 1;
        }
        return hash(M.det(set_Col, 0, M.get_n()));
    }

    List<K, V>** get_table () {
        return table;
    }

    int get_size () {
        return size;
    }

    Pair<K,  V> get_elem (K  key) {
        return table[get_hash(key)]->get_head()->data;
    }

    void remove_elem (K key) {
        if (table[get_hash(key)]->get_head() != nullptr) {
            if (table[get_hash(key)]->del_from_list(key)) {
                filled_size--;
            }
        }
    }

    void add_elem (K key, V value) {
        if (table[get_hash(key)]->get_head() == nullptr) {          //Добовление в таблицу, если коллизии нет
            table[get_hash(key)]->add_head(key, value);
            filled_size++;
            re_hash();
        } else if (table[get_hash(key)]->get_head()->check_list_on_collision(key)) {          //Изменение значения
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
        if (filled_size > (double)(size * flag_for_re_hash)) {
            List<K, V> **new_table;
            new_table = new List<K, V> *[size * 2];
            for (int i = 0; i < size * 2; i++) {
                new_table[i] = new List<K, V>;
            }

            for (Iterator iter = this->begin();  iter != this->end(); ++iter) {
                K current_key = table[iter.get_index()]->get_head()->data->get_key();
                V current_value = table[iter.get_index()]->get_head()->data->get_value();
                new_table[get_hash(current_key)]->add_head(current_key, current_value);
                table[iter.get_index()]->del_from_list(current_key);
            }
            size *= 2;

            clear_memory();
            table = new_table;
        }
    }

    class Iterator {
        int index;
        Elem<K, V>* current_elem;
        HashMap<K, V> *map;
    public:
        Iterator() : index(0), current_elem(nullptr), map(nullptr) {}

        Iterator(int index, Elem<K, V> *currentElem, HashMap<K, V> *mp) : index(index), current_elem(currentElem), map(mp) {}

        Pair<K, V> *operator->() {
            return current_elem->data;
        }

        Elem<K, V> operator*() {
            return *current_elem;
        }

        int get_index () {
            return index;
        }

        Iterator operator ++ () {
            if (current_elem->next != nullptr) {
                current_elem = current_elem->next;
            } else {
                index++;
                while (map->size > index && map->get_table()[index]->get_head() == nullptr) {
                    index++;
                }
                if (map->size <= index)
                    *this = Iterator();
                else
                    current_elem = map->get_table()[index]->get_head();
            }
            return *this;
        }

        bool operator != (Iterator iterator) {
            return current_elem != iterator.current_elem;
        }
    };

    Iterator begin() {
        int index = 0;
        while (this->get_table()[index]->get_head() == nullptr) {
            index++;
        }
        return Iterator(index, this->get_table()[index]->get_head(), this);
    }

    Iterator end() {
        return Iterator();
    }

    void print_all_elem () {
        for (Iterator iter = this->begin();  iter != this->end(); ++iter) {
            cout << iter.get_elem();
        }
    }

    int counting_unique_elem() {
        int number_unique_elem = 0;
//        for (Iterator iter(this); iter.get_elem() != get_end(); iter++) {
        for (Iterator iter = this->begin();  iter != this->end(); ++iter) {
            bool exist = true;
            for(Iterator iter2 = this->begin();  iter2 != iter; ++iter2)
                if(iter->get_value() == iter2->get_value()) {
                    exist = false;
                    break;
                }
            if (exist)
                number_unique_elem++;
        }
//        for (Elem<K, V> elem: *this) {
//            helper_iterator (elem., number_unique_elem,
//                                 iter.get_elem()->data->get_value(),
//                                 iter.get_elem()->data->get_key());
//        }
        return number_unique_elem;
    }

//    int counting_unique_elem() {
//        int number_unique_elem = 0;
//        for (int i = 0; i < size; i++) {
//            if (table[i]->get_head() != nullptr) {
//                helper_iterator (i, number_unique_elem,
//                                 table[i]->get_head()->data->get_value(),
//                                 table[i]->get_head()->data->get_key());
//            }
//        }
//        return number_unique_elem;
//    }

    void print_count_elem () {
        cout << filled_size << ' ' << counting_unique_elem();
    }
};


template <typename K, typename V>
class MultiHashMap : public HashMap<K, V> {
protected:
    friend class List<K, V>;
public:
    MultiHashMap() : HashMap<K, V>() {}

    MultiHashMap (int size) : HashMap<K, V>(size) {}

    void remove_elem (K key) {
        if (this->table[get_hash(key)]->get_head() != nullptr) {
            while (this->table[get_hash(key)]->del_all_elem_from_list(key)) {
                this->filled_size--;
            }
        }
    }

    void add_elem (K key, V value) {
        if (this->table[get_hash(key)]->get_head() == nullptr) {          //Добовление в таблицу, если коллизии нет
            this->table[get_hash(key)]->add_head(key, value);
            this->filled_size++;
            this->re_hash();
        } else {                                                       //Добовление, если произошла коллизия
            this->table[get_hash(key)]->add_head(key, value);
            this->filled_size++;
            this->re_hash();
        }
    }


    size_t get_hash(K key) {
        std::hash<K> hash;
        return hash(key) % this->size;
    }

    size_t get_hash(Matrix M) {
        std::hash<K> hash;

        //Код взят из 2-го семестра... не судите строго... пришлось пойти на жертвы....
        int set_Col[M.get_n()];                         //Массив с "пометками", какую строку мы вычеркиваем
        for (int i = 0; i < M.get_n(); i++) {
            set_Col[i] = 1;
        }
        return get_hash(M.det(set_Col, 0, M.get_n()));
    }

    List<K, V> getting_elem_with_unique_key (K current_key) {
        List<K, V> copy_list = List<K, V>();
        for (typename HashMap<K, V>::Iterator iter = this->begin();  iter != this->end(); ++iter) {
            if (iter.get_elem()->data->get_key() == current_key) {
                copy_list.add_head(iter.get_elem()->data->get_key(), iter.get_elem()->data->get_value());
            }
        }
        return copy_list;
    }

    int counting_elem_with_current_key (K current_key) {
        int count = 0;
        for (typename HashMap<K, V>::Iterator iter = this->begin();  iter != this->end(); ++iter) {
            if (iter.get_elem()->data->get_key() == current_key) {
                count++;
            }
        }
        return count;
    }
};


template <typename K, typename V>
void map () {
    MultiHashMap<K, V> hash_map;
//    HashMap<K, V> hash_map;
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

//    hash_map.print();
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