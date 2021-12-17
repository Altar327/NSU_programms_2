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
atomic_bool flag = true;
atomic_int score_pages = 0;

void create_new_site(string &address, string &content) {
    ofstream new_page_site;
    new_page_site.open("./input/" + address + ".html");
    new_page_site << content;
    new_page_site.close();
}

void new_Thread(vector<string> &all_pages, queue<string> &queue) {
    while (!que.empty()) {
        unique_lock<mutex> lock(mut);
        cond_var.wait(lock, []() -> bool { return !que.empty(); });

        string address = queue.front();
        queue.pop();
        score_pages++;
        mut.unlock();

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
            if (find(all_pages.begin(), all_pages.end(), address_new_pade) == all_pages.end()) {
                mut.lock();
                all_pages.push_back(address_new_pade);
                queue.push(address_new_pade);
                cond_var.notify_all();
                mut.unlock();
            }
            pos1 = content.find("<a href=\"");
        }
    }
};

int main() {
    clock_t start, finish;
    string start_address;
    int count_thread;
    cin >> start_address >> count_thread;
//    for (int count_thread = 1; count_thread < 500; count_thread++) {
    start = clock();
    que.push(start_address);
    all_pages.push_back(start_address);
    for (int i = 0; i < count_thread; i++) {
        all_thread.push_back(thread(new_Thread, ref(all_pages), ref(que)));
    }

    while (!que.empty()) {
        cond_var.notify_all();
    }

//    flag.store(false);

    for (int i = 0; i < all_thread.size(); i++) {
        cond_var.notify_all();
    }

    for (int i = 0; i < all_thread.size(); i++) {
        all_thread[i].join();
    }

    finish = clock();
    cout << (double) (finish - start) / CLOCKS_PER_SEC << " " << score_pages;

    return 0;
}