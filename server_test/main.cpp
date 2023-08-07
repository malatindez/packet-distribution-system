#include <iostream>

extern "C" __declspec(dllimport) float ComputeSqrt(float x);

int main()
{
    float x = 2.0f;
    float y = ComputeSqrt(x);
    std::cout << y << std::endl;
    return 0;
}