#include <stdx/math.h>

namespace stdx {

float abs(float value) { return (value < 0.0F) ? -value : value; }
double abs(double value) { return (value < 0.0) ? -value : value; }
long double abs(long double value) { return (value < 0.0L) ? -value : value; }
int abs(int value) { return (value < 0) ? -value : value; }
long abs(long value) { return (value < 0L) ? -value : value; }
long long abs(long long value) { return (value < 0LL) ? -value : value; }

float pow(float base, int exp) {
    float result = 1.0F;
    int absExp = abs(exp);
    while (absExp > 0) {
        if ((absExp & 1) != 0) {
            result *= base;
        }
        base *= base;
        absExp >>= 1;
    }
    return (exp < 0) ? 1.0F / result : result;
}

double pow(double base, int exp) {
    double result = 1.0;
    int absExp = abs(exp);
    while (absExp > 0) {
        if ((absExp & 1) != 0) {
            result *= base;
        }
        base *= base;
        absExp >>= 1;
    }
    return (exp < 0) ? 1.0 / result : result;
}
} // namespace stdx
