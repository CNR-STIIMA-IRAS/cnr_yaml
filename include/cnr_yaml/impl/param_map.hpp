#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_MAP__HPP
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_MAP__HPP

#include <boost/type_index.hpp>
#include <yaml-cpp/yaml.h>

#include <cnr_yaml/string.h>
#include <cnr_yaml/type_traits.h>

#if !defined(UNUSED)
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

namespace cnr
{
namespace yaml
{
  /* MOVED IN cnr_yaml.hpp - Evaluate if remove the file 
// =============================================================================================
// MAP
// =============================================================================================
template<typename T>
inline bool get_map(const YAML::Node& node, T&, std::string& what, const bool&)
{
  std::stringstream _node;
  _node << node;
  what = "The type ' "
        + boost::typeindex::type_id_with_cvr<decltype(T())>().pretty_name() 
          + "' is not supported. You must specilized your own 'get_map' template function\n Input Node: " + _node.str();
  return false;
}

template<>
inline bool get_map(const YAML::Node& node, YAML::Node& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  UNUSED(what);
  UNUSED(implicit_cast_if_possible);
  ret = node;
  return true;
}
// =============================================================================================
// END MAP
// =============================================================================================
*/
}   // namespace yaml
}   // namespace cnr

#endif  // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__UTILS__IMPL__PARAM_MAP__HPP