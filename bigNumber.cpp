
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
const BigNumber BigNumber::_zero("0");
const BigNumber BigNumber::_one("1");
const BigNumber BigNumber::_two("2");
int BigNumber::_precision = 120;
int BigNumber::_printPrecision = 99;
// ----------------------------------

#pragma region Helper

BigNumber::BigNumber()
{
}

void BigNumber::removeTrailingZeroes()
{
    int zeroesEndIndex = _digits.find_last_not_of('0', _digits.size() - 1);

    if (zeroesEndIndex < _decimalPoint || zeroesEndIndex == string::npos)
    {
        zeroesEndIndex = _decimalPoint;
    }
    _digits.erase(zeroesEndIndex + 1);
}

void BigNumber::removeLeadingZeroes()
{
    int zeroesEndIndex = _digits.find_first_not_of('0', 0);

    if (zeroesEndIndex > _decimalPoint - 1 || zeroesEndIndex == string::npos)
    {
        zeroesEndIndex = _decimalPoint - 1;
    }

    _digits.erase(0, zeroesEndIndex);
    _decimalPoint -= zeroesEndIndex;
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

    int leftFracLen = left._digits.size() - left._decimalPoint;
    int rightFracLen = right._digits.size() - right._decimalPoint;
    int diff = rightFracLen - leftFracLen;

    // Check which num has more digits after decimal point
    if (diff > 0)
        left._digits.append(diff, '0'); // Append to left
    else if (diff < 0)
        right._digits.append(-diff, '0'); // Append to right
}

// returns |this|<|other|
bool BigNumber::compareAbsValue(const BigNumber &other) const
{
    if (_decimalPoint > other._decimalPoint)
    {
        return false;
    }
    else if (_decimalPoint < other._decimalPoint)
    {
        return true;
    }

    BigNumber left = this->truncate(_precision);
    BigNumber right = other.truncate(_precision);

    int maxSize = max(left._digits.size(), right._digits.size());

    for (int i = 0; i < maxSize; i++)
    {

        if (i >= left._digits.size())
            return true;
        if (i >= right._digits.size())
            return false;

        if (left._digits[i] < right._digits[i])
            return true;

        if (left._digits[i] > right._digits[i])
            return false;
    }

    return false;
}

BigNumber BigNumber::addAbsValue(const BigNumber &other) const
{
    BigNumber res;
    res._isNegative = false;

    BigNumber left = *this;
    BigNumber right = other;

    allignDecimalPlaces(left, right);

    uint8_t carry = 0;
    uint8_t temp = 0;

    int i = left._digits.size() - 1;
    int j = right._digits.size() - 1;

    while (i >= 0 || j >= 0 || carry)
    {
        temp = carry;

        if (i >= 0)
            temp += left._digits[i--] - '0';
        if (j >= 0)
            temp += right._digits[j--] - '0';

        carry = temp / 10;

        res._digits += temp % 10 + '0';
    }

    reverse(res._digits.begin(), res._digits.end());

    res._decimalPoint = res._digits.size() - (left._digits.size() - left._decimalPoint);

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

// requires |this| >= |other|
BigNumber BigNumber::subtractAbsValue(const BigNumber &other) const
{
    assert(!compareAbsValue(other));

    BigNumber res;
    res._isNegative = false;

    BigNumber left = *this;
    BigNumber right = other;

    allignDecimalPlaces(left, right);

    int maxIntLen = max(left._decimalPoint, right._decimalPoint);
    int leftPad = maxIntLen - left._decimalPoint;
    int rightPad = maxIntLen - right._decimalPoint;

    left._digits.insert(0, leftPad, '0');
    left._decimalPoint += leftPad;

    right._digits.insert(0, rightPad, '0');
    right._decimalPoint += rightPad;

    int totalDigits = left._digits.size();
    res._digits.resize(totalDigits, '0');

    int borrow = 0;

    for (int i = totalDigits - 1; i >= 0; --i)
    {
        int leftDigit = left._digits[i] - '0' - borrow;
        borrow = 0;

        int rightDigit = right._digits[i] - '0';

        if (leftDigit < rightDigit)
        {
            leftDigit += 10;
            borrow = 1;
        }

        res._digits[i] = (leftDigit - rightDigit) + '0';
    }

    res._decimalPoint = left._decimalPoint;

    res = res.truncate(_precision);
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

    for (int i = _digits.size() - 1; i >= 0; i--)
    {

        int j = other._digits.size() - 1;

        BigNumber intermediate;

        while (j >= 0 || carry)
        {
            temp = carry;

            if (j >= 0)
                temp += (_digits[i] - '0') * (other._digits[j] - '0');

            carry = temp / 10;

            intermediate._digits += (temp % 10 + '0');
            j--;
        }
        reverse(intermediate._digits.begin(), intermediate._digits.end());

        int rightOffset = _digits.size() - 1 - i;

        intermediate._digits.append(rightOffset + 1, '0');

        intermediate._decimalPoint = intermediate._digits.size() - 1;

        res = res + intermediate;
    }

    int digitsAfterPoint1 = _digits.size() - _decimalPoint;
    int digitsAfterPoint2 = other._digits.size() - other._decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 + digitsAfterPoint2;

    int diff = res._decimalPoint - resDigitsAfterPoint - 1;

    if (diff < 0)
    {
        res._digits.insert(0, -diff, '0');
        res._decimalPoint = res._digits.size() - 1;
    }

    res._decimalPoint -= resDigitsAfterPoint;

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::divideAbsValue(const BigNumber &other) const
{

    BigNumber otherNormalized = other;

    otherNormalized._decimalPoint = otherNormalized._digits.size();
    otherNormalized.removeLeadingZeroes();
    otherNormalized._digits += "0";
    otherNormalized._isNegative = false;

    BigNumber res;

    BigNumber remainder("0");

    BigNumber zero("0"), one("1"), ten("10");

    bool resReachedDecPart = false;

    int digitsAfterPoint1 = _digits.size() - _decimalPoint;
    int digitsAfterPoint2 = other._digits.size() - other._decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 - digitsAfterPoint2;

    int maxDivDigits = max(BigNumber::_precision - resDigitsAfterPoint, 0);

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

            if (i < this->_digits.size())
            {
                // Add next digit of `this` to the remainder
                BigNumber nextDigit = this->_digits[i] - '0';

                remainder = remainder * ten + nextDigit;
                i++;
            }
            else
            {
                if (resReachedDecPart == false)
                {
                    res._decimalPoint = res._digits.size();
                    resReachedDecPart = true;
                }
                remainder = remainder * ten;
                takenZeroes++;
            }

            if (remainder < otherNormalized)
            {
                res._digits += "0";
            }
        } while (remainder < otherNormalized && takenZeroes <= maxDivDigits);

        BigNumber multiplier = 1;

        // Choose the multiplier
        while (multiplier * otherNormalized <= remainder)
        {
            multiplier += 1;
        }

        multiplier = multiplier - one;

        remainder = remainder - otherNormalized * multiplier;

        res._digits += multiplier._digits[0];
    }

    res._decimalPoint -= resDigitsAfterPoint;

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

#pragma endregion

#pragma region isInt, abs, truncate, round
// checks if the number is int (has no fractional part)
bool BigNumber::isInt() const
{
    if (this->_decimalPoint != this->_digits.size() - 1)

        return false;

    string frac = this->_digits.substr(this->_decimalPoint);

    if (frac == "0")
        return true;

    return false;
}

BigNumber BigNumber::abs() const
{

    BigNumber res = *this;
    res._isNegative = false;

    return res;
}

// Truncates the number to the required precision
// But does not add additinal zeroes
BigNumber BigNumber::truncate(int precision) const
{

    if (precision < 0)
        throw invalid_argument("Precision cannot be less than zero");

    BigNumber res = *this;

    int digitsAfterPoint = this->_digits.size() - this->_decimalPoint;

    if (digitsAfterPoint <= precision)
        return res;

    int diff = digitsAfterPoint - precision;

    res._digits = res._digits.substr(0, res._digits.size() - diff);

    if (precision == 0)
    {
        res._digits += "0";
    }
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::round(int precision) const
{
    if (precision < 0)
        throw invalid_argument("Precision cannot be less than zero");

    BigNumber res = *this;

    int digitsAfterPoint = this->_digits.size() - this->_decimalPoint;

    if (digitsAfterPoint <= precision)
        return res;

    int diff = digitsAfterPoint - precision;
    res._digits = res._digits.substr(0, res._digits.size() - diff);

    char lastDigit = _digits[_digits.size() - diff];

    // Check if the last digit is 5 or greater
    if (lastDigit >= '5')
    {
        // Make a temporary number with 0.0...0001 and add it to the number
        BigNumber toAdd;
        toAdd._digits.insert(0, precision, '0');
        toAdd._digits += "1";
        toAdd._decimalPoint = 1;

        toAdd._digits += "0";
        res += toAdd;
    }

    if (res._decimalPoint == res._digits.size())
    {
        res._digits += "0";
    }

    res.removeTrailingZeroes();

    return res;
}

bool BigNumber::isNegative() const
{
    return _isNegative;
}

#pragma endregion

#pragma region Operators

BigNumber BigNumber::operator+(const BigNumber &other) const
{

    BigNumber res;

    if (_isNegative == other._isNegative)
    {
        res = addAbsValue(other);
        res._isNegative = _isNegative;
    }
    else if (compareAbsValue(other))
    {
        res = other.subtractAbsValue(*this);
        res._isNegative = other._isNegative;
    }
    else
    {
        res = subtractAbsValue(other);
        res._isNegative = _isNegative;
    }

    return res;
}

BigNumber BigNumber::operator-(const BigNumber &other) const
{

    BigNumber res;
    if (_isNegative != other._isNegative)
    {
        res = addAbsValue(other);
        res._isNegative = _isNegative;
    }
    else if (compareAbsValue(other))
    {
        res = other.subtractAbsValue(*this);
        res._isNegative = !_isNegative;
    }
    else
    {
        res = subtractAbsValue(other);
        res._isNegative = _isNegative;
    }

    return res;
}

BigNumber BigNumber::operator*(const BigNumber &other) const
{

    BigNumber res;

    if (_isNegative == other._isNegative)
    {
        res = multiplyAbsValue(other);
        res._isNegative = false;
    }
    else
    {
        res = multiplyAbsValue(other);
        res._isNegative = true;
    }

    return res;
}

BigNumber BigNumber::operator/(const BigNumber &other) const
{

    BigNumber res;

    if (_isNegative == other._isNegative)
    {
        res = divideAbsValue(other);
        res._isNegative = false;
    }
    else
    {
        res = divideAbsValue(other);
        res._isNegative = true;
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

    if (_isNegative && !other._isNegative)
        return true;
    else if (_isNegative && other._isNegative)
        return compareAbsValue(other);
    else if (!_isNegative && other._isNegative)
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
    if (_digits.size() != other._digits.size())
        return false;

    if (_decimalPoint != other._decimalPoint)
        return false;

    if (_isNegative != other._isNegative)
        return false;

    for (int i = 0; i < _digits.size(); i++)
    {
        if (_digits[i] != other._digits[i])
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

    res._isNegative = !this->_isNegative;

    return res;
}

#pragma endregion

#pragma region Static methods

// Modifies precision to which all arithemetic operations truncate
// their results
// Also sets the printPrecision which is less than actual
// operations precision
void BigNumber::setPrecision(int precision)
{
    if (precision < 1)
        throw invalid_argument("Precision should not be less than 1");
    BigNumber::_precision = precision + 20;
    BigNumber::_printPrecision = precision;
}

int BigNumber::getPrecision()
{
    return BigNumber::_printPrecision;
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
        _isNegative = true;
        _digits = str.substr(1);
    }
    else
    {
        _digits = str;
        _isNegative = false;
    }

    int decimal = str.find(".");

    if (decimal == str.npos)
        _digits += ".0";

    _decimalPoint = _digits.find(".");
    _digits.erase(_decimalPoint, 1);

    *this = this->truncate(_precision);
    removeLeadingZeroes();
    removeTrailingZeroes();

    if (_digits == "-0.0")
    {
        _digits = "0.0";
        _isNegative = false;
    }
}

BigNumber::BigNumber(double num)
{

    string temp = to_string(num);
    int decLength = temp.find('.');
    if (decLength == string::npos)
        decLength == temp.size();

    std::stringstream ss;

    int requiredPres = __DBL_DIG__ - decLength;

    requiredPres = requiredPres < 0 ? 0 : requiredPres;

    ss.precision(requiredPres);
    ss << std::fixed << num;

    *this = BigNumber(ss.str());
}

string BigNumber::toString()
{

    BigNumber temp = this->round(BigNumber::_printPrecision);
    temp.removeTrailingZeroes();

    string res = temp._digits;

    res.insert(_decimalPoint, ".");
    if (_isNegative)
    {
        res.insert(0, "-");
    }

    return res;
}