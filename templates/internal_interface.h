#pragma once


/*
Copyright (c) 2020, Eric Hyer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



// 'a o e u' (without spaces) replaced by letters deliminated by '/' (aoeu)
// 's n t h' (without spaces) replaced by letters deliminated by '_' (snth)



#include <cstdint>
#include <string>
#include <vector>


int mm_count_snth();
std::string const & mm_at_snth(int index);
int mm_lookup_snth(std::string const & word, bool * found);
int mm_as_longest_snth(int index);
int mm_ordinal_summation_snth(int index);
std::vector<int8_t> const & mm_flagged_parts_of_speech_snth(int index);
bool mm_is_name_snth(int index);
bool mm_is_male_name_snth(int index);
bool mm_is_female_name_snth(int index);
bool mm_is_place_snth(int index);
bool mm_is_compound_snth(int index);
bool mm_is_acronym_snth(int index);
std::vector<int> const & mm_synonyms_snth(int index);
std::vector<int> const & mm_antonyms_snth(int index);
