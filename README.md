# cnr_yaml


[![Build Status - Plain CMake][p]][0]
[![codecov][c]][2] 
[![Codacy Badge][y]][3]
[![FOSSA Status][f]][4]


A package that extends the [`yaml-cpp`](https://github.com/jbeder/yaml-cpp).
The package provides a few functions to ease the manipulation of a `YAML::Node`.

## The Design

The package provides two main headers

  [`cnr_yaml.h`](include/cnr_yaml/cnr_yaml.h): this provides a few functions to ease the get/set of parameters from/to a `YAML::Node`.

  [`node_utils.h`](include/cnr_yaml/node_utils.h): this provides a few functions to ease the management of trees of `YAML::Node`.

### Get/Set Functions

The package provides a unique interface to get and set parameters from/to a `YAML::Node`.

The core is the file [cnr_yaml.h](./include/cnr_yaml/cnr_yaml.h).
The file provides four main functions:

```cpp
template <typename T>
bool get(const YAML::Node& node, T& ret, std::string& what, const bool& implicit_cast_if_possible);

template <typename T>
bool set(const T& value, YAML::Node& ret, std::string& what);
```

The functions extend the common access to the node `template<typename T> T YAML::Node().as<T>()`, similarly to the `template<typename T> bool YAML::decode<T>()` it returns a `true/false`.

The main difference consists of:

* Error and exceptions are caught, and the `std::string& what` reports what was the error.

* There is the possibility to allow an implicit cast. For example, if the node stores a vector `[1., 2., 3.]` and you ask for a `std::vector<double>`. See [below](#implicit-cast) for the step-by-step process description.

* It implements also the get/set for all the `Eigen::Matrix<>`.

Finally, the header implements a structure to get the value as an `std::option`, as shown by [Andrew Lipscomb](https://stackoverflow.com/questions/19994312/obtain-type-of-value-stored-in-yamlnode-for-yaml-cpp) in stackoverflow.

```cpp
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
```

This template is specialized [here](include/cnr_yaml/impl/cnr_yaml.hpp). This allows you to write code without exception-catching mechanisms whaen you are not sure about what is stored inside the `yaml`.

```cpp
std::optional<bool> as_bool          = YAML::as_if<bool, std::optional<bool> >(node)();
std::optional<int> as_int            = YAML::as_if<int, std::optional<int> >(node)();
std::optional<double> as_double      = YAML::as_if<double, std::optional<double> >(node)();
std::optional<std::string> as_string = YAML::as_if<std::string, std::optional<std::string> >(node)();
if (as_bool)
{
  tree = rclcpp::ParameterValue(as_bool.value());
}
else if (as_int)
{
  tree= rclcpp::ParameterValue(as_int.value());
}
else if (as_double)
{
  tree= rclcpp::ParameterValue(as_double.value());
}
else if (as_string)
{
  tree= rclcpp::ParameterValue(as_string.value());
}
else
{
  what = "The node is a scalar, but the conversion to rclcpp::Parameter failed";
  return false;
}
```

  **NOTE**
  In `yaml-cpp` everything can be a `std::string`. The `as_string` in the example will be always true! Therefore, pay attention to the verification order in the case!

### Node Management Utilities
A few functions to ease managing nodes are in the header [`nodes_utils.h`](include/cnr_yaml/node_utils.h)

* Merging nodes and override values if the key tree is the same.

```cpp
const YAML::Node merge_nodes(const YAML::Node& default_node, const YAML::Node& override_node);
```

* Add a tree of keys with empty nodes to a node.

```cpp
YAML::Node init_tree(const std::vector<std::string> seq, const YAML::Node& node);
```

* Get the value of a leaf of the node, using a key with delimiters to access it.

```cpp
bool get_leaf(const YAML::Node& node, const std::string& key, YAML::Node& leaf, std::string& what, const std::string& delimeters = "/.");
```

For example, you get `another_int2` from the following `yaml`

```yaml
nested_param:
  another_int: 7
  another_int2: 7.0
  nested_param:
    another_int: 7
    another_int2: 7.0
```

by calling

```cpp
get_leaf(root_node, "nested_param/nested_param/another_int2", output, what, "/");
```

if you prefer to use another delimiter, you can do it

```cpp
get_leaf(root_node, "nested_param.nested_param.another_int2", output, what, ".");
```

The errors are caught and put in the `what` return variable.

* Get the value of a leaf of the node as the previous, but the key is already split in a vector of strings

```cpp
YAML::Node get_leaf(const std::vector<std::string>& keys, const YAML::Node& node);
```

* Get the iterator to a value inside a node.

```cpp
YAML::iterator get_node(const std::string& key, YAML::iterator& node_begin, YAML::iterator& node_end);
```

### Complex Types

You must follow the standard way to allow the encoding and decoding of a complex type from `YAML::Node`. Here a simple example

```cpp
namespace YAML
{
template <>
struct convert<ComplexType>
{
  static Node encode(const ComplexType& rhs)
  {
    Node node;
    node["name"] = rhs.name;
    node["value"] = rhs.value;
    return node;
  }

  static bool decode(const Node& node, ComplexType& rhs)
  {
    if (!node.IsMap() || !node["name"] || !node["value"])
    {
      return false;
    }
    rhs.name = node["name"].as<std::string>();
    rhs.value = node["value"].as<double>();
    return true;
  }
};
}  // namespace YAML
```

### Implicit Cast

To enable the implicit cast you must define what are the types you consider equivalent by the specialization of two templates as follows:

```cpp
namespace cnr { 
namespace yaml 
{
template <>
struct decoding_type_variant_holder<int>
{
  using base = int;
  using variant = std::variant<int32_t, int64_t, int16_t, int8_t, double, long double, float>;
};

template <>
struct decoding_type_variant_holder<double>
{
  using base = int;
  using variant = std::variant<double, long double, float, int32_t, int64_t, int16_t, int8_t>;
};

}
}
```

### Contact

<mailto:nicola.pedrocchi@stiima.cnr.it>

## License

[![FOSSA Status][o]][5]

[p]:https://github.com/CNR-STIIMA-IRAS/cnr_yaml/actions/workflows/build_cmake.yml/badge.svg
[0]:https://github.com/CNR-STIIMA-IRAS/cnr_yaml/actions/workflows/build_cmake.yml

[c]:https://codecov.io/gh/CNR-STIIMA-IRAS/cnr_yaml/graph/badge.svg?token=fuuQUJHhaQ
[2]:https://codecov.io/gh/CNR-STIIMA-IRAS/cnr_yaml

[y]:https://api.codacy.com/project/badge/Grade/7f1834c02aa84b959ee9b7529deb48d6
[3]:https://app.codacy.com/gh/CNR-STIIMA-IRAS/cnr_yaml?utm_source=github.com&utm_medium=referral&utm_content=CNR-STIIMA-IRAS/cnr_yaml&utm_campaign=Badge_Grade_Dashboard

[f]:https://app.fossa.com/api/projects/git%2Bgithub.com%2FCNR-STIIMA-IRAS%2Fcnr_yaml.svg?type=shield&issueType=license
[4]:https://app.fossa.com/projects/git%2Bgithub.com%2FCNR-STIIMA-IRAS%2Fcnr_yaml?ref=badge_shield&issueType=license

[o]:https://app.fossa.com/api/projects/git%2Bgithub.com%2FCNR-STIIMA-IRAS%2Fcnr_yaml.svg?type=large
[5]:https://app.fossa.com/projects/git%2Bgithub.com%2FCNR-STIIMA-IRAS%2Fcnr_yaml?ref=badge_large
