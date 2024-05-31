#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <atomic>
#include <functional>
#include <thread>

#include "Strawberry/Core/Types/ReflexivePointer.hpp"


namespace Strawberry::Core
{
    class RepeatingTask
            : public EnableReflexivePointer<RepeatingTask>
    {
        public:
            /// Accepts a function taking no arguments.
            explicit RepeatingTask(std::function<void()> function)
                : mShouldRun(true)
                , mStartUp()
                , mFunction(std::move(function))
            {
                Start();
            }


            /// Accepts a function taking no arguments.
            explicit RepeatingTask(std::function<void()> function, std::function<void()> startup)
                : mShouldRun(true)
                , mStartUp(std::move(startup))
                , mFunction(std::move(function))
            {
                Start();
            }


            /// Will pass 'this' as the first argument always.
            explicit RepeatingTask(std::function<void(RepeatingTask*)> function)
                : mShouldRun(true)
                , mStartUp()
                , mFunction([function, this]
                {
                    function(this);
                })
            {
                Start();
            }


            /// Accepts a function taking no arguments.
            explicit RepeatingTask(std::function<void(RepeatingTask*)> startup, std::function<void()> function)
                : mShouldRun(true)
                , mStartUp([startup, this]
                {
                    startup(this);
                })
                , mFunction(std::move(function))
            {
                Start();
            }


            /// Will pass 'this' as the first argument always.
            explicit RepeatingTask(std::function<void()> startup, const std::function<void(RepeatingTask*)> function)
                : mShouldRun(true)
                , mStartUp(std::move(startup))
                , mFunction([function, this]
                {
                    function(this);
                })
            {
                Start();
            }


            /// Will pass 'this' as the first argument always.
            explicit RepeatingTask(std::function<void(RepeatingTask*)> startup, std::function<void(RepeatingTask*)> function)
                : mShouldRun(true)
                , mStartUp([startup, this]
                {
                    startup(this);
                })
                , mFunction([function, this]
                {
                    function(this);
                })
            {
                Start();
            }


            ~RepeatingTask()
            {
                mShouldRun = false;
                mThread.join();
            }


            [[nodiscard]] bool IsRunning() const
            {
                return mShouldRun || !mThread.joinable();
            }


            void Start()
            {
                mThread = std::thread([self = GetReflexivePointer()]()
                {
                    if (self->mStartUp)
                    {
                        (*self->mStartUp)();
                        self->mStartUp.Reset();
                    }

                    while (self->mShouldRun)
                    {
                        self->mFunction();
                    }
                });
            }


            void Stop()
            {
                if (IsRunning())
                {
                    mShouldRun = false;
                    mThread.join();
                }
            }


            RepeatingTask(const RepeatingTask& rhs)            = delete;
            RepeatingTask& operator=(const RepeatingTask& rhs) = delete;


            RepeatingTask(RepeatingTask&& rhs)
            {
                bool wasRunning = rhs.IsRunning();
                if (rhs.IsRunning())
                {
                    rhs.Stop();
                }

                mShouldRun = false;
                mThread    = std::move(rhs.mThread);
                mStartUp   = std::move(rhs.mStartUp);
                mFunction  = std::move(rhs.mFunction);

                if (wasRunning)
                {
                    Start();
                }
            }


            RepeatingTask& operator=(RepeatingTask&& rhs)
            {
                if (this != &rhs)
                {
                    std::destroy_at(this);
                    std::construct_at(this, std::forward<RepeatingTask&&>(rhs));
                }

                return *this;
            }

        private:
            std::atomic<bool>                      mShouldRun;
            std::thread                            mThread;
            Core::Optional<std::function<void()> > mStartUp;
            std::function<void()>                  mFunction;
    };
} // namespace Strawberry::Core
