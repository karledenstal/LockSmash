#include "Detection.h"

Detection* Detection::GetSingleton() {
    static Detection singleton;
    return &singleton;
}

void Detection::createDetection() {
    // ...
}