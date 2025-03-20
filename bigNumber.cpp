#include <iostream>
#include <cstdint>
#include <array>
#include <string>
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <sstream>
#include <cstring>
#include "bigNumber.h"

using namespace std;

// Static initialization
// ----------------------------------
BigNumber BigNumber::zero("0");
BigNumber BigNumber::one("1");
BigNumber BigNumber::two("2");
BigNumber BigNumber::PI("3.14159265358979323846264");
int BigNumber::precision = 100;
// ----------------------------------

BigNumber::BigNumber() {}

void BigNumber::removeTrailingZeroes()
{
    int zeroesEndIndex = digits.find_last_not_of('0', digits.size() - 1);

    if (zeroesEndIndex < decimalPoint ||
        zeroesEndIndex == string::npos)
    {
        zeroesEndIndex = decimalPoint;
    }
    digits.erase(zeroesEndIndex + 1);
}

void BigNumber::removeLeadingZeroes()
{
    int zeroesEndIndex = digits.find_first_not_of('0', 0);

    if (zeroesEndIndex > decimalPoint - 1 ||
        zeroesEndIndex == string::npos)
    {
        zeroesEndIndex = decimalPoint - 1;
    }

    digits.erase(0, zeroesEndIndex);
    decimalPoint -= zeroesEndIndex;
}

bool BigNumber::isValidInput(const string &str) const
{

    for (int i = 0; i < str.size(); i++)
    {
        if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-')
        {
            return false;
        }
    }
    return true;
}

void BigNumber::allignDecimalPlaces(BigNumber &left, BigNumber &right) const
{

    int leftFracLen = left.digits.size() - left.decimalPoint;
    int rightFracLen = right.digits.size() - right.decimalPoint;
    int diff = rightFracLen - leftFracLen;

    // Check which num has more digits after decimal point
    if (diff > 0)
        left.digits.append(diff, '0'); // Append to left
    else if (diff < 0)
        right.digits.append(-diff, '0'); // Append to right
}

// returns |this|<|other|
bool BigNumber::compareAbsValue(const BigNumber &other) const
{
    if (decimalPoint > other.decimalPoint)
    {
        return false;
    }
    else if (decimalPoint < other.decimalPoint)
    {
        return true;
    }

    int maxSize = max(digits.size(), other.digits.size());

    for (int i = 0; i < maxSize; i++)
    {

        if (i >= digits.size())
            return true;
        if (i >= other.digits.size())
            return false;

        if (digits[i] < other.digits[i])
            return true;

        if (digits[i] > other.digits[i])
            return false;
    }

    return false;
}

BigNumber BigNumber::addAbsValue(const BigNumber &other) const
{
    BigNumber res;
    res.isNegative = false;

    BigNumber left = *this;
    BigNumber right = other;

    allignDecimalPlaces(left, right);

    uint8_t carry = 0;
    uint8_t temp = 0;

    int i = left.digits.size() - 1;
    int j = right.digits.size() - 1;

    while (i >= 0 || j >= 0 || carry)
    {
        temp = carry;

        if (i >= 0)
            temp += left.digits[i--] - '0';
        if (j >= 0)
            temp += right.digits[j--] - '0';

        carry = temp / 10;

        res.digits += temp % 10 + '0';
    }

    reverse(res.digits.begin(), res.digits.end());

    res.decimalPoint = res.digits.size() - (left.digits.size() - left.decimalPoint);

    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

// requires |this| >= |other|
BigNumber BigNumber::subtractAbsValue(const BigNumber &other) const
{
    assert(!compareAbsValue(other));

    BigNumber res;
    res.isNegative = false;

    BigNumber left = *this;
    BigNumber right = other;

    allignDecimalPlaces(left, right);

    int maxIntLen = max(left.decimalPoint, right.decimalPoint);
    int leftPad = maxIntLen - left.decimalPoint;
    int rightPad = maxIntLen - right.decimalPoint;

    left.digits.insert(0, leftPad, '0');
    left.decimalPoint += leftPad;

    right.digits.insert(0, rightPad, '0');
    right.decimalPoint += rightPad;

    int totalDigits = left.digits.size();
    res.digits.resize(totalDigits, '0');

    int borrow = 0;

    for (int i = totalDigits - 1; i >= 0; --i)
    {
        int leftDigit = left.digits[i] - '0' - borrow;
        borrow = 0;

        int rightDigit = right.digits[i] - '0';

        if (leftDigit < rightDigit)
        {
            leftDigit += 10;
            borrow = 1;
        }

        res.digits[i] = (leftDigit - rightDigit) + '0';
    }

    res.decimalPoint = left.decimalPoint;

    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::multiplyAbsValue(const BigNumber &other) const
{
    BigNumber res("0.0");

    uint8_t carry = 0;
    uint8_t temp = 0;

    // check if any of the multipliers is zero
    if (*this == res || other == res)
    {
        return res;
    }

    for (int i = digits.size() - 1; i >= 0; i--)
    {

        int j = other.digits.size() - 1;

        BigNumber intermediate;

        while (j >= 0 || carry)
        {
            temp = carry;

            if (j >= 0)
                temp += (digits[i] - '0') * (other.digits[j] - '0');

            carry = temp / 10;

            intermediate.digits += (temp % 10 + '0');
            j--;
        }
        reverse(intermediate.digits.begin(), intermediate.digits.end());

        int rightOffset = digits.size() - 1 - i;

        intermediate.digits.append(rightOffset + 1, '0');

        intermediate.decimalPoint = intermediate.digits.size() - 1;

        res = res + intermediate;
    }

    int digitsAfterPoint1 = digits.size() - decimalPoint;
    int digitsAfterPoint2 = other.digits.size() - other.decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 + digitsAfterPoint2;

    int diff = res.decimalPoint - resDigitsAfterPoint - 1;

    if (diff < 0)
    {
        res.digits.insert(0, -diff, '0');
        res.decimalPoint = res.digits.size() - 1;
    }

    res.decimalPoint -= resDigitsAfterPoint;

    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::divideAbsValue(const BigNumber &other) const
{

    BigNumber otherNormalized = other;

    otherNormalized.decimalPoint = otherNormalized.digits.size();
    otherNormalized.removeLeadingZeroes();
    otherNormalized.digits += "0";
    otherNormalized.isNegative = false;

    BigNumber res;

    BigNumber remainder("0");

    BigNumber zero("0"), one("1"), ten("10");

    bool resReachedDecPart = false;

    int digitsAfterPoint1 = digits.size() - decimalPoint;
    int digitsAfterPoint2 = other.digits.size() - other.decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 - digitsAfterPoint2;

    int maxDivDigits = max(BigNumber::precision - resDigitsAfterPoint, 0);

    if (other == zero)
    {
        throw invalid_argument("The divisor should not be zero");
    }
    if (*this == zero)
    {
        return zero;
    }

    int takenZeroes = 0, i = 0;

    while (takenZeroes <= maxDivDigits)
    {

        // Choose enough digits to divide
        do
        {

            if (i < this->digits.size())
            {
                // Add next digit of `this` to the remainder
                BigNumber nextDigit(string(1, this->digits[i]));

                remainder = remainder * ten + nextDigit;
                i++;
            }
            else
            {
                if (resReachedDecPart == false)
                {
                    res.decimalPoint = res.digits.size();
                    resReachedDecPart = true;
                }
                remainder = remainder * ten;
                takenZeroes++;
            }

            if (remainder < otherNormalized)
            {
                res.digits += "0";
            }
        } while (remainder < otherNormalized && takenZeroes <= maxDivDigits);

        BigNumber multiplier("1");

        // Choose the multiplier
        while (multiplier * otherNormalized <= remainder)
        {
            multiplier = multiplier + one;
        }

        multiplier = multiplier - one;

        remainder = remainder - otherNormalized * multiplier;

        res.digits += multiplier.digits[0];
    }

    res.decimalPoint -= resDigitsAfterPoint;

    res = res.truncate(precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

// checks if the number is int (has no fractional part)
bool BigNumber::isInt() const
{
    if (this->decimalPoint != this->digits.size() - 1)

        return false;

    string frac = this->digits.substr(this->decimalPoint);

    if (frac == "0")
        return true;

    return false;
}

BigNumber BigNumber::abs() const
{

    BigNumber res = *this;
    res.isNegative = false;

    return res;
}

BigNumber BigNumber::operator+(const BigNumber &other) const
{

    BigNumber res;

    if (isNegative == other.isNegative)
    {
        res = addAbsValue(other);
        res.isNegative = isNegative;
    }
    else if (compareAbsValue(other))
    {
        res = other.subtractAbsValue(*this);
        res.isNegative = other.isNegative;
    }
    else
    {
        res = subtractAbsValue(other);
        res.isNegative = isNegative;
    }

    return res;
}

BigNumber BigNumber::operator-(const BigNumber &other) const
{

    BigNumber res;
    if (isNegative != other.isNegative)
    {
        res = addAbsValue(other);
        res.isNegative = isNegative;
    }
    else if (compareAbsValue(other))
    {
        res = other.subtractAbsValue(*this);
        res.isNegative = !isNegative;
    }
    else
    {
        res = subtractAbsValue(other);
        res.isNegative = isNegative;
    }

    return res;
}

BigNumber BigNumber::operator*(const BigNumber &other) const
{

    BigNumber res;

    if (isNegative == other.isNegative)
    {
        res = multiplyAbsValue(other);
        res.isNegative = false;
    }
    else
    {
        res = multiplyAbsValue(other);
        res.isNegative = true;
    }

    return res;
}

BigNumber BigNumber::operator/(const BigNumber &other) const
{

    BigNumber res;

    if (isNegative == other.isNegative)
    {
        res = divideAbsValue(other);
        res.isNegative = false;
    }
    else
    {
        res = divideAbsValue(other);
        res.isNegative = true;
    }

    return res;
}

BigNumber BigNumber::operator%(const BigNumber &other) const
{
    if (!isInt() || !other.isInt())
    {
        throw invalid_argument("Modulus requires integer operands");
    }
    if (other == BigNumber("0"))
    {
        throw invalid_argument("Modulus by zero");
    }

    BigNumber quotient = *this / other;
    BigNumber truncated = quotient.truncate(0);
    BigNumber product = truncated * other;
    BigNumber mod = *this - product;

    return mod;
}

bool BigNumber::operator<(const BigNumber &other) const
{

    if (isNegative && !other.isNegative)
        return true;
    else if (isNegative && other.isNegative)
        return compareAbsValue(other);
    else if (!isNegative && other.isNegative)
        return false;
    else
        return compareAbsValue(other);
}

bool BigNumber::operator<=(const BigNumber &other) const
{
    return !(*this > other);
}

bool BigNumber::operator>=(const BigNumber &other) const
{
    return !(operator<(other));
}

bool BigNumber::operator>(const BigNumber &other) const
{

    return !(*this < other) && !(*this == other);
}

bool BigNumber::operator!=(const BigNumber &other) const
{
    return !(*this == other);
}

bool BigNumber::operator==(const BigNumber &other) const
{
    if (digits.size() != other.digits.size())
        return false;

    if (decimalPoint != other.decimalPoint)
        return false;

    if (isNegative != other.isNegative)
        return false;

    for (int i = 0; i < digits.size(); i++)
    {
        if (digits[i] != other.digits[i])
            return false;
    }

    return true;
}

BigNumber BigNumber::operator=(const string &str)
{
    BigNumber res(str);

    *this = res;

    return *this;
}

BigNumber BigNumber::operator+=(const BigNumber &other)
{

    *this = *this + other;

    return *this;
}

BigNumber BigNumber::operator-=(const BigNumber &other)
{

    *this = *this - other;

    return *this;
}

BigNumber BigNumber::operator*=(const BigNumber &other)
{

    *this = *this * other;

    return *this;
}

BigNumber BigNumber::operator/=(const BigNumber &other)
{

    *this = *this / other;

    return *this;
}

BigNumber BigNumber::operator%=(const BigNumber &other)
{

    *this = *this % other;

    return *this;
}

// Static methods
// ----------------------------------------------------

// Calculate sine; n is in radians
BigNumber BigNumber::sin(BigNumber n)
{
    // Taylor series expansion at a =0

    BigNumber period = two * PI;

    while (n > period) // Reducing n by period (2*PI)
    {
        n -= period;
    }

    BigNumber x_i = n;

    BigNumber res = x_i;

    for (int i = 1; i < iterations; i++)
    {

        BigNumber converted_i(to_string(i));

        x_i *= (-n * n) / ((two * converted_i) * (two * converted_i + one)); // Getting next term from previous

        res += x_i;
    }

    return res;
}

// Calculate √x using newton's method
BigNumber BigNumber::sqroot(BigNumber n)
{

    if (n < zero)
    {
        throw std::invalid_argument("The argument should be non-negative");
    }

    // f(x) = x^2-n
    // f'(x) = 2x

    BigNumber x = n / two; // Initial guess

    for (int i = 0; i < BigNumber::iterations; i++)
    {
        BigNumber f_x = x * x - n;
        BigNumber fprime_x = two * x;

        x = x - f_x / fprime_x; // x_(n+1) = x_n - f(x_n)/f'(x_n)
    }

    return x;
}

// Sets the required precision for the `toString` method
// Also modifies the precision of the division operation
void BigNumber::setPrecision(int precision)
{
    if (precision < 1)
        throw invalid_argument("Precision should not be less than 1");
    BigNumber::precision = precision;
}

int BigNumber::getPrecision()
{
    return BigNumber::precision;
}

BigNumber BigNumber::factorial(int num)
{

    BigNumber res("1");

    if (num > 10000 || num < 0)
    {
        throw logic_error("Input is out of range");
    }

    for (int i = 1; i <= num; i++)
    {

        BigNumber multiplier(to_string(i));

        res = res * multiplier;
    }

    return res;
}

// ----------------------------------------------------

// Truncates the number to the required precision
// But does not add additinal zeroes
BigNumber BigNumber::truncate(int precision) const
{

    if (precision < 0)
        throw invalid_argument("Precision cannot be less than zero");

    BigNumber res = *this;

    int digitsAfterPoint = this->digits.size() - this->decimalPoint;

    if (digitsAfterPoint <= precision)
        return res;

    int diff = digitsAfterPoint - precision;

    res.digits = res.digits.substr(0, res.digits.size() - diff);

    if (precision == 0)
    {
        res.digits += "0";
    }

    return res;
}

BigNumber BigNumber::operator-() const
{

    BigNumber res = *this;

    res.isNegative = !this->isNegative;

    return res;
}

BigNumber::BigNumber(const string &str)
{

    if (!isValidInput(str))
    {
        throw logic_error("Invalid number format");
    }

    if (str[0] == '-')
    {
        isNegative = true;
        digits = str.substr(1);
    }
    else
    {
        digits = str;
        isNegative = false;
    }

    int decimal = str.find(".");

    if (decimal == str.npos)
        digits += ".0";

    decimalPoint = digits.find(".");
    digits.erase(decimalPoint, 1);

    removeLeadingZeroes();
    removeTrailingZeroes();

    if (digits == "-0.0")
    {
        digits = "0.0";
        isNegative = false;
    }
}

string BigNumber::toString()
{

    string res = this->truncate(BigNumber::precision).digits;

    res.insert(decimalPoint, ".");
    if (isNegative)
    {
        res.insert(0, "-");
    }

    return res;
}