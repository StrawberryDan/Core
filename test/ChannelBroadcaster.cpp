#include "Strawberry/Core/IO/ChannelBroadcaster.hpp"


using namespace Strawberry::Core;


int main()
{
    struct A {};

    struct B {};

    struct C {};

    struct D {};

    struct R1 : public Strawberry::Core::IO::ChannelReceiver<A, B>
    {
        void Receive(const A& a) override {}

        void Receive(const B& b)  override {}
    };

    struct R2 : public Strawberry::Core::IO::ChannelReceiver<A, B, C, D>
    {
        void Receive(const A& a) override {}

        void Receive(const B& b) override {}

        void Receive(const C& c) override {}

        void Receive(const D& d) override {}
    };

    IO::ChannelBroadcaster<A, B, C> b1;
    R1                              r1;
    R2                              r2;

    b1.Register(r1);
    b1.Register(r2);
}