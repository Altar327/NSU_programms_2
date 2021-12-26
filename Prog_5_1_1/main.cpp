#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <algorithm>
#include <ctime>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

queue<string> que;
mutex mut;
condition_variable cond_var;
vector<string> all_pages;
vector<thread> all_thread;
atomic_int score_que = 1;
atomic_bool flag = true;
atomic_int score_working_threads = 0;


void create_new_site(string &address, string &content) {
    ofstream new_page_site;
    new_page_site.open("./output/" + address + ".html");
    new_page_site << content;
    new_page_site.close();
}

void new_Thread() {
    while (true) {
        unique_lock<mutex> lock(mut, defer_lock);
        lock.lock();
//        cond_var.wait(lock, []() -> bool { return flag.load() == false || !que.empty(); });
        cond_var.wait(lock, []() -> bool { return flag.load() == false  || score_que.load() != 0; });
        if (flag.load() == false ) {
            lock.unlock();
            break;
        }
        score_working_threads++;
        string address = que.front();
        que.pop();
        score_que--;
        lock.unlock();

        ifstream read("./test_data/" + address + ".html");
        string content;
        string temp;
        while (read >> temp) {
            content.append(temp);
            content.append(" ");
        }
        create_new_site(address, content);
        size_t pos1 = content.find("<a href=\"");
        while (pos1 != string::npos) {
            content = content.substr(pos1, content.size() - pos1);
            size_t pos2 = content.find(".html\">");
            pos1 = content.find("://") + 3;
            string address_new_pade = content.substr(pos1, pos2 - pos1);
            content = content.substr(pos1, content.size() - pos1);
            lock.lock();
            if (find(all_pages.begin(), all_pages.end(), address_new_pade) == all_pages.end()) {
                all_pages.push_back(address_new_pade);
                que.push(address_new_pade);
                score_que++;
            }
            lock.unlock();
            pos1 = content.find("<a href=\"");
        }

        cond_var.notify_all();

        lock.lock();
        score_working_threads--;
        if (flag.load() == false) {
            break;
        }
        lock.unlock();
    }
};

int main() {
    clock_t start, finish;
    string start_address;
    int count_thread;
    ifstream read("input.txt");
    read >> start_address >> count_thread;
//    for (int count_thread = 1; count_thread < 500; count_thread++) {
    start = clock();
    que.push(start_address);
    all_pages.push_back(start_address);
    for (int i = 0; i < count_thread; i++) {
        all_thread.push_back(thread(new_Thread));
    }
    cond_var.notify_all();


    while (score_working_threads.load() != 0 || score_que.load() != 0) {
//        cond_var.notify_all();
    }
//
//    if (que.empty()) {
//        cout << "AAAAAAAAAAAAAAAAAAAAAAA" << endl;
//    }
//
//
//    while (!que.empty()) {
//        cout << que.front() << endl;
//        que.pop();
//    }

    mut.lock();
    flag = false;
    mut.unlock();

    for (int i = 0; i < count_thread; i++) {
        cond_var.notify_all();
    }

    for (int i = 0; i < count_thread; i++) {
        all_thread[i].join();
    }

    finish = clock();
    cout << (double) (finish - start) / CLOCKS_PER_SEC << " " << all_pages.size();
    return 0;
}