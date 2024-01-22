#ifndef VENDORS_HALO2_INCLUDE_BN254_SHPLONK_PROVING_KEY_H_
#define VENDORS_HALO2_INCLUDE_BN254_SHPLONK_PROVING_KEY_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include "rust/cxx.h"

namespace tachyon::halo2_api::bn254 {

class SHPlonkProvingKey {
 public:
  explicit SHPlonkProvingKey(rust::Slice<const uint8_t> pk_bytes);

  rust::Slice<const uint8_t> advice_column_phases() const;
  size_t blinding_factors() const;
  rust::Slice<const uint8_t> challenge_phases() const;
  rust::Vec<size_t> constants() const;
  size_t num_advice_columns() const;
  size_t num_challenges() const;
  size_t num_instance_columns() const;
  rust::Vec<uint8_t> phases() const;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

std::unique_ptr<SHPlonkProvingKey> new_proving_key(
    rust::Slice<const uint8_t> pk_bytes);

}  // namespace tachyon::halo2_api::bn254

#endif  // VENDORS_HALO2_INCLUDE_BN254_SHPLONK_PROVING_KEY_H_
