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

vector<std::string> keywords = {
        "val", "var", "add", "if", "then",
        "else", "let", "in", "function",
        "call"
};

class Env;

class Expression {
protected:
    Env* map;                   //Указатель на хеш-мап с доступними переменными.
                            // Фактически в каждой точки работы программы будут доступны все объявленые переменные
public:
    Expression (Env* map) : map(map) {}

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

    void create_variable (string key, Expression* value) {
        hash_map.insert({key, value});
    }

    Expression* from_env(string id) {
        if (hash_map[id] != nullptr) {
            return hash_map[id];
        } else {
            throw "ERROR";
        }
    }

//    void update_val (string id, Expression* new_value) {
//        if (hash_map[id] != nullptr) {
//            hash_map[id] = new_value;
//        } else {
//            throw "ERROR";
//        }
//    }

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
};

class Let : public Expression {
    string id;
    Expression* value;
    Expression* exp_body;
public:
    Let (string id, Expression* value, Expression* exp_body, Env* map) : Expression(map), id(id), value(value), exp_body(exp_body) {}

    Expression* eval() {
        this->map->create_variable(id, value->eval());
        return exp_body->eval();
    }

    void print () {
        cout << "(let " << id << " = ";
        value->print();
        cout << " in ";
        exp_body->print();
        cout << ")";
    }
};

class Function : public Expression {
    string id;
    Expression* body_func;
    Env start_map;
public:
    Function (string id, Expression* body_func, Env* map) : Expression(map), id(id), body_func(body_func), start_map(*map) {}

    Expression* eval() {
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
};

class Call : public Expression {
    Expression* f_expr;
    Expression* arg_expr;
//    stack ;
public:
    Call (Expression* f_expr, Expression* arg_expr, Env* map) : Expression(map), f_expr(f_expr), arg_expr(arg_expr) {
        if (dynamic_cast<Function*>(f_expr)) {
            throw "ERROR";
        }
    }

    Expression* eval() {
        this->map->create_variable_or_update_val(((Function*)f_expr->eval())->get_id(), arg_expr->eval());
        return new Val(this->map->get_value(((Function*)f_expr->eval())->get_body_func()->eval()), map);
    }

    void print () {
        cout << "(call ";
        f_expr->print();
        cout << " ";
        arg_expr->print();
        cout << ")";
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
            throw "ERROR";
        } else return true;
    }
//    Expression* parse_expression();

    Val* interpretation_Val() {
        int value;
        code >> value;
//        for (stoi(value)){
//            b = stoi(a);
//            cout << b << endl;
//        }
//        else throw "ERROR";

        check_closing_bracket();
        return new Val(value, env_map);
    }

    Let* interpretation_Let() {
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
        return new Let(id, exp_value, exp_body, env_map);
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

        string id;
        code >> id;
//        Expression* value = interpretation_expression();
        Expression* body_func = interpretation_expression();
        check_closing_bracket();
        return new Function(id, body_func, env_map);
    }
    Call* interpretation_Call() {
        Expression* f_expr = interpretation_expression();
        Expression* arg_expr = interpretation_expression();
//        Expression* arg_expr = interpretation_body_func_expression();                       //Если с допом 1, то используем это.
        check_closing_bracket();
        return new Call(f_expr, arg_expr, env_map);
    }

//    Expression* interpretation_body_func_expression() {}

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