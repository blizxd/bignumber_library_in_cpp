#include <iostream>
#include <cstdint>
#include <array>
#include <string>
#include <algorithm>
#include <assert.h>

using namespace std;

class BigNumber
{
private:
  string digits;
  uint16_t decimalPoint;
  bool isNegative;

  BigNumber() {}

  void removeTrailingZeroes()
  {
    if (digits.empty())
      return;
    int zeroesEndIndex = digits.find_last_not_of('0');
    if (zeroesEndIndex == string::npos)
    {
      digits = "0";
      decimalPoint = 1;
      return;
    }
    if (zeroesEndIndex < decimalPoint - 1)
      zeroesEndIndex = decimalPoint - 1;
    digits = digits.substr(0, zeroesEndIndex + 1);
  }

  void removeLeadingZeroes()
  {
    int zeroesEndIndex = digits.find_first_not_of('0');
    if (zeroesEndIndex == string::npos)
    {
      digits = "0";
      decimalPoint = 1;
      return;
    }
    if (zeroesEndIndex > decimalPoint - 1)
      zeroesEndIndex = decimalPoint - 1;
    digits = digits.substr(zeroesEndIndex);
    decimalPoint -= zeroesEndIndex;
  }

  bool isValidInput(const string &str)
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

    if (diff > 0)
      left.digits.append(diff, '0');
    else if (diff < 0)
      right.digits.append(-diff, '0');
  }

  bool compareAbsValue(const BigNumber &other) // |this|<|other|
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

  BigNumber subtractAbsValue(const BigNumber &other) const
  {
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

public:
  BigNumber operator+(const BigNumber &other)
  {
    BigNumber res;

    if (isNegative == other.isNegative)
    {
      res = addAbsValue(other);
      res.isNegative = isNegative;
    }
    else
    {
      if (compareAbsValue(other))
      {
        res = other.subtractAbsValue(*this);
        res.isNegative = other.isNegative;
      }
      else
      {
        res = subtractAbsValue(other);
        res.isNegative = isNegative;
      }
    }

    return res;
  }

  BigNumber operator-(const BigNumber &other)
  {
    BigNumber res;

    if (isNegative != other.isNegative)
    {
      res = addAbsValue(other);
      res.isNegative = isNegative;
    }
    else
    {
      if (compareAbsValue(other))
      {
        res = other.subtractAbsValue(*this);
        res.isNegative = !isNegative;
      }
      else
      {
        res = subtractAbsValue(other);
        res.isNegative = isNegative;
      }
    }

    return res;
  }

  bool operator<(const BigNumber &other)
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

  bool operator<=(const BigNumber &other)
  {
    return !(*this > other);
  }

  bool operator>=(const BigNumber &other)
  {
    return !(*this < other);
  }

  bool operator>(const BigNumber &other)
  {
    return !(*this < other) && !(*this == other);
  }

  bool operator!=(const BigNumber &other)
  {
    return !(*this == other);
  }

  bool operator==(const BigNumber &other)
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

  BigNumber(const string &str)
  {
    if (!isValidInput(str))
    {
      cout << "Incorrect number format " << endl;
      return;
    }

    isNegative = false;
    string processedStr = str;

    if (!str.empty() && str[0] == '-')
    {
      isNegative = true;
      processedStr = str.substr(1);
    }

    size_t decimalPos = processedStr.find('.');
    if (decimalPos == string::npos)
    {
      processedStr += ".0";
      decimalPos = processedStr.size() - 2;
    }

    digits = processedStr.substr(0, decimalPos) + processedStr.substr(decimalPos + 1);
    decimalPoint = decimalPos;

    removeLeadingZeroes();
    removeTrailingZeroes();

    if (digits.empty() || (digits.size() == 1 && digits[0] == '0'))
      isNegative = false;
  }

  string toString()
  {
    if (digits.empty())
      return "0.0";

    string res = digits;
    res.insert(decimalPoint, ".");
    if (isNegative)
      res.insert(0, "-");

    if (res.find('.') == string::npos)
      res += ".0";
    else if (res.back() == '.')
      res += "0";

    return res;
  }
};

int main()
{
  BigNumber n1("0002515.313100000");
  BigNumber n2("00122056.31231230000");
  BigNumber n3("-0000.313450000");
  BigNumber n4("0.00000");
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

  assert(n1.toString() == "2515.3131");
  assert(n2.toString() == "122056.3123123");
  assert(n3.toString() == "-0.31345");
  assert(n4.toString() == "0.0");
  assert(n5.toString() == "992.0");

  assert(n13.toString() == "510.45");
  assert(n16.toString() == "1337241041.215000133");
  assert(n23.toString() == "1000.6979");
  assert((n11 < n12) == false);
  assert((n21 < n22) == false);
  assert((n21 < n21) == false);
  assert((n32 < n31) == true);

  assert((n21 == n21) == true);
  assert((n21 >= n21) == true);
  assert((n21 <= n21) == true);
  assert((n21 > n22) == true);

  // New test cases for subtraction and mixed signs addition
  BigNumber sub1 = BigNumber("10.5") - BigNumber("3.2");
  assert(sub1.toString() == "7.3");

  BigNumber sub2 = BigNumber("2.5") - BigNumber("3.5");
  assert(sub2.toString() == "-1.0");

  BigNumber sub3 = BigNumber("5") - BigNumber("3");
  assert(sub3.toString() == "2.0");

  BigNumber sub4 = BigNumber("3") - BigNumber("5");
  assert(sub4.toString() == "-2.0");

  BigNumber sub5 = BigNumber("-5") - BigNumber("-3");
  assert(sub5.toString() == "-2.0");

  BigNumber sub6 = BigNumber("-3") - BigNumber("-5");
  assert(sub6.toString() == "2.0");

  BigNumber add1 = BigNumber("5") + BigNumber("-3");
  assert(add1.toString() == "2.0");

  BigNumber add2 = BigNumber("-5") + BigNumber("3");
  assert(add2.toString() == "-2.0");

  BigNumber add3 = BigNumber("-5") + BigNumber("-3");
  assert(add3.toString() == "-8.0");

  BigNumber add4 = BigNumber("5.5") + BigNumber("-2.5");
  assert(add4.toString() == "3.0");

  cout << "All tests passed!" << endl;
  return 0;
}