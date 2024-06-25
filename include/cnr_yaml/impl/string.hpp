#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__STRING_HPP
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__STRING_HPP

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include <Eigen/Core>

#include <cnr_yaml/type_traits.h>
#include <cnr_yaml/eigen.h>
#include <cnr_yaml/string.h>

namespace std
{

inline std::string to_string(const std::string& v)
{
  return v;
}

inline std::string to_string(const YAML::Node& v)
{
  std::stringstream ss;
  ss << v;
  return ss.str();
}

inline std::string to_string(const YAML::NodeType::value& v)
{
  switch(v)
  {
    case YAML::NodeType::Null: return "Null";
    case YAML::NodeType::Scalar: return "Scalar";
    case YAML::NodeType::Sequence: return "Sequence";
    case YAML::NodeType::Map: return "Map";
    case YAML::NodeType::Undefined: return "Undefined";
    default: return "Unknown";
  }
}

template<typename T, typename A>
inline std::string to_string(const std::vector<T, A>& v)
{
  std::string ret = "[ ";
  for (auto const& vi : v)
  {
    ret +=  to_string(vi) + " ";
  }
  return ret += "]";
}

template<typename T, std::size_t Nm>
inline std::string to_string(const std::array<T, Nm>& v)
{
  std::string ret = "[ ";
  for (auto const& vi : v)
  {
    ret +=  to_string(vi) + " ";
  }
  return ret += "]";
}

template <typename D>
inline std::string to_string(const Eigen::MatrixBase<D>& m)
{
  Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
  std::stringstream ss;
  ss << std::fixed << m.transpose().format(CleanFmt);
  return ss.str();
}

}  // namespace std

#endif // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__STRING_HPP