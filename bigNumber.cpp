
#include "bigNumber.h"
#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Static initialization
// ----------------------------------
BigNumber BigNumber::zero("0");
BigNumber BigNumber::one("1");
BigNumber BigNumber::two("2");
BigNumber BigNumber::PI("3."
                        "14159265358979323846264338327950288419716939937510582097494459230781640628"
                        "62089986280348253421170679821480865132823066470938446095505");
int BigNumber::precision = 120;
int BigNumber::printPrecision = 99;
// ----------------------------------

#pragma region Helper

BigNumber::BigNumber()
{
}

void BigNumber::removeTrailingZeroes()
{
    int zeroesEndIndex = digits.find_last_not_of('0', digits.size() - 1);

    if (zeroesEndIndex < decimalPoint || zeroesEndIndex == string::npos)
    {
        zeroesEndIndex = decimalPoint;
    }
    digits.erase(zeroesEndIndex + 1);
}

void BigNumber::removeLeadingZeroes()
{
    int zeroesEndIndex = digits.find_first_not_of('0', 0);

    if (zeroesEndIndex > decimalPoint - 1 || zeroesEndIndex == string::npos)
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

    BigNumber left = this->truncate(precision);
    BigNumber right = other.truncate(precision);

    int maxSize = max(left.digits.size(), right.digits.size());

    for (int i = 0; i < maxSize; i++)
    {

        if (i >= left.digits.size())
            return true;
        if (i >= right.digits.size())
            return false;

        if (left.digits[i] < right.digits[i])
            return true;

        if (left.digits[i] > right.digits[i])
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

    res = res.truncate(precision);
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

    res = res.truncate(precision);
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

    res = res.truncate(precision);
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

#pragma endregion

#pragma region isInt, abs, truncate, round
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
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::round(int precision) const
{
    if (precision < 0)
        throw invalid_argument("Precision cannot be less than zero");

    BigNumber res = *this;

    int digitsAfterPoint = this->digits.size() - this->decimalPoint;

    if (digitsAfterPoint <= precision)
        return res;

    int diff = digitsAfterPoint - precision;
    res.digits = res.digits.substr(0, res.digits.size() - diff);

    char lastDigit = digits[digits.size() - diff];

    // Check if the last digit is 5 or greater
    if (lastDigit >= '5')
    {
        // Make a temporary number with 0.0...0001 and add it to the number
        BigNumber toAdd;
        toAdd.digits.insert(0, precision, '0');
        toAdd.digits += "1";
        toAdd.decimalPoint = 1;

        toAdd.digits += "0";
        res += toAdd;
    }

    if (res.decimalPoint == res.digits.size())
    {
        res.digits += "0";
    }

    res.removeTrailingZeroes();

    return res;
}

#pragma endregion

#pragma region Operators

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

BigNumber &BigNumber::operator=(const string &str)
{
    BigNumber res(str);

    *this = res;

    return *this;
}

BigNumber &BigNumber::operator+=(const BigNumber &other)
{

    *this = *this + other;

    return *this;
}

BigNumber &BigNumber::operator-=(const BigNumber &other)
{

    *this = *this - other;

    return *this;
}

BigNumber &BigNumber::operator*=(const BigNumber &other)
{

    *this = *this * other;

    return *this;
}

BigNumber &BigNumber::operator/=(const BigNumber &other)
{

    *this = *this / other;

    return *this;
}

BigNumber &BigNumber::operator%=(const BigNumber &other)
{

    *this = *this % other;

    return *this;
}

BigNumber BigNumber::operator-() const
{

    BigNumber res = *this;

    res.isNegative = !this->isNegative;

    return res;
}

#pragma endregion

#pragma region Static methods

// ----------------------------------------------------
// Static methods
// ----------------------------------------------------

// Calculate sine; n is in radians
BigNumber BigNumber::sin(BigNumber n)
{
    // Taylor series expansion at a =0

    BigNumber period = two * PI;
    BigNumber PIOverTwo = PI / 2;

    while (n > period) // Reducing n by period (2*PI)
    {
        n -= period;
    }
    while (n > PIOverTwo)
    {
        n = PI - n;
    }

    BigNumber x_i = n;

    BigNumber res = x_i;

    for (int i = 1; i < iterations; i++)
    {
        x_i = x_i * intPow(n, 2) / ((2 * i) * (2 * i + 1)) * (-1);
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

BigNumber BigNumber::ln(BigNumber n)
{

#pragma region old
    // uint64_t steps = 10000;

    // if (n <= zero)
    // {
    //     throw std::invalid_argument("The argument should be positive");
    // }
    // if (n <= one)
    // {
    //     return -ln(one / n);
    // }

    // BigNumber res = zero, x_i = one;

    // BigNumber dx = (n - one) / BigNumber(steps);
    // for (int i = 0; i < steps; i++)
    // {

    //     x_i += dx;

    //     res += one / x_i * dx;
    // }

    // return res;

#pragma endregion

    if (n <= 0)
        throw invalid_argument("Argument should be positive");

    BigNumber ln2 = string("0."
                           "69314718055994530941723212145817656807550013436025525412068000949339362"
                           "19696947156058633269964186875");

    // use fact ln(2^p * g) = p * ln(2) + ln(g)
    // find p

    BigNumber p = 0;
    BigNumber g = n;
    while (g >= 2)
    {
        p += 1;
        g /= 2;
    }
    while (g < 1)
    {
        p -= 1;
        g *= 2;
    }

    // https://math.stackexchange.com/questions/4519431/taylor-series-convergence-for-logx/4519474#4519474
    BigNumber x_i;
    BigNumber res = 0;

    for (int i = 0; i < iterations; i++)
    {

        x_i = one / (2 * i + 1);
        x_i *= intPow((g - 1) / (g + 1), 2 * i + 1);
        res += x_i;
    }

    res *= 2;

    return p * ln2 + res;
}

BigNumber BigNumber::intPow(BigNumber base, int exponent)
{
    BigNumber res = one;
    for (int i = 0; i < exponent; i++)
    {
        res *= base;
    }
    return res;
}

bool BigNumber::isPrime(BigNumber n)
{
    if (n <= one || !n.isInt())
        return false;

    BigNumber upperBound = n;

    for (BigNumber i = two; BigNumber(i) < upperBound; i += one)
    {
        if (n % BigNumber(i) == zero)
            return false;
    }
    return true;
}

BigNumber BigNumber::nextPrime(BigNumber n)
{
    if (n < two)
        return two;

    BigNumber begin = n.truncate(0) + 1;

    while (!isPrime(begin))
    {
        begin += one;
    }
    return begin;
}

int BigNumber::find(BigNumber arr[], int size, BigNumber target)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == target)
            return i;
    }
    return -1;
}

void BigNumber::bubbleSort(BigNumber arr[], int size)
{

    bool swapped = false;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
        swapped = false;
    }
}

BigNumber BigNumber::getAverage(BigNumber arr[], int size)
{
    BigNumber sum = zero;
    for (int i = 0; i < size; i++)
        sum += arr[i];
    return sum / BigNumber(size);
}

BigNumber BigNumber::chudnovskyPI(int n)
{
#pragma region old

// BigNumber res = zero;

// for (int i = 0; i < n; i++)
// {
//     BigNumber term = zero;

//     term += BigNumber(13591409) + BigNumber(545140134) * BigNumber(i);
//     term *= factorial(6 * i);
//     if (i % 2 == 1)
//         term *= -one;

//     term /= factorial(3 * i) * intPow(factorial(i), 3) * intPow(BigNumber(640320), 3 * i);

//     res += term;
// }

// res *= one / (BigNumber(426880) * sqroot(BigNumber(10005)));

// res = one / res;

// return res;
#pragma endregion

    BigNumber a_k = one;
    BigNumber a_sum = one;
    BigNumber b_sum = zero;
    BigNumber C = 640320;

    BigNumber C3_OVER_24 = intPow(C, 3) / 24;

    for (int i = 1; i < n; i++)
    {
        a_k *= -(6 * i - 5) * (2 * i - 1) * (6 * i - 1);
        a_k /= intPow(i, 3) * C3_OVER_24;
        a_sum += a_k;
        b_sum += a_k * i;
    }

    BigNumber total = a_sum * 13591409 + b_sum * 545140134;
    BigNumber pi = sqroot(10005) * 426880 / total;

    return pi.round(n);
}

// Modifies precision to which all arithemetic operations truncate
// their results
// Also sets the printPrecision which is less than actual
// operations precision
void BigNumber::setPrecision(int precision)
{
    if (precision < 1)
        throw invalid_argument("Precision should not be less than 1");
    BigNumber::precision = precision + 20;
    BigNumber::printPrecision = precision;
}

int BigNumber::getPrecision()
{
    return BigNumber::printPrecision;
}

#pragma endregion

// ----------------------------------------------------

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

    *this = this->truncate(precision);
    removeLeadingZeroes();
    removeTrailingZeroes();

    if (digits == "-0.0")
    {
        digits = "0.0";
        isNegative = false;
    }
}

BigNumber::BigNumber(double num) : BigNumber(to_string(num))
{
}

string BigNumber::toString()
{

    BigNumber temp = this->round(BigNumber::printPrecision);
    temp.removeTrailingZeroes();

    string res = temp.digits;

    res.insert(decimalPoint, ".");
    if (isNegative)
    {
        res.insert(0, "-");
    }

    return res;
}