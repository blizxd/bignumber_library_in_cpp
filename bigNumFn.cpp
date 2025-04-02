#include "bigNumFn.h"
#include "bigNumber.h"
#include <exception>
#include <iostream>

using namespace std;
namespace BigNumFn
{

    // Internal variables
    namespace
    {
        const BigNumber zero("0");
        const BigNumber one("1");
        const BigNumber two("2");
        const BigNumber
            PI("3."
               "14159265358979323846264338327950288419716939937510582097494459230781640628"
               "62089986280348253421170679821480865132823066470938446095505");
        const BigNumber ln2 =
            string("0."
                   "69314718055994530941723212145817656807550013436025525412068000949339362"
                   "19696947156058633269964186875");
        const int sinIterations = 100;
        const int lnIterations = 100;
        const int sqRootIterations = 60;

    }

    // Calculate sine; n is in radians
    BigNumber sin(const BigNumber &arg)
    {

        // Taylor series expansion at a =0

        BigNumber period = two * PI;
        BigNumber PIOverTwo = PI / 2;
        BigNumber n = arg;

        bool isNeg = n.isNegative();
        if (n.isNegative())
            n *= -1;

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

        for (int i = 1; i < sinIterations; i++)
        {
            x_i = x_i * intPow(n, 2) / ((2 * i) * (2 * i + 1)) * (-1);
            res += x_i;
        }

        return isNeg ? -res : res;
    }

    // Calculate √x using newton's method
    BigNumber sqroot(const BigNumber &n)
    {

        if (n < zero)
        {
            throw std::invalid_argument("The argument should be non-negative");
        }

        BigNumber oneOverN = one / n;
        BigNumber x = oneOverN / two; // Initial guess

        // Newton method for initial approximation of 1/sqroot(n)
        // f(x) = x^2-n
        // f'(x) = 2x
        for (int i = 0; i < 3; i++)
        {
            BigNumber f_x = x * x - oneOverN;
            BigNumber fprime_x = two * x;

            x = x - f_x / fprime_x; // x_(n+1) = x_n - f(x_n)/f'(x_n)
        }

#pragma region Newtons method

        BigNumber nOverTwo = n / 2;
        BigNumber threeOverTwo = 1.5;

        // Then calculate the 1/sqroot(n)
        // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
        // #Iterative_methods_for_reciprocal_square_roots
        for (int i = 0; i < sqRootIterations; i++)
        {
            x *= threeOverTwo - nOverTwo * intPow(x, 2);
        }

#pragma endregion

#pragma region Halleys method

        // BigNumber sqrootThreeOverEight =
        //     string("0."
        //            "61237243569579452454932101867647284799148687016416753210817314181274009436432875663"
        //            "49648582761600587046486503066535472812147163649594393354064459878093196382072281868"
        //            "8191914281190752631772794255587033");

        // BigNumber fifteenOverEight = string("1.875");
        // BigNumber sqrootTwentyFiveOverSix =
        //     string("2."
        //            "04124145231931508183107006225490949330495623388055844036057713937580031454776252211"
        //            "65495275872001956821621676888451576040490545498647977846881532926977321273574272896"
        //            "063971427063584210590931418529011");

        // BigNumber constBeforeY = sqrootThreeOverEight * n;

        // for (int i = 0; i < BigNumber::iterations; i++)
        // {
        //     BigNumber y_n = constBeforeY * intPow(x, 2);
        //     x *= fifteenOverEight - y_n * (sqrootTwentyFiveOverSix - y_n);
        // }

#pragma endregion

        BigNumber finalRes = n * x;

        return finalRes;
    }

    BigNumber factorial(int num)
    {

        BigNumber res("1");

        if (num > 10000 || num < 0)
        {
            throw logic_error("Input is out of range");
        }

        for (int i = 1; i <= num; i++)
        {

            res = res * i;
        }

        return res;
    }

    BigNumber ln(const BigNumber &n)
    {

        if (n <= 0)
            throw invalid_argument("Argument should be positive");

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

        BigNumber gMinusOneOverGPlusOne = (g - 1) / (g + 1);

        for (int i = 0; i < lnIterations; i++)
        {

            x_i = one / (2 * i + 1);
            x_i *= intPow(gMinusOneOverGPlusOne, 2 * i + 1);
            res += x_i;
        }

        res *= 2;

        return p * ln2 + res;
    }

    BigNumber intPow(const BigNumber &base, int exponent)
    {
        BigNumber res = one;
        for (int i = 0; i < exponent; i++)
        {
            res *= base;
        }
        return res;
    }

    bool isPrime(const BigNumber &n)
    {
        if (n <= one || !n.isInt())
            return false;

        BigNumber upperBound = (sqroot(n)).truncate(0) + 1;

        for (BigNumber i = two; i < upperBound; i += one)
        {
            if (n % i == zero)
                return false;
        }
        return true;
    }

    BigNumber nextPrime(const BigNumber &n)
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

    int find(const BigNumber arr[], int size, const BigNumber &target)
    {
        for (int i = 0; i < size; i++)
        {
            if (arr[i] == target)
                return i;
        }
        return -1;
    }

    void bubbleSort(BigNumber arr[], int size)
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

    BigNumber getAverage(const BigNumber arr[], int size)
    {
        BigNumber sum = zero;
        for (int i = 0; i < size; i++)
            sum += arr[i];
        return sum / BigNumber(size);
    }

    BigNumber chudnovskyPI(int n)
    {

        BigNumber a_k = one;
        BigNumber a_sum = one;
        BigNumber b_sum = zero;
        BigNumber C = 640320;

        BigNumber SQROOT_10005 =
            string("100."
                   "02499687578100594479218787635777800159502436869631465713551156965096785"
                   "38643042923111879484999732977551938893695661811101310349073901991031130"
                   "110817620021084773209484713755522300964025814877362598");

        BigNumber C3_OVER_24 = intPow(C, 3) / 24;
        BigNumber C3_OVER_24_INV = BigNumber(1.0) / C3_OVER_24;

        for (int i = 1; i < n; i++)
        {
            a_k *= -(6 * i - 5) * (2 * i - 1) * (6 * i - 1);
            a_k /= intPow(i, 3);
            a_k *= C3_OVER_24_INV;

            a_sum += a_k;
            b_sum += a_k * i;
        }

        BigNumber total = a_sum * 13591409 + b_sum * 545140134;
        BigNumber pi = SQROOT_10005 * 426880 / total;

        return pi.round(n);
    }

}
