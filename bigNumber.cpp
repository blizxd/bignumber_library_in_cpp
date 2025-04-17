
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

// ----------------------------------
// Static initialization
// ----------------------------------
const BigNumber BigNumber::_zero("0");
const BigNumber BigNumber::_one("1");
const BigNumber BigNumber::_two("2");
const BigNumber BigNumber::_ten("10");

int BigNumber::_precision = 110;
int BigNumber::_printPrecision = 99;

// ----------------------------------

#pragma region Algorithms
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

    if (left._digits.size() < 19 && right._digits.size() < 19)
    {
        res = left.directAdd(right);
        res._digits.insert(0, 1, '0');

        // remove the zero after the decimal point
        res._digits.erase(res._digits.end() - 1);
    }
    else
    {
        res._digits.reserve(std::max(left._digits.size(), right._digits.size()) + 3);

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
    }
    res._decimalPoint = res._digits.size() - (left._digits.size() - left._decimalPoint);

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

// requires |this| >= |other|
BigNumber BigNumber::subtractAbsValue(const BigNumber &other) const
{
    assert(!this->compareAbsValue(other));

#pragma region old code
    // BigNumber res;
    // res._isNegative = false;

    // BigNumber left = *this;
    // BigNumber right = other;

    // allignDecimalPlaces(left, right);

    // int maxIntLen = max(left._decimalPoint, right._decimalPoint);
    // int leftPad = maxIntLen - left._decimalPoint;
    // int rightPad = maxIntLen - right._decimalPoint;

    // left._digits.insert(0, leftPad, '0');
    // left._decimalPoint += leftPad;

    // right._digits.insert(0, rightPad, '0');
    // right._decimalPoint += rightPad;

    // int totalDigits = left._digits.size();
    // res._digits.resize(totalDigits, '0');

    // if (left._digits.size() < 19 && right._digits.size() < 19)
    // {
    //     res = left.directSubtract(right);
    //     res._digits.erase(res._digits.end() - 1);

    //     int zeroesToAdd = totalDigits - res._digits.size();

    //     res._digits.insert(0, zeroesToAdd, '0');
    // }
    // else
    // {
    //     int borrow = 0;

    //     for (int i = totalDigits - 1; i >= 0; --i)
    //     {
    //         int leftDigit = left._digits[i] - '0' - borrow;
    //         borrow = 0;

    //         int rightDigit = right._digits[i] - '0';

    //         if (leftDigit < rightDigit)
    //         {
    //             leftDigit += 10;
    //             borrow = 1;
    //         }

    //         res._digits[i] = (leftDigit - rightDigit) + '0';
    //     }
    // }
    // res._decimalPoint = left._decimalPoint;

#pragma endregion

    BigNumber res;
    res._isNegative = false;

    BigNumber left = *this;
    BigNumber right = other;

    allignDecimalPlaces(left, right);

    if (left._digits.size() < 19 && right._digits.size() < 19)
    {
        res = left.directSubtract(right);
        res._digits.insert(0, 1, '0');

        // remove the zero after the decimal point
        res._digits.erase(res._digits.end() - 1);
    }
    else
    {
        res._digits.reserve(std::max(left._digits.size(), right._digits.size()) + 3);

        uint8_t borrow = 0;
        uint8_t temp = 0;
        uint8_t toSubtract = 0;

        int i = left._digits.size() - 1;
        int j = right._digits.size() - 1;

        while (i >= 0 || j >= 0)
        {
            temp = 0;

            borrow /= 10;
            toSubtract = borrow;

            temp += left._digits[i--] - '0';
            if (j >= 0)
                toSubtract += right._digits[j--] - '0';

            if (toSubtract > temp)
            {
                temp += 10;
                borrow = 10;
            }
            temp -= toSubtract;

            res._digits += temp % 10 + '0';
        }

        reverse(res._digits.begin(), res._digits.end());
    }
    int shiftDecimalPoint = (left._digits.size() - left._decimalPoint);

    if (shiftDecimalPoint >= res._digits.size())
    {
        int offset = shiftDecimalPoint - res._digits.size();
        res._digits.insert(0, offset + 1, '0');
    }

    res._decimalPoint = res._digits.size() - shiftDecimalPoint;

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::multiplyAbsValue(const BigNumber &other) const
{
    BigNumber res = BigNumber::_zero;

    res._digits.reserve(this->_digits.size() + other._digits.size() + 5);

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

    bool resReachedDecPart = false;

    int digitsAfterPoint1 = _digits.size() - _decimalPoint;
    int digitsAfterPoint2 = other._digits.size() - other._decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 - digitsAfterPoint2;

    int maxDivDigits = max(BigNumber::_precision - resDigitsAfterPoint, 0);

    if (other == BigNumber::_zero)
    {
        throw invalid_argument("The divisor should not be zero");
    }
    if (*this == BigNumber::_zero)
    {
        return BigNumber::_zero;
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

                remainder = remainder * BigNumber::_ten + nextDigit;
                i++;
            }
            else
            {
                if (resReachedDecPart == false)
                {
                    res._decimalPoint = res._digits.size();
                    resReachedDecPart = true;
                }
                remainder = remainder * BigNumber::_ten;
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

        multiplier = multiplier - BigNumber::_one;

        remainder = remainder - otherNormalized * multiplier;

        res._digits += multiplier._digits[0];
    }

    res._decimalPoint -= resDigitsAfterPoint;

    res = res.truncate(_precision);
    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
}

BigNumber BigNumber::directAdd(const BigNumber &other) const
{
    // assert(this->_digits.size() < 19);
    // assert(other._digits.size() < 19);

    uint64_t left = std::stoll(this->_digits);
    uint64_t right = std::stoll(other._digits);

    uint64_t result = left + right;

    return std::to_string(result);
}

BigNumber BigNumber::directSubtract(const BigNumber &other) const
{

    // assert(this->_digits.size() < 19);
    // assert(other._digits.size() < 19);
    // assert(!this->compareAbsValue(other));

    uint64_t left = std::stoll(this->_digits);
    uint64_t right = std::stoll(other._digits);

    uint64_t result = left - right;

    return std::to_string(result);
}

BigNumber BigNumber::directMultiply(const BigNumber &other) const
{
    // assert(this->_digits.size() + other._digits.size() < 21);

    uint64_t left = std::stoll(this->_digits) / 10;
    uint64_t right = std::stoll(other._digits) / 10;

    uint64_t result = left * right;

    return std::to_string(result);
}

BigNumber BigNumber::fastMod(const BigNumber &p) const
{
    BigNumber a = *this;

    assert(a >= 0 && p > 0);

    while (a >= p)
    {
        a -= p;
    }
    return a;
}

#pragma endregion

#pragma region multiplication

// splitNumber function specifies the number of digits
// to extract from the right
std::pair<BigNumber, BigNumber> BigNumber::splitNumber(int pos) const
{
    std::pair<BigNumber, BigNumber> result;

    int length = this->_digits.length() - 1;

    result.first = this->_digits.substr(0, length - pos);
    result.second = this->_digits.substr(length - pos, pos);

    return result;
}

BigNumber BigNumber::shiftIntegerLeft(int decPlaces) const
{

    BigNumber result = *this;

    result._digits.append(decPlaces, '0');

    result._decimalPoint += decPlaces;

    return result;
}

int BigNumber::makeEqualLength(BigNumber &n1, BigNumber &n2)
{
    int diff = n1._digits.length() - n2._digits.length();
    if (diff > 0)
    {
        n2._digits.insert(0, diff, '0'); // prepend to left
        n2._decimalPoint += diff;
    }
    else if (diff < 0)
    {
        n1._digits.insert(0, -diff, '0'); // prepend to right
        n1._decimalPoint += -diff;
    }
    return n1._digits.length();
}

// find product of two integers using karatsuba algorithm
// https://en.wikipedia.org/wiki/Karatsuba_algorithm
// not recursive(currently)
BigNumber BigNumber::karatsubaMult(const BigNumber &other) const
{
    // assert(this->isInt());
    // assert(other.isInt());

    // base case
    if (this->_digits.size() + other._digits.size() < 21)
        return this->directMultiply(other);

    BigNumber left = *this;
    BigNumber right = other;

    int n = makeEqualLength(left, right);

    // Add zeros in the beginning of
    // the strings when length of integer part
    // is odd
    if (n % 2 == 0)
    {
        left._digits.insert(0, 1, '0');
        right._digits.insert(0, 1, '0');
        n++;
    }

    int n2 = n / 2;

    auto leftSplit = left.splitNumber(n2);
    auto rightSplit = right.splitNumber(n2);

    BigNumber z0, z1, z2;

    z0 = leftSplit.second.karatsubaMult(rightSplit.second);
    z1 = (leftSplit.first + leftSplit.second);
    z1 = z1.karatsubaMult(rightSplit.first + rightSplit.second);
    z2 = leftSplit.first.karatsubaMult(rightSplit.first);

    // z0 = leftSplit.second.multiplyAbsValue(rightSplit.second);
    // z1 = (leftSplit.first + leftSplit.second);
    // z1 = z1.multiplyAbsValue(rightSplit.first + rightSplit.second);
    // z2 = leftSplit.first.multiplyAbsValue(rightSplit.first);

    BigNumber p = z2.shiftIntegerLeft(n2 * 2);
    BigNumber q = z1 - z2 - z0;
    q = q.shiftIntegerLeft(n2);

    return p + q + z0;
}

BigNumber BigNumber::optimizedMultAbsValue(const BigNumber &other) const
{
    BigNumber leftNormalized{this->_digits};
    BigNumber rightNormalized{other._digits};

    int sizeDifference = std::abs((int64_t)this->_digits.size() - (int64_t)other._digits.size());

    BigNumber result;
    if (sizeDifference > 80)
        return this->multiplyAbsValue(other);
    else
        result = leftNormalized.karatsubaMult(rightNormalized);

    // Decimal places handling logic
    int digitsAfterPoint1 = _digits.size() - _decimalPoint;
    int digitsAfterPoint2 = other._digits.size() - other._decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 + digitsAfterPoint2;

    int diff = result._decimalPoint - resDigitsAfterPoint;

    if (diff <= 0)
    {
        result._digits.insert(0, -diff + 1, '0');
        result._decimalPoint = result._digits.size() - 1;
    }

    result._decimalPoint -= resDigitsAfterPoint;

    result = result.truncate(_precision);
    result.removeLeadingZeroes();
    result.removeTrailingZeroes();

    return result;
}

BigNumber BigNumber::multiplyHandler(const BigNumber &other) const
{
    return optimizedMultAbsValue(other);
    // return multiplyAbsValue(other);
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
    // negative number might get truncated to -0.0
    res._isNegative = res < 0;
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
        res = res.abs() + toAdd;
        res._isNegative = this->_isNegative;
    }

    if (res._decimalPoint == res._digits.size())
    {
        res._digits += "0";
    }

    // a negative number might get rounded to -0.0
    res._isNegative = res < 0;

    res.removeTrailingZeroes();

    return res;
}

bool BigNumber::isOdd() const
{
    if (!isInt())
        return false;

    uint8_t lastDigit = this->_digits[this->_digits.size() - 2] - '0';

    return lastDigit % 2 == 1;
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
        res = multiplyHandler(other);
        res._isNegative = false;
    }
    else
    {
        res = multiplyHandler(other);
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
        throw invalid_argument("Modulus requires integer operands");

    BigNumber div = other.abs();

    if (other == 0)
        throw invalid_argument("Modulus by zero");

    if (*this < 0)
        return -(-(*this) % div);

    // If size difference is small, do modulus by repeated subtraction
    if (std::abs((int64_t)this->_digits.size() - (int64_t)other._digits.size()) < 8)
        return fastMod(div);

    // Else do by division
    BigNumber res = *this;
    BigNumber q = (res / other).truncate(0);

    res = res - q * other;

    return res;
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
    BigNumber::_precision = precision + 6;
    BigNumber::_printPrecision = precision;
}

int BigNumber::getPrecision()
{
    return BigNumber::_printPrecision;
}

bool BigNumber::isValidInput(const string &str)
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

void BigNumber::allignDecimalPlaces(BigNumber &left, BigNumber &right)
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

#pragma endregion

// ----------------------------------------------------

#pragma region extra

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

    if (_digits == "00")
        _isNegative = false;
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

    res.insert(temp._decimalPoint, ".");
    if (_isNegative)
    {
        res.insert(0, "-");
    }

    return res;
}

#pragma endregion