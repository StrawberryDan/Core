#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <mutex>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
    class ConditionVariable
    {
        public:
            void Wait();


            template<std::predicate P>
            void Wait(P predicate)
            {
                std::unique_lock lk(mMutex);
                mConditionVariable.wait(mMutex, predicate);
            }


            void WaitFor(double seconds);


            template<std::predicate P>
            void WaitFor(double seconds, P predicate)
            {
                std::unique_lock lk(mMutex);
                mConditionVariable.wait(mMutex, std::chrono::duration<double>(seconds), predicate);
            }


            void NotifyOne();
            void NotifyAll();

        private:
            std::mutex              mMutex;
            std::condition_variable mConditionVariable;
    };
} // namespace Strawberry::Core
