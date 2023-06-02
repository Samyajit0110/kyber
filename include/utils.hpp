#pragma once
#include "params.hpp"
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>

// IND-CPA-secure Public Key Encryption Scheme Utilities
namespace kyber_utils {

// Given a bytearray of length N, this function converts it to human readable
// hex string of length N << 1 | N >= 0
inline const std::string
to_hex(const uint8_t* const bytes, const size_t len)
{
  std::stringstream ss;
  ss << std::hex;

  for (size_t i = 0; i < len; i++) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(bytes[i]);
  }

  return ss.str();
}

// Compile-time compute IND-CCA-secure Kyber KEM public key length ( in bytes )
template<const size_t k>
static inline constexpr size_t
get_kem_public_key_len()
  requires(kyber_params::check_k(k))
{
  return k * 12 * 32 + 32;
}

// Compile-time compute IND-CCA-secure Kyber KEM secret key length ( in bytes )
template<const size_t k>
static inline constexpr size_t
get_kem_secret_key_len()
  requires(kyber_params::check_k(k))
{
  constexpr size_t t0 = k * 12 * 32;
  constexpr size_t t1 = get_kem_public_key_len<k>();

  return t0 + t1 + 32 + 32;
}

// Compile-time compute IND-CCA-secure Kyber KEM cipher text length ( in bytes )
template<const size_t k, const size_t du, const size_t dv>
static inline constexpr size_t
get_kem_cipher_len()
  requires(kyber_params::check_decrypt_params(k, du, dv))
{
  return k * du * 32 + dv * 32;
}

}
