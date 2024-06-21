#include <cassert>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string.hpp>

//#include <cnr_yaml/filesystem.h>
#include <cnr_yaml/string.h>
#include <cnr_yaml/node_utils.h>

namespace cnr
{
namespace yaml
{

const YAML::Node merge_nodes(const YAML::Node& default_node, const YAML::Node& override_node)
{
  if (!override_node.IsMap())
  {
    // If override_node is not a map, merge result is override_node, unless override_node is null
    return override_node.IsNull() ? default_node : override_node;
  }
  if (!default_node.IsMap())
  {
    // If default_node is not a map, merge result is override_node
    return override_node;
  }
  if (!default_node.size())
  {
    return YAML::Node(override_node);
  }
  // Create a new map 'new_node' with the same mappings as default_node, merged with override_node
  auto new_node = YAML::Node(YAML::NodeType::Map);
  for (auto node : default_node)
  {
    if (node.first.IsScalar())
    {
      const std::string& key = node.first.Scalar();
      new_node[key] = bool(override_node[key]) ? merge_nodes(node.second, override_node[key]) : node.second;
    }
    else
    {
      new_node[node.first] = node.second;
    }
  }
  // Add the mappings from 'override_node' not already in 'new_node'
  for (auto node : override_node)
  {
    if (node.first.IsScalar())
    {
      const std::string& key = node.first.Scalar();
      new_node[key] =
          bool(default_node[key]) ? merge_nodes(default_node[key], node.second) : new_node[key] = node.second;
    }
    else
    {
      new_node[node.first] = node.second;
    }
  }
  return YAML::Node(new_node);
}

YAML::Node init_tree(const std::vector<std::string> seq, const YAML::Node& node)
{
  if (seq.size() == 0)
  {
    return YAML::Node(node);
  }
  else
  {
    auto new_node = YAML::Node(YAML::NodeType::Map);
    new_node[seq.back()] = node;
    if (seq.size() == 1)
    {
      return new_node;
    }

    std::vector<std::string> new_seq = seq;
    ;
    new_seq.erase(new_seq.end() - 1);
    return init_tree(new_seq, new_node);
  }
}

YAML::iterator get_node(const std::string& key, YAML::iterator& node_begin, YAML::iterator& node_end)
{
  YAML::iterator it = node_begin;
  for (it = node_begin; it != node_end; ++it)
  {
    if (it->first.IsScalar())
    {
      const std::string& node_key = it->first.Scalar();
      if (node_key == key)
      {
        return it;
      }
    }
  }
  return it;
}

void get_keys_tree(const std::string& ns, const YAML::Node& node, std::vector<std::string>& tree)
{
  auto l = __LINE__;
  try
  {
    if (!node.IsDefined())
    {
      throw std::runtime_error("Node not defined!");
    }
    l = __LINE__;
    if (node.IsMap())
    {
      l = __LINE__;
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
      {
        std::string key = (ns.empty() ? "/" : ns) + "/" + it->first.as<std::string>() + "/";
        if (it->second.IsMap())
        {
          l = __LINE__;
          get_keys_tree(key, it->second, tree);
        }
        else
        {
          l = __LINE__;
          tree.push_back(key);
        }
      }
    }
    // else if(node.IsScalar() )
    // {
    //   std::string key = node.begin()->first.as<std::string>();
    //   auto value = node.begin()->second;
    //   tree.push_back( (ns.empty() ? "/" : ns) + "/" + key +"/");
    //   get_keys_tree( tree.back(), value, tree );
    // }
  }
  catch (YAML::InvalidNode& e)
  {
    std::cout << __LINE__ << "@" << l << ":" << e.what() << std::endl;
    std::cout << "Input Node: \n" << node << std::endl;
    std::cout << "Continue" << std::endl;
    assert(0);
  }
  catch (std::exception& e)
  {
    std::cout << __LINE__ << "@" << l << ":" << e.what() << std::endl;
    std::cout << "Input Node Type: " << node.Type() << std::endl;
    std::cout << "Input Node: \n" << node << std::endl;
    std::cout << "Continue" << std::endl;
    assert(0);
  }
  return;
}

void get_nodes_tree(const std::string& ns, const YAML::Node& node,
                    std::vector<std::pair<std::string, YAML::Node>>& tree)
{
  auto l = __LINE__;
  try
  {
    if (!node.IsDefined())
    {
      throw std::runtime_error("Node not defined!");
    }
    l = __LINE__;
    if (node.IsMap())
    {
      l = __LINE__;
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
      {
        std::string _ns = ns.empty() ? "/" : ns.back() == '/' ? ns.substr(0, ns.length() - 1) : ns;
        std::string _key = _ns + "/" + it->first.as<std::string>();
        tree.push_back(std::make_pair(_key, it->second));
        if (it->second.IsMap())
        {
          l = __LINE__;
          get_nodes_tree(_key, it->second, tree);
        }
      }
    }
  }
  catch (YAML::InvalidNode& e)
  {
    std::cout << __LINE__ << "@" << l << ":" << e.what() << std::endl;
    std::cout << "Input Node: \n" << node << std::endl;
    std::cout << "Continue" << std::endl;
    assert(0);
  }
  catch (std::exception& e)
  {
    std::cout << __LINE__ << "@" << l << ":" << e.what() << std::endl;
    std::cout << "Input Node Type: " << node.Type() << std::endl;
    std::cout << "Input Node: \n" << node << std::endl;
    std::cout << "Continue" << std::endl;
    assert(0);
  }
  return;
}

YAML::Node get_leaf(const std::vector<std::string>& keys, const YAML::Node& node)
{
  if (keys.size() == 0)
  {
    return node;
  }

  auto it = keys.begin();
  std::string key = keys.front();
  for (YAML::const_iterator yt = node.begin(); yt != node.end(); ++yt)
  {
    const std::string& node_key = yt->first.as<std::string>();
    if (node_key == key)
    {
      if (keys.size() > 1)
      {
        std::vector<std::string> remaining_keys(it + 1, keys.end());
        return get_leaf(remaining_keys, yt->second);
      }
      else
      {
        auto ret = YAML::Node(YAML::NodeType::Map);
        ret[yt->first.as<std::string>()] = yt->second;
        return ret;
      }
    }
  }
  std::cout << "Weird error " << key << " < ";
  for (YAML::const_iterator yt = node.begin(); yt != node.end(); ++yt)
  {
    const std::string& node_key = yt->first.as<std::string>();
    std::cout << node_key << " ";
  }
  std::cout << ">" << std::endl;
  return YAML::Node();
}

/**
 * @brief
 *
 * @param root
 * @return std::vector<std::pair<std::string, YAML::Node>>
 */
std::vector<std::pair<std::string, YAML::Node>> toNodeList(const YAML::Node& root)
{
  std::vector<std::pair<std::string, YAML::Node>> tree;  // mapped file names;
  cnr::yaml::get_nodes_tree("", root, tree);
  return tree;
}

/**
 * @brief
 *
 * @param node
 * @param key
 * @return true
 * @return false
 */
bool has(const YAML::Node& node, const std::string& key)
{
  return bool(node[key]);
}

/**
 * @brief
 *
 * @param node
 * @return size_t
 */
size_t size(const YAML::Node& node)
{
  if (!node.IsSequence())
  {
    std::stringstream err;
    err << __PRETTY_FUNCTION__ << ":" << std::to_string(__LINE__) << ": "
        << "The node is not a sequence!";
    throw std::runtime_error(err.str().c_str());
  }
  return node.size();
}

/**
 * @brief Get the leaf object
 *
 * @param node
 * @param key
 * @param leaf
 * @param what
 * @return true
 * @return false
 */
bool get_leaf(const YAML::Node& node, const std::string& key, YAML::Node& leaf, std::string& what, const std::string& delimeters)
{
  const YAML::Node _node = node;
  std::vector<std::string> tokens; 
  std::vector<std::size_t> positions;
  for(const auto & d : delimeters)
  {
    auto _p = key.find_first_of(d);
    if(_p != std::string::npos)
    {
      positions.push_back(_p);
    }
  }
  std::string _token = positions.size()==0 ? key : key.substr(0, *std::min_element(positions.begin(), positions.end()));
  std::string _key  = positions.size()==0 ? "" : key.substr(*std::min_element(positions.begin(), positions.end())+1, key.length());

  if (_node[_token])
  {
    if(_key.empty())
    {
      leaf = _node[_token];
      return true;
    }
    else
    {
      return get_leaf(_node[_token], _key, leaf, what, delimeters);
    }
  }
  what = "The key '"+key+"' has been resolved in the token '"+_token+"' (and '"+_key+"') that is not in the node dictionary (Input Node: " + std::to_string(_node) + ")";
  return false;
}

}  // namespace yaml
}  // namespace cnr