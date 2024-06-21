#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__PARAM_H
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__PARAM_H

#include <string>
#include <optional>
#include <yaml-cpp/yaml.h>

namespace cnr
{
namespace yaml
{

/**
 * @brief Get the scalar object
 *
 * @tparam T
 * @param node
 * @param ret
 * @param what
 * @return true
 * @return false
 */
template <typename T>
bool get(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible);

/**
 * @brief
 *
 * @tparam T
 * @param node
 * @param key
 * @param value
 * @param format : by now only format for int is supported. Specify if it is 'dec' or 'hex'
 */
template <typename T>
bool set(const T& value, YAML::Node& ret, std::string& what);

}  // namespace yaml
}  // namespace cnr

namespace YAML
{
template <typename T>
struct as_if<T, std::optional<T>>
{
  explicit as_if(const Node& node);
  const Node& node;

  std::optional<T> operator()() const;
};

}
// ========================================

#include <cnr_yaml/impl/cnr_yaml.hpp>

#endif // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__PARAM_H
