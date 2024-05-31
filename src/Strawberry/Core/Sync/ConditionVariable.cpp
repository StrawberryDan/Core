//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ConditionVariable.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
    void ConditionVariable::Wait()
    {
        std::unique_lock lk(mMutex);
        mConditionVariable.wait(lk);
    }


    void ConditionVariable::WaitFor(double seconds)
    {
        std::unique_lock lk(mMutex);
        mConditionVariable.wait_for(lk, std::chrono::duration<double>(seconds));
    }


    void ConditionVariable::NotifyOne()
    {
        mConditionVariable.notify_one();
    }


    void ConditionVariable::NotifyAll()
    {
        mConditionVariable.notify_all();
    }
} // namespace Strawberry::Core
