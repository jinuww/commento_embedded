#include <iostream>
#include <chrono> // 시간 측정을 위한 헤더

// 1. 일반 함수
float calculateValue_Normal(float sensorReading) {
    return sensorReading * 1.5f + 10.0f;
}

// 2. 인라인 함수
inline float calculateValue_Inline(float sensorReading) {
    return sensorReading * 1.5f + 10.0f;
}

int main() {
    const int iterations = 10000000; 
    const float input = 25.5f;
    float result = 0.0f;

    // --- 일반 함수 속도 측정 ---
    auto start_normal = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        result = calculateValue_Normal(input);
    }
    auto end_normal = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_normal = end_normal - start_normal;
    std::cout << "Normal function elapsed time: " << elapsed_normal.count() << " seconds\n";


    // --- 인라인 함수 속도 측정 ---
    auto start_inline = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        result = calculateValue_Inline(input);
    }
    auto end_inline = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_inline = end_inline - start_inline;
    std::cout << "Inline function elapsed time: " << elapsed_inline.count() << " seconds\n";

    return 0;
}