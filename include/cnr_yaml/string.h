#ifndef CNR_YAML_UTILITIES_INCLUDE_CNR_YAML_UTILITIES_STRING
#define CNR_YAML_UTILITIES_INCLUDE_CNR_YAML_UTILITIES_STRING

#include <vector>
#include <string>
#include <Eigen/Core>
#include <yaml-cpp/node/node.h>

namespace std
{

std::string to_string(const std::string& v);
std::string to_string(const YAML::NodeType::value& v);
std::string to_string(const YAML::Node& v);

template<typename T, typename A>
std::string to_string(const std::vector<T, A>& v);

template<typename T, std::size_t Nm>
std::string to_string(const std::array<T, Nm>& v);

template <typename D>
std::string to_string(const Eigen::MatrixBase<D>& m);

}  // namespace std

#include <cnr_yaml/impl/string.hpp>

#endif /* CNR_YAML_UTILITIES_INCLUDE_CNR_YAML_UTILITIES_STRING */