#ifndef IO_ROW_H
#define IO_ROW_H
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>
#include <ctime>
#include <any>
#include <functional>
#include <bitset>
#include <map>
#include <type_traits>

#include "global_entities.h"
#include "io_field.h"

class IO_table;

enum class IO_row_spec {  // https://www.w3schools.com/html/html_form_attributes.asp
    auto_complete,
    no_validate
};

class IO_row_company { // just a stub to demonstrate the type
public:
    int kludge {};
    void print() const {}
    template< class Archive>
    void serialize( Archive & archive ) {
        archive( kludge );
    }
};
//class IO_row_person_opt {  // OBSOLETE, will delete later once we are sure of our design.
//public:
// //vector<std::unique_ptr<IO_data_type>  todo: should this be part of the design?
// // NOTE: any std::optional<> data member should NOT have an initializer providing a value! This is depeneded upon in set_field_num();
// /* 00 */ std::optional<std::string>		  name				{};
// /* 01 */ std::optional<double>				  balance			{};
// /* 02 */ std::optional<int> 	 			  age				{};    // NOTE: std::optional here allows for a "NULLable field" in the sense of SQL.
// /* 03 */ std::optional<bool>				  is_cpp_programmer {};    // todo: how to represent NOTHING or std::monostate?
// /* 04 */ std::optional<std::tm>			  date_of_birth 	{};  // this is a human readable C type, C++20 has a replacement.
// /* 05 */ std::optional<std::string>		  optional_data 	{};  // NOTE: an optional string field is not NULLable, but can be blank
// /* 06 */ std::optional<bool>   			  yesno 			{};
// /* 07 */ std::optional<std::chrono::system_clock::time_point>	system_time_stamp {};  // todo:  TODO is this even human readable in a decent format?  c++20?
// /* 08 */ std::optional<uint64_t>			  guid				{};
// // ^^ number above should match IO_ROW_FIELDS_NUM_PERSON
//    //bool set_row_field_at_num( IO_table table, unsigned short const num, std::string const & val );
//    void print() const;
//};
class IO_row_person {
public:
 //vector<std::unique_ptr<IO_data_type>  todo: should this be part of the design?
 // WARNING: data members should NOT have an initializer providing a value, because it will be ignored! Create a default value (within valid_values) instead.
 /* 00 */ std::string		  name				{};
 /* 01 */ double			  balance			{};
 /* 02 */ int 	 			  age				{};
 /* 03 */ bool				  is_cpp_programmer {};
 /* 04 */ std::chrono::system_clock::time_point	date_of_birth 	{};
 /* 05 */ std::string		  optional_data 	{};
 /* 06 */ bool   			  is_opt_in 		{};
 /* 07 */ std::chrono::system_clock::time_point	system_time_stamp {};  // todo:  TODO is this even human readable in a decent format?  c++20?
 /* 08 */ uint64_t			  guid				{};
          std::bitset<IO_ROW_FIELDS_NUM_MAX>	is_null_field		    {};  // Each bit represents one of the above fields,
          bool				  is_deleted		{false};
                // with 1 meaning the field is null and the corresponding value is not to be used. All initialized to all false.
 // ^^                ^^number above should match IO_ROW_FIELDS_NUM_PERSON, this comment is partially obsolete for now but may have relevance if we convert field specs to a std::array.
    //bool set_row_field_at_num( IO_table table, unsigned short const num, std::string const & val );
    void print() const;
    template< class Archive>
    void serialize( Archive & archive ) {
        archive(
            name,
            balance,
            age,
            is_cpp_programmer,
            date_of_birth,
            optional_data,
            is_opt_in,
            system_time_stamp,
            guid,
            is_null_field,
            is_deleted
            );
    }
};
//class IO_row_ordered_person {
//public:
//    // WARNING: data members should NOT have an initializer providing a value, because it will be ignored! Create a default value (within valid_values) instead.
//    /* 00 */ std::string		  name				{};
//    /* 01 */ double			      balance			{};
//    /* 02 */ int 	 			  age				{};    // NOTE: std::optional here allows for a "NULLable field" in the sense of SQL.
//    /* 03 */ bool				  is_cpp_programmer {};    // todo: how to represent NOTHING or std::monostate?
//    /* 04 */ std::tm			  date_of_birth 	{};  // this is a human readable C type, C++20 has a replacement.
//    /* 05 */ std::string		  optional_data 	{};  // NOTE: an optional string field is not NULLable, but can be blank
//    /* 06 */ bool   			  is_opt_in 		{};
//    /* 07 */ std::chrono::system_clock::time_point	system_time_stamp {};  // todo:  TODO is this even human readable in a decent format?  c++20?
//    /* 08 */ uint64_t			  guid				{};
//    std::bitset<IO_ROW_FIELDS_NUM_MAX>	  is_null_field		{};  // Each bit represents one of the above fields,
//    bool				  		  is_deleted		{false};
//    // with 1 meaning the field is null and the corresponding value is not to be used. All initialized to all false.
//    // ^^                ^^number above should match IO_ROW_FIELDS_NUM_PERSON, this comment is partially obsolete for now but may have relevance if we convert field specs to a std::array.
//    //bool set_row_field_at_num( IO_table table, unsigned short const num, std::string const & val );
//    void print() const;
//};
using Person_pk_t = struct {
    decltype( IO_row_person::name )    key_value_p0 {};  // part 0 of n
};

using Person_index1_key_t = struct {
    decltype( IO_row_person::name )    key_value_p0 {};  // part 0 of n
    decltype( IO_row_person::balance ) key_value_p1 {};
};

struct Person_pk_compare_t {  // todo: use variadic templates, and or combine with next struct
    bool operator()( Person_pk_t const & lhs, Person_pk_t const & rhs) const {
        return lhs.key_value_p0 < rhs.key_value_p0;  // https://foonathan.net/2018/07/ordering-relations-programming/
    }
};

struct Person_index1_compare_t {  // todo: use variadic templates
    bool operator()( Person_index1_key_t const & lhs, Person_index1_key_t const & rhs) const {
        return std::tie(lhs.key_value_p0, lhs.key_value_p1) < std::tie(rhs.key_value_p0, rhs.key_value_p1);  // https://foonathan.net/2018/07/ordering-relations-programming/
    }
};

/*template< typename Key_t, typename Data_t, typename Compar_t, typename Unique_t >
//class IO_row_index2 {
//public:
//    std::variant< std::map	   < Key_t, Data_t, Compar_t >,
//                  std::multimap< Key_t, Data_t, Compar_t >
//                > row_var;
//    Key_t key_value {};
//    IO_row_index() {
//        if constexpr ( std::is_same<Unique_t,bool>::value ) {  // todo: TODO a better way to do this?
//            std::map< Key_t, Data_t, Compar_t > rows;
//            row_var = rows;  // todo: TODO is this a memory leak or dangling pointer?
//        } else {
//            std::multimap< Key_t, Data_t, Compar_t > rows;
//            row_var = rows;
//        }
//    }
//    void print() const {}
//};
*/

extern IO_row_person io_candidate_row_person;

//using IO_row_company_map_pk =  IO_row_map<Person_index1_key_t, IO_row_person, Person_index1_compar_t>;  // NOTE ERROR is "person" this is just a stub to allow it's use in std::variant below.
//extern IO_row_company_map_pk io_row_company_map_pk;

//using IO_row_person_map_pk =  IO_row_map< decltype(IO_row_person::name), IO_row_person, Person_pk_compar_t>;
//extern IO_row_person_map_pk io_row_person_map_pk;

//using IO_row_person_index1 =  IO_row_map<Person_index1_key_t, IO_row_person, Person_index1_compar_t>;
//extern IO_row_person_index1 io_row_person_index1;

/* Holds IO_fields for a row during data entry as it is validated and collected.
 * And acts as location to place data into the struct/record/row of the table.
 */

using IO_row_variant = std::variant<
    IO_row_company,
    IO_row_person
>;

using IO_row_ptr_variant = std::variant<
    IO_row_company *,
    IO_row_person  *
>;

using IO_row_index_variant = std::variant<
    Person_pk_t,
    Person_index1_key_t
>;

using IO_row_index_compare_variant = std::variant<
    Person_pk_compare_t,
    Person_index1_compare_t
>;

// std::map< Person_pk_t, 			IO_row_person,  Person_pk_compare_t > rows_ordered {};
// std::map< IO_row_index_variant, IO_row_variant, Person_pk_compare_t > rows_ordered2 {};

//using IO_row_map_pk_variant = std::variant<
    //IO_row_company_map_pk,
    //IO_row_person_map_pk
//>;

#endif // IO_ROW_H
