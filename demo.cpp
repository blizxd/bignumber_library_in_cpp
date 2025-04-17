#include "bigNumFn.h"
#include "bigNumber.h"
#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define TEST_OP(a, op, b, res) assert(BigNumber(#a) op BigNumber(#b) == BigNumber(#res))
#define TEST_METHOD(a, method, res) assert(BigNumber(#a).method == BigNumber(#res));
#define TEST_METHOD_BOOL(a, method, res) assert(BigNumber(#a).method == res);
#define TEST_COMPARE(a, op, b, res) assert((BigNumber(#a) op BigNumber(#b)) == res)

using namespace std;

template <typename T> uint64_t measureTime(T t)
{
    auto start = chrono::high_resolution_clock::now();

    t();

    auto end = chrono::high_resolution_clock::now();
    auto total = chrono::duration_cast<chrono::milliseconds>(end - start);

    return total.count();
}

int main()
{
    /*
    Testing
    */
    using std::cout, std::endl;
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

    TEST_COMPARE(505, <, 5.45, false);
    TEST_COMPARE(888.888, <, 111.8099, false);
    TEST_COMPARE(888.888, <, 888.888, false);
    TEST_COMPARE(0.3334440, <, 0.3334441, true);

    TEST_COMPARE(888.888, ==, 888.888, true);
    TEST_COMPARE(888.888, >=, 888.888, true);
    TEST_COMPARE(888.888, <=, 888.888, true);

    TEST_COMPARE(888.888, >, 111.8099, true);

#pragma endregion

#pragma region Testing abs

    assert((-n1).abs() == n1);
    assert(n1.abs() == n1);
    assert((ZERO).abs() == ZERO);

#pragma endregion

#pragma region Subtraction / Addition

    TEST_OP(0, +, 0, 0);
    TEST_OP(-5, +, 3, -2);
    TEST_OP(5, +, -3, 2);
    TEST_OP(505, +, 5.45, 510.45);
    TEST_OP(13000040.33, +, 1324241000.885000133, 1337241041.215000133);
    TEST_OP(888.888, +, 111.8099, 1000.6979);

    TEST_OP(8884.111, -, 8884.111, 0.0);
    TEST_OP(8884.111, -, 1381.333, 7502.778);
    TEST_OP(1381.333, -, 103.5, 1277.833);
    TEST_OP(103.5, -, 1381.333, -1277.833);
    TEST_OP(0.00000001, -, 0.000000000, 0.00000001);
    TEST_OP(3414141.237413, -, -4141.54534, 3418282.782753);

    assert(
        BigNumber(
            "0.0000000000000000000000000000004000000000000000000000000000000000000000000001") -
            BigNumber(
                "0.0000000000000000000000000000000000000000000000000000000000000000000000000001") ==
        BigNumber(
            "0.0000000000000000000000000000004000000000000000000000000000000000000000000000"));
    assert(
        BigNumber(
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000") -
            BigNumber(
                "0.9999999999999999999999999999999999999999999999999999999999999999999999999999") ==
        BigNumber(
            "0.0000000000000000000000000000000000000000000000000000000000000000000000000001"));

#pragma endregion

#pragma region Multiplication
    TEST_OP(38, *, 108, 4104.0);
    TEST_OP(38, *, -108, -4104.0);
    TEST_OP(8713902.317381273, *, 13.318380013, 116055062.459045128823696549);
    TEST_OP(0, *, 888.888, 0.0);
    TEST_OP(0.00238, *, 1315.55, 3.131009);

#pragma endregion

#pragma region Factorial

    // assert(BigNumFn::factorial(10).toString() == "3628800.0");
    // assert(BigNumFn::factorial(20).toString() == "2432902008176640000.0");
    // assert(BigNumFn::factorial(30).toString() == "265252859812191058636308480000000.0");
    // assert(BigNumFn::factorial(40).toString() == "815915283247897734345611269596"
    //                                              "115894272000000000.0");
    // assert(BigNumFn::factorial(80).toString() ==
    //        "715694570462638022948115337231865321655846573423657525771094450"
    //        "58227039255480148842668944867280814080000000000000000000.0");
    // assert(BigNumFn::factorial(80).toString() ==
    //        "715694570462638022948115337231865321655846573423657525"
    //        "7710944505822703925548014884266894486728081408000000000"
    //        "0000000000.0");
    // assert(BigNumFn::factorial(90).toString() ==
    //        "148571596448176149730952273362082573788556996128468876"
    //        "6942216863704985393094065876545992131370884059645617234"
    //        "469978112000000000000000000000.0");
    // assert(BigNumFn::factorial(100).toString() ==
    //        "933262154439441526816992388562667004907159682643816214"
    //        "6859296389521759999322991560894146397615651828625369792"
    //        "0827223758251185210916864000000000000000000000000.0");

#pragma endregion

#pragma region Division

    TEST_OP(3684549.96, /, 655.2, 5623.55);
    TEST_OP(330, /, 11, 30.0);
    TEST_OP(5623.55, *, 655.2, 3684549.96);
    TEST_OP(0, /, 108, 0.0);
    TEST_OP(-3684549.96, /, 655.2, -5623.55);
    TEST_OP(3684549.96, /, -655.2, -5623.55);
    TEST_OP(3.131009, /, 0.00238, 1315.55);

#pragma endregion

#pragma region Truncate

    TEST_METHOD(0.13134, truncate(3), 0.131);
    TEST_METHOD(0.13134, truncate(5), 0.13134);
    TEST_METHOD(1000, truncate(1), 1000);

#pragma endregion

#pragma region Round

    TEST_METHOD(0.1315, round(3), 0.132);
    TEST_METHOD(0.13134, round(5), 0.13134);
    TEST_METHOD(1000.5, round(0), 1001);
    TEST_METHOD(-1000.5, round(0), -1001);

#pragma endregion

#pragma region isInt

    TEST_METHOD_BOOL(3132213, isInt(), true);
    TEST_METHOD_BOOL(0, isInt(), true);
    TEST_METHOD_BOOL(-313.2213, isInt(), false);
    TEST_METHOD_BOOL(-0.00000001, isInt(), false);
    TEST_METHOD_BOOL(-0.00000000, isInt(), true);

#pragma endregion

#pragma region Modulus
    TEST_OP(5, %, 3, 2);
    TEST_OP(10, %, 3, 1);
    TEST_OP(-7, %, 3, -1);
    TEST_OP(7, %, -3, 1);
    TEST_OP(-7, %, -3, -1);
    TEST_OP(0, %, 5, 0);
    TEST_OP(25, %, 5, 0);
    TEST_OP(100, %, 101, 100);
    TEST_OP(123456789, %, 1000, 789);
#pragma endregion

#pragma region required for test
    assert(BigNumFn::sqroot(0.47757561893).toString() ==
           "0.6910684618255994468819967992347294702963950606816492239604015606472946339100888190"
           "2246729"
           "7781586546");

    assert(BigNumFn::ln(5).toString() == "1."
                                         "609437912434100374600759333226187639525601354268517721912"
                                         "647891474178987707657764630133878093179611");

    assert(BigNumFn::sin(20).toString() == "0."
                                           "9129452507276276543760999838456823012979325837081899563"
                                           "05128114768848919349072976044480049640469976");

    assert(BigNumFn::sqroot(100000000).toString() == "10000.0");

    assert(BigNumFn::sin(-12.678).toString() ==
           "-0."
           "111397692189140567046372728408531447761129459003373822311973345385656456165709076833220"
           "769881585879");

#pragma endregion

#pragma region sqroot

    cout << BigNumFn::sqroot(2).toString() << endl;
    cout << BigNumFn::sqroot(7).toString() << endl;
    cout << BigNumFn::sqroot(0.000000000008).toString() << endl;
    cout << BigNumFn::sqroot(88).toString() << endl;
    cout << BigNumFn::sqroot((string) "321312312331888888883254325523532525").toString() << endl;

    cout << BigNumFn::ceilSqroot(15).toString() << endl;
    cout << BigNumFn::ceilSqroot(9).toString() << endl;
    cout << BigNumFn::ceilSqroot(8888).toString() << endl;

#pragma endregion

#pragma region timings

    cout << "Square root of 2 " << measureTime([]() -> void { BigNumFn::sqroot(2); }) << "ms \n";
    cout << "Next prime " << measureTime([]() -> void { BigNumFn::nextPrime(3524); }) << "ms \n";

#pragma endregion

    return 0;
}