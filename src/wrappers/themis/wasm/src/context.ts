import { ThemisErrorCode } from "./themis_error";
import { KeyKind } from "./secure_keygen";

export interface YourOwnEmscriptenModule extends EmscriptenModule {
  writeArrayToMemory: typeof writeArrayToMemory;
  _memset: (buffer: number, offer: number, length: number) => void;
  setValue: typeof setValue;
  getValue: typeof getValue;

  _secure_comparator_create(): number;

  _secure_comparator_destroy(ptr: number): ThemisErrorCode;

  _secure_comparator_append_secret(
    comparatorPtr: number,
    secret_ptr: number,
    secret_length: number
  ): ThemisErrorCode;

  _secure_comparator_proceed_compare(
    comparatorPtr: number,
    request_ptr: number,
    request_length: number,
    reply_ptr: number | null,
    reply_length_ptr: number
  ): ThemisErrorCode;

  _secure_comparator_begin_compare(
    comparatorPtr: number,
    message_ptr: number | null,
    message_length_ptr: number
  ): ThemisErrorCode;

  _themis_gen_sym_key(
    key_ptr: number | null,
    key_len_ptr: number
  ): ThemisErrorCode;

  _themis_gen_ec_key_pair(
    private_ptr: number | null,
    private_len_ptr: number,
    public_ptr: number | null,
    public_len_ptr: number
  ): ThemisErrorCode;

  _themis_get_asym_key_kind(buffer_ptr: number, buffer_len: number): KeyKind;

  _themis_is_valid_asym_key(
    buffer_ptr: number,
    buffer_len: number
  ): ThemisErrorCode;

  _themis_secure_cell_encrypt_context_imprint(
    master_key_ptr: number,
    master_key_length: number,
    message_ptr: number,
    message_length: number,
    context_ptr: number,
    context_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_decrypt_context_imprint(
    master_key_ptr: number,
    master_key_length: number,
    message_ptr: number,
    message_length: number,
    context_ptr: number,
    context_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_encrypt_seal(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_decrypt_seal(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_encrypt_seal_with_passphrase(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_decrypt_seal_with_passphrase(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_encrypt_token_protect(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    token_ptr: number | null,
    token_length_ptr: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_cell_decrypt_token_protect(
    master_key_ptr: number,
    master_key_length: number,
    context_ptr: number,
    context_length: number,
    message_ptr: number,
    message_length: number,
    token_ptr: number,
    token_length_ptr: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_message_encrypt(
    private_key_ptr: number,
    privateKey_length: number,
    public_key_ptr: number,
    publicKey_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_message_decrypt(
    private_key_ptr: number,
    privateKey_length: number,
    public_key_ptr: number,
    publicKey_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_message_sign(
    private_key_ptr: number,
    privateKey_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _themis_secure_message_verify(
    public_key_ptr: number,
    public_key_length: number,
    message_ptr: number,
    message_length: number,
    result_ptr: number | null,
    result_length_ptr: number
  ): ThemisErrorCode;

  _secure_session_create(
    session_id_ptr: number,
    sessionID_length: number,
    private_key_ptr: number,
    privateKey_length: number,
    callbacks_ptr: number
  ): number;

  _secure_session_destroy(sessionPtr: number): ThemisErrorCode;

  _secure_session_is_established(sessionPtr: ThemisErrorCode): number;

  _secure_comparator_get_result(comparatorPtr: number): ThemisErrorCode;

  _secure_session_generate_connect_request(
    sessionPtr: number,
    request_ptr: number | null,
    request_length_ptr: number
  ): ThemisErrorCode;

  _secure_session_unwrap(
    sessionPtr: number,
    message_ptr: number,
    message_length: number,
    reply_ptr: number | null,
    reply_length_ptr: number
  ): ThemisErrorCode;

  _secure_session_wrap(
    sessionPtr: number,
    message_ptr: number,
    message_length: number,
    wrapped_ptr: number | null,
    wrapped_length_ptr: number
  ): ThemisErrorCode;

  allocate: (
    slab: number[] | ArrayBufferView | number | ArrayBuffer,
    allocator: number,
    ptr?: number
  ) => number;
  ALLOC_STACK: typeof ALLOC_STACK;
  addFunction: typeof addFunction;
}

const context: { libthemis?: YourOwnEmscriptenModule } = {};

export default context;
