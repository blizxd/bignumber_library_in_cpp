#include <iostream>
#include <cstdint>
#include <array>
#include <string>
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <sstream>

#define TEST_OP(a, op, b, res) assert(BigNumber(#a) op BigNumber(#b) == BigNumber(#res))
#define TEST_METHOD(a, method, res) assert(BigNumber(#a).method == BigNumber(#res));

using namespace std;

// I decided to store numbers as just digits without any '-' or '.'

class BigNumber
{
private:
  string digits;
  uint16_t decimalPoint;
  bool isNegative = false;

  static inline int precision = 100;

  BigNumber() {}

  void removeTrailingZeroes()
  {
    int zeroesEndIndex = digits.find_last_not_of('0', digits.size() - 1);

    if (zeroesEndIndex < decimalPoint ||
        zeroesEndIndex == string::npos)
    {
      zeroesEndIndex = decimalPoint;
    }
    digits.erase(zeroesEndIndex + 1);
  }

  void removeLeadingZeroes()
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

  bool isValidInput(const string &str) const
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

  void allignDecimalPlaces(BigNumber &left, BigNumber &right) const
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
  bool compareAbsValue(const BigNumber &other) const
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

  BigNumber addAbsValue(const BigNumber &other) const
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
  BigNumber subtractAbsValue(const BigNumber &other) const
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

  BigNumber multiplyAbsValue(const BigNumber &other) const
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

  BigNumber divideAbsValue(const BigNumber &other) const
  {

    BigNumber otherNormalized = other;

    otherNormalized.decimalPoint = otherNormalized.digits.size();
    otherNormalized.removeLeadingZeroes();
    otherNormalized.digits += "0";

    BigNumber res;

    BigNumber remainder("0");

    BigNumber zero("0"), one("1"), ten("10");

    bool resReachedDecPart = false;

    int digitsAfterPoint1 = digits.size() - decimalPoint;
    int digitsAfterPoint2 = other.digits.size() - other.decimalPoint;

    int resDigitsAfterPoint = digitsAfterPoint1 - digitsAfterPoint2;

    int maxDivDigits = BigNumber::precision - resDigitsAfterPoint;

    if (other == zero)
    {
      throw invalid_argument("The divisor should not be zero");
    }
    if (*this == zero)
    {
      return zero;
    }

    int takenZeroes = 0, i = 0;

    while (takenZeroes < maxDivDigits)
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
      } while (remainder < otherNormalized && takenZeroes < maxDivDigits);

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

    res.removeLeadingZeroes();
    res.removeTrailingZeroes();

    return res;
  }

public:
  BigNumber abs() const
  {

    BigNumber res = *this;
    res.isNegative = false;

    return res;
  }

  BigNumber operator+(const BigNumber &other) const
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

  BigNumber operator-(const BigNumber &other) const
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

  BigNumber operator*(const BigNumber &other) const
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

  BigNumber operator/(const BigNumber &other) const
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

  bool operator<(const BigNumber &other) const
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

  bool operator<=(const BigNumber &other) const
  {
    return !(*this > other);
  }

  bool operator>=(const BigNumber &other) const
  {
    return !(operator<(other));
  }

  bool operator>(const BigNumber &other) const
  {

    return !(*this < other) && !(*this == other);
  }

  bool operator!=(const BigNumber &other) const
  {
    return !(*this == other);
  }

  bool operator==(const BigNumber &other) const
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

  BigNumber operator=(const string &str)
  {
    BigNumber res(str);

    *this = res;

    return *this;
  }

  // Sets the required precision for the `toString` method
  // Also modifies the precision of the division operation
  static void setPrecision(int precision)
  {
    if (precision < 1)
      throw invalid_argument("Precision should not be less than 1");
    BigNumber::precision = precision;
  }

  static int getPrecision()
  {
    return BigNumber::precision;
  }

  // Truncates the number to the required precision
  // But does not add additinal zeroes
  BigNumber truncate(int precision) const
  {
    BigNumber res = *this;

    int digitsAfterPoint = this->digits.size() - this->decimalPoint;

    if (digitsAfterPoint <= precision)
      return res;

    int diff = digitsAfterPoint - precision;

    res.digits = res.digits.substr(0, res.digits.size() - diff);

    return res;
  }

  BigNumber operator-() const
  {

    BigNumber res = *this;

    res.isNegative = !this->isNegative;

    return res;
  }

  BigNumber(const string &str)
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

  static BigNumber factorial(int num)
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

  string toString()
  {

    string res = this->truncate(BigNumber::precision).digits;

    res.insert(decimalPoint, ".");
    if (isNegative)
    {
      res.insert(0, "-");
    }

    return res;
  }
};
int main()
{
  /*
  Testing
  */

#pragma region Initialization
  BigNumber n1("0002515.313100000");
  BigNumber n2("00122056.31231230000");
  BigNumber n3("-0000.313450000");
  BigNumber ZERO("0.00000");
  BigNumber n5("000992");

  BigNumber n11("00505");
  BigNumber n12("0005.45");
  BigNumber n13 = n11 + n12;

  BigNumber n14("13000040.33");
  BigNumber n15("1324241000.885000133");
  BigNumber n16 = n14 + n15;

  BigNumber n21("888.888");
  BigNumber n22("111.8099");
  BigNumber n23 = n21 + n22;

  BigNumber n31("00000.3334441");
  BigNumber n32("0000.3334440");

  BigNumber n41("8884.111");
  BigNumber n42("1381.333");
  BigNumber n43("103.5");

  BigNumber n51("38");
  BigNumber n52("108");
  BigNumber n53("8713902.317381273");
  BigNumber n54("0013.318380013");
#pragma endregion

#pragma region Testing toString
  assert(n1.toString() == "2515.3131");
  assert(n2.toString() == "122056.3123123");
  assert(n3.toString() == "-0.31345");
  assert(ZERO.toString() == "0.0");
  assert(n5.toString() == "992.0");

  assert(n13.toString() == "510.45");
  assert(n16.toString() == "1337241041.215000133");
  assert(n23.toString() == "1000.6979");
#pragma endregion

#pragma region Testing comparison
  assert(n11 < n12 == false);
  assert(n21 < n22 == false);
  assert(n21 < n21 == false);
  assert(n32 < n31 == true);

  assert(n21 == n21 == true);
  assert(n21 >= n21 == true);
  assert(n21 <= n21 == true);

  assert(n21 > n22 == true);
#pragma endregion

#pragma region Testing abs

  assert((-n1).abs() == n1);
  assert(n1.abs() == n1);
  assert((ZERO).abs() == ZERO);

#pragma endregion

#pragma region Subtraction / Addition

  TEST_OP(-5, +, 3, -2);
  TEST_OP(5, +, -3, 2);
  assert((n41 - n41).toString() == "0.0");
  assert((-n41).toString() == "-8884.111");
  assert(n41 - n41 == ZERO);
  assert((n42 - n43).toString() == "1277.833");
  assert((n43 - n42).toString() == "-1277.833");

#pragma endregion

#pragma region Multiplication
  assert((n51 * n52).toString() == "4104.0");
  assert((n51 * (-n52)).toString() == "-4104.0");
  assert((n53 * n54).toString() == "116055062.459045128823696549");
  assert(ZERO * n21 == ZERO);
  TEST_OP(0.00238, *, 1315.55, 3.131009);

#pragma endregion

#pragma region Factorial

  assert(BigNumber::factorial(10).toString() == "3628800.0");
  assert(BigNumber::factorial(20).toString() == "2432902008176640000.0");
  assert(BigNumber::factorial(30).toString() == "265252859812191058636308480000000.0");
  assert(BigNumber::factorial(40).toString() ==
         "815915283247897734345611269596"
         "115894272000000000.0");
  assert(BigNumber::factorial(80).toString() ==
         "715694570462638022948115337231865321655846573423657525771094450"
         "58227039255480148842668944867280814080000000000000000000.0");
  assert(BigNumber::factorial(80).toString() ==
         "715694570462638022948115337231865321655846573423657525"
         "7710944505822703925548014884266894486728081408000000000"
         "0000000000.0");
  assert(BigNumber::factorial(90).toString() ==
         "148571596448176149730952273362082573788556996128468876"
         "6942216863704985393094065876545992131370884059645617234"
         "469978112000000000000000000000.0");
  assert(BigNumber::factorial(100).toString() ==
         "933262154439441526816992388562667004907159682643816214"
         "6859296389521759999322991560894146397615651828625369792"
         "0827223758251185210916864000000000000000000000000.0");

#pragma endregion

#pragma region Division

  BigNumber n61("3684549.96"), n62("655.2"), n65("5623.55");
  BigNumber n63("330"), n64("11");
  BigNumber n71("3.131009"), n72("0.00238");

  assert((n63 / n64).toString() == "30.0");
  assert((n61 / n62).toString() == "5623.55");
  assert((n65 * n62).toString() == "3684549.96");
  assert((ZERO / n52).toString() == "0.0");
  assert((-n61 / n62).toString() == "-5623.55");
  assert((n71 / n72).toString() == "1315.55");

  TEST_OP(3.131009, /, 0.00238, 1315.55);

  BigNumber::setPrecision(10);

  TEST_OP(1213441, /, 313414, 3.8716872890);
  TEST_OP(912431274897118178471, /, 31132.0313797000001, 29308440036202697.2732357821);
#pragma endregion

#pragma region Truncate

  TEST_METHOD(0.13134, truncate(3), 0.131);
  TEST_METHOD(0.13134, truncate(5), 0.13134);
  TEST_METHOD(1000, truncate(1), 1000);

#pragma endregion

  return 0;
}