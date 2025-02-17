#include "Strawberry/Core/Process.hpp"


using namespace Strawberry::Core;


int main()
{
#ifdef STRAWBERRY_TARGET_WINDOWS
    Process process("dir");
    Process::Result result = process.Wait();
    AssertEQ(result, 0);
#else
    #warning No test for Core::Process on this platform!
#endif
}