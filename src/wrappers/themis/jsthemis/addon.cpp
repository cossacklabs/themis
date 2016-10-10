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

#include <node.h>
#include "secure_message.hpp"
#include "secure_keygen.hpp"
#include "secure_session.hpp"
#include "secure_cell_seal.hpp"
#include "secure_cell_context_imprint.hpp"
#include "secure_cell_token_protect.hpp"
#include "secure_comparator.hpp"

void InitAll(v8::Handle<v8::Object> exports) {
  jsthemis::SecureMessage::Init(exports);
  jsthemis::KeyPair::Init(exports);
  jsthemis::SecureSession::Init(exports);
  jsthemis::SecureCellSeal::Init(exports);
  jsthemis::SecureCellContextImprint::Init(exports);
  jsthemis::SecureCellTokenProtect::Init(exports);
  jsthemis::SecureComparator::Init(exports);
}

NODE_MODULE(jsthemis, InitAll)
