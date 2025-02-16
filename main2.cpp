#include <iostream>
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>

using namespace std;

class BigNumber
{
public:
  static const int bits = 16;
  uint32_t digits[bits]{};

  static string multiply_by_two(const string &num)
  {
    string res;
    int carry = 0;
    for (int i = num.size() - 1; i >= 0; --i)
    {
      int digit = num[i] - '0';
      int product = digit * 2 + carry;
      carry = product / 10;
      res.push_back((product % 10) + '0');
    }
    if (carry != 0)
    {
      res.push_back(carry + '0');
    }
    reverse(res.begin(), res.end());
    return res;
  }

  static string multiply_by_2_32(const string &num)
  {
    string res = num;
    for (int i = 0; i < 32; ++i)
    {
      res = multiply_by_two(res);
    }
    return res;
  }

  static string add_string(const string &a, const string &b)
  {
    string res;
    int carry = 0;
    int i = a.size() - 1;
    int j = b.size() - 1;
    while (i >= 0 || j >= 0 || carry)
    {
      int sum = carry;
      if (i >= 0)
        sum += a[i--] - '0';
      if (j >= 0)
        sum += b[j--] - '0';
      carry = sum / 10;
      res.push_back((sum % 10) + '0');
    }
    reverse(res.begin(), res.end());
    return res;
  }

public:
  // Arithmetic operators and others...

  string toString()
  {
    string res = "0";
    bool all_zero = true;
    for (int i = 0; i < bits; ++i)
    {
      if (digits[i] != 0)
      {
        all_zero = false;
        break;
      }
    }
    if (all_zero)
    {
      return "0";
    }

    for (int i = bits - 1; i >= 0; --i)
    {
      res = multiply_by_2_32(res);
      uint32_t current_digit = digits[i];
      string digit_str = to_string(current_digit);
      res = add_string(res, digit_str);
    }

    return res;
  }
};

int main()
{
  // Example usage:
  BigNumber num;
  num.digits[0] = 112;            // Represents 1
  cout << num.toString() << endl; // Output: 1

  BigNumber num2;
  num2.digits[1] = 312;            // Represents 312*2^32 = 4294967296
  cout << num2.toString() << endl; // Output: 4294967296

  return 0;
}