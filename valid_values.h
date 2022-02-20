#ifndef VALID_VALUES_H
#define VALID_VALUES_H
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <memory>
#include <variant>
#include <optional>
#include <chrono>
#include <functional>
#include "global_entities.h"
#include "print_functions.h"

using std::cerr, std::cout;

class LengthRange { // invariants: min <= max., both >= 0;
public:
    size_t 				min 	{0};
    size_t 				max 	{60};
    void print() const {
       std::cerr << "LenghtRange{min: "<<min<<", max:"<<max<<"}";
    }
};
std::ostream &operator<<(std::ostream &os, const LengthRange &pd);

class IntegerValueRange { // invariants: min <= max., both >= 0;
public:
    Data_type_integer 	min   	{ std::numeric_limits<Data_type_integer>::min() };
    Data_type_integer 	max   	{ std::numeric_limits<Data_type_integer>::max() };
    int			  		step 	{};
    bool 				is_check_value_between	{true};  // false represents excluding the range.
    void print() const {
#define THIS_ this
       std::cerr << "IntegerValueRange{min: "<<THIS_->min<<", max:"<<THIS_->max<<", step: "<<THIS_->step<<", is_check_value_between: "<<THIS_->is_check_value_between<<"}";
#undef THIS_
    }
};
std::ostream &operator<<(std::ostream &os, const IntegerValueRange &pd);

class DecimalValueRange { // invariants: min <= max., both >= 0;
public:
    Data_type_decimal 	min			{ std::numeric_limits<Data_type_decimal>::lowest() };
    Data_type_decimal 	max			{ std::numeric_limits<Data_type_decimal>::max()    };
    int 				precision 	{2};
    short				significant_values { std::numeric_limits<Data_type_decimal>::digits10 };  // todo: should this be ::digits, without the 10?
    int			  		step 		{0};				// todo: implement this.
    bool 				is_check_value_between	{true}; // false represents excluding the range.
    void print() const {
#define THIS_ this
       std::cerr << "DecimalValueRange{min: "<<THIS_->min<< ", max:"<<THIS_->max<<", precision: "<<THIS_->precision<<", significant values: "<<THIS_->significant_values<<",  step: "<<THIS_->step<<
            ", is_check_value_between: "<<THIS_->is_check_value_between<<"}";
#undef THIS_
    }
};
std::ostream &operator<<(std::ostream &os, const DecimalValueRange &pd);

class ScientificValueRange { // invariants: min <= max., both >= 0;
public:
    Data_type_scientific 	min			{ std::numeric_limits<Data_type_scientific>::lowest() };
    Data_type_scientific 	max			{ std::numeric_limits<Data_type_scientific>::max()    };
    //int 				precision 	{2};
    short				significant_values { std::numeric_limits<Data_type_scientific>::digits10 };  // todo: should this be ::digits, without the 10?
    int			  		step 		{0};				// todo: implement this.
    bool 				is_check_value_between	{true}; // false represents excluding the range.
    void print() const {
#define THIS_ this
       //std::cerr << "ScientificValueRange{min: "<<THIS_->min<< ", max:"<<THIS_->max<<", precision: "<<THIS_->precision<<", significant values: "<<THIS_->significant_values<<",  step: "<<THIS_->step<<
       std::cerr << "ScientificValueRange{min: "<<THIS_->min<< ", max:"<<THIS_->max<<", significant values: "<<THIS_->significant_values<<",  step: "<<THIS_->step<<
            ", is_check_value_between: "<<THIS_->is_check_value_between<<"}";
#undef THIS_
    }
};
std::ostream &operator<<(std::ostream &os, const ScientificValueRange &pd);

enum class CharacterCaseHandling {
    none,
    force_upper,
    force_lower,
    force_initial_cap,
    force_initial_capitals
};
enum class DefaultHandling {
    default_is_to_approve,  // implies a bool
    default_is_to_deny,  // implies a bool
    default_is_provided,
    default_is_to_notify, // accept any keypress, just a notification.
    na
};

const std::string 			NOT_USER_PROVIDED {"NOTAVAILABLE"};  // the user can type in this value and it will be considered as a 'skip one value', or 'NULL', or user did not respond.
const std::string 			YES 			  {"Yes"};  // todo: TODO why can't this be a constexpr?  also can't be a string_view.
const std::string 			NO  			  {"No"};
const std::set<std::string> VALUE_DEFAULT_APPROVE_EQUIVALENTS { YES,"yes","Y","y","true"};
const std::set<std::string> VALUE_DEFAULT_DENY_EQUIVALENTS    { NO,"no","N","n","false"};

enum class ValidValuesHandling {
    validate_membership_valid_values,  // data must match one valid value
    no_membership_validation,  // but we do check lengths.
    no_valid_values_provided
};
enum class ValidationFieldError {  // https://developer.mozilla.org/en-US/docs/Web/API/ValidityState
    value_missing,
    regex_mismatch,
    range_overflow,
    range_underflow,
    range_not_between,
    range_error, // for stoi()
    precision_excess,  //  too many digits after decimal point.
    unable_to_convert, // for stoi()
    step_mismatch,
    too_long,
    too_short,
    system_calculated,  // user input cannot write this value.
    type_mismatch,    // as per mozilla
    bad_input         // as per mozilla
    /*  equal to // ??
        not equal to
        greater than
        less than
        greater than or equal to
        less than or equal to */
};

struct ValidationReport {
    size_t			row_index {};
    size_t			field_index {};
    std::string 	error_message {};
};

enum class ValidityHandlingDialog {  // todo:  obsolete
    default_is_to_approve,  // implies a bool
    default_is_to_deny,  // implies a bool
    default_is_provided,
    default_is_to_notify, // accept any keypress, just a notification.
    // above used as a logical group for handling defaults, below specify 2 cases, but not defaults.  todo:  could make this better with invariants in ValidityCritera...  todo:  could make this better with invariants in ValidityCritera...
    check_valid_values,
    no_specific_value_checking  // but we do check lengths.
};

//enum class DefaultFieldHandling {  // todo: obsolete, notify is just an alphanum with length zero.
//    default_is_to_approve,  // implies a bool
//    default_is_to_deny,  // implies a bool
//    default_is_provided,
//    default_is_to_notify, // accept any keypress, just a notification.
//};

class ValidityCriteriaDialog { // invariants: values must have some values if Default_handling is default_is_provided.
public:
    LengthRange                    lengths 							 {0, std::max( YES.length(), NO.length())};  // NOTE: also applies to both values vector and defaults below!
    std::vector<std::string>       valid_values 					 {};  // remember to consider the case of the characters ie. upper/lower.
    //const std::vector<std::string> value_default_approve_equivalents { YES,"yes","Y","y"}; // note: use .at(0) for a defaulted value
    //const std::vector<std::string> value_default_deny_equivalents    { NO,"no","N","n"}; // note: use .at(0) for a defaulted value
    const std::string 		   	   value_default_approve_prompt      { YES };
    const std::string 			   value_default_deny_prompt 		 { NO };
    const std::string 			   value_default_provided 			 {"FILL THIS VALUE if used!"}; // a little debugging help.  only a default if length > 0.
    ValidityHandlingDialog         default_handling 				 { ValidityHandlingDialog::no_specific_value_checking };
    void print() const {
    }
};
class ValidValueBoolean {
public:
    Data_type_boolean		value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    bool					is_default;  // Invariant: No more than one value in the container should be true.
    void print() const {
        cerr << "ValidValueBoolean::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<", is_default:"<<is_default<<". ";
    }
};
class ProhibitedValueBoolean {
public:
    Data_type_boolean 		value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    void print() const {
        cerr << "ProhibitedValueBoolean::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<". ";
    }
};
class ValidValueAlphanum {
public:
    Data_type_alphanumeric 	value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    bool					is_default;  // Invariant: No more than one value in the container should be true.
    void print() const {
        cerr << "ValidValueAlphanum::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<", is_default:"<<is_default<<". ";
    }
};
class ProhibitedValueAlphanum {
public:
    Data_type_alphanumeric 	value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    void print() const {
        cerr << "ProhibitedValueAlphanum::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<". ";
    }
};

class ValidValueInteger {
public:
    Data_type_integer 		value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    bool					is_default;  // Invariant: No more than one value in the container should be true.
    void print() const {
        cerr << "ValidValueInteger::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<", is_default:"<<is_default<<". ";
    }
};
class ProhibitedValueInteger {
public:
    Data_type_decimal 		value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    void print() const {
        cerr << "ProhibitedValueInteger::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<". ";
    }
};
class ValidValueDecimal {
public:
    Data_type_decimal 		value {};
    bool					is_enabled;  // Allows for us to disable values depending on business rules.
    bool					is_default;  // Invariant: No more than one value in the container should be true.
    void print() const {
        cerr << "ValidValueDecimal::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<", is_default:"<<is_default<<". ";
    }
};
class ProhibitedValueDecimal {
public:
    Data_type_decimal 	value {};
    bool			is_enabled;  // Allows for us to disable values depending on business rules.
    void print() const {
        cerr << "ProhibitedValueDecimal::value: "<<value<<", "<<", is_enabled:"<<is_enabled<<". ";
    }
};

template< typename T >
struct ValidValueCompare {
    bool operator() (const T &  lhs, const T & rhs ) const { return lhs.value < rhs.value; }
};

template< typename T >
struct ProhibitedValueCompare {
    bool operator() (const T &  lhs, const T & rhs ) const { return lhs.value < rhs.value; }
};

using ValidValueBoolean_set 		= std::set< ValidValueBoolean, 		ValidValueCompare<ValidValueBoolean> >;
using ValidValueAlphanum_set 		= std::set< ValidValueAlphanum, 	ValidValueCompare<ValidValueAlphanum> >;
using ValidValueInteger_set 		= std::set< ValidValueInteger, 		ValidValueCompare<ValidValueInteger> >;
using ValidValueDecimal_set 		= std::set< ValidValueDecimal, 		ValidValueCompare<ValidValueDecimal> >;
using ProhibitedValueBoolean_set 	= std::set< ProhibitedValueBoolean, ProhibitedValueCompare<ProhibitedValueBoolean> >;
using ProhibitedValueAlphanum_set 	= std::set< ProhibitedValueAlphanum,ProhibitedValueCompare<ProhibitedValueAlphanum> >;
using ProhibitedValueInteger_set 	= std::set< ProhibitedValueInteger, ProhibitedValueCompare<ProhibitedValueInteger> >;
using ProhibitedValueDecimal_set 	= std::set< ProhibitedValueDecimal, ProhibitedValueCompare<ProhibitedValueDecimal> >;

using ValidValueSet_var		 = std::variant<
    ValidValueBoolean_set,
    ValidValueAlphanum_set,
    ValidValueDecimal_set,
    ValidValueInteger_set
>;

using ProhibitedValueSet_var = std::variant<
    ProhibitedValueBoolean_set,
    ProhibitedValueAlphanum_set,
    ProhibitedValueInteger_set,
    ProhibitedValueDecimal_set
>;

template< typename T>
void do_vv( T const & vs) {
    for (auto & v: vs) {
/*        struct ValuesVisitor {
//            void operator() (ValidValueBoolean & vv) {
//                cerr << "value:"<<vv.value<<", is_enabled:"<<vv.is_enabled<<", is_default:"<<vv.is_default;
//            }
//            void operator() (ValidValueAlphanum & vv) {
//                cerr << "value:"<<vv.value<<", is_enabled:"<<vv.is_enabled<<", is_default:"<<vv.is_default;
//            }
//            void operator() (ValidValueInteger & vv) {
//                cerr << "value:"<<vv.value<<", is_enabled:"<<vv.is_enabled<<", is_default:"<<vv.is_default;
//            }
//            void operator() (ValidValueDecimal & vv) {
//                cerr << "value:"<<vv.value<<", is_enabled:"<<vv.is_enabled<<", is_default:"<<vv.is_default;
//            }
//        };
//        std::visit( ValuesVisitor(), v );
*/
        cerr << "value:"<<v.value<<", is_enabled:"<<v.is_enabled<<", is_default:"<<v.is_default;
    }
}
template< typename T>
void do_pv( T const & vs) {
    for (auto & v: vs) {
        cerr << "value:"<<v.value<<", is_enabled:"<<v.is_enabled;
    }
}
template< typename T >
void print_vv( T const values_set_var) {
        struct SetVisitor {
            void operator() (ValidValueBoolean_set const & vs) const { do_vv(vs); }
            void operator() (ValidValueAlphanum_set const & vs) const { do_vv(vs); }
            void operator() (ValidValueInteger_set const & vs) const { do_vv(vs); }
            void operator() (ValidValueDecimal_set const & vs) const { do_vv(vs); }
        };
        std::visit( SetVisitor(), values_set_var );
}

template< typename T >
void print_pv( T const values_set_var) {
        struct SetVisitor {
            void operator() (ProhibitedValueBoolean_set const & vs) const { do_pv(vs); }
            void operator() (ProhibitedValueAlphanum_set const & vs) const { do_pv(vs); }
            void operator() (ProhibitedValueInteger_set const & vs) const { do_pv(vs); }
            void operator() (ProhibitedValueDecimal_set const & vs) const { do_pv(vs); }
        };
        std::visit( SetVisitor(), values_set_var );
}

class ValidityFieldSpecBase {
public:
    LengthRange					lengths_input			{};	   // used to specifiy the lengths that are acceptable during input, even for a number we don't want 10000 digits, so we limit the length.
    bool  		  				is_strip_control_chars 	{true};  // remove everything except human readable alphanumeric and normal punctuation.  https://en.cppreference.com/w/cpp/string/byte/isprint, isspace, isgraph

    std::unordered_set<char>	prohibited_characters	{};
    ValidValueSet_var 			valid_values 			{};	// NOTE: numeric checks are presented as a string and converted, remember to consider the case of the characters ie. upper/lower  // todo: complete this: add this to constructor!
    bool						is_valid_values_enabled	 {false};  // does not imply that there are valid values value.
    bool						is_default_value_enabled {false};  // does not imply that there is a default value.
    ProhibitedValueSet_var 		values_prohibited 		{}; // remember to consider the case of the characters ie. upper/lower  // todo: complete this: add this to constructor!
    //std::set<ValidValue_var> & 	valid_values_shared 			{};	// NOTE: numeric checks are presented as a string and converted, remember to consider the case of the characters ie. upper/lower  // todo: complete this: add this to constructor!
    //std::set<ProhibitedValue_var> & values_prohibited_shared 		{}; // remember to consider the case of the characters ie. upper/lower  // todo: complete this: add this to constructor!

    std::string   			  	regex_pattern 			{}; // could incorporate the min/max length, but that would be complicated?
    std::function<void (int)>	custom_validation_fn 	{};
    void print() const {
        cerr << " ValidityFieldSpecBase: lenghts_input:"; lengths_input.print();
        cerr << ", is_strip_control_chars:"<<is_strip_control_chars;
        cerr << ", prohibited_characters:"<<prohibited_characters;
        cerr << ", valid_values:";
        print_vv( valid_values );
        cerr << ", is_valid_values_enabled: "<<is_valid_values_enabled;
        cerr << ", is_default_value_enabled: "<<is_default_value_enabled;
        cerr << ", values_prohibited:";
        print_pv( values_prohibited );
        cerr << ", regex_pattern:"<<regex_pattern;
        cerr << ". ";
    }
};

class ValidityFieldSpecBool {
public:
    const ValidityFieldSpecBase base							  {};
   // const std::set<std::string> value_default_approve_equivalents {{YES},"yes","Y","y"};  // note: use .at(0) for a defaulted value
   // const std::set<std::string> value_default_deny_equivalents  {{NO},"no","N","n"}; 	  // note: use .at(0) for a defaulted value
    const std::string 		    value_default_approve_prompt 	{YES};
    const std::string 			value_default_deny_prompt 		{NO};
    bool 						is_ignore_case_on_validation	{true};  // todo: what does this do?  not implemented yet.
};
class ValidityFieldSpecAlphanum {
public:
    const ValidityFieldSpecBase base							{};
    const LengthRange			lengths_storage					{};
    bool 						is_ignore_case_on_validation	{true};  // todo: what does this do?  not implemented yet.
    bool 						is_password						{false};
};
class ValidityFieldSpecInteger {
public:
    const ValidityFieldSpecBase base							{};
    IntegerValueRange 			range 				{};
    bool						is_signed			{true};  // this could be redundant with min/max but could improve performance. todo: complete this: implement
};
class ValidityFieldSpecDecimal {
public:
    const ValidityFieldSpecBase base							{};
    DecimalValueRange 			range 				{};
    bool						is_signed			{true};  // this could be redundant with min/max but could improve performance. todo: complete this: implement
};
class ValidityFieldSpecScientific {			// not implemented yet.
public:
    const ValidityFieldSpecBase base							{};
    DecimalValueRange 			range 				{};  // todo: MISSING !!!!!!!!!!!!!!!!
    bool						is_signed			{true};  // this could be redundant with min/max but could improve performance. todo: complete this: implement
};
using ValidityFieldSpec_var = std::variant< ValidityFieldSpecBool, ValidityFieldSpecAlphanum, ValidityFieldSpecInteger, ValidityFieldSpecDecimal /*, ValidityFieldSpecScientific */ >;

enum class IO_field_trim {
    ws_left,
    ws_right,
    ws_both,
    ws_left_and_inter,  // also trim multiple blanks between words
    ws_right_and_inter,
    ws_both_and_inter,
    none
};
enum class IO_field_alignment {  // todo: complete this.
    left,
    right,
    center,
    none
};
enum class IO_field_echo_mode {  // todo: complete this.
    normal,
    no_echo,
    password,  				// show * for each kb_normal_char entered, shows length of password.
    password_echo_one_char, // show one char after it is entered but don't show entire password, instead show * for prior characters.
    password_echo_fixed,
    password_echo_while_editing  // forgot what I meant.
};
class PromptFieldSpecification {
public:
    IO_field_echo_mode  echo_mode   			{IO_field_echo_mode::normal};
    IO_field_alignment  alignment				{IO_field_alignment::none};
    IO_field_trim	  	trim					{IO_field_trim::ws_both};
    bool				is_show_hint			{true};
    bool				is_show_errors			{true};
    bool				is_compact				{false};
};

#endif // VALID_VALUES_H
