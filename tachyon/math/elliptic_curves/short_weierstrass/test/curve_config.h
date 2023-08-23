#ifndef TACHYON_MATH_ELLIPTIC_CURVES_SHORT_WEIERSTRASS_TEST_CURVE_CONFIG_H_
#define TACHYON_MATH_ELLIPTIC_CURVES_SHORT_WEIERSTRASS_TEST_CURVE_CONFIG_H_

#include "tachyon/math/elliptic_curves/affine_point.h"
#include "tachyon/math/elliptic_curves/curve_config.h"
#include "tachyon/math/elliptic_curves/jacobian_point.h"
#include "tachyon/math/elliptic_curves/point_xyzz.h"
#include "tachyon/math/elliptic_curves/projective_point.h"
#include "tachyon/math/elliptic_curves/short_weierstrass/sw_curve.h"
#include "tachyon/math/finite_fields/test/gf7.h"
#include "tachyon/math/geometry/point2.h"

namespace tachyon::math {
namespace test {

template <typename _BaseField, typename _ScalarField>
class CurveConfig {
 public:
  using BaseField = _BaseField;
  using ScalarField = _ScalarField;

  // A: Mont(0)
  constexpr static BigInt<1> kA = BigInt<1>(0);
  // B: Mont(5)
  constexpr static BigInt<1> kB = BigInt<1>(3);
  // Generator: (Mont(5), Mont(5))
  constexpr static Point2<BigInt<1>> kGenerator =
      Point2<BigInt<1>>(BigInt<1>(3), BigInt<1>(3));
};

using AffinePoint = math::AffinePoint<SWCurve<CurveConfig<GF7, GF7>>>;
using ProjectivePoint = math::ProjectivePoint<SWCurve<CurveConfig<GF7, GF7>>>;
using JacobianPoint = math::JacobianPoint<SWCurve<CurveConfig<GF7, GF7>>>;
using PointXYZZ = math::PointXYZZ<SWCurve<CurveConfig<GF7, GF7>>>;
#if defined(TACHYON_GMP_BACKEND)
using AffinePointGmp = math::AffinePoint<SWCurve<CurveConfig<GF7Gmp, GF7Gmp>>>;
using ProjectivePointGmp =
    math::ProjectivePoint<SWCurve<CurveConfig<GF7Gmp, GF7Gmp>>>;
using JacobianPointGmp =
    math::JacobianPoint<SWCurve<CurveConfig<GF7Gmp, GF7Gmp>>>;
using PointXYZZGmp = math::PointXYZZ<SWCurve<CurveConfig<GF7Gmp, GF7Gmp>>>;
#endif  // defined(TACHYON_GMP_BACKEND)

}  // namespace test
}  // namespace tachyon::math

#endif  // TACHYON_MATH_ELLIPTIC_CURVES_SHORT_WEIERSTRASS_TEST_CURVE_CONFIG_H_