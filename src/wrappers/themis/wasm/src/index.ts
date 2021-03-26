// Copyright (c) 2019 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import context, { YourOwnEmscriptenModule } from "./context";
import libthemisFn from "./libthemis.js";

export { SecureCellSeal } from "./secure_cell_seal";
export { SecureCellTokenProtect } from "./secure_cell_token_protect";
export { SecureCellContextImprint } from "./secure_cell_context_imprint";
export { ThemisError, ThemisErrorCode } from "./themis_error";
export {
  SecureMessageSign,
  SecureMessage,
  SecureMessageVerify,
} from "./secure_message";
export { SecureSession } from "./secure_session";
export { KeyPair, PrivateKey, PublicKey, SymmetricKey } from "./secure_keygen";
export { SecureComparator } from "./secure_comparator";

export const initialize = async (wasmPath: string) => {
  const libthemis = await libthemisFn({
    onRuntimeInitialized: function () {},
    locateFile: function () {
      return wasmPath;
    },
  });

  context.libthemis = libthemis as YourOwnEmscriptenModule;
  return libthemis;
};
