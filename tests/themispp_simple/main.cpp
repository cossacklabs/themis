#include <themispp/secure_keygen.hpp>

int main()
{
    std::vector<uint8_t> key = themispp::gen_sym_key();
    if (key.empty()) {
        return 1;
    }
    return 0;
}
