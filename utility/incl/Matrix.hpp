// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
#ifndef SB_UTILITY_MATRIX_HPP
#define SB_UTILITY_MATRIX_HPP

///////////////////////////////////
// STD C++
#include <cstring>
///////////////////////////////////

namespace sb
{
namespace utility
{
    template<typename T, size_t N, size_t M>
    struct Matrix
    {
        Matrix(const T (&v)[N][M])
        {
            memcpy(values, v, sizeof(values));
        }

        Matrix(const Matrix<T, N, M>& other)
        {
            memcpy(values, other.values, sizeof(values));
        }

        Matrix()
        : values{{0}}
        {
        }

        Matrix<T, N, M>& operator=(const Matrix<T, N, M>& other)
        {
            memcpy(values, other.values, sizeof(values));
            return *this;
        }

        T* operator[](size_t n)
        {
            return values[n];
        }

        const T* operator[](size_t n) const
        {
            return values[n];
        }

        template<typename OtherT, size_t OtherM>
        Matrix<T, M, OtherM> transposedMul(const Matrix<OtherT, N, OtherM>& other) const
        {
            Matrix<T, M, OtherM> out;
            for(size_t mOther = 0; mOther < OtherM; mOther++)
            {
                for(size_t m = 0; m < M; m++)
                {
                    for(size_t n = 0; n < N; n++)
                    {
                        out[m][mOther] += values[n][m] * other[n][mOther];
                    }
                }
            }
            return out;
        }

        T values[N][M];
    };
}
}

#endif // SB_UTILITY_MATRIX_HPP
