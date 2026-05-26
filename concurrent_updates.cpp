// clang++ -std=c++17 -Wall -Wextra -pedantic concurrent_updates.cpp -pthread -o concurrent_updates
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

using namespace std;

struct ThreadState {
  int64_t temp = 0;
  int64_t done = 0;
};

void read_count(vector<ThreadState>& states, int64_t id, int64_t count) {
  states[id].temp = count;
}

void write_count(vector<ThreadState>& states, int64_t id, int64_t& count) {
  count = states[id].temp + 1;
  states[id].done++;
}

int64_t random_threaded_run() {
  atomic<int64_t> count(0);
  vector<thread> threads;

  for (int64_t id = 0; id < 100; id++) {
    threads.emplace_back([&count, id] {
      random_device rd;
      mt19937 gen(rd() + static_cast<uint32_t>(id));
      uniform_int_distribution<int64_t> dist(0, 50);

      for (int64_t i = 0; i < 100; i++) {
        int64_t temp = count.load(memory_order_relaxed);
        this_thread::sleep_for(chrono::microseconds(dist(gen)));
        count.store(temp + 1, memory_order_relaxed);
      }
    });
  }

  for (thread& t : threads) {
    t.join();
  }

  return count.load(memory_order_relaxed);
}

int64_t maximum_path() {
  int64_t count = 0;

  for (int64_t id = 0; id < 100; id++) {
    for (int64_t i = 0; i < 100; i++) {
      int64_t temp = count;
      count = temp + 1;
    }
  }

  return count;
}

int64_t minimum_path() {
  vector<ThreadState> states(100);
  int64_t count = 0;

  for (int64_t id = 0; id < 100; id++) {
    read_count(states, id, count);
  }

  write_count(states, 0, count);

  read_count(states, 0, count);
  write_count(states, 0, count);

  for (int64_t iter = 3; iter <= 99; iter++) {
    int64_t helper = iter - 2;
    write_count(states, helper, count);
    read_count(states, 0, count);
    write_count(states, 0, count);
  }

  write_count(states, 98, count);
  read_count(states, 0, count);

  for (int64_t id = 1; id < 100; id++) {
    if (states[id].done == 0) {
      write_count(states, id, count);
    }

    while (states[id].done < 100) {
      read_count(states, id, count);
      write_count(states, id, count);
    }
  }

  write_count(states, 0, count);

  return count;
}

int main() {
  cout << "Random threaded runs:" << endl;

  for (int64_t i = 0; i < 10; i++) {
    cout << "run " << i + 1 << ": " << random_threaded_run() << endl;
  }

  cout << endl;
  cout << "Maximum possible final count: " << maximum_path() << endl;
  cout << "Minimum possible final count: " << minimum_path() << endl;

  return 0;
}
