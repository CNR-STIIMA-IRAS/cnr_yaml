#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__EIGEN__H
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__EIGEN__H

#include <vector>
#include <Eigen/Core>
#include <type_traits>
#include <yaml-cpp/node/convert.h>

namespace cnr
{
namespace yaml
{

bool copy(double& lhs, const double& rhs);

bool copy(double& lhs, const std::vector<double>& rhs);

bool copy(std::vector<double>& lhs, const double& rhs);

template <typename D>
bool copy(double& lhs, const Eigen::MatrixBase<D>& rhs);

template <typename D, typename E>
bool copy(Eigen::MatrixBase<D>& lhs, const Eigen::MatrixBase<E>& rhs);

template <typename D>
bool copy(Eigen::MatrixBase<D>& lhs, const double& rhs);

template <typename D>
bool copy(Eigen::MatrixBase<D>& lhs, const std::vector<double>& rhs);

template <typename D>
bool copy(std::vector<double>& lhs, const Eigen::MatrixBase<D>& rhs);

//! resize
template <typename D, typename std::enable_if<(Eigen::MatrixBase<D>::RowsAtCompileTime == Eigen::Dynamic) ||
                                                  (Eigen::MatrixBase<D>::ColsAtCompileTime == Eigen::Dynamic),
                                              int>::type = 0>
bool resize(Eigen::MatrixBase<D> const& m, int rows, int cols = 1);

template <typename D, typename std::enable_if<(Eigen::MatrixBase<D>::RowsAtCompileTime != Eigen::Dynamic) &&
                                                  (Eigen::MatrixBase<D>::ColsAtCompileTime != Eigen::Dynamic),
                                              int>::type = 0>
bool resize(Eigen::MatrixBase<D> const& m, int rows, int cols = 1);

bool resize(const double& m, int rows, int cols = 1);

template <typename T>
bool resize(std::vector<T>& m, int rows, int cols = 1);

template <typename T>
bool resize(std::vector<T>& m1, const std::vector<T>& m2);

template <typename T>
bool resize(T& /*m1*/, const T& /*m2*/);

}  // namespace yaml
}  // namespace cnr

#include <iostream>

namespace YAML
{
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
struct convert<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>>
{
  using Mat = Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>;

  static Node encode(const Mat& rhs)
  {
    Node ret(NodeType::Sequence);
    constexpr bool should_be_a_vector = (_Rows == 1 || _Cols == 1);

    try
    {
      if constexpr (should_be_a_vector)
      {
        std::vector<_Scalar> vv(rhs.data(), rhs.data() + rhs.rows() * rhs.cols());

        for (const auto& v : vv)
        {
          ret.push_back(v);
        }
        return ret;
      }
      else  // matrix expected
      {
        for (int ir = 0; ir < _Rows; ir++)
        {
          Node row(NodeType::Sequence);
          for (int ic = 0; ic < _Cols; ic++)
          {
            row.push_back(rhs(ir, ic));
          }
          ret.push_back(row);
        }
      }
    }
    catch (std::exception& e)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": Exception! Input:\n"
                << rhs << "\nWhat: " << e.what() << std::endl;
      return Node(NodeType::Null);
    }
    catch (...)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": Unhandled Exception! Input:\n" << rhs << std::endl;
      return Node(NodeType::Null);
    }
    return ret;
  }

  static bool decode(const Node& node, Mat const& rhs)
  {
    if (!node.IsSequence())
    {
      return false;
    }

    constexpr bool should_be_a_vector = (Mat::RowsAtCompileTime == 1 || Mat::ColsAtCompileTime == 1);

    Mat& _rhs = const_cast<Mat&>(rhs);
    try
    {
      if constexpr (should_be_a_vector)
      {
        std::vector<_Scalar> vv = node.as<std::vector<_Scalar>>();

        int dim = static_cast<int>(vv.size());
        int rows = Mat::RowsAtCompileTime == Eigen::Dynamic ? dim : Mat::RowsAtCompileTime;
        int cols = Mat::ColsAtCompileTime == Eigen::Dynamic && Mat::RowsAtCompileTime != Eigen::Dynamic ?
                       dim :
                       Mat::ColsAtCompileTime;

        if (!cnr::yaml::resize(_rhs, rows, cols))
        {
          std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": It was expected a Vector (" << _Rows << "x" << _Cols
                    << ") while the param store a " << dim << "-vector. Input Node:\n"
                    << node;
          return false;
        }
        for (int i = 0; i < static_cast<int>(vv.size()); i++)
          _rhs(i) = vv.at(static_cast<int>(i));
      }
      else  // matrix expected
      {
        std::vector<std::vector<double>> vv;
        if (node[0].IsScalar())
        {
          auto _vv = node.as<std::vector<double>>();
          for (const auto& _v : _vv)
            vv.push_back({ _v });
        }
        else
        {
          vv = node.as<std::vector<std::vector<double>>>();
        }

        int rows = static_cast<int>(vv.size());
        int cols = static_cast<int>(vv.front().size());
        if (!cnr::yaml::resize(_rhs, rows, cols))
        {
          std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": It was expected a Vector (" << _Rows << "x" << _Cols
                    << ") while the param store a Vector (" << rows << "x" << cols << "). Input Node:\n"
                    << node;
          return false;
        }
        for (int i = 0; i < rows; i++)
          for (int j = 0; j < cols; j++)
            _rhs(i, j) = vv.at(static_cast<int>(i)).at(static_cast<int>(j));
      }
    }
    catch (std::exception& e)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": Exception!\nInput node:\n"
                << node << "\nWhat: " << e.what() << std::endl;
      return false;
    }
    catch (...)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": Unhandled Exception! Input node:\n"
                << node << std::endl;
      return false;
    }
    return true;
  }
};
}  // namespace YAML

#include <cnr_yaml/impl/eigen_impl.hpp>

#endif  // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__EIGEN__H
