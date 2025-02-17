#include "Strawberry/Core/Types/Uninitialised.hpp"
#include <vector>


using namespace Strawberry::Core;


int main()
{
    static int numConstructed = 0;
    numConstructed = 0;

    struct UninitialisedTester
    {
        UninitialisedTester()
        {
            numConstructed++;
        }


        ~UninitialisedTester()
        {
            numConstructed--;
        }
    };

    using T = Uninitialised<UninitialisedTester>;

    std::vector<T> data(5);

    AssertEQ(numConstructed, 0);
    data[0].Construct();
    AssertEQ(numConstructed, 1);
    data[1].Construct();
    data[2].Construct();
    AssertEQ(numConstructed, 3);
    data[1].Destruct();
    AssertEQ(numConstructed, 2);
    data[0].Destruct();
    data[2].Destruct();
    AssertEQ(numConstructed, 0);
}