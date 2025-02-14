#include <Eigen/Core>
#include <array>
#include <filesystem>
#include <gtest/gtest.h>
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include <cnr_yaml/cnr_yaml.h>

namespace detail
{
struct unwrapper
{
  explicit unwrapper(std::exception_ptr pe) : pe_(pe)
  {
  }

  operator bool() const
  {
    return bool(pe_);
  }

  friend auto operator<<(std::ostream& os, unwrapper const& u) -> std::ostream&
  {
    try
    {
      std::rethrow_exception(u.pe_);
      return os << "no exception";
    }
    catch (std::runtime_error const& e)
    {
      return os << "runtime_error: " << e.what();
    }
    catch (std::logic_error const& e)
    {
      return os << "logic_error: " << e.what();
    }
    catch (std::exception const& e)
    {
      return os << "exception: " << e.what();
    }
    catch (...)
    {
      return os << "non-standard exception";
    }
  }
  std::exception_ptr pe_;
};
}  // namespace detail

auto unwrap(std::exception_ptr pe)
{
  return detail::unwrapper(pe);
}

template <class F>
::testing::AssertionResult does_not_throw(F&& f)
{
  try
  {
    f();
    return ::testing::AssertionSuccess();
  }
  catch (...)
  {
    return ::testing::AssertionFailure() << unwrap(std::current_exception());
  }
}

std::map<std::string, std::map<std::string, std::vector<double>>> statistics;

#define EXECUTION_TIME(...)                                                                                            \
  {                                                                                                                    \
    struct timespec start, end;                                                                                        \
    clock_gettime(CLOCK_MONOTONIC, &start);                                                                            \
    __VA_ARGS__;                                                                                                       \
    clock_gettime(CLOCK_MONOTONIC, &end);                                                                              \
    double time_taken;                                                                                                 \
    time_taken = double(end.tv_sec - start.tv_sec) * 1e9;                                                              \
    time_taken = double(time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;                                            \
    std::cout << "Elapsed time [us]: " << time_taken * 1e6 << std::endl;                                               \
  }

#include <cnr_yaml/eigen.h>

TEST(EigeTest, EigenTest)
{
  double d_lhs, d_rhs = 123.4;
  std::vector<double> v_lhs_1;
  std::vector<double> v_lhs_2;
  std::vector<double> v_lhs_3;
  std::vector<double> v_rhs;
  std::vector<double> v_rhs_1{ 1.1 };

  std::vector<double> v_rhs_2{ 1.1, 2.2 };
  std::vector<double> v_rhs_3{ 1.1, 2.2, 3.3 };
  Eigen::VectorXd ve_lhs_x(3);
  ve_lhs_x << 1.1, 2.2, 3.3;
  Eigen::MatrixXd me_lhs_x(3, 1);
  me_lhs_x << 1.1, 2.2, 3.3;
  Eigen::MatrixXd me_lhs_x_2(3, 1);
  me_lhs_x_2 << 1.1, 2.2, 3.3;
  Eigen::Matrix<double, 1, 1> ve_lhs_1;
  ve_lhs_1 << 1.1;
  Eigen::Matrix<double, 2, 1> ve_lhs_2;
  ve_lhs_2 << 1.1, 2.2;
  Eigen::Matrix<double, 3, 1> ve_lhs_3;
  ve_lhs_3 << 1.1, 2.2, 3.3;
  Eigen::VectorXd ve_rhs_x(3);
  ve_rhs_x << 1.1, 2.2, 3.3;
  Eigen::MatrixXd me_rhs_x(3, 1);
  me_rhs_x << 1.1, 2.2, 3.3;
  Eigen::MatrixXd me_rhs_x_2(3, 1);
  me_rhs_x_2 << 1.1, 2.2, 3.3;
  Eigen::Matrix<double, 1, 1> ve_rhs_1;
  ve_rhs_1 << 1.1;
  Eigen::Matrix<double, 2, 1> ve_rhs_2;
  ve_rhs_2 << 1.1, 2.2;
  Eigen::Matrix<double, 3, 1> ve_rhs_3;
  ve_rhs_3 << 1.1, 2.2, 3.3;

  EXPECT_TRUE(cnr::yaml::copy(d_lhs, d_rhs));
  EXPECT_TRUE(d_lhs == d_rhs);

  EXPECT_TRUE(cnr::yaml::copy(d_lhs, v_rhs_1));
  EXPECT_TRUE(d_lhs == v_rhs_2[0]);

  EXPECT_FALSE(cnr::yaml::copy(d_lhs, v_rhs_2));

  EXPECT_TRUE(cnr::yaml::copy(v_rhs_1, d_lhs));
  EXPECT_TRUE(d_lhs == v_rhs_1[0]);

  EXPECT_FALSE(cnr::yaml::copy(v_rhs, d_lhs));

  EXPECT_FALSE(cnr::yaml::copy(d_lhs, ve_lhs_x));
  EXPECT_TRUE(d_lhs == ve_lhs_x(0));

  EXPECT_TRUE(cnr::yaml::copy(me_lhs_x_2, me_lhs_x));

  EXPECT_TRUE(cnr::yaml::copy(me_lhs_x, d_rhs));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_x, d_rhs));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_1, d_rhs));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_2, d_rhs));

  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_x, v_rhs_1));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_x, v_rhs_2));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_x, v_rhs_3));
  EXPECT_TRUE(cnr::yaml::copy(me_lhs_x, v_rhs_1));
  EXPECT_TRUE(cnr::yaml::copy(me_lhs_x, v_rhs_2));
  EXPECT_TRUE(cnr::yaml::copy(me_lhs_x, v_rhs_3));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_1, v_rhs_1));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_2, v_rhs_2));
  EXPECT_TRUE(cnr::yaml::copy(ve_lhs_3, v_rhs_3));
  EXPECT_FALSE(cnr::yaml::copy(ve_lhs_1, v_rhs_2));
  EXPECT_FALSE(cnr::yaml::copy(ve_lhs_2, v_rhs_1));

  EXPECT_TRUE(cnr::yaml::resize(ve_lhs_x,10));
  EXPECT_FALSE(cnr::yaml::resize(ve_lhs_3,10));

  EXPECT_TRUE(cnr::yaml::resize(d_lhs,1));
  EXPECT_FALSE(cnr::yaml::resize(d_lhs,2));

  EXPECT_TRUE(cnr::yaml::resize(v_lhs_1,10));
  EXPECT_FALSE(cnr::yaml::resize(v_lhs_1,10,10));

  EXPECT_TRUE(cnr::yaml::resize(v_lhs_1,v_lhs_2)); 

  {
    Eigen::Matrix<double, 3, 1> _ve_rhs_3;
    Eigen::Matrix<double, 2, 1> _ve_rhs_2;
    Eigen::Matrix<double, -1, 1> _ve_rhs_x;
    YAML::Node v;
    bool ok = false;
    EXPECT_TRUE(does_not_throw([&] {v["val"] = YAML::convert<Eigen::Matrix<double, 3, 1>>::encode(ve_rhs_3);}));
    EXPECT_TRUE(does_not_throw([&] { ok = YAML::convert<Eigen::Matrix<double, 3, 1> >::decode(v["val"], _ve_rhs_3); }));
    EXPECT_TRUE(ok);
    EXPECT_TRUE(does_not_throw([&] { ok = YAML::convert<Eigen::Matrix<double, 3, 1> >::decode(v, _ve_rhs_3); }));
    EXPECT_FALSE(ok);
    EXPECT_TRUE(does_not_throw([&] { ok = YAML::convert<Eigen::Matrix<double, 2, 1> >::decode(v["val"], _ve_rhs_2);}));
    EXPECT_FALSE(ok);
    EXPECT_TRUE(does_not_throw([&] { ok = YAML::convert<Eigen::Matrix<double, -1, 1> >::decode(v["val"], _ve_rhs_x); }));
    EXPECT_TRUE(ok);
  }

}

#include <cnr_yaml/string.h>


TEST(YamlUtilities, StringTests)
{
  EXPECT_EQ(std::to_string("null"), "null");
  EXPECT_EQ(std::to_string(YAML::NodeType::Null), "Null");
  EXPECT_EQ(std::to_string(YAML::NodeType::Map), "Map");
  EXPECT_EQ(std::to_string(YAML::NodeType::Scalar), "Scalar");
  EXPECT_EQ(std::to_string(YAML::NodeType::Sequence), "Sequence");
  EXPECT_EQ(std::to_string(YAML::NodeType::Undefined), "Undefined");

  {
    YAML::Node v;
    v["root"] = "ciao";
    EXPECT_EQ(std::to_string(v), "root: ciao");
  }

  {
    std::vector<double> v{0.1,0.2,0.3};
    EXPECT_EQ( std::to_string(v), "[ 0.100000, 0.200000, 0.300000 ]");
  }

  {
    std::array<double,3> v{0.1,0.2,0.3};
    EXPECT_EQ( std::to_string(v), "[ 0.100000, 0.200000, 0.300000 ]");
  }

  {
    Eigen::Vector3d v{0.1,0.2,0.3};
    EXPECT_EQ( std::to_string(v), "[ 0.1000, 0.2000, 0.3000 ]");
  }
}


// ====================================================================================================================
// === GLOBAL VARIABLES
// ===============================================================================================
// ====================================================================================================================
YAML::Node node;

template <typename T>
bool call(const std::string& key, T& value, bool implicit_cast_if_possible = true)
{
  std::string what;
  YAML::Node leaf;
  std::stringstream ss;
  ss << "Input key: " << key << std::endl;
  ss << "Input Node Type: " << std::to_string(node) << std::endl;
  ss << "Input Node: " << std::to_string(node) << std::endl;

  if (!cnr::yaml::get_leaf(node, key, leaf, what, "/."))
  {
    std::cerr << "======================================== get_leaf error" << std::endl;
    std::cerr << ss.str();
    std::cerr << "What: " << what << std::endl;
    std::cerr << "========================================" << std::endl;
    return false;
  }

  if (!cnr::yaml::get(leaf, value, what, implicit_cast_if_possible))
  {
    std::cerr << "======================================== get error" << std::endl;
    std::cerr << "What: " << what << std::endl;
    std::cerr << "========================================" << std::endl;
    return false;
  }

  std::cout << "Value: " << std::to_string(value) << std::endl;
  return true;
}

namespace cnr
{
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

}  // namespace yaml
}  // namespace cnr

TEST(YamlUtilities, BasicTests)
{
  YAML::Node only_root;
  only_root["root"] = "ciao";
  std::string value;
  std::string what;
  bool implicit_cast_if_possible = false;

  bool ok = cnr::yaml::get(only_root, value, what, implicit_cast_if_possible);
  EXPECT_FALSE(ok);
  ok = cnr::yaml::get(only_root["root"], value, what, implicit_cast_if_possible);
  std::cout << "========================================" << std::endl;
  std::cout << "Input Node Type: " << std::to_string(node.Type()) << std::endl;
  std::cout << "Input Node: " << node << std::endl;
  if (!ok)
  {
    std::cerr << "What: " << what << std::endl;
  }
  std::cout << "========================================" << std::endl;
}

TEST(YamlUtilities, ClientUsageBasicTypes)
{
  std::string what;
  std::vector<double> v_double;
  EXPECT_TRUE(call("double_array", v_double));
  EXPECT_TRUE(v_double.size() == 2 && v_double[0] == 7.5 && v_double[1] == 400.4);

  std::vector<double> v_double_2;
  EXPECT_TRUE(call("double_array_2", v_double_2));
  EXPECT_TRUE(v_double_2.size() == 2 && v_double_2[0] == 7 && v_double_2[1] == 400);

  std::array<double, 2> a_double;
  EXPECT_TRUE(call("double_array", a_double));
  EXPECT_TRUE(a_double[0] == 7.5 && a_double[1] == 400.4);

  std::array<double, 3> a_double_2;
  EXPECT_FALSE(call("double_array", a_double_2));

  Eigen::VectorXd ve_double;
  EXPECT_TRUE(call("double_array", ve_double));
  EXPECT_TRUE(ve_double.rows() == 2 && ve_double(0) == 7.5 && ve_double(1) == 400.4);

  Eigen::MatrixXd me_double;
  EXPECT_TRUE(call("double_array", me_double));
  EXPECT_TRUE(me_double.rows() == 2 && me_double.cols() == 1 && me_double(0, 0) == 7.5 && me_double(1, 0) == 400.4);

  Eigen::Matrix<double, 2, 1> me_double_21;
  EXPECT_TRUE(call("double_array", me_double_21));
  EXPECT_TRUE(me_double_21.rows() == 2 && me_double_21.cols() == 1 && me_double_21(0, 0) == 7.5 &&
              me_double_21(1, 0) == 400.4);

  Eigen::Matrix<double, 2, 2> me_double_22;
  EXPECT_FALSE(call("double_array", me_double_22));

  double val = 0.0;
  EXPECT_TRUE(call("double_value", val));
  EXPECT_TRUE(val == 3.14);

  std::vector<int> v_int;
  EXPECT_TRUE(call("int_array", v_int));
  EXPECT_TRUE(v_int.size() == 4 && v_int[0] == 10 && v_int[1] == 11 && v_int[2] == 12 && v_int[3] == 13);

  std::array<int, 4> a_int;
  EXPECT_TRUE(call("int_array", a_int));
  EXPECT_TRUE(a_int[0] == 10 && a_int[1] == 11 && a_int[2] == 12 && a_int[3] == 13);

  EXPECT_FALSE(call("int_array_2", v_int, false));
  EXPECT_TRUE(call("int_array_2", v_int, true));
  EXPECT_TRUE(v_int.size() == 4 && v_int[0] == 10 && v_int[1] == 11 && v_int[2] == 12 && v_int[3] == 13);

  int val_int = 0;
  EXPECT_TRUE(call("int_value", val_int));
  EXPECT_TRUE(val_int == 5);

  std::vector<std::string> v_string;
  EXPECT_TRUE(call("string_array", v_string));
  EXPECT_TRUE(v_string.size() == 3 && v_string[0] == "Nice" && v_string[1] == "more" && v_string[2] == "params");

  std::string val_string;
  EXPECT_TRUE(call("string_value", val_string, true));
  EXPECT_TRUE(val_string == "Hello Universe");

  EXPECT_TRUE(call("string_value", val_string, false));
  EXPECT_TRUE(val_string == "Hello Universe");

  std::vector<bool> v_bool;
  EXPECT_TRUE(call("bool_array", v_bool));
  EXPECT_TRUE(v_bool.size() == 3 && v_bool[0] && !v_bool[1] && v_bool[2]);

  bool val_bool = false;
  EXPECT_TRUE(call("bool_value", val_bool));
  EXPECT_TRUE(val_bool);

  std::vector<uint16_t> v_bytes;
  EXPECT_TRUE(call("bytes_array", v_bytes));
  EXPECT_TRUE(v_bytes.size() == 3 && v_bytes[0] == 0x01 && v_bytes[1] == 0xF1 && v_bytes[2] == 0xA2);

  EXPECT_TRUE(call("nested_param/another_int", val_int));
  EXPECT_TRUE(call("nested_param.another_int", val_int));
  EXPECT_TRUE(val_int == 7);

  EXPECT_TRUE(call("nested_param/another_int2", val_int));
  EXPECT_TRUE(call("nested_param.another_int2", val_int));
  EXPECT_TRUE(val_int == 7);

  EXPECT_TRUE(call("nested_param/nested_param/another_int2", val_int));
  EXPECT_TRUE(call("nested_param.nested_param.another_int2", val_int));
  EXPECT_TRUE(call("nested_param.nested_param/another_int2", val_int));
  EXPECT_TRUE(call("nested_param/nested_param.another_int2", val_int));
  EXPECT_TRUE(val_int == 7);

  EXPECT_TRUE(call("n1/n3/v1", v_string));
  EXPECT_TRUE(v_string.size() == 3 && v_string[0] == "s1" && v_string[1] == "s2" && v_string[2] == "s3");

  std::vector<std::vector<std::string>> vv_string;
  EXPECT_TRUE(call("n1/n4/vv1", vv_string));
  EXPECT_TRUE(vv_string.size() == 3 && vv_string.front().size() == 3 && vv_string[0][0] == "s11" &&
              vv_string[0][1] == "s12" && vv_string[0][2] == "s13");
  EXPECT_TRUE(vv_string.size() == 3 && vv_string.front().size() == 3 && vv_string[1][0] == "s21" &&
              vv_string[1][1] == "s22" && vv_string[1][2] == "s23");
  EXPECT_TRUE(vv_string.size() == 3 && vv_string.front().size() == 3 && vv_string[2][0] == "s31" &&
              vv_string[2][1] == "s32" && vv_string[2][2] == "s33");

  std::vector<std::vector<int>> vv_int;
  EXPECT_TRUE(call("n1/n4/vv2", vv_int));
  EXPECT_TRUE(vv_int.size() == 3 && vv_int.front().size() == 3 && vv_int[0][0] == 11 && vv_int[0][1] == 12 &&
              vv_int[0][2] == 13);
  EXPECT_TRUE(vv_int.size() == 3 && vv_int.front().size() == 3 && vv_int[1][0] == 21 && vv_int[1][1] == 22 &&
              vv_int[1][2] == 23);
  EXPECT_TRUE(vv_int.size() == 3 && vv_int.front().size() == 3 && vv_int[2][0] == 31 && vv_int[2][1] == 32 &&
              vv_int[2][2] == 33);

  std::array<std::array<int, 3>, 3> aa_int;
  EXPECT_TRUE(call("n1/n4/vv2", aa_int));
  EXPECT_TRUE(aa_int[0][0] == 11 && aa_int[0][1] == 12 && aa_int[0][2] == 13);
  EXPECT_TRUE(aa_int[1][0] == 21 && aa_int[1][1] == 22 && aa_int[1][2] == 23);
  EXPECT_TRUE(aa_int[2][0] == 31 && aa_int[2][1] == 32 && aa_int[2][2] == 33);

  std::array<std::array<int, 2>, 3> aa_int_2;
  EXPECT_FALSE(call("n1/n4/vv2", aa_int_2));

  std::array<std::vector<int>, 3> av_int;
  EXPECT_TRUE(call("n1/n4/vv2", av_int));
  EXPECT_TRUE(av_int.front().size() == 3 && av_int[0][0] == 11 && av_int[0][1] == 12 &&
              av_int[0][2] == 13);
  EXPECT_TRUE(av_int.front().size() == 3 && av_int[1][0] == 21 && av_int[1][1] == 22 &&
              av_int[1][2] == 23);
  EXPECT_TRUE(av_int.front().size() == 3 && av_int[2][0] == 31 && av_int[2][1] == 32 &&
              av_int[2][2] == 33);

  std::array<std::vector<int>, 2> av_int_2;
  EXPECT_FALSE(call("n1/n4/vv2", av_int_2));

  std::vector<std::array<int, 3>> va_int;
  EXPECT_TRUE(call("n1/n4/vv2", va_int));
  EXPECT_TRUE(va_int.size() == 3 && va_int.front().size() == 3 && va_int[0][0] == 11 && va_int[0][1] == 12 &&
              va_int[0][2] == 13);
  EXPECT_TRUE(va_int.size() == 3 && va_int.front().size() == 3 && va_int[1][0] == 21 && va_int[1][1] == 22 &&
              va_int[1][2] == 23);
  EXPECT_TRUE(va_int.size() == 3 && va_int.front().size() == 3 && va_int[2][0] == 31 && va_int[2][1] == 32 &&
              va_int[2][2] == 33);

  std::vector<std::array<int, 2>> va_int_2;
  EXPECT_FALSE(call("n1/n4/vv2", va_int_2));

  std::vector<std::vector<double>> vv_double;
  EXPECT_TRUE(call("n1/n4/vv2", vv_double, true));
  EXPECT_TRUE(vv_double.size() == 3 && vv_double.front().size() == 3 && vv_double[0][0] == 11 &&
              vv_double[0][1] == 12 && vv_double[0][2] == 13);
  EXPECT_TRUE(vv_double.size() == 3 && vv_double.front().size() == 3 && vv_double[1][0] == 21 &&
              vv_double[1][1] == 22 && vv_double[1][2] == 23);
  EXPECT_TRUE(vv_double.size() == 3 && vv_double.front().size() == 3 && vv_double[2][0] == 31 &&
              vv_double[2][1] == 32 && vv_double[2][2] == 33);

  EXPECT_TRUE(call("n1/n4/vv2", vv_double, false));

  EXPECT_TRUE(call("n1/n4/vv2", me_double));
  EXPECT_TRUE(me_double.rows() == 3 && me_double.cols() == 3 && me_double(0, 0) == 11 && me_double(0, 1) == 12 &&
              me_double(0, 2) == 13);
  EXPECT_TRUE(me_double.rows() == 3 && me_double.cols() == 3 && me_double(1, 0) == 21 && me_double(1, 1) == 22 &&
              me_double(1, 2) == 23);
  EXPECT_TRUE(me_double.rows() == 3 && me_double.cols() == 3 && me_double(2, 0) == 31 && me_double(2, 1) == 32 &&
              me_double(2, 2) == 33);

  EXPECT_TRUE(call("n1/n4/vv3", vv_double, true));
  EXPECT_TRUE(vv_double.size() == 2 && vv_double.front().size() == 3 && vv_double[0][0] == 11.1 &&
              vv_double[0][1] == 12.2 && vv_double[0][2] == 13.3);
  EXPECT_TRUE(vv_double.size() == 2 && vv_double.front().size() == 3 && vv_double[1][0] == 21.1 &&
              vv_double[1][1] == 22.2 && vv_double[1][2] == 23.3);

  EXPECT_TRUE(call("n1/n4/vv3", vv_double, false));

  EXPECT_TRUE(call("n1/n4/vv3", me_double));
  EXPECT_TRUE(me_double.rows() == 2 && me_double.cols() == 3 && me_double(0, 0) == 11.1 && me_double(0, 1) == 12.2 &&
              me_double(0, 2) == 13.3);
  EXPECT_TRUE(me_double.rows() == 2 && me_double.cols() == 3 && me_double(1, 0) == 21.1 && me_double(1, 1) == 22.2 &&
              me_double(1, 2) == 23.3);

  EXPECT_FALSE(call("n1/n4/vv3", me_double_21));
}

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);

  const std::string& yaml_file_root_path = TEST_DIR;
  std::string yaml_file{ yaml_file_root_path + "/tests/config.yaml" };

  std::cout << "Reading yaml parameter files located at" << yaml_file << std::endl;

  if (!std::filesystem::exists(yaml_file) ||
      !(std::filesystem::is_regular_file(yaml_file) || std::filesystem::is_symlink(yaml_file)))
  {
    std::cerr << "Could not open param file " << yaml_file << std::endl;
    ;
  }
  else
  {
    // load the YAML file using the external rosparam command
    node = YAML::LoadFile(yaml_file);
  }

  return RUN_ALL_TESTS();
}
