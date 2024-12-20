#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_SEQUENCE__HPP
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_SEQUENCE__HPP

#include <boost/type_index.hpp>
#include <yaml-cpp/yaml.h>
#include <Eigen/Core>

#include <cnr_yaml/type_traits.h>
#include <cnr_yaml/eigen.h>
#include <cnr_yaml/string.h>
#include <cnr_yaml/impl/cnr_yaml.hpp>

#if !defined(UNUSED)
#define UNUSED(expr)                                                                                                   \
  do                                                                                                                   \
  {                                                                                                                    \
    (void)(expr);                                                                                                      \
  } while (0)
#endif

#if !defined(CATCH)
#define CATCH(X)                                                                                                       \
  catch (YAML::Exception & e)                                                                                          \
  {                                                                                                                    \
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": "                                                        \
              << "YAML Exception, Error in the extraction of an object of type '"                                      \
              << boost::typeindex::type_id_with_cvr<decltype(X)>().pretty_name() << "'" << std::endl                   \
              << "Node: " << std::endl                                                                                 \
              << node << std::endl                                                                                     \
              << "What: " << std::endl                                                                                 \
              << e.what() << std::endl;                                                                                \
  }                                                                                                                    \
  catch (std::exception & e)                                                                                           \
  {                                                                                                                    \
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": "                                                        \
              << "Exception, Error in the extraction of an object of type '"                                           \
              << boost::typeindex::type_id_with_cvr<decltype(X)>().pretty_name() << "'" << std::endl                   \
              << "Node: " << std::endl                                                                                 \
              << node << std::endl                                                                                     \
              << "What: " << std::endl                                                                                 \
              << e.what() << std::endl;                                                                                \
  }
#endif

namespace cnr
{
namespace yaml
{

// =============================================================================================
// GET SEQUENCE
// =============================================================================================
// ffwd declarations
template <typename Derived>
bool _get_sequence_eigen(const YAML::Node& node, Eigen::MatrixBase<Derived> const& ret, std::string& what, const bool& implicit_cast_if_possible);

template <typename T, typename A>
bool _get_sequence(const YAML::Node& node, std::vector<T, A>& ret, std::string& what, const bool& implicit_cast_if_possible);

template <typename T, typename A>
bool _get_sequence(const YAML::Node& node, std::vector<std::vector<T, A>>& ret, std::string& what, const bool& implicit_cast_if_possible);

template <typename T, std::size_t N>
bool _get_sequence(const YAML::Node& node, std::array<T, N>& ret, std::string& what, const bool& implicit_cast_if_possible);

template <typename T, std::size_t N, std::size_t M>
bool _get_sequence(const YAML::Node& node, std::array<std::array<T, M>, N>& ret, std::string& what, const bool& implicit_cast_if_possible);

/**
 * @brief Generic template function
 *
 * @tparam T
 * @tparam A
 * @param node
 * @param ret
 * @param what
 * @return true
 * @return false
 */
template <typename T, typename A>
inline bool _get_sequence(const YAML::Node& node, std::vector<T, A>& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  if (!node.IsSequence())
  {
    what = "Tried to extract a '" + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
           "' but the node is " + std::to_string(node.Type()) +
           "\n>> Input Node:\n" + std::to_string(node);

    return false;
  }

  try
  {
    ret.clear();
    for (std::size_t i = 0; i < node.size(); i++)
    {
      T v = T();
      if (!decode<T, 0, std::variant_size<typename decoding_type_variant_holder<T>::variant>::value>(node[i], v, what, implicit_cast_if_possible))
      {
        what = "Error in the extraction of the element #" + std::to_string(i)
               + ": " + what + "\nInput Node:" + std::to_string(node);
        return false;
      }
      ret.push_back(v);
    }
    return true;
  }
  CATCH(ret);

  return false;
}

template <typename T, typename A>
inline bool _get_sequence(const YAML::Node& node, std::vector<std::vector<T, A>>& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  bool ok = false;
  YAML::Node config(node);
  if (!config.IsSequence())
  {
    std::stringstream _node;
    _node << node;
    what = "Tried to extract a '" + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
           "' but the node is " + std::to_string(config.Type()) + "\n Input Node:\n" + _node.str();
  }
  else
  {
    try
    {
      ret.clear();
      for (std::size_t i = 0; i < node.size(); i++)
      {
        if (!config[i].IsSequence())
        {
          std::stringstream _node;
          _node << config;
          what = "Tried to extract a '" + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
                  "' but the node is " + std::to_string(config.Type()) + "\n Input Node:\n" + _node.str();
        }
        std::vector<T> v;
        ok = _get_sequence(config[i], v, what, implicit_cast_if_possible);
        if (!ok)
        {
          std::stringstream _node;
          _node << node;
          what = "Error in the extraction of the element #" + std::to_string(i)
                 + ". Type of the sequence: " + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
                    "' but the node is " + std::to_string(config.Type()) + "\n Input Node:\n" + _node.str();
          break;
        }
        ret.push_back(v);
      }
    }
    CATCH(ret);
  }
  return ok;
}

template <typename T, std::size_t N>
inline bool _get_sequence(const YAML::Node& node, std::array<T, N>& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  bool ok = false;
  try
  {
    std::vector<T> tmp;
    ok = _get_sequence(node, tmp, what, implicit_cast_if_possible);
    if(tmp.size() != N)
    {
      std::stringstream _node;
      _node << node;
      what = "Tried to extract a '" + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
             "'  but there is a size mismathc between the expected and the got one.\n Input Node:\n" + _node.str();
      ok = false;
    }
    else if(ok)
    {
      std::copy_n(tmp.begin(), N, ret.begin());
    }
  }
  CATCH(ret);

  return ok;
}

template <typename T, std::size_t N, std::size_t M>
inline bool _get_sequence(const YAML::Node& node, std::array<std::array<T, M>, N>& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  bool ok = false;
  try
  {
    std::vector<std::vector<T>> tmp;
    ok = _get_sequence(node, tmp, what, implicit_cast_if_possible);
    if ((tmp.size() != N) || (tmp.front().size() != M))
    {
      std::stringstream _node;
      _node << node;
      what = "Tried to extract a '" + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
             "' ?error in size? \n Input Node:\n" + _node.str();
      ok = false;
    }
    else if(ok)
    {
      for (std::size_t i = 0; i < N; i++)
      {
        std::copy_n(tmp.at(i).begin(), M, ret.at(i).begin());
      }
    }
  }
  CATCH(ret);

  return ok;
}

template <typename Derived>
inline bool _get_sequence_eigen(const YAML::Node& node, Eigen::MatrixBase<Derived> const& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  bool ok = false;
  Eigen::MatrixBase<Derived>& _ret = const_cast<Eigen::MatrixBase<Derived>&>(ret);
  YAML::Node config(node);
  std::stringstream _node;
  _node << node;

  int expected_rows = Eigen::MatrixBase<Derived>::RowsAtCompileTime;
  int expected_cols = Eigen::MatrixBase<Derived>::ColsAtCompileTime;
  bool should_be_a_vector = (expected_rows == 1 || expected_cols == 1);

  try
  {
    if (should_be_a_vector)
    {
      std::vector<double> vv;
      ok = _get_sequence(config, vv, what, implicit_cast_if_possible);
      if (!ok)
      {
        what = "Failed in extracting a vector from the Node " + _node.str();
        return false;
      }

      int dim = static_cast<int>(vv.size());
      if (!resize(_ret, (expected_rows == 1 ? 1 : dim), (expected_rows == 1 ? dim : 1)))
      {
        what = "It was expected a Vector (" + std::to_string(expected_rows) + "x" + std::to_string(expected_cols) +
               ") while the param store a " + std::to_string(dim) + "-vector. Input Node:\n" + _node.str();
        return false;
      }
      for (int i = 0; i < dim; i++)
      {
        _ret(i) = vv.at(static_cast<std::size_t>(i));
      }
    }
    else  // matrix expected
    {
      std::vector<std::vector<double>> vv;
      ok = _get_sequence(node, vv, what, implicit_cast_if_possible);
      if (!ok)
      {
        what = "Failed in extracting a vector from the Node " + _node.str();
        return false;
      }

      int rows = static_cast<int>(vv.size());
      int cols = static_cast<int>(vv.front().size());
      if (!resize(_ret, rows, cols))
      {
        what = "It was expected a Vector (" + std::to_string(expected_rows) + "x" + std::to_string(expected_cols) +
               ") while the param store a Vector (" + std::to_string(rows) + "x" + std::to_string(cols) + "). Input Node:\n" + _node.str();
        return false;
      }
      for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
          _ret(i, j) = vv.at(static_cast<int>(i)).at(static_cast<int>(j));
    }
    ok = true;
  }
  catch (std::exception& e)
  {
    what = ("Got an exception! Input node:\n" + _node.str() + "\nWhat: ") + e.what();
    ok = false;
  }
  catch (...)
  {
    what = "Got an unhandled exception! Input node:\n" + _node.str();
    ok = false;
  }
  return ok;
}

template <typename T>
inline bool _get_sequence(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  if constexpr (is_eigen_matrix<T>::value)
  {
    return _get_sequence_eigen(node, ret, what, implicit_cast_if_possible);
  }
  UNUSED(node);
  UNUSED(ret);

  std::stringstream _node;
  _node << node;
  what = "The type ' " + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() +
         "' is not supported. You must specilized your own 'get_sequence' template function\n Input Node: " + _node.str();

  return false;
}

// /**
//  * @brief Get the sequence object
//  *
//  * @tparam T
//  * @param node
//  * @param ret
//  * @param what
//  * @return true
//  * @return false
//  */
// template <typename T>
// inline bool get_sequence(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
// {
//   return _get_sequence(node, ret, what, implicit_cast_if_possible);
// }
// =============================================================================================
// END GET SEQUENCE
// =============================================================================================


}  // namespace yaml
}  // namespace cnr

#endif  // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_SEQUENCE__HPP