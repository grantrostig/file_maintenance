#include <set>
#include <strings.h>  // POSIX
#include "valid_values.h"
#include "io_table.h"
#include "action_io_row.h"

template<typename IO_field_spec_t >
bool is_check_pk_component_equal( IO_field_spec_t fs, const IO_row_variant &data_row ) {
    // invariant: assumes that the fields are NOT NULL, ie. they are correct primary keys which cannot be NULL.

    // NOTE data_row may not be the correct starting point, verify it, this may be better: &(std::get<1>(data_row).name) + fs->memory_offset_from_row_begin);
    // auto var_of_data_type_kludge = fs.data_location;
    // auto the_value_in_data_row = *(reinterpret_cast< decltype( var_of_data_type_kludge ) const *>( reinterpret_cast<char *>(&const_cast<IO_row_variant &>(data_row)) + fs.memory_offset_from_row_begin));
    auto the_value_in_data_row = *(reinterpret_cast< std::decay_t<decltype( fs.data_location )> const *>( reinterpret_cast<char *>(&const_cast<IO_row_variant &>(data_row)) + fs.memory_offset_from_row_begin));
    if constexpr ( std::is_same< std::decay_t<IO_field_spec_t>, IO_field_spec_alphanumeric >::value  ) {
        if ( strcasecmp( fs.getData_value_location().value().c_str(), the_value_in_data_row.c_str()) == 0 )  // older fn name is: stricmp()  POSIX  // todo: use c++ to do this when c++ is ready for it, including locales.
            return true;
        else
            return false;
    } else {
        if ( fs.getData_value_location().value() == the_value_in_data_row )  // todo:  floating point numbers should match exactly since the true value is entered, not calculated.  Any epsilon would be applicable to all equal values.
            return true;
        else
            return false;
    }
}

std::optional<std::vector<size_t>> get_pk_components( IO_table const & table ) {
    size_t field_index {};
    size_t num_pk_components {};
    std::vector<size_t> pk_components_found {};
    for (auto & fsv:table.spec.fields) {
        std::visit( [&field_index, &num_pk_components, &pk_components_found] (auto & fs) {
            if ( fs.is_primary_key_component  ) {
                ++num_pk_components;
                pk_components_found.push_back( field_index );
            }
            ++field_index;
        }, fsv );
    }
    if ( !pk_components_found.empty() )
        return pk_components_found;
    else
        return std::nullopt;
}

std::optional<std::vector<ValidationReport>> IO_table::check_rows_for_duplicate_pk( std::optional<size_t> exempted_row_index ) {
    std::vector< ValidationReport > validation_reports 	{};
    if ( auto pk_components_opt = get_pk_components( *this ) ) {
        size_t num_pk_components = pk_components_opt.value().size();
        size_t row_index {0};
        for ( auto & data_row:rows ) {  // check primary key components for duplicate row
            if ( !exempted_row_index || ( exempted_row_index.value() != row_index )) {
                std::vector<size_t> pk_duplicate_components_found {};
                for (auto & field_index:pk_components_opt.value() ) {
                    /*if ( auto fs = std::get_if<IO_field_spec_alphanumeric>(&fsv)) {
                if ( fs->is_primary_key_component  ) {
                    ++num_pk_components;
                    if ( is_check_pk_component_equal( fs, data_row ) )
                        pk_duplicate_components_found.push_back(field_index);
                    ++field_index;
                }
            } */
                    auto fsv = spec.fields.at( field_index );
                    std::visit( [&field_index, &pk_duplicate_components_found, &data_row ] (auto & fs) {
                        if ( is_check_pk_component_equal( fs, data_row ) )
                            pk_duplicate_components_found.push_back( field_index );
                    }, fsv );
                }
                if ( !pk_duplicate_components_found.empty() && num_pk_components == pk_duplicate_components_found.size() ) {
                    for (auto d_pk:pk_duplicate_components_found )
                        validation_reports.push_back( { row_index, d_pk, " component of the key value is not unique to a row entry.  Please use a unique value."});
                    break;
                }
            }
            ++row_index;
        }
    }
    if ( !validation_reports.empty() )
        return validation_reports;
    else
        return std::nullopt;
}

std::optional<IO_row_variant> IO_table::retrieve_row( size_t const row_index ) {  // NOTE: this probably doesn't need to be an optional.  // todo:
    return rows.at(row_index);
}

std::optional<std::vector<ValidationReport> > IO_table::validate_candidate_row_infull( std::optional<size_t> exempted_row_index) {
    std::vector< ValidationReport > validation_reports {};  // NOTE: it is planned that there be more than one validation routine, although not present at this time.
    if ( auto more_reports_opt = check_rows_for_duplicate_pk( exempted_row_index ) )
        std::copy( more_reports_opt.value().begin(), more_reports_opt.value().end(), std::back_inserter(validation_reports) );
    if ( !validation_reports.empty() )
        return validation_reports;
    else
        return std::nullopt;
}

void IO_table::print_rows() const { cerr << "Table Rows:\n"; for (auto & row_var:rows ) { std::visit( [] (auto const & row) { row.print(); }, row_var ); } }

void IO_table::print_spec() const { cerr << "Table Specification:\n"; spec.print(); }

void IO_table::print() const { cerr << "Table - full info:\n"; print_spec(); print_rows(); }

void IO_table_spec::clear_row_fields_data() { for ( auto & fields_var: fields ) std::visit( []( auto & field ) ->void { field.clear_data_value(); }, fields_var ); }

// void IO_table_spec::clear_candidate_row_locations() { for ( auto & fields_var: fields ) std::visit( []( auto & field ) ->void { field.clear_data_location(); }, fields_var ); }

void IO_table_spec::print() const {
    cerr << "Table spec for table with name of: "<< name << endl;
    for (auto & spec_var:fields )
        std::visit( [] (auto const & spec) { spec.print(); }, spec_var );
    cerr << "Temporary Row (used for staging a full row before insertion into the table) for table with name of: "<< name << endl;
    std::visit( [] (auto const & row) { row->print(); }, io_candidate_row);
}

// static ValidityFieldSpecAlphanum validityFieldSpecAlphanum {};
static ValidityFieldSpec_var  validityFieldSpecName   {
    ValidityFieldSpecAlphanum {  // todo: TODO: does this need to be static?  Only used in same translation unit.
        ValidityFieldSpecBase {
            LengthRange {0,50}, true, {'\a'},
            // ValidValueAlphanum_set {}, 													/*enabled*/true, /*default*/true,
            ValidValueAlphanum_set { {"Friend1Name",true,false}, {"Friend2NameDefault",true,true}, {"Friend3Name",true,false}, {"Friend4Name",true,false}},
            /* is_valid_values_enabled */true, /* is_default_value_enabled */true,
            ProhibitedValueAlphanum_set { {"prohibit",true}, {NOT_USER_PROVIDED,true}},  // NOTE: USER_NOT_PROVIDED is prohibited because the field is NOT optional.  todo: complete this: make this an invariant.
            /*regex_pattern*/{},/*custom_validation_fn*/{}
        },
        LengthRange {0,30},
        true,   // is_ignore_case_on_validation
        false   // is_password
    }
};

static ValidityFieldSpec_var validityFieldSpecBalance { ValidityFieldSpecDecimal {
    ValidityFieldSpecBase {
        LengthRange {0,50}, true, {'\a'},
        ValidValueDecimal_set {},															/*is_valid_values_enabled*/	false,/*is_default_value_enabled*/ false,
        ProhibitedValueDecimal_set  {{99.99,true}},
        /*regex_pattern*/{},/*custom_validation_fn*/{}
    },
    DecimalValueRange {} , /* is_signed */  true
}};
static ValidityFieldSpec_var validityFieldSpecAge 	  { ValidityFieldSpecInteger {
    ValidityFieldSpecBase {
        LengthRange {0,50}, true, {'\a'},
        ValidValueInteger_set {}, 															/*is_valid_values_enabled*/	false,/*is_default_value_enabled*/ false,
        ProhibitedValueInteger_set  {{1,true}},
        /*regex_pattern*/{},/*custom_validation_fn*/ {},
    },
    IntegerValueRange {0,144,0,true}, /* is_signed */ false
}};
static ValidityFieldSpec_var validityFieldSpecIsCpp   { ValidityFieldSpecBool {
    ValidityFieldSpecBase {
        LengthRange {0,50}, true, {'\a'},
        ValidValueBoolean_set {{true,true,true}, {false,true,false}},   					/*is_valid_values_enabled*/	true,/*is_default_value_enabled*/ true,
        ProhibitedValueBoolean_set {},
        /*regex_pattern*/{},/*custom_validation_fn*/ {},
    },
    /*value_default_approve_prompt*/ {}, /*value_default_deny_prompt*/ {},/*is_ignore_case_on_validation*/ {}
}};

//std::vector< IO_field_spec_variant > io_table_persons_field_specs {
IO_field_spec_variants io_table_persons_field_specs {
    IO_field_spec_alphanumeric {
        0, 0,
"name", 				"Name", 					"full name with given name first", 	"help text.", 												"enter data here",
        LengthRange {0,60},
        &validityFieldSpecName, 	PromptFieldSpecification {}, super_user,
true /*is_primary_key_component*/, true /*is_optional*/, {} /*is_initial_focus*/, {} /*is_current_focus*/, {} /*is_greyed_out*/, {} /*is_programmer_only*/, {} /* is_system_calculated_read_only_ */,
        io_candidate_row_person.name,
        io_candidate_row_person.is_null_field
    },
    IO_field_spec_decimal {
        1, (char *)&io_candidate_row_person.balance - (char *)&io_candidate_row_person.name,
"balance", 	 			"Balance in account",		"an amount in local currency", 		"help text.", 												"enter data here",
        LengthRange {0,10},
        &validityFieldSpecBalance, 	PromptFieldSpecification {}, super_user,
false /*is_primary_key_component*/, true /*is_optional*/, {} /*is_initial_focus*/, {} /*is_current_focus*/, {} /*is_greyed_out*/, {} /*is_programmer_only*/, {} /* is_system_calculated_read_only_ */,
        io_candidate_row_person.balance,
        io_candidate_row_person.is_null_field
    },
    IO_field_spec_integer {
        2, (char *)&io_candidate_row_person.age - (char *)&io_candidate_row_person.name ,
"age", 	 				"Person's age",	        	"a number in years", 				"The number of times around the sun. Hope that helps.", 	"enter data here",
        LengthRange {0, 10},
        &validityFieldSpecAge, 		PromptFieldSpecification {}, super_user,
true /*is_primary_key_component*/, true /*is_optional*/, {} /*is_initial_focus*/, {} /*is_current_focus*/, {} /*is_greyed_out*/, {} /*is_programmer_only*/, {} /* is_system_calculated_read_only_ */,
        io_candidate_row_person.age,
        io_candidate_row_person.is_null_field
    },
    IO_field_spec_bool {
        3, (char *)&io_candidate_row_person.is_cpp_programmer - (char *)&io_candidate_row_person.name ,
"is_cpp_programmer",	"Modern C++_programmer?",	"desc", 							"help text.", 												"enter data here",
        LengthRange {0,10},
        &validityFieldSpecIsCpp, 	PromptFieldSpecification {}, super_user,
false /*is_primary_key_component*/, true /*is_optional*/, {} /*is_initial_focus*/, {} /*is_current_focus*/, {} /*is_greyed_out*/, {} /*is_programmer_only*/, {} /* is_system_calculated_read_only_ */,
        io_candidate_row_person.is_cpp_programmer,
        io_candidate_row_person.is_null_field
    },
    //IO_field_spec_tm {
    ///* 04 */ "date_of_birth",		"Date of Birth",        "desc", "help", true, 	"enter data here", "", "regex", true, IO_echo_mode::normal, IO_alignment::none, guest_user, false, false, false, false
    ///, std::shared_ptr<std::string> {&io_row_person.balance} },
    //IO_field_spec_alphanumeric {
    ///* 05 */ "optional_data",		"Optional text data",	"desc", "help", true, 	"enter data here", "", "regex", false, IO_echo_mode::normal, IO_alignment::none, super_user, false, false, false, false,
    /// { {0, 4048}, {} , std::shared_ptr<std::string> {&io_row_person.balance}},
    //IO_field_spec_bool {
    ///* 06 */ "is_opt_in", 	    		"Opted in? (Y/N)",	"desc", "help", true, 	"enter data here", "", "regex", true, IO_echo_mode::normal, IO_alignment::none, super_user, false, false, false, false
    ///, std::shared_ptr<std::string> {&io_row_person.balance} },
    //IO_field_spec_time_point {
    ///* 07 */ "system_time_stamp",	"Sytem originated time stamp","desc", "help", true, "enter data here", "", "regex", false, IO_echo_mode::normal, IO_alignment::none, super_user, true,  false, false, false
    ///, std::shared_ptr<std::string> {&io_row_person.balance} },
    //IO_field_spec_uint64 {
    ///* 08 */ "guid",				"Globally unique",	 	 "desc", "help", true, "enter data here", "", "regex", false, IO_echo_mode::normal, IO_alignment::none, super_user, true,  true, false, false
    ///, std::shared_ptr<std::string> {&io_row_person.balance} },
    // ^^ number above should match IO_ROW_FIELDS_NUM_PERSON
};

IO_table io_table_persons {
    // *** spec ***
    IO_table_spec {
      /*name*/ 						"person",
      /*full descrition*/  			"Holds a set of person records.",
      /*help text*/ 				"Persons are people who may or may not be c++ programmers.",
      /*fields - init the vector*/ 	io_table_persons_field_specs,
      /*IO_row - init one*/		   	& io_candidate_row_person,
    },
    // *** rows ***
    std::vector< IO_row_variant > { // we can optionally preload some sample data here:
//field position#: 0						1       2	3	  4    5															  	6      7   8
IO_row_person { "Grant Rostig",   			12.34, 	60, true,  {}, "Is very interested in hearing criticism of this program.", 		true , {}, 0},
IO_row_person { "Bjarne Stroustup", 		42.42, 	 0,	true,  {}, "Creator of C++, and has guided its development for many years.",false, {}, 0},
IO_row_person { "Mr. Unknown Age",   		00.01, 	{}, true,  {}, "Age is NULL!!", 												false, {}, 0},
IO_row_person { "Mr. GUID", 				-5.0,    4, true,  {}, "Negative balance", 												false, {}, 0},
IO_row_person { "Mr. Not C++ Programmer",   -5.0,  888, false, {}, "Default Age is too large!!", 									false, {}, 0},
    }
};


