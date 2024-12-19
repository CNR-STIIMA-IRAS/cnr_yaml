#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM__HPP
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM__HPP

#include <type_traits>
#include <yaml-cpp/node/type.h>
#include <yaml-cpp/node/convert.h>

#include <cnr_yaml/cnr_yaml.h>

#include <cnr_yaml/string.h>
#include <cnr_yaml/type_traits.h>
#include <cnr_yaml/eigen.h>
#include <cnr_yaml/node_utils.h>

// BUGFIX - recursive declrataion - raised in U24.04
// #include <cnr_yaml/impl/param_sequence.hpp>    // moved at the file end. 
// #include <cnr_yaml/impl/param_map.hpp>         // moved at the file end. Evaluate if remove.
// #include <cnr_yaml/impl/param_insert.hpp>      // useless file. To be removed.

namespace cnr
{
namespace yaml
{

template <typename T> bool get_sequence(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible);
template<typename T>  bool get_map(const YAML::Node& node, T&, std::string& what, const bool& implicit_cast_if_possible);

// =====================================================================================================================
//
// Get
//
//
// =====================================================================================================================
template <typename To, typename From,
          std::enable_if<std::is_convertible<std::decay_t<To>, std::decay_t<From>>::value, int>::type* = nullptr>
inline void cast(To&& to, From&& from)
{
  to = from;
}

template <typename To, typename From,
          std::enable_if<!std::is_convertible<std::decay_t<To>, std::decay_t<From>>::value &&
                             cnr::yaml::are_compatible_vectors<std::decay_t<To>, std::decay_t<From>>::value,
                         double>::type* = nullptr>
inline void cast(To&& to, From&& from)
{
  container_cast(to, from);
}

template <
    typename To, typename From,
    std::enable_if<!std::is_convertible<std::decay_t<To>, std::decay_t<From>>::value &&
                   !cnr::yaml::are_compatible_vectors<std::decay_t<To>, std::decay_t<From>>::value>::type* = nullptr>
inline void cast(To&& to, From&& from)
{
  to = from;
}

template <typename T, unsigned int I, unsigned int N, std::enable_if<(I == N), bool>::type = true>
inline bool decode(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  bool ok = false;
  switch (node.Type())
  {
    case YAML::NodeType::Scalar:
      ok = false;
      break;
    case YAML::NodeType::Sequence:
      ok = cnr::yaml::get_sequence(node, ret, what, implicit_cast_if_possible);
      break;
    case YAML::NodeType::Map:
      ok = cnr::yaml::get_map(node, ret, what, implicit_cast_if_possible);
      break;
    default:
      ok = false;
      break;
  }

  if (!ok)
  {
    what = "Error! Deconding the type '" + boost::typeindex::type_id_with_cvr<decltype(ret)>().pretty_name() +
           "' from the node was not possible, neither using the available alternatives. Input Node:\n" +
           std::to_string(node);
  }
  return ok;
}

template <typename T, unsigned int I, unsigned int N, std::enable_if<(I >= 0 && I < N), int>::type = 1>
inline bool decode(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  using variant = typename decoding_type_variant_holder<T>::variant;
  using type = std::variant_alternative<I, variant>::type;
  try
  {
    if (!implicit_cast_if_possible)
    {
      if constexpr (std::is_same<type, T>::value)
      {
        return YAML::convert<type>::decode(node, ret);
      }
      else
      {
        return decode<T, I + 1, N>(node, ret, what, implicit_cast_if_possible);
      }
    }
    else
    {
      type _ret;
      if (!YAML::convert<type>::decode(node, _ret))
      {
        return decode<T, I + 1, N>(node, ret, what, implicit_cast_if_possible);
      }
      else
      {
        cast(std::move(ret), std::move(_ret));
        return true;
      }
    }
  }
  catch (const std::exception& e)
  {
    what += std::string(e.what());
  }
  catch (...)
  {
    what += "Unknown error in decoding a '" + boost::typeindex::type_id_with_cvr<decltype(ret)>().pretty_name() +
            "' from node\n" + std::to_string(node) + "";
  }
  return decode<T, I + 1, N>(node, ret, what, implicit_cast_if_possible);
}

template <typename T>
inline bool get(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  try
  {
    if (decode<T, 0, std::variant_size<typename decoding_type_variant_holder<T>::variant>::value>(
            node, ret, what, implicit_cast_if_possible))
    {
      return true;
    }
  }
  catch (const std::exception& e)
  {
    what = std::string(e.what());
  }
  catch (...)
  {
    what = "Unknown error in decoding a '" + boost::typeindex::type_id_with_cvr<T>().pretty_name() +
           "' from node\n" + std::to_string(node) + "";
  }

  return false;
}

// =====================================================================================================================
//
// Set
//
//
// =====================================================================================================================
template <typename T, unsigned int I, unsigned int N, std::enable_if<(I == N), bool>::type* = nullptr>
inline bool encode(const T&, YAML::Node&, std::string& what)
{
  what = "Error! Encoding the type '" + boost::typeindex::type_id_with_cvr<T>().pretty_name() +
         "' from the node was not possible, neither using the available alternatives.";
  return false;
}

template <typename T, unsigned int I, unsigned int N, std::enable_if<(I >= 0 && I < N), int>::type* = nullptr>
inline bool encode(const T& value, YAML::Node& ret, std::string& what)
{
  using const_type = std::decay_t<const T&>;
  using variant = typename encoding_type_variant_holder<const_type>::variant;
  using type = std::variant_alternative<I, variant>::type;
  type _value;

  try
  {
    what = "Input Type: " + boost::typeindex::type_id_with_cvr<const T&>().pretty_name() +
           ", variant type: " + boost::typeindex::type_id_with_cvr<type>().pretty_name();
    cast(std::move(_value), std::move(value));
    ret = YAML::convert<type>::encode(_value);
    what += ", Return Object: " + std::to_string(ret);
    what += ", Node Type: " + std::to_string(ret.Type());
    return true;  // encode<T, I+1, N>(value, ret, what);
  }
  catch (const std::exception& e)
  {
    what = std::string(e.what()) + ", " + what;
  }
  catch (...)
  {
    what = "Unknown error. " + what;
  }
  return encode<T, I + 1, N>(value, ret, what);
}

template <typename T>
inline bool set(const T& value, YAML::Node& ret, std::string& what)
{
  std::string err = "Error! Implicit Cast not used. Failed in encoding a '" +
                    boost::typeindex::type_id_with_cvr<T>().pretty_name() + "'";
  try
  {
    if (encode<T, 0, std::variant_size<typename encoding_type_variant_holder<T>::variant>::value>(value, ret, what))
    {
      return true;
    }
  }
  catch (const std::exception& e)
  {
    what = err + " What: " + std::string(e.what());
  }
  catch (...)
  {
    what = err + " Unknown error in encoding a '" + boost::typeindex::type_id_with_cvr<T>().pretty_name() + "'";
  }
  return false;
}

}  // namespace yaml
}  // namespace cnr

namespace YAML
{

template <typename T>
as_if<T, std::optional<T>>::as_if(const Node& node) : node(node)
{
}

template <typename T>
std::optional<T> as_if<T, std::optional<T>>::operator()() const
{
  std::optional<T> val;
  T decoded;
  if ((node.m_pNode != nullptr) && convert<T>::decode(node, decoded))
  {
    val = std::move(decoded);
  }

  return val;
}

// There is already a string partial specialisation, so we need a full specialisation here
template <>
struct as_if<std::string, std::optional<std::string>>
{
  explicit as_if(const Node& node) : node(node)
  {
  }
  const Node& node;

  std::optional<std::string> operator()() const
  {
    std::optional<std::string> val;
    std::string decoded;
    if ((node.m_pNode != nullptr) && convert<std::string>::decode(node, decoded))
    {
      val = std::move(decoded);
    }

    return val;
  }
};

template <typename T, typename A>
struct as_if<std::vector<T, A>, std::optional<std::vector<T, A>>>
{
  explicit as_if(const Node& node) : node(node)
  {
  }
  const Node& node;

  std::optional<std::vector<T, A>> operator()() const
  {
    std::optional<std::vector<T, A>> val;
    if ((node.m_pNode != nullptr) && node.IsSequence())
    {
      std::vector<T, A> decoded;
      for (std::size_t i = 0; i < node.size(); i++)
      {
        T element;
        if (convert<T>::decode(node[i], element))
        {
          decoded.push_back(std::move(element));
        }
        else
        {
          return std::nullopt;
        }
      }
      val = std::move(decoded);
    }

    return val;
  }
};
}  // namespace YAML


// BUGFIX - recursive declrataion - raised in U24.04 
#include <cnr_yaml/impl/param_sequence.hpp>
#include <cnr_yaml/impl/param_map.hpp>
namespace cnr
{
namespace yaml
{
/**
 * @brief Get the sequence object
 *
 * @tparam T
 * @param node
 * @param ret
 * @param what
 * @return true
 * @return false
 */
template <typename T>
inline bool get_sequence(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible)
{
  return _get_sequence(node, ret, what, implicit_cast_if_possible);
}

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



}  // namespace yaml
}  // namespace cnr



#endif  // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM__HPP