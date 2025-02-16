#include <iostream>
#include <cstdint>
#include <array>
#include <string>

using namespace std;

class BigNumber
{
private:
  static const char bits = 16;
  uint8_t digits[bits]{};
  static const char period = 100;

public:
  // Trying to store numbers in base 100

  // Arithmetic operators
  BigNumber operator+(const BigNumber &num)
  {
    BigNumber res("0");

    uint64_t carry = 0;
    uint64_t temp = 0;

    for (int i = 0; i < BigNumber::bits; i++)
    {

      temp = this->digits[i] + num.digits[i] + carry;

      res.digits[i] = temp % 100;

      carry = temp / 100;
    }

    return res;
  };
  BigNumber operator-(BigNumber);
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
    string res;

    for (int i = BigNumber::bits - 1; i >= 0; i--)
    {

      if (digits[i] == 0)
        continue;

      res += to_string(digits[i]);
    }
    return res;
  };

  BigNumber(string str)
  {

    if (str.size() % 2 == 1)
      str.insert(0, "0");

    uint8_t size = str.size();

    if (size > bits * 2)
    {
      cout << "The number size is over 32 digits\n";
      return;
    }

    for (int i = 0; i < size / 2; i++)
    {

      string substr = str.substr(i * 2, 2);

      digits[size / 2 - i - 1] = stoi(substr);
    }
  }
};

int main()
{

  BigNumber n1("0000000000000000000000000000000000");
  BigNumber n2("13133183243243424321122223");

  BigNumber n3 = n1 + n2;

  cout << n3.toString();

  return 0;
}