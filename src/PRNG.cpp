#include "PRNG.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

uint rand(uint modulus) {
    return gen() % modulus;
}
