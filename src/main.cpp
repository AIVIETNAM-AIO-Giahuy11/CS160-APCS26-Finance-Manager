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