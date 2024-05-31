#pragma once


namespace Strawberry::Core
{
    void DebugBreak();

    [[noreturn]] void Unreachable();


    template<typename T>
    [[noreturn]] T Unreachable()
    {
        Unreachable();
    }
} // namespace Strawberry::Core
