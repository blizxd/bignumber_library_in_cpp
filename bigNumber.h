#ifndef BIG_NUMBER_H
#define BIG_NUMBER_H

#include <string>
#include <cstdint>

class BigNumber
{
private:
    std::string digits;
    uint16_t decimalPoint;
    bool isNegative = false;

    static int precision;
    static const int iterations = 100;

    static BigNumber zero;
    static BigNumber one;
    static BigNumber two;
    static BigNumber PI;

    BigNumber();

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

    BigNumber operator=(const std::string &str);
    BigNumber operator+=(const BigNumber &other);
    BigNumber operator-=(const BigNumber &other);
    BigNumber operator*=(const BigNumber &other);
    BigNumber operator/=(const BigNumber &other);
    BigNumber operator%=(const BigNumber &other);
    BigNumber operator-() const;

    static BigNumber sin(BigNumber n);
    static BigNumber sqroot(BigNumber n);
    static void setPrecision(int precision);
    static int getPrecision();
    BigNumber truncate(int precision) const;
    static BigNumber factorial(int num);

    BigNumber(const std::string &str);
    std::string toString();
};

#endif // BIG_NUMBER_H