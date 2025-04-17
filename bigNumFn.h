#pragma once

class BigNumber;

namespace BigNumFn
{
    BigNumber sin(const BigNumber &n);
    BigNumber sqroot(const BigNumber &n);
    BigNumber ceilSqroot(const BigNumber &arg);
    BigNumber factorial(int num);
    BigNumber ln(const BigNumber &n);
    BigNumber intPow(const BigNumber &base, int exponent);
    BigNumber modPower(BigNumber a, BigNumber exponent, const BigNumber &p);
    BigNumber gcd(const BigNumber &a, const BigNumber &b);
    bool isPrimeFerma(const BigNumber &n, int k = 1);
    bool isPrime(const BigNumber &n);
    BigNumber nextPrime(const BigNumber &n);
    int find(const BigNumber arr[], int size, const BigNumber &target);
    void bubbleSort(BigNumber arr[], int size);
    BigNumber getAverage(const BigNumber arr[], int size);
    BigNumber chudnovskyPI(int n);

}