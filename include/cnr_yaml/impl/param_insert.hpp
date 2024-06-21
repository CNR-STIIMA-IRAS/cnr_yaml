#ifndef CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM_INSERT_HPP
#define CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM_INSERT_HPP

#include <iomanip>
#include <string>
#include <boost/type_index.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>

#include <cnr_yaml/eigen.h>
#include <cnr_yaml/type_traits.h>

#if !defined(UNUSED)
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

#if !defined(CATCH)
#define CATCH(X)\
  catch(YAML::Exception& e)\
  {\
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": "\
        << "YAML Exception, Error in the extraction of an object of type '"\
          << boost::typeindex::type_id_with_cvr<decltype( X )>().pretty_name() \
            << "'" << std::endl\
              << "Node: " << std::endl\
                << node << std::endl\
                  << "What: " << std::endl\
                    << e.what() << std::endl;\
    }\
    catch (std::exception& e)\
    {\
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": "\
        << "Exception, Error in the extraction of an object of type '"\
          << boost::typeindex::type_id_with_cvr<decltype( X )>().pretty_name() \
            << "'" << std::endl\
              << "Node: " << std::endl\
                << node << std::endl\
                  << "What: " << std::endl\
                    << e.what() << std::endl;\
    }
#endif



namespace cnr
{
namespace yaml
{


template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

template<typename T>
inline void insert(YAML::Node& node, const std::string& key, const T& value, const std::string& format)
{
  UNUSED(format);
  try
  {
    if(key.length())
    {
      node[key] = value;
    }
    else
    {
      node = value;
    }
  }
  catch(const std::exception& e)
  {
    throw std::runtime_error(e.what());
  }
}

template<>
inline void insert(YAML::Node& node, const std::string& key, const int& value, const std::string& format)
{
  std::string val;
  if(format=="dec")
  {
    val = std::to_string(value);
  }
  else if(format=="hex")
  {
    val = int_to_hex(value);
  }
  try
  {
    if(key.length())
    {
      node[key] = val;
    }
    else
    {
      node = val;
    }
  }
  catch(const std::exception& e)
  {
    throw std::runtime_error(e.what());
  }
}


}
}
#endif  // CNR_YAML_UTILITIES__INCLUDE__CNR_YAML_UTILITIES__IMPL__PARAM_INSERT_HPP