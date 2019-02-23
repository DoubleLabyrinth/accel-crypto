#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../Array.hpp"
#include "../Intrinsic.hpp"
#include <memory.h>
#include <assert.h>

namespace accel::Hash {

    class SHA256_ALG {
    private:
        static inline const uint32_t _K[64] = {
            0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
            0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
            0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
            0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
            0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
            0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
            0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
            0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
        };

        SecureWiper<Array<uint32_t, 8>> _StateWiper;
        Array<uint32_t, 8> _State;
    public:
        static constexpr size_t BlockSize = 64;
        static constexpr size_t DigestSizeValue = 32;

        SHA256_ALG() noexcept :
            _StateWiper(_State),
            _State{ 0x6A09E667u,
                    0xBB67AE85u,
                    0x3C6EF372u,
                    0xA54FF53Au,
                    0x510E527Fu,
                    0x9B05688Cu,
                    0x1F83D9ABu,
                    0x5BE0CD19u } {}

        void Cycle(const void* pData, size_t Rounds) noexcept {
            uint32_t Buffer[64] = {};
            uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0;
            auto MessageBlock = reinterpret_cast<const uint32_t(*)[16]>(pData);

            for (size_t i = 0; i < Rounds; ++i) {
                for (int j = 0; j < 16; ++j)
                    Buffer[j] = ByteSwap(MessageBlock[i][j]);

                for (int j = 16; j < 64; j++) {
                    Buffer[j] =
                            RotateShiftRight(Buffer[j - 2], 17) ^
                            RotateShiftRight(Buffer[j - 2], 19) ^
                            Buffer[j - 2] >> 10;
                    Buffer[j] += Buffer[j - 7];
                    Buffer[j] +=
                            RotateShiftRight(Buffer[j - 15], 7) ^
                            RotateShiftRight(Buffer[j - 15], 18) ^
                            Buffer[j - 15] >> 3;
                    Buffer[j] += Buffer[j - 16];
                }

                a = _State[0];
                b = _State[1];
                c = _State[2];
                d = _State[3];
                e = _State[4];
                f = _State[5];
                g = _State[6];
                h = _State[7];

                for (int j = 0; j < 64; j++) {
                    uint32_t T1 =
                        h +
                        (
                            RotateShiftRight(e, 6) ^
                            RotateShiftRight(e, 11) ^
                            RotateShiftRight(e, 25)
                        ) +
                        ((e & f) ^ (~e & g)) + _K[j] + Buffer[j];
                    uint32_t T2 =
                        (
                            RotateShiftRight(a, 2) ^
                            RotateShiftRight(a, 13) ^
                            RotateShiftRight(a, 22)
                        ) + ((a & b) ^ (a & c) ^ (b & c));
                    h = g;
                    g = f;
                    f = e;
                    e = d + T1;
                    d = c;
                    c = b;
                    b = a;
                    a = T1 + T2;
                }

                _State[0] += a;
                _State[1] += b;
                _State[2] += c;
                _State[3] += d;
                _State[4] += e;
                _State[5] += f;
                _State[6] += g;
                _State[7] += h;
            }
        }

        //
        //  Once Finish(...) is called, this object should be treated as const
        //
        void Finish(const void* pTailData, size_t TailDataSize, uint64_t ProcessedBytes) noexcept {
            assert(TailDataSize <= 2 * BlockSize - sizeof(uint64_t) - 1);

            uint8_t FormattedTailData[2 * BlockSize] = {};
            size_t Rounds;

            memcpy(FormattedTailData, pTailData, TailDataSize);
            FormattedTailData[TailDataSize] = 0x80;
            Rounds = TailDataSize >= BlockSize - sizeof(uint64_t) ? 2 : 1;
            *reinterpret_cast<uint64_t*>(FormattedTailData + (Rounds > 1 ? (2 * BlockSize - sizeof(uint64_t)) : (BlockSize - sizeof(uint64_t)))) =
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
            _State[5] = ByteSwap(_State[5]);
            _State[6] = ByteSwap(_State[6]);
            _State[7] = ByteSwap(_State[7]);
        }

        Array<uint8_t, DigestSizeValue> Digest() const noexcept {
            return _State.AsArrayOf<uint8_t, DigestSizeValue>();
        }
    };

}
