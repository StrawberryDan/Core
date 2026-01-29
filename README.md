# Strawberry Core

A core utility library for most C++ projects. Supports compilation with GCC, AppleClang, and Clang.

## Features including
* Base-64 Encoding/Decoding
* Universal and Channel based event broadcasters and receivers (Listener pattern)
* Endian-ness conversion
* Logging
* Linear, Smooth-linear and Perlin Noise functions
* Rational, Periodic, and Clamped/Saturating Number types
* Linear Algebra primitives (Vectors & Matrices)
* Synchronisation primitives (e.g. ProducerConsumerQueue, Mutex Wrappers)
* Thread Pools and Worker Threads
* Improved implementation of Variant types allowing derivation of Variant types based on set operations of thoses types (i.e. defining variant types as a union of two other variant types.)
* Improved implementation of Optional and Result/Expected types without possibility of unsafe access (in non-release builds.)
* Cross-platform implementations of some non-cross platform functionality (e.g. AlignedAlloc.)
* Process Creation/Management

## Planned Features
* Encoding-aware strings
