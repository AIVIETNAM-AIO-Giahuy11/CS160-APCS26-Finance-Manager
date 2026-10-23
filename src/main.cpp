<<<<<<< Updated upstream
#include <iostream>
#include <numeric>
#include <DynamicArray.h>
#include <random>
#include <vector>
#include <algorithm>

int main()
{
    std::mt19937 generator(5);
    DynamicArray<int> myVec = {67, 42, 36, 36, 42, 42, 6, 7, 69};
    // std::iota(myVec.begin()+5, myVec.end(), -5);
    for (int i = 0; i<(int)myVec.size(); i++) std::cout << myVec[i] << ' ';
    std::cout << '\n';

    std::shuffle(myVec.begin(), myVec.end(), generator);
    for (int i = 0; i<(int)myVec.size(); i++) std::cout << myVec[i] << ' ';
    std::cout << '\n';

    std::sort(myVec.begin(), myVec.end());
    for (int i = 0; i<(int)myVec.size(); i++) std::cout << myVec[i] << ' ';
    std::cout << '\n';

    for (int i = 1; i<=1; i++) myVec.pop_back();
    for (int i = 0; i<(int)myVec.size(); i++) std::cout << myVec[i] << ' ';
    std::cout << '\n';

    DynamicArray<int> myVec2 = myVec;
    for (int i = 0; i<(int)myVec.size(); i++) std::cout << myVec2[i] << ' ';
    std::cout << '\n';

    return 0;
}
=======
// Simple tests for DynamicArray<T>. No external framework needed.
//
// Build & run:
//   g++ -std=c++17 -Wall -Wextra -fsanitize=address,undefined -g test_dynamic_array.cpp -o test_da
//   ./test_da

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "DynamicArray.h"

static int g_passed = 0;
static int g_failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (cond) {                                                              \
            ++g_passed;                                                          \
        } else {                                                                 \
            ++g_failed;                                                          \
            std::cerr << "  FAILED: " #cond " (line " << __LINE__ << ")\n";      \
        }                                                                        \
    } while (0)

#define RUN(test)                                   \
    do {                                            \
        std::cout << "Running " #test "...\n";      \
        test();                                     \
    } while (0)

// Counts live objects so we can detect leaks / double destruction.
struct Counted {
    static int alive;
    int v;
    Counted(int x = 0) : v(x) { ++alive; }
    Counted(const Counted& o) : v(o.v) { ++alive; }
    Counted(Counted&& o) noexcept : v(o.v) { ++alive; }
    Counted& operator=(const Counted&) = default;
    ~Counted() { --alive; }
};
int Counted::alive = 0;

void test_starts_empty() {
    DynamicArray<int> a;
    CHECK(a.size() == 0);
    CHECK(a.empty());
}

void test_push_back_and_index() {
    DynamicArray<int> a;
    a.push_back(10);
    a.push_back(20);
    a.push_back(30);
    CHECK(a.size() == 3);
    CHECK(a[0] == 10);
    CHECK(a[1] == 20);
    CHECK(a[2] == 30);
    CHECK(a.front() == 10);
    CHECK(a.back() == 30);
}

void test_grows_past_initial_capacity() {
    DynamicArray<int> a;
    for (int i = 0; i < 1000; ++i) a.push_back(i);
    CHECK(a.size() == 1000);
    CHECK(a.capacity() >= 1000);
    bool ok = true;
    for (int i = 0; i < 1000; ++i) ok = ok && (a[i] == i);
    CHECK(ok);
}

void test_pop_back() {
    DynamicArray<int> a;
    a.push_back(1);
    a.push_back(2);
    a.pop_back();
    CHECK(a.size() == 1);
    CHECK(a.back() == 1);
    a.pop_back();
    CHECK(a.empty());
}

void test_pop_back_on_empty_throws() {
    DynamicArray<int> a;
    bool threw = false;
    try { a.pop_back(); } catch (const std::out_of_range&) { threw = true; }
    CHECK(threw);
}

// void test_at_bounds_check() {
//     DynamicArray<int> a;
//     a.push_back(5);
//     CHECK(a.at(0) == 5);
//     bool threw = false;
//     try { a.at(1); } catch (const std::out_of_range&) { threw = true; }
//     CHECK(threw);
// }

void test_clear() {
    DynamicArray<int> a;
    for (int i = 0; i < 10; ++i) a.push_back(i);
    a.clear();
    CHECK(a.size() == 0);
    CHECK(a.empty());
    a.push_back(7);  // still usable after clear
    CHECK(a.size() == 1);
    CHECK(a[0] == 7);
}

void test_reserve() {
    DynamicArray<int> a;
    a.reserve(100);
    CHECK(a.capacity() >= 100);
    CHECK(a.size() == 0);
}

void test_copy_is_deep() {
    DynamicArray<int> a;
    a.push_back(1);
    a.push_back(2);
    DynamicArray<int> b(a);
    b[0] = 99;
    CHECK(a[0] == 1);
    CHECK(b[0] == 99);
    CHECK(b.size() == 2);
}

void test_copy_assignment() {
    DynamicArray<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(9);
    
    b = a;
    CHECK(b.size() == 2);
    CHECK(b[1] == 2);
    b = b;  // self-assignment must be safe
    CHECK(b.size() == 2);
}

void test_move() {
    DynamicArray<int> a;
    for (int i = 0; i < 5; ++i) a.push_back(i);
    DynamicArray<int> b(std::move(a));
    CHECK(b.size() == 5);
    CHECK(b[4] == 4);
    CHECK(a.size() == 0);  // moved-from is empty
}

void test_iteration() {
    DynamicArray<int> a;
    for (int i = 1; i <= 4; ++i) a.push_back(i);
    int sum = 0;
    for (int x : a) sum += x;
    CHECK(sum == 10);
}

void test_strings() {
    DynamicArray<std::string> a;
    a.push_back("hello");
    a.push_back(std::string(100, 'x'));
    CHECK(a[0] == "hello");
    CHECK(a[1].size() == 100);
}

void test_push_back_of_own_element() {
    DynamicArray<int> a;
    a.push_back(42);
    for (int i = 0; i < 50; ++i) a.push_back(a[0]);  // triggers reallocation
    bool ok = true;
    
    for (std::size_t i = 0; i < a.size(); ++i) 
    {
        std::cout << a[i] << ' ';
        ok = ok && (a[i] == 42);
    }
    std::cout << '\n';
    CHECK(ok);
}

void test_no_leaks() {
    Counted::alive = 0;
    {
        DynamicArray<Counted> a;
        for (int i = 0; i < 20; ++i) a.push_back(Counted(i));
        CHECK(Counted::alive == 20);
        a.pop_back();
        CHECK(Counted::alive == 19);
        DynamicArray<Counted> b(a);
        CHECK(Counted::alive == 38);
        a.clear();
        CHECK(Counted::alive == 19);
    }
    CHECK(Counted::alive == 0);
}

int main() {
    RUN(test_starts_empty);
    RUN(test_push_back_and_index);
    RUN(test_grows_past_initial_capacity);
    RUN(test_pop_back);
    RUN(test_pop_back_on_empty_throws);
    // // RUN(test_at_bounds_check);
    RUN(test_clear);
    RUN(test_reserve);
    RUN(test_copy_is_deep);
    RUN(test_copy_assignment);
    // RUN(test_move);
    RUN(test_iteration);
    RUN(test_strings);
    RUN(test_push_back_of_own_element);
    RUN(test_no_leaks);

    std::cout << "\n" << g_passed << " checks passed, " << g_failed << " failed\n";
    return g_failed == 0 ? 0 : 1;
}
>>>>>>> Stashed changes
