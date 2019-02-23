#pragma once
#include "../SecureWiper.hpp"
#include "../Array.hpp"
#include "../Intrinsic.hpp"
#include <memory.h>
#include <assert.h>

namespace accel::Hash {

    class SHA1_ALG {
    private:
        SecureWiper<Array<uint32_t, 5>> _StateWiper;
        Array<uint32_t, 5> _State;
    public:
        static constexpr size_t BlockSizeValue = 64;
        static constexpr size_t DigestSizeValue = 20;

        SHA1_ALG() noexcept :
            _StateWiper(_State),
            _State{ 0x67452301u,
                    0xEFCDAB89u,
                    0x98BADCFEu,
                    0x10325476u,
                    0xC3D2E1F0u } {}

        void Cycle(const void* pData, size_t Rounds) noexcept {
            uint32_t Buffer[80] = {};
            uint32_t a, b, c, d, e;
            auto MessageBlock = reinterpret_cast<const uint32_t(*)[BlockSizeValue / sizeof(uint32_t)]>(pData);

            for (size_t i = 0; i < Rounds; ++i) {
                for (int j = 0; j < 16; ++j)
                    Buffer[j] = ByteSwap(MessageBlock[i][j]);

                for (int j = 16; j < 80; ++j)
                    Buffer[j] = RotateShiftLeft(Buffer[j - 3] ^
                                                           Buffer[j - 8] ^
                                                           Buffer[j - 14] ^
                                                           Buffer[j - 16], 1);
                
                a = _State[0];
                b = _State[1];
                c = _State[2];
                d = _State[3];
                e = _State[4];

                for (int j = 0; j < 20; ++j) {
                    uint32_t T = RotateShiftLeft(a, 5);
                    T += ((b & c) ^ (~b & d)) + e + 0x5A827999 + Buffer[j];
                    e = d;
                    d = c;
                    c = RotateShiftLeft(b, 30);
                    b = a;
                    a = T;
                }
                for (int j = 20; j < 40; ++j) {
                    uint32_t T = RotateShiftLeft(a, 5);
                    T += (b ^ c ^ d) + e + 0x6ED9EBA1 + Buffer[j];
                    e = d;
                    d = c;
                    c = RotateShiftLeft(b, 30);
                    b = a;
                    a = T;
                }
                for (int j = 40; j < 60; ++j) {
                    uint32_t T = RotateShiftLeft(a, 5);
                    T += ((b & c) ^ (b & d) ^ (c & d)) + e + 0x8F1BBCDC + Buffer[j];
                    e = d;
                    d = c;
                    c = RotateShiftLeft(b, 30);
                    b = a;
                    a = T;
                }
                for (int j = 60; j < 80; ++j) {
                    uint32_t T = RotateShiftLeft(a, 5);
                    T += (b ^ c ^ d) + e + 0xCA62C1D6 + Buffer[j];
                    e = d;
                    d = c;
                    c = RotateShiftLeft(b, 30);
                    b = a;
                    a = T;
                }
                _State[0] += a;
                _State[1] += b;
                _State[2] += c;
                _State[3] += d;
                _State[4] += e;
            }
        }

        //
        //  Once Finish(...) is called, this object should be treated as const
        //
        void Finish(const void* pTailData, size_t TailDataSize, uint64_t ProcessedBytes) noexcept {
            assert(TailDataSize <= 2 * BlockSizeValue - sizeof(uint64_t) - 1);

            uint8_t FormattedTailData[2 * BlockSizeValue] = {};
            size_t Rounds;

            memcpy(FormattedTailData, pTailData, TailDataSize);
            FormattedTailData[TailDataSize] = 0x80;
            Rounds = TailDataSize >= BlockSizeValue - sizeof(uint64_t) ? 2 : 1;
            *reinterpret_cast<uint64_t*>(FormattedTailData + (Rounds > 1 ? (2 * BlockSizeValue - sizeof(uint64_t)) : (BlockSizeValue - sizeof(uint64_t)))) =
                    ByteSwap<uint64_t>(ProcessedBytes * 8);

            Cycle(FormattedTailData, Rounds);

            {   // clear FormattedTailData
                volatile uint8_t* p = FormattedTailData;
                size_t s = sizeof(FormattedTailData);
                while (s--) *p++ = 0;
            }

            _State[0] = ByteSwap(_State[0]);
            _State[1] = ByteSwap(_State[1]);
            _State[2] = ByteSwap(_State[2]);
            _State[3] = ByteSwap(_State[3]);
            _State[4] = ByteSwap(_State[4]);
        }

        Array<uint8_t, DigestSizeValue> Digest() const noexcept {
            return _State.AsArrayOf<uint8_t, DigestSizeValue>();
        }
    };

}

