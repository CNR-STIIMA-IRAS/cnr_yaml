#ifndef CNR_YAML_UTILITIES_INCLUDE_CNR_YAML_UTILITIES_IMPL_EIGEN_IMPL
#define CNR_YAML_UTILITIES_INCLUDE_CNR_YAML_UTILITIES_IMPL_EIGEN_IMPL

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#if !defined(UNUSED)
#define UNUSED(expr)                                                                                                   \
  do                                                                                                                   \
  {                                                                                                                    \
    (void)(expr);                                                                                                      \
  } while (0)
#endif

#include <iostream>
#include <yaml-cpp/node/node.h>
#include <cnr_yaml/eigen.h>

namespace cnr
{
namespace yaml
{

/**
 * RESIZE - SAFE FUNCTION CALLED ONLY IF THE MATRIX IS DYNAMICALLY CREATED AT RUNTIME
 */
template <typename Derived,
          typename std::enable_if<(Eigen::MatrixBase<Derived>::RowsAtCompileTime == Eigen::Dynamic) ||
                                      (Eigen::MatrixBase<Derived>::ColsAtCompileTime == Eigen::Dynamic),
                                  int>::type>
inline bool resize(Eigen::MatrixBase<Derived> const& m, int rows, int cols)
{
  Eigen::MatrixBase<Derived>& mat = const_cast<Eigen::MatrixBase<Derived>&>(m);
  if constexpr((Eigen::MatrixBase<Derived>::RowsAtCompileTime == Eigen::Dynamic) &&
      (Eigen::MatrixBase<Derived>::ColsAtCompileTime == Eigen::Dynamic))
  {
    mat.derived().resize(rows, cols);
  }
  else if constexpr(Eigen::MatrixBase<Derived>::RowsAtCompileTime == Eigen::Dynamic)
  {
    mat.derived().resize(rows, Eigen::NoChange);
  }
  else if constexpr(Eigen::MatrixBase<Derived>::ColsAtCompileTime == Eigen::Dynamic)
  {
    mat.derived().resize(Eigen::NoChange, cols);
  }
  return (mat.derived().rows() == rows) && (mat.derived().cols() == cols);
}

/**
 * RESIZE - SAFE FUNCTION CALLED ONLY IF THE MATRIX IS DYNAMICALLY CREATED AT RUNTIME
 */
template <typename Derived,
          typename std::enable_if<(Eigen::MatrixBase<Derived>::RowsAtCompileTime != Eigen::Dynamic) &&
                                      (Eigen::MatrixBase<Derived>::ColsAtCompileTime != Eigen::Dynamic),
                                  int>::type>
inline bool resize(Eigen::MatrixBase<Derived> const& /*m*/, int rows, int cols)
{
  return Eigen::MatrixBase<Derived>::RowsAtCompileTime == rows && Eigen::MatrixBase<Derived>::ColsAtCompileTime == cols;
}

/**
 * RESIZE - SAFE FUNCTION CALLED ONLY IF THE MATRIX IS DYNAMICALLY CREATED AT RUNTIME
 */
inline bool resize(const double& m, int rows, int cols)
{
  UNUSED(m);
  return rows == 1 && cols == 1;
}

template <typename T>
inline bool resize(std::vector<T>& m, int rows, int cols)
{
  if (!rows || (cols != 1))
  {
    return false;
  }
  m.resize(rows);
  return true;
}

template <typename T>
inline bool resize(std::vector<T>& m1, const std::vector<T>& m2)
{
  m1.resize(m2.size());
  return true;
}

template <typename T>
inline bool resize(T& /*m1*/, const T& /*m2*/)
{
  return true;
}

/**
 * RESIZE - SAFE FUNCTION CALLED ONLY IF THE MATRIX IS DYNAMICALLY CREATED AT RUNTIME
 */
template <typename Derived,
          typename std::enable_if<(Eigen::MatrixBase<Derived>::RowsAtCompileTime == Eigen::Dynamic) ||
                                      (Eigen::MatrixBase<Derived>::ColsAtCompileTime == Eigen::Dynamic),
                                  int>::type>
inline bool checkInputDim(const std::string& id, const Eigen::MatrixBase<Derived>& m, int rows, int cols,
                          std::string& error)
{
  if ((m.rows() != rows) || (m.cols() != cols))
  {
    error += std::string(__PRETTY_FUNCTION__) + ":" + std::to_string(__LINE__) + ":\n";
    error += id +
             " dimension mismatch."
             " Object size: " +
             std::to_string(m.rows()) + "x" + std::to_string(m.cols()) +
             ","
             " Expected size: " +
             std::to_string(rows) + "x" + std::to_string(cols) + "";
    return false;
  }
  return true;
}

/**
 * CHECK - SKIP, SINCE THE DIMENSION IS CHECKED AT COMPILE TIME
 */
template <typename Derived,
          typename std::enable_if<(Eigen::MatrixBase<Derived>::RowsAtCompileTime != Eigen::Dynamic) &&
                                      (Eigen::MatrixBase<Derived>::ColsAtCompileTime != Eigen::Dynamic),
                                  int>::type>
inline bool checkInputDim(const std::string& id, const Eigen::MatrixBase<Derived>& m, int rows, int cols,
                          std::string& error)
{
  UNUSED(id);
  UNUSED(m);
  UNUSED(error);
  return Eigen::MatrixBase<Derived>::RowsAtCompileTime == rows && Eigen::MatrixBase<Derived>::ColsAtCompileTime == cols;
}

inline bool checkInputDim(const std::string& id, const double& m, int rows, int cols, std::string& error)
{
  UNUSED(error);
  UNUSED(m);
  UNUSED(id);
  return rows == 1 && cols == 1;
}

template <typename Derived>
inline void checkInputDimAndThrowEx(const std::string& id, const Eigen::MatrixBase<Derived>& m, int rows, int cols)
{
  std::string error = std::string(__PRETTY_FUNCTION__) + ":" + std::to_string(__LINE__) + ":\n";
  if (!checkInputDim(id, m, rows, cols, error))
  {
    throw std::runtime_error(error.c_str());
  }
}

inline void checkInputDimAndThrowEx(const std::string& id, const double& m, int rows, int cols)
{
  UNUSED(m);
  if (rows != 1 && cols != 1)
  {
    throw std::runtime_error((id + ": expected a 1x1 (double) while a matrix has been assigned").c_str());
  }
}

// double, double
inline bool copy(double& lhs, const double& rhs)
{
  lhs = rhs;
  return true;
}

// double, double
inline bool copy(double& lhs, const std::vector<double>& rhs)
{
  if (rhs.size() != 1)
    return false;
  lhs = rhs.front();
  return true;
}

// double, double
inline bool copy(std::vector<double>& lhs, const double& rhs)
{
  if (lhs.size() != 1)
    return false;
  lhs.front() = rhs;
  return true;
}

// double, matrix
template <typename Derived>
inline bool copy(double& lhs, const Eigen::MatrixBase<Derived>& rhs)
{
  if (rhs.rows() != 1 || rhs.cols() != 1)
  {
    return false;
  }
  lhs = rhs(0, 0);
  return true;
}

// matrix, matrix
template <typename Derived, typename OtherDerived>
inline bool copy(Eigen::MatrixBase<Derived>& lhs, const Eigen::MatrixBase<OtherDerived>& rhs)
{
  if (!cnr::yaml::resize(lhs, rhs.rows(), rhs.cols()))
    return false;
  lhs = rhs;
  return true;
}

// matrix, double
template <typename Derived>
inline bool copy(Eigen::MatrixBase<Derived>& lhs, const double& rhs)
{
  lhs.setConstant(rhs);
  return true;
}

// matrix, double
template <typename Derived>
inline bool copy(Eigen::MatrixBase<Derived>& lhs, const std::vector<double>& rhs)
{
  if (!cnr::yaml::resize(lhs, int(rhs.size()), 1))
    return false;
  for (int i = 0; i < lhs.rows(); i++)
    lhs(i) = rhs.at(size_t(i));
  return true;
}

// matrix, double
template <typename Derived>
inline bool copy(std::vector<double>& lhs, const Eigen::MatrixBase<Derived>& rhs)
{
  if (int(lhs.size()) != rhs.rows())
  {
    lhs.resize(rhs.rows());
  }
  for (int i = 0; i < rhs.rows(); i++)
    lhs.at(size_t(i)) = rhs(i);
  return true;
}

inline double* data(double& v)
{
  return &v;
}

template <typename Derived>
inline typename Derived::Scalar* data(Eigen::MatrixBase<Derived>& m)
{
  Eigen::Ref<Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime>> _m(m);
  return _m.data();
}

inline const double* data(const double& v)
{
  return &v;
}

template <typename Derived>
inline const typename Derived::Scalar* data(const Eigen::MatrixBase<Derived>& m)
{
  return m.derived().data();
}

inline double norm(const double& m)
{
  return std::fabs(m);
}

template <typename Derived>
inline double norm(const Eigen::MatrixBase<Derived>& m)
{
  return m.norm();
}

inline double normalized(const double& m)
{
  UNUSED(m);
  return 1.0;
}

template <typename Derived>
inline Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime>
normalized(const Eigen::MatrixBase<Derived>& m)
{
  Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> ret;
  ret = m.normalized();
  return ret;
}

// template <typename T>
// inline int size(const T& m)
// {
//   UNUSED(m);
//   return 1;
// }

// template <typename Derived>
// inline int size(const Eigen::MatrixBase<Derived>& m)
// {
//   return m.size();
// }

// template <typename Derived>
// inline int size(const std::vector<Derived>& m)
// {
//   return m.size();
// }

// template <typename Derived>
// inline int size(const std::vector<std::vector<Derived>>& m)
// {
//   return m.size() * m.front().size();
// }

// template <typename T>
// inline int rows(const T& m)
// {
//   UNUSED(m);
//   return 1;
// }

// template <typename T>
// inline int rows(const std::vector<T>& m)
// {
//   return static_cast<int>(m.size());
// }

// template <typename T>
// inline int rows(const std::vector<std::vector<T>>& m)
// {
//   return static_cast<int>(m.size());
// }

// template <typename Derived>
// inline int rows(const Eigen::MatrixBase<Derived>& m)
// {
//   return m.rows();
// }

inline int rank(const double& m)
{
  UNUSED(m);
  return 1;
}

template <typename Derived>
inline int rank(const Eigen::MatrixBase<Derived>& m)
{
  Eigen::FullPivLU<Derived> lu_decomp(m);
  return lu_decomp.rank();
}

template <typename T>
inline int cols(const T& m)
{
  UNUSED(m);
  return 1;
}

template <typename T>
inline int cols(const std::vector<T>& m)
{
  return static_cast<int>(m.size());
}

template <typename T>
inline int cols(const std::vector<std::vector<T>>& m)
{
  return static_cast<int>(m.front().size());
}

template <typename Derived>
inline int cols(const Eigen::MatrixBase<Derived>& m)
{
  return m.cols();
}

inline void setConstant(double& m, const double& v)
{
  m = v;
}

template <typename Derived>
inline void setConstant(Eigen::MatrixBase<Derived>& m, const double& v)
{
  m.setConstant(v);
}

inline void setDiagonal(double& m, const double& v)
{
  m = v;
}

inline void setDiagonal(double& m, const std::vector<double>& v)
{
  if (v.size() != 1)
    throw std::runtime_error("setDiagonal failed! The input vector has the size greater than 1!");
  m = v.front();
}

template <typename D>
inline void setDiagonal(double& m, const Eigen::MatrixBase<D>& v)
{
  if (v.size() != 1)
    throw std::runtime_error("setDiagonal failed! The input vector has the size greater than 1!");
  m = v(0, 0);
}

template <typename Derived>
inline void setDiagonal(Eigen::MatrixBase<Derived>& m, const double& v)
{
  if (rows(m) != cols(m))
    throw std::runtime_error("The input matrix is not squared!");
  for (int i = 0; i < rows(m); i++)
    m(i, i) = v;
}

template <typename D>
inline void setDiagonal(Eigen::MatrixBase<D>& m, const std::vector<double>& v)
{
  if (static_cast<int>(v.size()) != std::min(m.rows(), m.cols()))
    throw std::runtime_error("setDiagonal failed! The input vector has the size greater than the matrix!");
  for (size_t i = 0; i < v.size(); i++)
  {
    m(static_cast<int>(i), static_cast<int>(i)) = v.at(i);
  }
}

inline void saturate(double& v, const double& min, const double& max)
{
  v = std::max(std::min(v, max), min);
}

template <typename Derived>
inline void saturate(Eigen::MatrixBase<Derived>& m, const double& min, const double& max)
{
  for (int i = 0; i < m.rows(); i++)
  {
    for (int j = 0; j < m.cols(); j++)
    {
      m(i, j) = std::max(std::min(m(i, j), max), min);
    }
  }
}

template <typename Derived>
inline void saturate(Eigen::MatrixBase<Derived>& m, const Eigen::MatrixBase<Derived>& min,
                     const Eigen::MatrixBase<Derived>& max)
{
  assert(m.rows() == min.rows());
  assert(m.rows() == max.rows());
  assert(m.cols() == min.cols());
  assert(m.cols() == max.cols());

  for (int i = 0; i < m.rows(); i++)
  {
    for (int j = 0; j < m.cols(); j++)
    {
      m(i, j) = std::max(std::min(m(i, j), max(i, j)), min(i, j));
    }
  }
}

inline double dot(const double& m1, const double& m2)
{
  return m1 * m2;
}

template <typename Derived, typename OtherDerived>
inline double dot(const Eigen::MatrixBase<Derived>& m1, const Eigen::MatrixBase<OtherDerived>& m2)
{
  return m1.dot(m2);
}

//============
template <typename MatType>
using PseudoInverseType =
    Eigen::Matrix<typename MatType::Scalar, MatType::ColsAtCompileTime, MatType::RowsAtCompileTime>;

template <typename MatType>
PseudoInverseType<MatType> pseudoInverse(const MatType& a, double epsilon)
{
  using WorkingMatType = Eigen::Matrix<typename MatType::Scalar, Eigen::Dynamic, Eigen::Dynamic, 0,
                                       MatType::MaxRowsAtCompileTime, MatType::MaxColsAtCompileTime>;
  Eigen::BDCSVD<WorkingMatType> svd(a, Eigen::ComputeThinU | Eigen::ComputeThinV);
  svd.setThreshold(epsilon * std::max(a.cols(), a.rows()));
  Eigen::Index rank = svd.rank();
  Eigen::Matrix<typename MatType::Scalar, Eigen::Dynamic, MatType::RowsAtCompileTime, 0,
                Eigen::BDCSVD<WorkingMatType>::MaxDiagSizeAtCompileTime, MatType::MaxRowsAtCompileTime>
      tmp = svd.matrixU().leftCols(rank).adjoint();
  tmp = svd.singularValues().head(rank).asDiagonal().inverse() * tmp;
  return svd.matrixV().leftCols(rank) * tmp;
}

inline double div(const double& a, const double b)
{
  return a / b;
}

template <typename AType, typename BType>  //  C = B^-1 x A  (bc x ac ) = (bc x br)  x (ar x ac )
using DivType = Eigen::Matrix<typename AType::Scalar, BType::ColsAtCompileTime, AType::ColsAtCompileTime>;

template <typename AType, typename BType>
inline DivType<AType, BType> div(const AType& a, const BType& b)
{
  DivType<AType, BType> ret;
  PseudoInverseType<BType> binv = pseudoInverse(b);
  ret = binv * a;
  return ret;
}

}  // namespace yaml
}  // namespace cnr


#endif /* INCLUDE_CNR_YAML_UTILITIES_IMPL_EIGEN_IMPL */
