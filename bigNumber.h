#ifndef BIG_NUMBER_H
#define BIG_NUMBER_H

#include <cstdint>
#include <string>

class BigNumber
{
private:
    std::string digits;
    uint16_t decimalPoint;
    bool isNegative = false;

    static int precision;
    static int printPrecision;
    static const int iterations = 100;

    static BigNumber zero;
    static BigNumber one;
    static BigNumber two;
    static BigNumber PI;

    void removeTrailingZeroes();
    void removeLeadingZeroes();
    bool isValidInput(const std::string &str) const;
    void allignDecimalPlaces(BigNumber &left, BigNumber &right) const;
    bool compareAbsValue(const BigNumber &other) const;
    BigNumber addAbsValue(const BigNumber &other) const;
    BigNumber subtractAbsValue(const BigNumber &other) const;
    BigNumber multiplyAbsValue(const BigNumber &other) const;
    BigNumber divideAbsValue(const BigNumber &other) const;

public:
    bool isInt() const;
    BigNumber abs() const;
    BigNumber truncate(int precision) const;
    BigNumber round(int precision) const;

    BigNumber operator+(const BigNumber &other) const;
    BigNumber operator-(const BigNumber &other) const;
    BigNumber operator*(const BigNumber &other) const;
    BigNumber operator/(const BigNumber &other) const;
    BigNumber operator%(const BigNumber &other) const;

    bool operator<(const BigNumber &other) const;
    bool operator<=(const BigNumber &other) const;
    bool operator>=(const BigNumber &other) const;
    bool operator>(const BigNumber &other) const;
    bool operator!=(const BigNumber &other) const;
    bool operator==(const BigNumber &other) const;

    BigNumber &operator=(const std::string &str);
    BigNumber &operator+=(const BigNumber &other);
    BigNumber &operator-=(const BigNumber &other);
    BigNumber &operator*=(const BigNumber &other);
    BigNumber &operator/=(const BigNumber &other);
    BigNumber &operator%=(const BigNumber &other);
    BigNumber operator-() const;

    // -------------------------------
    // Static methods
    // -------------------------------

    static BigNumber sin(BigNumber n);
    static BigNumber sqroot(BigNumber n);
    static BigNumber factorial(int num);
    static BigNumber ln(BigNumber n);
    static BigNumber intPow(BigNumber base, int exponent);
    static bool isPrime(BigNumber n);
    static BigNumber nextPrime(BigNumber n);
    static int find(BigNumber arr[], int size, BigNumber target);
    static void bubbleSort(BigNumber arr[], int size);
    static BigNumber getAverage(BigNumber arr[], int size);
    static BigNumber chudnovskyPI(int n);

    // -------------------------------
    // -------------------------------

    static void setPrecision(int precision);
    static int getPrecision();

    BigNumber(const std::string &str);
    BigNumber(double num);
    BigNumber();
    std::string toString();
};

#endif // BIG_NUMBER_H