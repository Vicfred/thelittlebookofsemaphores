// clang++ -std=c++17 -Wall -Wextra -pedantic concurrent_writes.cpp -pthread -o concurrent_writes
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <thread>

using namespace std;

void pause_randomly(mt19937& gen) {
  uniform_int_distribution<int64_t> dist(0, 2000);
  this_thread::sleep_for(chrono::microseconds(dist(gen)));
}

void thread_a(atomic<int64_t>& x, atomic<int64_t>& printed) {
  random_device rd;
  mt19937 gen(rd());

  pause_randomly(gen);
  x.store(5, memory_order_relaxed);

  pause_randomly(gen);
  printed.store(x.load(memory_order_relaxed), memory_order_relaxed);
}

void thread_b(atomic<int64_t>& x) {
  random_device rd;
  mt19937 gen(rd());

  pause_randomly(gen);
  x.store(7, memory_order_relaxed);
}

int main() {
  int64_t output5_final5 = 0;
  int64_t output5_final7 = 0;
  int64_t output7_final7 = 0;
  int64_t output7_final5 = 0;

  for (int64_t i = 0; i < 1000; i++) {
    atomic<int64_t> x(0);
    atomic<int64_t> printed(-1);

    thread a(thread_a, ref(x), ref(printed));
    thread b(thread_b, ref(x));

    a.join();
    b.join();

    int64_t output = printed.load(memory_order_relaxed);
    int64_t final_value = x.load(memory_order_relaxed);

    if (output == 5 && final_value == 5) {
      output5_final5++;
    } else if (output == 5 && final_value == 7) {
      output5_final7++;
    } else if (output == 7 && final_value == 7) {
      output7_final7++;
    } else if (output == 7 && final_value == 5) {
      output7_final5++;
    }

    if (i < 20) {
      cout << "output = " << output << ", final x = " << final_value << endl;
    }
  }

  cout << endl;
  cout << "output 5, final 5: " << output5_final5 << endl;
  cout << "output 5, final 7: " << output5_final7 << endl;
  cout << "output 7, final 7: " << output7_final7 << endl;
  cout << "output 7, final 5: " << output7_final5 << endl;

  return 0;
}
