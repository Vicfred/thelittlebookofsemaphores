// clang++ -std=c++17 -Wall -Wextra -pedantic nondeterminism.cpp -pthread -o nondeterminism
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

using namespace std;

void print_char(char c) {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int64_t> dist(0, 5);

  this_thread::sleep_for(chrono::milliseconds(dist(gen)));

  cout << c;
}

int main() {
  for (int64_t i = 0; i < 20; i++) {
    thread a(print_char, 'a');
    thread b(print_char, 'b');

    a.join();
    b.join();

    cout << endl;
  }

  return 0;
}
