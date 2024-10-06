#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
    template <typename... Functions>
    class Overload
        : Functions...
    {
    public:
        Overload(Functions&&... functions)
            : Functions(std::forward<Functions>(functions))...
        {}

        using Functions::operator ()...;
    };

    template <typename... Functions>
    Overload(Functions... functions) -> Overload<Functions...>;
}