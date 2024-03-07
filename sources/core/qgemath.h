#ifndef __QGE_MATH_H__
#define __QGE_MATH_H__

int next_power_of2(int x) {
    if (x == 1) return 2;
    int ret = 1;
    while (ret < x) ret <<= 1;
    return ret;
}

float RandomFloat() {
    float Max = RAND_MAX;
    return ((float)rand() / Max);
}

float RandomFloatRange(float Start, float End) {
    if (End == Start) {
        printf("Invalid random range: (%f, %f)\n", Start, End);
        exit(0);
    }

    float Delta = End - Start;
    float RandomValue = RandomFloat() * Delta + Start;
    return RandomValue;
}

#endif // !__QGE_MATH_H__