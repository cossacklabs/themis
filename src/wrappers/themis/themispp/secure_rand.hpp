/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef THEMISPP_SECURE_RAND_HPP_
#define THEMISPP_SECURE_RAND_HPP_

#include <vector>
#include <themis/themis.h>
#include "exception.hpp"

namespace themispp{

    template <size_t block_length_t_p>
    class secure_rand_t{
	public:
	    secure_rand_t():
		n_(block_length_t_p, 0){}

	    std::vector<uint8_t>& get(){
		soter_rand(&n_[0], block_length_t_p);
		return n_;
	    }
	private:
	    std::vector<uint8_t> n_;
    };
}//themis

#endif