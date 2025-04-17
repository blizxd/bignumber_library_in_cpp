#pragma once

#include "bigNumFn.h"
#include <cstdint>
#include <string>

class BigNumber
{
private:
    std::string _digits;
    uint16_t _decimalPoint;
    bool _isNegative = false;

    static int _precision;
    static int _printPrecision;

    static const BigNumber _zero;
    static const BigNumber _one;
    static const BigNumber _two;
    static const BigNumber _ten;

    void removeTrailingZeroes();
    void removeLeadingZeroes();
    static bool isValidInput(const std::string &str);
    static void allignDecimalPlaces(BigNumber &left, BigNumber &right);
    bool compareAbsValue(const BigNumber &other) const;

    BigNumber addAbsValue(const BigNumber &other) const;
    BigNumber subtractAbsValue(const BigNumber &other) const;
    BigNumber multiplyAbsValue(const BigNumber &other) const;
    BigNumber divideAbsValue(const BigNumber &other) const;

    BigNumber directAdd(const BigNumber &other) const;
    BigNumber directSubtract(const BigNumber &other) const;
    BigNumber directMultiply(const BigNumber &other) const;
    BigNumber fastMod(const BigNumber &p) const;

    std::pair<BigNumber, BigNumber> splitNumber(int pos) const;
    static int makeEqualLength(BigNumber &n1, BigNumber &n2);
    BigNumber shiftIntegerLeft(int decPlaces) const;
    BigNumber karatsubaMult(const BigNumber &other) const;
    BigNumber optimizedMultAbsValue(const BigNumber &other) const;
    BigNumber multiplyHandler(const BigNumber &other) const;

public:
    bool isInt() const;
    bool isNegative() const;
    BigNumber abs() const;
    BigNumber truncate(int precision) const;
    BigNumber round(int precision) const;
    bool isOdd() const;

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

    static void setPrecision(int precision);
    static int getPrecision();

    BigNumber(const std::string &str);
    BigNumber(double num);
    BigNumber();
    std::string toString();
};
