#pragma once
#include <stddef.h>
#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__)
#include <x86intrin.h>

#ifndef __forceinline
#define __forceinline __attribute__((always_inline)) inline
#endif

namespace accel::Intrinsic {

    //
    //  Begin ByteSwap
    //

    template<typename __IntegerType>
    __IntegerType ByteSwap(__IntegerType x);

    template<>
    __forceinline
    int16_t ByteSwap(int16_t x) {
        return __builtin_bswap16(x);
    }

    template<>
    __forceinline
    uint16_t ByteSwap(uint16_t x) {
        return __builtin_bswap16(x);
    }

    template<>
    __forceinline
    int32_t ByteSwap(int32_t x) {
        return __builtin_bswap32(x);
    }

    template<>
    __forceinline
    uint32_t ByteSwap(uint32_t x) {
        return __builtin_bswap32(x);
    }

    template<>
    __forceinline
    int64_t ByteSwap(int64_t x) {
        return __builtin_bswap64(x);
    }

    template<>
    __forceinline
    uint64_t ByteSwap(uint64_t x) {
        return __builtin_bswap64(x);
    }

    //
    //  Begin RotateShiftLeft
    //

    template<typename __IntegerType>
    __IntegerType RotateShiftLeft(__IntegerType x, int shift);

    template<>
    int8_t RotateShiftLeft<int8_t>(int8_t x, int shift) {
        return __rolb(x, shift);
    }

    template<>
    uint8_t RotateShiftLeft<uint8_t>(uint8_t x, int shift) {
        return __rolb(x, shift);
    }

    template<>
    int16_t RotateShiftLeft<int16_t>(int16_t x, int shift) {
        return __rolw(x, shift);
    }

    template<>
    uint16_t RotateShiftLeft<uint16_t>(uint16_t x, int shift) {
        return __rolw(x, shift);
    }

    template<>
    int32_t RotateShiftLeft<int32_t>(int32_t x, int shift) {
        return __rold(x, shift);
    }

    template<>
    uint32_t RotateShiftLeft<uint32_t>(uint32_t x, int shift) {
        return __rold(x, shift);
    }

    template<>
    int64_t RotateShiftLeft<int64_t>(int64_t x, int shift) {
        return __rolq(x, shift);
    }

    template<>
    uint64_t RotateShiftLeft<uint64_t>(uint64_t x, int shift) {
        return __rolq(x, shift);
    }

    //
    //  Begin RotateShiftRight
    //

    template<typename __IntegerType>
    __IntegerType RotateShiftRight(__IntegerType x, int shift);

    template<>
    int8_t RotateShiftRight<int8_t>(int8_t x, int shift) {
        return __rorb(x, shift);
    }

    template<>
    uint8_t RotateShiftRight<uint8_t>(uint8_t x, int shift) {
        return __rorb(x, shift);
    }

    template<>
    int16_t RotateShiftRight<int16_t>(int16_t x, int shift) {
        return __rorw(x, shift);
    }

    template<>
    uint16_t RotateShiftRight<uint16_t>(uint16_t x, int shift) {
        return __rorw(x, shift);
    }

    template<>
    int32_t RotateShiftRight<int32_t>(int32_t x, int shift) {
        return __rord(x, shift);
    }

    template<>
    uint32_t RotateShiftRight<uint32_t>(uint32_t x, int shift) {
        return __rord(x, shift);
    }

    template<>
    int64_t RotateShiftRight<int64_t>(int64_t x, int shift) {
        return __rorq(x, shift);
    }

    template<>
    uint64_t RotateShiftRight<uint64_t>(uint64_t x, int shift) {
        return __rorq(x, shift);
    }
}

#else
#error "Unknown compiler"
#endif



