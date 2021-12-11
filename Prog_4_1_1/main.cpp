#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <stack>
#include <algorithm>
#include <unordered_map>

using namespace std;

//Все "константные" заготовки языка
const char opening_bracket = '(';
const char closing_bracket = ')';
const char assign = '=';
const char mins = '-';

class Env;

class Expression {
protected:
    Env* map;                   //Указатель на хеш-мап с доступними переменными.
                            // Фактически в каждой точки работы программы будут доступны все объявленые переменные
public:
    Expression (Env* map) : map(map) {}

    Env* get_map() {
        return map;
    }

    virtual void print () = 0;
    virtual Expression* eval () = 0;         //Функция обработки "команды"
};

class Val : public Expression {
    int value;
public:
    Val (int value, Env* map) : Expression(map), value(value) {}

    int get_val () {
        return value;
    }

    void print () {
        cout << "(val " << value << ")";
    }

    Expression* eval() {
        return this;
    }
};

class Env {
    unordered_map<string, Expression*> hash_map;
public:
    Env () {}

    Env (Env* env) {
        hash_map = env->hash_map;
    }

    unordered_map<string, Expression*> get_map() {
        return hash_map;
    }

    void updata_map(unordered_map<string, Expression*> new_map) {
        hash_map = new_map;
    }

    Expression* from_env(string id) {
        if (hash_map[id] != nullptr) {
            return hash_map[id];
        } else {
            throw "ERROR";
        }
    }

    void create_variable_or_update_val (string id, Expression* new_value) {
            hash_map.insert({id, new_value});
            hash_map[id] = new_value;
    }

    int get_value (Expression* exp) {
        if (dynamic_cast<Val*>(exp)) {
            return ((Val*)exp)->get_val();
        } else {
            throw "ERROR";
        }
    }
};

class Var : public Expression {
    string id;
public:
    Var (string id, Env* map) : Expression(map), id(id) {}

    void print () {
        cout << "(var " << id << ")";
    }

    string get_id () {
        return id;
    }

    Expression* eval() {
        return map->from_env(id);
    }

};

class Add : public Expression {
    Expression* e1;
    Expression* e2;
public:
    Add (Expression* e1, Expression* e2, Env* map) : Expression(map), e1(e1), e2(e2) {}

    void print () {
        cout << "(add ";
        e1->print();
        cout << " ";
        e2->print();
        cout<< ")";
    }

    Expression* eval() {
        return new Val(this->map->get_value(e1->eval()) + this->map->get_value(e2->eval()), map);
    }

    ~Add () {
        delete e1;
        delete e2;
    }
};

class Exp_if : public Expression {
    Expression* val_1;
    Expression* val_2;
    Expression* exp_true;
    Expression* exp_false;
public:
    Exp_if (Expression* val_1, Expression* val_2, Expression* exp_true, Expression* exp_false, Env* map)
        : Expression(map), val_1(val_1), val_2(val_2), exp_true(exp_true), exp_false(exp_false) {}

    Expression* eval () {
        if (((Val*) val_1->eval())->get_val() > ((Val*) val_2->eval())->get_val()) {
            return exp_true->eval();
        } else {
            return exp_false->eval();
        }
    }

    void print () {
        cout << "(if ";
        val_1->print();
        cout << " ";
        val_2->print();
        cout << " then ";
        exp_true->print();
        cout << " else ";
        exp_false->print();
        cout << ")";
    }

    ~Exp_if () {
        delete val_1;
        delete val_2;
        delete exp_true;
        delete exp_false;
    }
};

class Let : public Expression {
    string id;
    Expression* value;
    Expression* exp_body;
public:
    Let (string id, Expression* value, Expression* exp_body, Env* map) : Expression(map), id(id), value(value), exp_body(exp_body) {}

    Expression* eval() {
        this->map->create_variable_or_update_val(id, value->eval());
        exp_body->get_map()->updata_map(this->map->get_map());
        return exp_body->eval();
    }

    void print () {
        cout << "(let " << id << " = ";
        value->print();
        cout << " in ";
        exp_body->print();
        cout << ")";
    }

    ~Let () {
        delete value;
        delete exp_body;
    }
};

class Function : public Expression {
    string id;
    Expression* body_func;
    Env* start_map;
public:
    Function (string id, Expression* body_func, Env* start_map, Env* map) : Expression(map), id(id), body_func(body_func), start_map(start_map) {}

    Expression* eval() {
        this->map->updata_map(start_map->get_map());
        return this;
    }

    string get_id () {
        return id;
    }

    Expression* get_body_func () {
        return body_func;
    }

    void print () {
        cout << "(function " << id << " ";
        body_func->print();
        cout << ")";
    }

    ~Function() {
        delete body_func;
    }
};

class Call : public Expression {
    Expression* f_expr;
    Expression* arg_expr;
public:
    Call (Expression* f_expr, Expression* arg_expr, Env* map) : Expression(map), f_expr(f_expr), arg_expr(arg_expr) {}

    Expression* eval() {
        if (!dynamic_cast<Function*>(f_expr->eval())) {
            throw "ERROR";
        }

        ((Function*)f_expr->eval())->get_map()->create_variable_or_update_val(((Function*)f_expr->eval())->get_id(), arg_expr->eval());
        ((Function*)f_expr->eval())->get_map()->create_variable_or_update_val(((Var*)f_expr)->get_id(), f_expr->eval());
        Expression* exp = (Expression *) (((Function*)f_expr->eval())->get_body_func()->eval());
//        return new Val(((Function*)f_expr->eval())->get_map()->get_value(((Function*)f_expr->eval())->get_body_func()->eval()), map);
        return exp;
    }

    void print () {
        cout << "(call ";
        f_expr->print();
        cout << " ";
        arg_expr->print();
        cout << ")";
    }

    ~Call () {
        delete f_expr;
        delete arg_expr;
    }
};

class Set : public Expression {
    string id;
    Expression* exp;
public:
    Set (string id, Expression* exp,Env* map) : Expression(map), id(id), exp(exp) {}

    void print () {
        cout << "(set " << id << " ";
        exp->print();
        cout << ")";
    }

    Expression* eval () {
        this->map->create_variable_or_update_val(id, exp);
        return this;
    }

    ~Set () {
        delete exp;
    }
};

class Block : public Expression {
    vector<Expression *> vec;
public:
    Block (vector<Expression *> vec, Env* map) : Expression(map), vec(move(vec)) {}


    void print () {
        cout << "(set ";
        for (auto &elem: vec) {
            elem->print();
            cout << " ";
        }
        cout << ")";
    }

    Expression* eval () {
        for (int i = 0; i < vec.size() - 1; i++) {
            vec[i]->eval();
        }
        return vec.back()->eval();
    }

    ~Block () {
        for (auto &elem: vec) {
            delete elem;
        }
    }
};

class Interpreter {
    stringstream code;
    Env* env_map = new Env();
protected:

    bool check_closing_bracket () {
        string str;
        code >> str;
        if (str[0] != closing_bracket) {
            if (str[0] == opening_bracket) {
                return false;
            } else {
                throw "ERROR";
            }
        } else return true;
    }

    Val* interpretation_Val() {
        int value;
        code >> value;

        check_closing_bracket();
        return new Val(value, env_map);
    }

    Let* interpretation_Let() {
        Env* env_map_current = new Env();
        Env* env_map_copy = env_map;
        env_map = env_map_current;

        //Считываем имя переменной
        string id;
        code >> id;
        //Считываем знак
        string str_assign;
        code >> str_assign;
        if (str_assign[0] != assign) {
            throw "ERROR";
        }
        //Считываем expression к которому мы приравниваем переменную
        Expression* exp_value = interpretation_expression();
        //Считываем ключевое слово in
        string str_in;
        code >> str_in;
        if (str_in != "in") {
            throw "ERROR";
        }
        //Считываем expression в который мы подставляем значение той переменной, которую мы уже считали
        Expression* exp_body = interpretation_expression();
        check_closing_bracket();
        Let* exp = new Let(id, exp_value, exp_body, env_map);
        env_map = env_map_copy;
        return exp;
    }

    Var* interpretation_Var() {
        string id;
        code >> id;
        check_closing_bracket();
        return new Var(id, env_map);
    }
    Add* interpretation_Add() {
        Expression* exp_1 = interpretation_expression();
        Expression* exp_2 = interpretation_expression();
        check_closing_bracket();
        return new Add(exp_1, exp_2, env_map);
    }
    Exp_if* interpretation_Exp_if () {
        Expression* val_1 = interpretation_expression();
        Expression* val_2 = interpretation_expression();
        string temp;
        code >> temp;
        if (temp != "then") {
            throw "ERROR";
        }
        Expression* exp_true = interpretation_expression();
        code >> temp;
        if (temp != "else") {
            throw "ERROR";
        }
        Expression* exp_false = interpretation_expression();
        check_closing_bracket();
        return new Exp_if(val_1, val_2, exp_true, exp_false, env_map);
    }
    Function* interpretation_Function() {
        Env* env_map_current = new Env();
        Env* env_map_copy = env_map;
        env_map = env_map_current;

        string id;
        code >> id;
        Expression* body_func = interpretation_expression();
        check_closing_bracket();
        Function* func = new Function(id, body_func, env_map_copy, env_map);
        env_map = env_map_copy;
        return func;
    }
    Call* interpretation_Call() {
        Expression* f_expr = interpretation_expression();
        Expression* arg_expr = interpretation_expression();
        check_closing_bracket();
        return new Call(f_expr, arg_expr, env_map);
    }

    Set* interpretation_Set() {
        string id;
        code >> id;

        Expression* exp = interpretation_expression();

        check_closing_bracket();

        return new Set(id, exp, env_map);
    }

    Block* interpretation_Block() {
        vector<Expression*> vec;
        while (check_closing_bracket()) {
            code.putback('(');
            vec.push_back(interpretation_expression());
        }
        check_closing_bracket();
        return new Block(vec, env_map);
    }

    Expression* interpretation_expression() {
        string key_word;
        code >> key_word;
        if (key_word[0] == opening_bracket) {
//            Expression *expression = nullptr;
            code >> key_word;
//            cout << key_word << " ";
            if (key_word == "val") {
                return interpretation_Val();
            } else if (key_word == "let") {
                return interpretation_Let();
            } else if (key_word == "var") {
                return interpretation_Var();
            } else if (key_word == "add") {
                return interpretation_Add();
            } else if (key_word == "if") {
                return interpretation_Exp_if();
            } else if (key_word == "function") {
                return interpretation_Function();
            } else if (key_word == "call") {
                return interpretation_Call();
            } else if (key_word == "set") {
                return interpretation_Set();
            } else if (key_word == "block") {
                return interpretation_Block();
            } else {
                throw "ERROR";
            }
        } else {
            throw "ERROR";
        }
        // Читаем закрывающую скобку
    }
public:
    Interpreter(string code) : code(code) {}

    Expression* interpretation() {
        Expression* exp = interpretation_expression();
        exp->eval()->print();
        delete exp;
    }
};

int main() {
    try {
        // Считываем код
        ifstream read("input.txt");
        string code;
        string temp;
        while (read >> temp) {
            for (int i = 0; i < temp.size(); i++) {              //Делаем через индекс так как insert работает через номер элемента
                if ((temp[i] == opening_bracket) && i != temp.size() - 1) {
                    temp.insert(++i, " ");
                    i++;
                } else if ((temp[i] == closing_bracket || temp[i] == assign) && i != 0) {
                    temp.insert(i, " ");
                    i++;
                }
            }
            code.append(temp);
            if (temp[temp.size() - 1] != mins) {
                code.append(" ");
            }
        }
//        cout << code;
        // Думаем как работает код, и работает ли

        Interpreter interpreter(code);
        interpreter.interpretation();
    }
    catch (const char* e)
    {
        cout << "ERROR";
    }

    return 0;
}