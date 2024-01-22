#include "tachyon/crypto/commitments/pedersen/pedersen.h"

#include "gtest/gtest.h"

#include "tachyon/base/buffer/vector_buffer.h"
#include "tachyon/math/elliptic_curves/bn/bn254/g1.h"

namespace tachyon::crypto {

namespace {

class PedersenTest : public testing::Test {
 public:
  constexpr static size_t kMaxSize = 3;

  using VCS = Pedersen<math::bn254::G1JacobianPoint, kMaxSize>;

  static void SetUpTestSuite() { math::bn254::G1Curve::Init(); }
};

}  // namespace

TEST_F(PedersenTest, CommitPedersen) {
  VCS vcs;
  ASSERT_TRUE(vcs.Setup());

  std::vector<math::bn254::Fr> v =
      base::CreateVector(kMaxSize, []() { return math::bn254::Fr::Random(); });

  math::bn254::Fr r = math::bn254::Fr::Random();
  math::bn254::G1JacobianPoint commitment;
  ASSERT_TRUE(vcs.Commit(v, r, &commitment));

  math::VariableBaseMSM<math::bn254::G1JacobianPoint> msm;
  math::bn254::G1JacobianPoint msm_result;
  ASSERT_TRUE(msm.Run(vcs.generators(), v, &msm_result));

  EXPECT_EQ(commitment, msm_result + r * vcs.h());
}

TEST_F(PedersenTest, BatchCommitPedersen) {
  VCS vcs;
  ASSERT_TRUE(vcs.Setup());

  size_t num_vectors = 10;

  std::vector<std::vector<math::bn254::Fr>> v_vec =
      base::CreateVector(num_vectors, []() {
        return base::CreateVector(kMaxSize,
                                  []() { return math::bn254::Fr::Random(); });
      });

  std::vector<math::bn254::Fr> r_vec = base::CreateVector(
      num_vectors, []() { return math::bn254::Fr::Random(); });

  vcs.SetBatchMode(num_vectors);
  for (size_t i = 0; i < num_vectors; ++i) {
    ASSERT_TRUE(vcs.Commit(v_vec[i], r_vec[i], i));
  }
  std::vector<math::bn254::G1JacobianPoint> batch_commitments =
      vcs.GetBatchCommitments();
  EXPECT_EQ(vcs.batch_commitment_state().batch_mode, false);
  EXPECT_EQ(vcs.batch_commitment_state().batch_count, size_t{0});

  math::VariableBaseMSM<math::bn254::G1JacobianPoint> msm;
  std::vector<math::bn254::G1JacobianPoint> msm_results;
  msm_results.resize(num_vectors);
  for (size_t i = 0; i < num_vectors; ++i) {
    ASSERT_TRUE(msm.Run(vcs.generators(), v_vec[i], &msm_results[i]));
  }

  EXPECT_EQ(batch_commitments, msm_results);
}

TEST_F(PedersenTest, Copyable) {
  VCS expected;
  ASSERT_TRUE(expected.Setup());

  base::Uint8VectorBuffer write_buf;
  ASSERT_TRUE(write_buf.Write(expected));

  write_buf.set_buffer_offset(0);

  VCS value;
  ASSERT_TRUE(write_buf.Read(&value));

  EXPECT_EQ(expected.h(), value.h());
  EXPECT_EQ(expected.generators(), value.generators());
}

}  // namespace tachyon::crypto
