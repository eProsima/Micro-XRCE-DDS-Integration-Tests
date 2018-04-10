/****************************************************************************
 *
 * Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "TestCommon.h"


bool operator==(const std::array<uint8_t, 2>& left, const uint16_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))));
}

bool operator==(const uint16_t left, const std::array<uint8_t, 2>& right)
{
    return right == left;
}

bool operator==(const std::array<uint8_t, 3>& left, const uint32_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))) &&
            (left[2] == (0xFF & (right >> 16))));
}

bool operator==(const uint32_t left, const std::array<uint8_t, 3>& right)
{
    return right == left;
}

bool operator==(const std::array<uint8_t, 4>& left, const uint32_t& right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))) &&
            (left[2] == (0xFF & (right >> 16))) &&
            (left[3] == (0xFF & (right >> 24))));
}

bool operator==(const dds::xrce::ClientKey& left, const ClientKey& right)
{
    return ((left[0] == right.data[0]) &&
            (left[1] == right.data[1]) &&
            (left[2] == right.data[2]) &&
            (left[3] == right.data[3]));
}

bool operator==(const uint32_t& left, const std::array<uint8_t, 4>& right)
{
    return right == left;
}


bool are_the_same(const std::vector<uint8_t>& _vector, const uint8_t* _array, const uint32_t& array_length)
{
    return array_length == _vector.size() &&
           std::equal(_vector.begin(), _vector.end(), _array, [](const uint8_t& a, const uint8_t& b){return a == b;});
}

bool operator==(const std::array<unsigned char, 2>& left, const ObjectId& right)
{
    // TODO check it works.
    return left == right;
}

bool operator==(const std::array<unsigned char, 2>& left, const RequestId& right)
{
    // TODO check it works.
    return left == right;
}
