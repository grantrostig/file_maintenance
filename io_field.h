#ifndef IO_FIELD_H
#define IO_FIELD_H
#include <string>
#include <vector>
#include <set>
#include <variant>
#include <chrono>
#include <any>
#include <memory>
#include <limits>
#include <bitset>
#include <type_traits>
#include "global_entities.h"
//#include "lib_tty.h"
#include "valid_values.h"
#include "interaction_result.h"
class IO_table;

enum class Security_role {
    read_person,
    read_write_person,
    write_person,
    backup_person,
    restore_person,
    admin_person,
    unknown
};
enum class Security_region {
    a,
    b,
    c,
    d,
    e,
    a_and_e, 	// ugly yup!
    all,
    unknown
};
enum class Security_level {
    public_info,
    private_info,
    confidential,
    secret,
    top_secret,
    unknown
};
enum class Security_action {
    read,
    write,
    update,
    execute,
    all,
    unknown
};
class Security_spec {
public:
    std::vector< Security_role >   roles   { Security_role	::admin_person };
    std::vector< Security_region > regions { Security_region::all };
    std::vector< Security_level >  levels  { Security_level	::top_secret };
    std::vector< Security_action > actions { Security_action::all };
};

extern const Security_spec super_user;
extern const Security_spec guest_user;

/*  universal_commands <CTRL-ENTER>
    https://www.w3schools.com/html/html_form_attributes.asp
    autocomplete
    autofocus
    form
    formaction
    formenctype
    formmethod
    formnovalidate
    formtarget
    height and width
    list
    min and max
    multiple
    pattern (regexp)
    placeholder
    required
    step
    ----
    between
    not between
    equal to
    not equal to
    greater than
    less than
    greater than or equal to
    less than or equal to
    like
    ** and **
    ** OR **
*/

enum class IO_data_type {  // will not be used after fixup that is needed. replaced by ones below.
    boolean,
    character,  // char
    uint64,
    time_point,
    tm,
    alphanumeric,
    integer,
    decimal,
    scientific,

    integer_long,
    integer_long_long,
    decimal_long
};
#define IO_FIELD_SPEC_CONSTRUCTOR_VARS \
        size_t field_index_in_row_, \
        long memory_offset_from_row_begin_, \
        std::string id_name_, 							std::string user_prompt_,     				std::string full_description_, 		std::string help_,         				std::string pre_fill_value_, \
        LengthRange lengths_input_,				 		ValidityFieldSpec_var * validity_spec_, \
        PromptFieldSpecification prompt_field_spec_, 	Security_spec security_spec_,  		 		bool is_primary_key_component_,							bool is_optional_, \
        bool is_initial_focus_, 						bool is_current_focus_,           			bool is_greyed_out_,				bool is_programmer_only_,  				bool is_system_calculated_read_only_

#define IO_FIELD_SPEC_CONSTRUCTOR_ASSIGN \
        field_index_in_row(field_index_in_row_), \
        memory_offset_from_row_begin(memory_offset_from_row_begin_), \
        id_name(id_name_), 								user_prompt( user_prompt_ ),     			full_description(full_description_), help(help_),         					pre_fill_value(pre_fill_value_), \
        lengths_input(lengths_input_), 			 		validity_spec( validity_spec_), \
        prompt_field_spec(prompt_field_spec_), 			security_spec(security_spec_),  			is_primary_key_component(is_primary_key_component_),	is_optional(is_optional_), \
        is_initial_focus(is_initial_focus_), 			is_current_focus(is_current_focus_),       	is_greyed_out(is_greyed_out_),		is_programmer_only(is_programmer_only_), is_system_calculated_read_only( is_system_calculated_read_only_)

#define IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE \
        field_index_in_row = field_index_in_row_; \
        memory_offset_from_row_begin = memory_offset_from_row_begin_; \
        id_name = id_name_; 							user_prompt =  user_prompt_ ;     			full_description = full_description_; 	help = help_;         				pre_fill_value = pre_fill_value_; \
        lengths_input = lengths_input_; 			 	validity_spec = validity_spec_; \
        prompt_field_spec = prompt_field_spec_; 		security_spec = security_spec_;  	    	is_primary_key_component=is_primary_key_component_;		is_optional = is_optional_; \
        is_initial_focus = is_initial_focus_; 			is_current_focus = is_current_focus_;       is_greyed_out = is_greyed_out_;		is_programmer_only = is_programmer_only_; is_system_calculated_read_only =  is_system_calculated_read_only_;

class IO_field_spec {
public:
    size_t					field_index_in_row		{};  // used as index into candidate_row.is_null_value[] bools.
    std::string	  			id_name   			 	{"FILL id name in!"};
    std::string	  			user_prompt 		    {"FILL user_prompt text in!"};
    std::string	  			full_description 	    {"FILL full description in!"};
    std::string	  			help 			        {"FILL help in!"};
    std::string   		    pre_fill_value 			{}; // appears in the field to give help, but disappears as soon as someone starts typing.  // actually we are using full description in this manner.  todo: resolve issue.  // not used yet.
    LengthRange				lengths_input			{0,60};	   // used to specifiy the lengths that are acceptable during input, even for a number we don't want 10000 digits, so we limit the length.
    ValidityFieldSpec_var	* validity_spec			{nullptr};
    PromptFieldSpecification prompt_field_spec	{};  // not used yet.
    Security_spec    		security_spec 			{ super_user };
    bool 		  			is_primary_key_component {false}; // When all pk components are concatinated, they designate what must be unique. A pk component must not be nullable, therefore not is_optional. If all are true then no fully duplicate rows are allowed. // todo: complete this invariant.
    bool 		  			is_optional 			{false};  // todo: implemented?? Means that the user need not respond to this field. That might imply that the field would have to be nullable (especially in the case of bool and numbers), but not so much for std::string, since we might consider "" to be considered to NULL or "not answered".  On the other hand if it had a default value then it would not be null in fact.
    bool 		  			is_initial_focus 	 	{false};  // not used yet.
    bool 		  			is_current_focus 	 	{false};  // not used yet.
    bool		  			is_greyed_out 	 		{false};  // not used yet.
    bool 	      			is_programmer_only 		{false};  // used by the computer program only, not visible to any user!!
    bool		  			is_system_calculated_read_only {false};  // field is read only for users, but programmer of course can set a value for this field.
    bool					is_data_value_null		{};			// NOTE: not used.
    long					memory_offset_from_row_begin {};    // NOTE: for the first field in a row, the offset is 0 since there is no prior field in the row, for all other field's value, the value is the sum of the size_of proir values in the row.
    virtual void 			print() const;
    IO_field_spec() {}
    IO_field_spec(
            IO_FIELD_SPEC_CONSTRUCTOR_VARS
            ):
            IO_FIELD_SPEC_CONSTRUCTOR_ASSIGN
            {}
/*        std::string id_name_, 					std::string user_prompt_,     				 std::string full_description_, 		std::string help_,         				std::string pre_fill_value_,
//        LengthRange lengths_input_,				 ValidityFieldSpec_var validity_spec_,
//        PromptFieldSpecification prompt_field_spec_, Security_spec security_spec_,  		bool is_optional_,
//        bool is_initial_focus_, 				bool is_current_focus_,           			 bool is_greyed_out_,					bool is_programmer_only_,  				bool is_system_calculated_read_only_

//        ): id_name(id_name_), 					user_prompt( user_prompt_ ),     			 full_description(full_description_), 	help(help_),         					pre_fill_value(pre_fill_value_),
//        lengths_input(lengths_input_), 			 validity_spec(validity_spec_),
//        prompt_field_spec(prompt_field_spec_), 			security_spec(security_spec_),  	is_optional(is_optional_),
//        is_initial_focus(is_initial_focus_), 	is_current_focus(is_current_focus_),           	is_greyed_out(is_greyed_out_),		is_programmer_only(is_programmer_only_),  is_system_calculated_read_only( is_system_calculated_read_only_)
      */
    virtual ~IO_field_spec() {}  // todo: I'm guessing... TODO  do I need delete or =0? rule of whatever?
};
// todo: complete this: validate() fixup() functions - preprocess the field to for example remove ()- on a phone number or credit card.
class IO_field_spec_bool 			: public IO_field_spec {
public:
    Data_type_boolean & 				data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_boolean					data_value_search_first			{};
    Data_type_boolean					data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location) > first {};
        std::remove_reference_t< decltype (data_location) > last {};
    } data_range ;
    IO_field_spec_bool(
            IO_FIELD_SPEC_CONSTRUCTOR_VARS
            , Data_type_boolean & data_location_
            , std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location_
            ):
            data_location( data_location_ )
            , is_null_field_location( is_null_field_location_ )
            { IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE }
    void print() const override final;
    // IO_field_spec_bool() = default;
    void 								clear_data_value();

    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    std::optional<Kb_regular_value> 	getData_value_str() const;
    void 								setData_value(const Data_type_boolean &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_bool::Data_range 		getData_range() const;
    //std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const candidate );
    std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const candidate );
};
class IO_field_spec_character 		: public IO_field_spec {
public:
    Data_type_character & 									data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> 						& is_null_field_location			;
    Data_type_character										data_value_search_first			{};
    Data_type_character										data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    void print() const override final;
    //IO_field_spec_character(
    void 								clear_data_value();
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void setData_value(const Data_type_character &value);
    std::optional<Kb_regular_value>     getData_value_str() const;
    void 								setData_range(const Data_range &value);
    IO_field_spec_character::Data_range 		getData_range() const;
};
class IO_field_spec_uint64  		: public IO_field_spec { // c++ type: to be used for GUI and similar
public:
    Data_type_uint64 & 						data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_uint64						data_value_search_first			{};
    Data_type_uint64						data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    void print() const override final;
    //IO_field_spec_uint64(
    void 								clear_data_value();
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void                                setData_value(const Data_type_uint64 &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_uint64::Data_range 		getData_range() const;
};
class IO_field_spec_time_point 		: public IO_field_spec {  // c++ type // is_future_date_allowed, is_weekdays_only, is_friday..., within_xxxx_year/month
public:
    Data_type_time_point & 				 	data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_time_point					data_value_search_first			{};
    Data_type_time_point					data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    void print() const override final;
    //IO_field_spec_time_point(
    void 								clear_data_value();
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(const Data_type_time_point &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_time_point::Data_range 		getData_range() const;
};
class IO_field_spec_tm 				: public IO_field_spec {         // c time type
public:
    Data_type_tm &				 		data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_tm						data_value_search_first			{};
    Data_type_tm						data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    void print() const override final;
    // IO_field_spec_tm(
    void 								clear_data_value();
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(const Data_type_tm &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_tm::Data_range 		getData_range() const;
};
class IO_field_spec_alphanumeric 	: public IO_field_spec { 	// std::string
public:
    Data_type_alphanumeric & 			data_location						;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    std::remove_reference_t< decltype (data_location)> search_filter {""}; // empty string searches all values.
    Data_type_alphanumeric				data_value_search_first							{};
    Data_type_alphanumeric				data_value_search_last							{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    void print() const override final;
    // IO_field_spec_alphanumeric() = default;
    IO_field_spec_alphanumeric(
        //std::string id_name_, 					std::string user_prompt_,     				 std::string full_description_, 		std::string help_,         				std::string pre_fill_value_, \
        //LengthRange lengths_input_,				 ValidityFieldSpec_var  * validity_spec_, \
        //PromptFieldSpecification prompt_field_spec_, Security_spec security_spec_,  		bool is_optional_, \
        //bool is_initial_focus_, 				bool is_current_focus_,           			 bool is_greyed_out_,					bool is_programmer_only_,  				bool is_system_calculated_read_only_
            IO_FIELD_SPEC_CONSTRUCTOR_VARS
            , Data_type_alphanumeric & data_location_
            , std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location_
            ): data_location(data_location_)
            , is_null_field_location(is_null_field_location_)
             { IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE }
/*        {
//        id_name = id_name_; 					user_prompt =  user_prompt_ ;     			 full_description = full_description_; 	help = help_;         					pre_fill_value = pre_fill_value_; \
//        lengths_input = lengths_input_; 			 validity_spec = validity_spec_ ; \
//        prompt_field_spec = prompt_field_spec_; 			security_spec = security_spec_;  	is_optional = is_optional_; \
//        is_initial_focus = is_initial_focus_; 	is_current_focus = is_current_focus_;           	is_greyed_out = is_greyed_out_;		is_programmer_only = is_programmer_only_;  is_system_calculated_read_only =  is_system_calculated_read_only_;
//    	}*/
    void 								clear_data_value();
    //std::optional<ValidationFieldError> validate_data( Data_type_alphanumeric const & candidate );
    std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const & candidate);
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(const Data_type_alphanumeric &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_alphanumeric::Data_range 		getData_range() const;
};
class IO_field_spec_integer 		: public IO_field_spec {
public:
    Data_type_integer &					data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_integer					data_value_search_first			{};
    Data_type_integer					data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    IO_field_spec_integer(
            IO_FIELD_SPEC_CONSTRUCTOR_VARS
            , Data_type_integer & data_location_
            , std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location_
            ): data_location(data_location_)
            , is_null_field_location(is_null_field_location_)
            { IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE }
    void print() const override final;
    // IO_field_spec_integer() = default;
    void 								clear_data_value();
    //std::optional<ValidationFieldError> validate_data( Data_type_integer const candidate );
    std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const candidate );
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(Data_type_integer const & value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_integer::Data_range 		getData_range() const;
};
class IO_field_spec_decimal 		: public IO_field_spec {
public:
    Data_type_decimal & 				data_location	   	;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_decimal					data_value_search_first		   	{};
    Data_type_decimal					data_value_search_last		   	{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range ;
    IO_field_spec_decimal(
            IO_FIELD_SPEC_CONSTRUCTOR_VARS
            , Data_type_decimal & data_location_
            , std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location_
            ): data_location(data_location_)
            , is_null_field_location(is_null_field_location_)
            { IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE }
    void print() const override final;
    // IO_field_spec_decimal() = default;
    void 								clear_data_value();
    // std::optional<ValidationFieldError> validate_data(Data_type_decimal const candidate);
    std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const candidate );
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(const Data_type_decimal &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_decimal::Data_range 		getData_range() const;
};
class IO_field_spec_scientific 	: public IO_field_spec {
public:
    Data_type_scientific & 				data_location		;
    std::bitset<IO_ROW_FIELDS_NUM_MAX> & is_null_field_location			;
    Data_type_scientific				data_value_search_first			{};
    Data_type_scientific				data_value_search_last			{};
    struct Data_range {
        std::remove_reference_t< decltype (data_location)> first {};
        std::remove_reference_t< decltype (data_location)> last {};
    } data_range;
    void print() const override final;
    // IO_field_spec_scientific() = default;
    void 								clear_data_value();
    //std::optional<ValidationFieldError> validate_data(Data_type_scientific const candidate);
    std::optional<ValidationFieldError> validate_data( std::remove_reference_t<decltype (data_location)> const candidate );
    std::optional<Kb_regular_value> 	getData_value_str() const;
    using rv = std::optional< std::remove_reference_t< decltype(data_location) >>;  // todo: TODO: can this be templated outside of this derived class?
    rv 									getData_value_location() const;
    void 								setData_value(const Data_type_scientific &value);
    void 								setData_range(const Data_range &value);
    IO_field_spec_scientific::Data_range 		getData_range() const;
};

#undef IO_FIELD_SPEC_CONSTRUCTOR_VARS
#undef IO_FIELD_SPEC_CONSTRUCTOR_ASSIGN
#undef IO_FIELD_SPEC_CONSTRUCTOR_INITIALIZE

/* IO_data_to_cpp_type_map convert_string_value_to_field_type_value(IO_table const & table, unsigned short const num, std::string const & val );

//class IO_field {
//public:
//    std::any 	  field_value	{};
//    std::any 	  field_storage_location	{}; // todo: do I need this?

//    // virtual void 		setField_value(const std::any &value);
//    // virtual std::any 	getField_value() const;
//    virtual bool 	 	validate_input_value( Kb_value_plus const & value_plus ) {
//        return true;
//    }
//    virtual bool 		set_value( Kb_value_plus const & value_plus ) {
//        validate_input_value( value_plus );
//        return true;
//    }
//    virtual ~IO_field() = default;
//};

//class IO_field_bool : IO_field {
//public:
//    std::shared_ptr< IO_field_spec > io_field_spec_bool;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        if ( kb_regular_value == "Y" )
//            field_value = true;
//        else
//            field_value = false;
//        return true;
//    }
//    bool set_value(const Kb_value_plus &value_plus) override final;
//    //~IO_field_bool() = default;
//};

//class IO_field_character : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_character > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }
//    //~IO_field_bool() = default;
//};
//class IO_field_uint64 : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_uint64 > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {//    std::string	  user_prompt 		{"FILL user_prompt text in!"};

//        //    // IO_field_specs_variant field_spec { IO_field_spec_alphanumeric {}};
//        //    std::string	  full_description 	{"FILL full description in!"};
//        //    std::string	  help 			 	{"FILL help in!"};

//        //    bool 		  is_optional 		{false}; // means that the user need not respond to this field. That would imply that the field would have to be nullable (especially in the case of bool and numbers, but not so much for std::string, since we might consider "" to be considered to NULL or "not answered".

//        //    std::string   pre_fill_value 	{};
//        //    std::string   default_value 	{};  	  // is placed instead of the NULL, which is available if optional (which implies nullable).
//        //    std::string   regex_pattern 	{};      // could incorporate the min/max length, but that would be complicated?

//        //    bool          is_null_able 		{false}; // means that this field will be std::optional<> for anything except std::sting?? todo: what is difference between "" and NULL and <default>?

//        //    IO_echo_mode  echo_mode_default {IO_echo_mode::normal};
//        //    IO_alignment  alignment			{IO_alignment::none};

//        //    Security_spec security_spec  		{ super_user };
//        //    bool		  is_system_calculated_read_only {false};
//        //    bool 	      is_programmer_only 	{false};  // used by the computer program only, not visible to any user!!
//        //    bool 		  is_initial_focus 	 	{false};
//        //    bool		  is_greyed_out 	 	{false};
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }
//    //~IO_field_bool() = default;
//};
//class IO_field_time_point : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_time_point > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }
//    //~IO_field_bool() = default;
//};
//class IO_field_tm : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_tm > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }                std::visit( AssignVisitor(), table.spec.fields.at(2), ir);

//    //~IO_field_bool() = default;
//};

//class IO_field_alphanumeric : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_tm > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }
//    //~IO_field_bool() = default;
//};

//class IO_field_integer : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_integer > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;

//        io_field_spec_alphanumeric->lengths_input.max;

//        return true;
//    }
//    //~IO_field_bool() = default;
//};

//class IO_field_decimal : IO_field {
//public:
//    std::shared_ptr< IO_field_spec_decimal > io_field_spec_alphanumeric;
//    bool validate_input_value( Kb_value_plus const & value_plus ) override final {
//        auto [ kb_regular_value, hot_key, file_status ] = value_plus;
//        field_value = kb_regular_value;
//        return true;
//    }
//    //~IO_field_bool();
//};
*/

#endif // IO_FIELD_H
