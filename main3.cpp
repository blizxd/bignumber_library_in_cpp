#include <iostream>
#include <cstdint>
#include <array>
#include <string>
#include <algorithm>

using namespace std;

class BigNumber
{
private:
  string digits;

  int decimalIndex;

  BigNumber() {}

  void allignDecimalPlaces(string &left, string &right)
  {

    int leftDecimalIndex = left.find(".");
    int rightDecimalIndex = right.find(".");

    int leftFracLen = left.size() - leftDecimalIndex;
    int rightFracLen = right.size() - rightDecimalIndex;
    int diff = rightFracLen - leftFracLen;

    // Check which num has more digits after decimal point
    if (diff > 0)
      left.append(diff, '0'); // Append to left
    else if (diff < 0)
      right.append(-diff, '0'); // Append to right
  }

  void removeLeadingZeroes()
  {

    bool onlyZeroes = true;

    string res;

    int decimalPoint = digits.find(".");

    for (int i = 0; i < decimalPoint - 1; i++)
    {

      if (digits[i] == '0' && onlyZeroes)
        continue;
      onlyZeroes = false;
      res += digits[i];
    }

    decimalIndex = res.size() + 1;
    digits = res + digits.substr(decimalPoint - 1, digits.size() - decimalPoint + 1);
  }

  void removeTrailingZeroes()
  {

    bool onlyZeroes = true;

    string res;

    int decimalPoint = digits.find(".");

    for (int i = digits.size() - 1; i >= decimalPoint + 2; i--)
    {

      if (digits[i] == '0' && onlyZeroes)
        continue;
      onlyZeroes = false;
      res += digits[i];
    }

    reverse(res.begin(), res.end());

    digits = digits.substr(0, decimalPoint + 2) + res;
  }

public:
  // Arithmetic operators
  BigNumber operator+(const BigNumber &num)
  {

    BigNumber res;

    // Ensure we are not modifying original {this}
    string leftDigits = this->digits;
    string rightDigits = num.digits;

    allignDecimalPlaces(leftDigits, rightDigits);

    uint8_t carry = 0;
    uint8_t temp;

    int i = leftDigits.size() - 1;
    int j = rightDigits.size() - 1;

    while (i >= 0 || j >= 0 || carry)
    {

      // If current index corresponds to dec point
      // Append . to the result and continue
      if (i == decimalIndex)
      {
        res.digits += '.';
        i--;
        j--;
        continue;
      }
      temp = carry;

      if (i >= 0)
        temp += leftDigits[i--] - '0';
      if (j >= 0)
        temp += rightDigits[j--] - '0';

      carry = temp / 10;

      res.digits.push_back((temp % 10) + '0');
    }

    reverse(res.digits.begin(), res.digits.end());
    return res;
  };
  BigNumber operator-(const BigNumber &num)
  {

    BigNumber res;

    // Ensure we are not modifying original {this}
    string leftDigits = this->digits;
    string rightDigits = num.digits;

    allignDecimalPlaces(leftDigits, rightDigits);

    uint8_t borrow = 0;
    uint8_t temp;

    int i = leftDigits.size() - 1;
    int j = rightDigits.size() - 1;

    while (i >= 0 || j >= 0)
    {

      // If current index corresponds to dec point
      // Append . to the result and continue
      if (i == decimalIndex)
      {
        res.digits += '.';
        i--;
        j--;
        continue;
      }

      temp = 0;

      if (i >= 0)
        temp += leftDigits[i--] - '0' - borrow;
      if (j >= 0)
      {
        if (temp < rightDigits[j] - '0')
        {
          borrow = 1;
          temp += 10;
        }
        else
        {
          borrow = 0;
        }
        temp -= rightDigits[j] - '0';

        j--;
      }

      res.digits.push_back(temp + '0');
    }

    reverse(res.digits.begin(), res.digits.end());
    return res;
  }
  BigNumber operator*(BigNumber);
  BigNumber operator/(BigNumber);

  // Assignment operator
  BigNumber operator=(BigNumber);

  // Comparison operators
  BigNumber operator>(BigNumber);
  BigNumber operator>=(BigNumber);
  BigNumber operator==(BigNumber);
  BigNumber operator<=(BigNumber);
  BigNumber operator<(BigNumber);
  BigNumber operator!=(BigNumber);

  // To string
  string toString()
  {

    return digits;
  };

  BigNumber(const string &str)
  {

    if (str.empty() || str[0] == ',')
    {
      cout << "Invalid string\n";
      return;
    }

    int decimalPoint = str.find(".");

    digits = str;

    if (decimalPoint == string::npos)
      digits += ".0";

    removeLeadingZeroes();
    removeTrailingZeroes();
  }
};

int main()
{

  BigNumber n1("2515");
  BigNumber n2("1056.8");

  BigNumber n3 = n1 - n2;

  cout << n1.toString() << endl;
  cout << n2.toString() << endl;
  cout << n3.toString() << endl;

  return 0;
}