#include <string>
#include <iterator>
#include "global_entities.h"
#include "action_dialog.h"
#include "state_menu.h"
#include "valid_values.h"
#include "interaction_result.h"
#include "field_navigation_interaction_map.h"
#include "window_panel.h"
#include "io_field.h"

struct Value_nup {
    std::optional<Kb_regular_value> kb_regular_value_opt {};
    Hot_key hot_key {};
    File_status file_status {};  // like Kb_value_plus but allows for NOT_VALUE_PROVIDED, by making Kb_regular_value optional to support is_optional.
};

template< typename Field_spec_T >
void prompt_for_field_data( State_menu & state, Field_spec_T const & field_spec, Kb_regular_value const & current_value, std::string const & progress_message ) {
    pagination_reset( state, { 0, 40 } ); // todo: complete this, is this reset true? 0 lines because we just use one again and again, lenght is an estimate!
    static size_t prior_prompt_length 			{};
    static size_t prior_current_value_echo_length 	{};
    static size_t prior_progress_message_length {};
    // static std::string prior_prompt 			{};
    cout << '\r';  // clear the current line of prior prompts first and then re-write everything.
    for ( auto i = prior_prompt_length + prior_current_value_echo_length + prior_progress_message_length; i-->0; )
        cout << ' ';

    Kb_regular_value current_value_echo {};
    current_value_echo.reserve( current_value.size() );
    switch ( field_spec.prompt_field_spec.echo_mode ) {
    case IO_field_echo_mode::normal:
        current_value_echo = current_value;
        break;
    case IO_field_echo_mode::no_echo:
        break;
    case IO_field_echo_mode::password:
    case IO_field_echo_mode::password_echo_while_editing:
        current_value_echo.insert( 0, current_value.size(), '*');
        break;
    case IO_field_echo_mode::password_echo_one_char:
        if ( current_value.size() > 0) {
            current_value_echo.insert( 0, current_value.size()-1, '*');
            current_value_echo += current_value.at(current_value.size());
        }
        break;
    case IO_field_echo_mode::password_echo_fixed:
        current_value_echo = "*...";
    }
    cout << '\r'<<field_spec.user_prompt<<">>"<<current_value_echo;
    if ( progress_message.length() > 0 ) {
        cout << "  {"<<progress_message<<"}";
        prior_progress_message_length = 3 + progress_message.length() + 1;
        for ( size_t i = prior_progress_message_length; i-->0; ) cout << '\b';
    }
    cout.flush();
    prior_prompt_length        = field_spec.user_prompt.length() + 2;
    prior_current_value_echo_length = current_value_echo.length();
    /* switch (validity.default_handling) {
    case ValidityHandlingDialog::default_is_to_approve:
        cout <<"(Yes/No,";
        cout <<"default="<<validity.value_default_approve_prompt << ")>>";
        break;
    case ValidityHandlingDialog::default_is_to_deny:
        cout <<"(Yes/No,";
        cout <<"default="<<validity.value_default_deny_prompt << ")>>";
        break;
    case ValidityHandlingDialog::default_is_provided:
        cout <<"(one valid value,";
        cout <<"(default="<<validity.value_default_provided << ")>>";
        break;
    case ValidityHandlingDialog::default_is_to_notify:
        cout <<">>";
        break;
    case ValidityHandlingDialog::no_specific_value_checking:
        cout <<"(any one value";
        if ( validity.lengths.min || validity.lengths.min ) cout <<" lenght min:"<<validity.lengths.min<<","<<validity.lengths.max<<")>>"<<endl; else cout<< ")>>"<<endl;
        break;
    case ValidityHandlingDialog::check_valid_values:
        cout <<"(one valid value)>>";
        break;
    }
    */
}

/* void prompt_for_dialog( State_menu & state, std::string const & prompt, ValidityCriteriaDialog const & validity ) {
    pagination_reset( state, {1,20} ); // todo: complete this, is this reset true?  20 is an estimate!
    cout << prompt;
    switch (validity.default_handling) {
    case ValidityHandlingDialog::default_is_to_approve:
        cout <<"(Yes/No,";
        cout <<"default="<<validity.value_default_approve_prompt << ")>>";
        break;
    case ValidityHandlingDialog::default_is_to_deny:
        cout <<"(Yes/No,";
        cout <<"default="<<validity.value_default_deny_prompt << ")>>";
        break;
    case ValidityHandlingDialog::default_is_provided:
        cout <<"(one valid value,";
        cout <<"(default="<<validity.value_default_provided << ")>>";
        break;
    case ValidityHandlingDialog::default_is_to_notify:
        cout <<">>";
        break;
    case ValidityHandlingDialog::no_specific_value_checking:
        cout <<"(any one value";
        if ( validity.lengths.min || validity.lengths.min ) cout <<" lenght min:"<<validity.lengths.min<<","<<validity.lengths.max<<")>>"<<endl; else cout<< ")>>"<<endl;
        break;
    case ValidityHandlingDialog::check_valid_values:
        cout <<"(one valid value)>>";
        break;
    }
}
*/

template< typename T >
void exclude_disallowed_fields( T const & field_spec ) {
    if ( field_spec.is_programmer_only || field_spec.is_system_calculated_read_only )
            throw std::logic_error( "Cannot input a system calculated field."+std::to_string(__LINE__)+":"+__FUNCTION__); // todo: complete this: code above case's.
    if ( field_spec.is_greyed_out )
            throw std::logic_error( "Cannot input a greyed out/disabled field."+std::to_string(__LINE__)+":"+__FUNCTION__); // todo: complete this: code above case's.
}

template< typename T >
Kb_regular_value existing_o_default( std::optional<T> const & default_value, std::optional<Kb_regular_value> const & existing_value ) {
    if      ( existing_value.has_value() ) {
        cerr <<"\n existing_o_default() existing_value.value(): "<< existing_value.value()<<endl;
        return existing_value.value();
    }
    else if ( default_value.has_value() ) {
        cerr <<"\n existing_o_default() default_value.value(): " << default_value.value()<<endl;
        return default_value.value();
    }
    else {
        cerr <<"\n existing_o_default() No Value."; return {}; }
}

template< typename T >
std::optional<Kb_regular_value> to_string_typed_value( T typed_value ) {
    using T2 = std::decay_t<T>;
    cerr<< "to_string_default_value():START\n";
    if ( typed_value.has_value() ) {
        if      constexpr (   std::is_same<T, std::optional<Data_type_alphanumeric>>::value ) {
            cerr<< "to_string_typed_value():"<< typed_value.value()<<endl;
            return typed_value.value();
        }
        else if constexpr (   std::is_same<T2, std::optional<Data_type_integer>>::value
                           || std::is_same<T2, std::optional<Data_type_decimal>>::value
                           || std::is_same<T2, std::optional<Data_type_scientific>>::value )
            // else if constexpr ( std::is_arithmetic<T>::value )  // todo: TODO: how to pull out the type of the optional.
            return std::to_string( typed_value.value() );
        else if constexpr (   std::is_same<T2, std::optional<Data_type_boolean>>::value )
            return typed_value.value() ? YES : NO;
        // todo: complete this: add date and other types.
    }
    cerr<< "to_string_typed_value():END without value.\n";
    return std::nullopt;
}

template< typename T >
auto to_string_typed_value2( T typed_value ) { // todo: consider combining this with prior function.  Difficult due to the std::optional<>.
    using T2 = std::decay_t<T>;
    cerr<< "to_string_default_value2():START\n";
    cerr<< "to_string_typed_value2():"<< typed_value<<endl;
    if ( true ) {
        if      constexpr (   std::is_same<T2, Data_type_alphanumeric>::value ) {
            return typed_value;
        }
        else if constexpr (   std::is_same<T2, Data_type_integer>::value
                           || std::is_same<T2, Data_type_decimal>::value
                           || std::is_same<T2, Data_type_scientific>::value )
            // else if constexpr ( std::is_arithmetic<T>::value )  // todo: TODO: how to pull out the type of the optional.
            return std::to_string( typed_value );
        else if constexpr (   std::is_same<T2, Data_type_boolean>::value )
            return typed_value ? YES : NO;
        // todo: complete this: add date and other types.
    }
    cerr<< "to_string_typed_value():END without value.\n";
    // return std::nullopt;
    return std::string {};
}

InteractionResultNav find_interaction_result_nav( Hot_key const & hot_key, InteractionCategory const cat ) {
    static bool is_verified_FNIMap {false};
    if ( !is_verified_FNIMap ) {
        fieldNavigationInteractionMap.verify();  // do this only once in the beginning of a program run.
        is_verified_FNIMap = true;
    }

    auto const v = std::find_if( fieldNavigationInteractionMap.mappings.begin(),
                                 fieldNavigationInteractionMap.mappings.end(),
                    [&hot_key, cat] (FieldNavInteractionRelation r) {
                         return (r.category == cat && r.field_nav == hot_key.f_completion_nav );
                    });
    if ( v == fieldNavigationInteractionMap.mappings.end() )
        //throw std::logic_error( "Missing entry in FNIMap."+std::to_string(__LINE__)+":"+__FUNCTION__); // todo: complete this: code above case's.
        assert(( "Missing entry in FNIMap.",false));
    else
        return v->interaction_nav;
}
/* Is this a valid action for the user to want to take?
 * Is used after the fact, where we can no longer tell the user.
 * Is used as a logic error check, ie. a post-invariant
 * todo: we should let the user know when the bad field completion hot_key is entered.
 */
bool verify_interaction_result_nav( Hot_key const & hot_key, InteractionCategory const cat ) {  // is this ever used? no?
    InteractionResultNav irn {};
    irn = find_interaction_result_nav( hot_key, cat );
    if ( irn == InteractionResultNav::na ) {
        return false;    // todo: It is too late to do this now! See above todo:. action_dialog_modal_notify( state, "Invalid hot key pressed, did you intend <ENTER>? Try again. Press <ENTER> now also, to continue."); // we discard the ir from notification.
    }
    else return true;
}

/****** returns true if we need more input from user
 */
bool process_hk_editing_mode( State_menu & state, HotKeyFunctionCat const & function_cat, /*OUT*/ bool & is_editing_mode_insert ) {
    if ( function_cat == HotKeyFunctionCat::editing_mode ) {
        is_editing_mode_insert = !is_editing_mode_insert;
        return true;
    }
    return false;
}

// NOTE: takes user input, may be the only one in this series that does.
template< typename T >
std::optional<Hot_key> process_hk_help( State_menu & state, T const & field_spec, HotKeyFunctionCat const & function_cat ) {
    if ( function_cat == HotKeyFunctionCat::help_popup ) {  // we have no value only a help hot_key
        InteractionResult ir = action_dialog_modal_notify( state, field_spec.help);  // this currently throws away any hot_keys, todo: may not be optimal
        return ir.hot_key;
    }
    return std::nullopt;
}

/****** returns true if we need more input from user
 */
bool process_intra_field_nav( FieldIntraNav const intra_f_nav, Kb_regular_value & /*INOUT*/ current_value ) {
    switch ( intra_f_nav ) {
    case FieldIntraNav::backspace_left_erase:
    case FieldIntraNav::erase_left:
        if ( current_value.length() > 0 ) // todo: complete this:  take into account the position of the cursor, not just at the end.
            current_value = current_value.substr( 0, current_value.length() - 1 );
        else
            cout << '\a';  // can't do that.
        return true;
    case FieldIntraNav::delete_char:
    case FieldIntraNav::kill:
    case FieldIntraNav::move_left:
    case FieldIntraNav::move_right:
    case FieldIntraNav::goto_begin:
    case FieldIntraNav::goto_end:
        cout << '\a';  				// todo: complete this: implement all of these.
        return true;
    case FieldIntraNav::na:
        break;						// keep going for more fun.
    } // *** end switch() ***
    return false;
}

bool check_for_numeric_type( IO_field_spec_variant const & field_spec) {
    return        ( std::holds_alternative< IO_field_spec_integer >	( field_spec ) ||
                    std::holds_alternative< IO_field_spec_decimal >	( field_spec )
                  //   || std::holds_alternative< IO_field_spec_scientific > 	( field_spec )   // todo: this.
                  );
}

bool check_for_bool_type( IO_field_spec_variant const & field_spec) {
    return        ( std::holds_alternative< IO_field_spec_bool > ( field_spec ) );
}

bool is_lone_negative_sign_o_0_length ( Kb_regular_value const & current_value, std::string /*OUT*/ & deficiency_message  ) {
    if ( current_value.length() == 0 ) {
        deficiency_message = "a zero length response is not allowed. enter a number";
        return true;
    }
    if ( current_value == "-" ) {
        deficiency_message = "a lone negative sign cannot be submitted, editing required, enter a number.";
        return true;
    }
    return false;
}

/****** Determine if user has finished with the field.  This is designated by the user pressing certain hot_keys.
 returns false if not handled or not relevant and we want to proceed with checks
 * if the valid_values feature is enabled, some hot_keys (up arrow, down arrow) have alternate meanings.
 * Returns: in general, returns true if we need more input from user
 * todo: complete this: fix documentation
 * true, 	gotten_field_data not changed (should be NULL), deficiency message updated 	- if we need another user input
 * false, 	gotten_field_data not changed (should be NULL), deficiency message unchanged	- WARNING: if valid_value processing must be done by another function.
 * false, 	gotten_field_data {actual values}, 			  deficiency message unchanged 	- if user has completed user input on this field value.
 */
bool process_field_completion_nav( State_menu						state,
                                   IO_field_spec_variant const 		& field_spec,
                                   Kb_regular_value const			& current_value,
                                   Hot_key const 					& hot_key,
                                   File_status const 				& file_status,
                                   std::optional<Value_nup> 		& /*IN&OUT*/ gotten_field_data,
                                   std::string 						& /*IN&OUT*/ deficiency_message,
                                   bool const 						  is_valid_values_enabled,		    // todo: why do I need this?
                                   bool const 						  is_user_entered_a_valid_char ) { 		// todo: why do I need this?

                                   // bool const is_check_for_a_number = false,	// todo: don't need this, could check the field_spec?
                                   // bool const is_bool               = false ) {	// todo: don't need this, could check the field_spec?

    //bool const is_numeric_field { std::is_same<std::decay_t< hot_key. , ValidityFieldSpecDecimal >::value };	// todo: don't need this, could check the field_spec?
    // if ( is_check_for_a_number && is_bool ) assert(false);  // invariant, todo: there are many more similar to this.
    Kb_regular_value 	fixed_current_value 	{};
    bool 				skip_additional_checks {false};  // Our return value, with a strange name! NOTE we also have *OUT* variables. Also shows that deficiency message has been updated, but this is not relevant to logic, but does prove we need this bool.

    // check for illegal field completion values for the InteractionCategory.
    if ( InteractionResultNav::na == find_interaction_result_nav( hot_key, state.action_top() )) {
        deficiency_message = "Invalid field completion hot_key " + hot_key.my_name + " pressed, try again.";
        return true;  // skip_additional_checks
    }

    switch ( hot_key.f_completion_nav ) {
    case FieldCompletionNav::up_one_field:
    case FieldCompletionNav::page_up:
        gotten_field_data = { {}, hot_key, file_status }; // set gotten_field_data to end field loop, throw away any value,
        skip_additional_checks = true;
        break;
    case FieldCompletionNav::browse_up:
    case FieldCompletionNav::browse_down:
        if ( is_valid_values_enabled && !is_user_entered_a_valid_char ) {
            gotten_field_data = std::nullopt;
            skip_additional_checks = false;
            break;
        }
        if ( hot_key.f_completion_nav == FieldCompletionNav::browse_up ) {  // same as ::up_one_field
            gotten_field_data = { {}, hot_key, file_status };
            skip_additional_checks = true;
        }
        // __attribute__((fallthrough));
        /* fall through */  // todo: which of these fall throughs work for which compilers and linters?
        [[fallthrough]];
    case FieldCompletionNav::down_one_field: // most likely outcome
        // *** preliminary valid integer/decimal/scientific existence (stoi... compatibility) checks can't be checked until user has submitted the field
        if        ( check_for_numeric_type( field_spec ) ) {
            if ( is_lone_negative_sign_o_0_length( current_value, deficiency_message )) {
                gotten_field_data = std::nullopt;
                skip_additional_checks = true;
            }
            else {
                gotten_field_data = { current_value, hot_key, file_status };
                skip_additional_checks = true;
            }
        } else if ( check_for_bool_type( field_spec ) ) {
            auto const itr1 = VALUE_DEFAULT_APPROVE_EQUIVALENTS.find(current_value );
            auto const itr2 = VALUE_DEFAULT_DENY_EQUIVALENTS.find(current_value );
            if ( itr1 == VALUE_DEFAULT_APPROVE_EQUIVALENTS.end() && itr2 == VALUE_DEFAULT_DENY_EQUIVALENTS.end() ) {
                deficiency_message = "value must be Y,N,y,n,yes,no,true or false." ;
                gotten_field_data = std::nullopt;
                skip_additional_checks = true;
            }
            else {
                gotten_field_data = { current_value, hot_key, file_status };
                skip_additional_checks = true;
            }
        }
        else {
            gotten_field_data = { current_value, hot_key, file_status };
            skip_additional_checks = true;
        }
        break;
    case FieldCompletionNav::eof:
        // *** force preliminary valid integer existence (stoi compatibility) checks
        if ( check_for_numeric_type( field_spec ) &&
             ( current_value.length() == 0  || current_value == "-" || current_value == "+" ) )
            fixed_current_value = "0";
        gotten_field_data = { fixed_current_value, hot_key, file_status};
        cout << "end of file recieved, processing current value, and exiting program."<<endl;
        skip_additional_checks = true;
        break;
    case FieldCompletionNav::save_form_as_is:
    case FieldCompletionNav::exit_with_prompts:
        // *** force preliminary valid integer existence (stoi compatibility) checks
        if ( check_for_numeric_type( field_spec ) &&
             ( current_value.length() == 0  || current_value == "-" || current_value == "+" ) )
            fixed_current_value = "0";
        gotten_field_data =  { fixed_current_value, hot_key, file_status};
        cout << "processing current value, and exiting current row data entry."<<endl;
        skip_additional_checks = true;  // we need type and valid_value checking!
        break;
    case FieldCompletionNav::esc:
        gotten_field_data =  { {}, hot_key, file_status};
        cout << "escape ESC received, discarding current value, and exiting current row data entry."<<endl;
        skip_additional_checks = true;  // we throw away the value and skip additional checks.
        break;
    case FieldCompletionNav::interrupt_signal:  // AKA exit_immediately
    case FieldCompletionNav::kill_signal:
    case FieldCompletionNav::quit_signal:
        gotten_field_data =  { {}, hot_key, file_status};
        cout << "operating system signal received, discarding current value, and exiting program without prompts to save data."<<endl;
        skip_additional_checks = true;  // we throw away the value and skip additional checks.
        break;
    case FieldCompletionNav::skip_one_field:
        gotten_field_data =  { {}, hot_key, file_status};
        skip_additional_checks = true;  // we throw away the value and skip additional checks.
        break;
    case FieldCompletionNav::page_down:
    case FieldCompletionNav::skip_to_end_of_fields:
        gotten_field_data =  { {}, hot_key, file_status};
        skip_additional_checks = true;  // we throw away the value and skip additional checks.
        break;
    //case FieldCompletionNav::help:
            //throw std::logic_error( "specialize_dialog_field_completion_nav():Bad FieldCompletion_Nav."+std::to_string(__LINE__)+":"+__FUNCTION__); // todo: complete this: code above case's.
    case FieldCompletionNav::na: 	// don't set-up the gotten_field_data, further processing is required by other functions
        skip_additional_checks = false;
        // don't put assert false here!!  this is legitimate, we just got a regular char.
        break;
    }
    return skip_additional_checks;
}

// returns false if not handled or not relevant and we want to proceed with checks
template< typename Itr_type >
bool process_valid_values_nav( Itr_type 				& cur_itr, 					// todo: should these be copies so I don't mess up caller?
                               Itr_type const 			& valid_value_itr_begin,
                               Itr_type const 			& valid_value_itr_end,
                               Kb_regular_value 		& /*OUT*/ current_value,
                               Hot_key const 			& hot_key,
                               std::optional<Value_nup> & /*OUT*/ gotten_field_data,
                               std::string 				& /*OUT*/ deficiency_message ) {
    bool is_continue_top_of_loop {false};  // Also shows that deficiency message has been updated, but this is not relevant to logic, but does prove we need this bool.

    switch ( hot_key.f_completion_nav ) {
    case FieldCompletionNav::browse_up:
        if ( cur_itr != valid_value_itr_begin )
            --cur_itr;
        else
            deficiency_message = "current value is top of the list.";
        if ( cur_itr != valid_value_itr_end )
            current_value = to_string_typed_value2( cur_itr->value );
        is_continue_top_of_loop = true; // on recieving an arrow hot_key, we must immediately prompt the user after this function.
        break;
    case FieldCompletionNav::browse_down:
        if ( cur_itr != valid_value_itr_end )
            ++cur_itr;
        if ( cur_itr != valid_value_itr_end )
            current_value = to_string_typed_value2( cur_itr->value );
        else {
            deficiency_message = "current value is bottom of the list.";
            --cur_itr;  // get off of end of container!!
        }
        is_continue_top_of_loop = true; // on recieving an arrow hot_key, we must immediately prompt the user after this function.
        break;
    default:
        is_continue_top_of_loop = false; // we are only working on up and down arrow, so default is to do nothing.
        break;
    }
    if ( is_continue_top_of_loop )
        gotten_field_data = std::nullopt;	// if the user selected a value using up or down arrows, then we must show the value to the user, and user must accept it via a completion hot_key.
    return is_continue_top_of_loop;
}

template< typename VSpec_type >
std::optional<std::string> check_numeric_invalidity( VSpec_type const 		 & validity_spec,
                                          Kb_regular_value const & current_value ) {  // todo: should be able to do this without templates, using typeid()
    std::string deficiency_message;
    if ( current_value != "-" && current_value != "+")
        try {  // check if more than a prefixed sign character
        // Data_type_integer narrowed_value = stoi( current_value );
        // decltype ( validity_spec.range.max ) narrowed_value = stoi( current_value );
        decltype ( validity_spec.range.max ) narrowed_value {};

        if (      std::is_same< std::decay_t<VSpec_type>,
                  ValidityFieldSpecInteger >::value )
            narrowed_value = stoi( current_value );  // todo: complete this: needs to be stol if Data_type_integer is long versus int.
        else if ( std::is_same< std::decay_t<VSpec_type>,
                  ValidityFieldSpecDecimal >::value )
            narrowed_value = stod( current_value );
        else if ( std::is_same< std::decay_t<VSpec_type>,
                  ValidityFieldSpecScientific >::value )  // todo: complete this:
            narrowed_value = stold( current_value );
        else assert(false);

        if ( ! validity_spec.is_signed &&
             narrowed_value < 0 )
            deficiency_message = "current number is negative, must be positive.";

        if ( validity_spec.range.is_check_value_between ) {
            if ( validity_spec.range.min > narrowed_value || narrowed_value > validity_spec.range.max )
                deficiency_message = "number is not between/within the range";
        }
        else {  // opposite of between
            if ( validity_spec.range.min < narrowed_value || narrowed_value < validity_spec.range.max)
                deficiency_message = "number is not outside/external to the range";
        }
    } catch ( std::invalid_argument const & ) {
        deficiency_message = "not a valid whole number";
    } catch ( std::out_of_range const & ) {
        deficiency_message = "number is beyond the type's capacity";
    }
    if ( ! deficiency_message.empty() )
        return deficiency_message;
    return std::nullopt;
}

// returns false if not handled or not relevant and we want to proceed with checks
bool process_numeric_decorators( size_t const 				length,
                                 Kb_regular_value const 	& kb_regular_value,
                                 IO_field_spec_variant const& field_spec ,
                                 std::string 				& deficiency_message ) {
    assert( kb_regular_value.length() == 1 );// allow comma,  todo: anywhere: not good.  study locales and number punctiation.
    if ( ! check_for_numeric_type( field_spec ) )
        return false;

    bool is_allowed_digit 		{false};
    bool is_excepted_decorator 	{true};
    bool is_allowed 			{true}; 	// Disallow anything but numbers except for these allowed decorators AKA excepted_decorators. at this point we have only two, but with this if/else code structure others could be added easily.

    if ( std::isdigit( static_cast<unsigned char>( kb_regular_value.at(0) )))
        is_allowed_digit = true;
    else
    {
        if ( kb_regular_value.at(0) == '-' || kb_regular_value.at(0) == '+' ) {
            if ( length == 0 ) {  // first position/leading negative sign
                is_excepted_decorator = true;
            } else {
                deficiency_message = "negative/postive sign is only allowed at begining of number." ;
                is_excepted_decorator = false;
            }
        } else {
            if ( kb_regular_value.at(0) == '.' ) {  // todo: complete this: fix for locale of '.' or ','.
                if ( std::holds_alternative< IO_field_spec_decimal >( field_spec ) )  {
                    deficiency_message = "no decimal point is allowed in a whole number." ;  // todo: complete this: fix for locale of '.' or ','.
                    is_excepted_decorator = false;
                } else {
                    is_excepted_decorator = true;
                }
            } else {  // final else provides message for !isdigit()
                deficiency_message = "typed character must be a number, or starting leading negative sign." ;
                is_allowed_digit = false;  // yes this is the default value, but assinged here for clarity.
            }
        }
    }
    if ( !is_allowed_digit && !is_excepted_decorator ) {
        is_allowed = false;
    }
    return !is_allowed;
}

/******
 */
template< typename T >
auto find_default_valid_value_itr( T const & valid_values_set ) {
    //auto compar = [](ValidValueAlphanum const & vv ){return/* vv->is_default ==*/ true;};
    //auto result = std::find( valid_values_set.begin(), valid_values_set.end(), compar );  // todo: TODO: what is wrong here? compile error on "=="
    for ( auto i = valid_values_set.begin(); i != valid_values_set.end(); i++ ) {
        if ( i->is_default == true ) return i;
    }
    // if (result != valid_values.end()) return result;
    return valid_values_set.end();
}

template< typename Vv_itr, typename End_itr/*, typename T*/ >  // todo: TODO fix this ugly hack! kludge
// Vv_itr::value_type::value_type retrieve_default( Vv_itr const & valid_value_itr, End_itr const & end_itr ) {
auto retrieve_default ( Vv_itr const & valid_value_itr, End_itr const & end_itr/*, T & value*/ ) -> std::optional<decltype (valid_value_itr->value)> {
    if ( valid_value_itr != end_itr ) {
            return valid_value_itr->value;
    }
    else
        return std::nullopt;
}

template< typename T >
bool check_prohibited_characters( T const & validity_spec, char const ch, std::string & deficiency_message ) {
    if ( auto prohibited 			 = validity_spec.base.prohibited_characters;
              prohibited.find( ch ) != prohibited.end() ) {
        cout << '\a';
        deficiency_message = "the prohibited character just entered has been ignored.";
        return true;
    }
    return false;
}

template< typename T >
bool check_max_input_length( T const 				& field_spec,
                             Kb_regular_value const & current_value,
                             std::string 			& deficiency_message ) {
     //                          bool & is_user_entered_a_valid_char  ) {
    if ( current_value.length() == field_spec.lengths_input.max ) {
        cout << '\a';
        deficiency_message = "reached maximum length, edit/change or submit it as it is.";
        return true;
    }
    return false;
}

/****** Prompt and input a value character by character from the user terminal based on a field_spec.
 * Input: 	'field_spec'
 * Output: 	'field' which optionally contains a partially validated value represented as a string.
 * Mutates: prints to terminal using pagination.
 * Calls:	- prompt_field_data() which tells user what is expected by printing to the user terminal.
 * 			- calls 'get_kb_keys_raw()' which provides a partially validated value.
 *
 * Callers:	- called directly by programmer - unlikely.
 * 			- called by an 'action_dialog_modal_io_field' function or similar.
 * The user input consists of either a character (to build a value suitable to the file spec) or a hot_key (represents the intent of the user). File_status is not used yet.
 * The characters are concatinated untill a hot_key is entered by user.
 * The hot_key prompts either:
 * 	+ editing the existing value
 * 	+ OR designated completion of the value, which can ask for storage of value, or throwing it away and exiting the function.
 * The value is partially validated at every keystroke.
 * Valid value (and additionally default value) processing changes the meaning of hot_keys: up arrow and down arrow.
 * <TAB> means NOT_USER_PROVIDED nup, meaning an optional field is the be NULL.
 */
Value_nup input_field_response( State_menu 						      & state,
                                IO_field_spec_bool const 			  & field_spec,
                                std::optional<Kb_regular_value> const & existing_value ) {
    exclude_disallowed_fields( field_spec ); // todo: complete this: field_spec.lengths_input.max don't think we can/need to enforce min for an integer.
    pagination_reset( state, { 1, 40 } ); cout << '\n';  // create a new line on display and use that one line again and again as we reprompt for every character typed.// todo: complete this, is this reset true?  20 is an estimate!
    std::optional<Value_nup> gotten_field_data				{std::nullopt};
    std::string 			 deficiency_message 		    { field_spec.full_description };  // initially shows generic hint, but can show user's error in input.
    bool 					 is_editing_mode_insert 		{true};
    bool					 is_user_entered_a_valid_char	{false};

    auto const 				 validity_spec 					= std::get<ValidityFieldSpecBool> (
                                                                            *field_spec.validity_spec );  // todo: TODO can I replace this type with a decltype?
    auto const 			     my_set_variant 				= validity_spec.base.valid_values;
    auto const 			     valid_value_set 				= std::get<ValidValueBoolean_set> (my_set_variant );  // todo: TODO can I replace this type with a decltype?
    auto const			     valid_value_itr 				= find_default_valid_value_itr( valid_value_set );
    auto const			     valid_value_itr_begin			= valid_value_set.begin();
    auto const			     valid_value_itr_end			= valid_value_set.end();
    auto 			         current_valid_value_itr 		{ valid_value_itr != valid_value_itr_end ?
                                                              valid_value_itr : valid_value_itr_begin };  // set default if present, else at begining
    auto const 				 default_typed_value 			{ retrieve_default( current_valid_value_itr,
                                                                                valid_value_set.end())};
    std::optional<Kb_regular_value>
                             default_value 					{ to_string_typed_value( default_typed_value) };
    Kb_regular_value 		 current_value 					{ existing_o_default(
                                                                        default_value, existing_value ) };  // we build up the value we want to return character by character.

    while ( ! gotten_field_data.has_value() ) {
        prompt_for_field_data( state, field_spec, current_value, deficiency_message );
        deficiency_message = field_spec.full_description;  // reset hint, since user has seen deficiency from prior entry error.
        auto [kb_regular_value, hot_key, file_status] =
                get_kb_keys_raw( 1, false,
                    field_spec.prompt_field_spec.echo_mode == IO_field_echo_mode::normal ? true : false,
                    validity_spec.base.is_strip_control_chars,
                    false );
        // *** Start Hot_key Handling ***  Here we fully handle user intents expressed by hot_keys and loop or proceed to non-hot_key processing.  bool represents handling, else keep checking/handling.
        if ( auto hk_opt = process_hk_help( state, field_spec, hot_key.function_cat ) ) {
                hot_key = hk_opt.value();  // Reset to new hot_key, we now have a new user intent after displaying the help and the user's response.  // todo: not sure I don't just need function_cat, but this may evolve.
                //if ( hot_key.function_cat == HotKeyFunctionCat::help_popup )  // todo: which hot_keys would be just continue to top , versus those to be handled below?
                if ( hot_key.function_cat != HotKeyFunctionCat::nav_field_completion &&
                     hot_key.function_cat != HotKeyFunctionCat::navigation_esc )  // todo: which hot_keys would be just continue to top , versus those to be handled below?
                    continue;
        }
        if ( process_hk_editing_mode( state, hot_key.function_cat, is_editing_mode_insert ))
            continue;
        if ( process_intra_field_nav( hot_key.intra_f_nav, current_value ))
            continue;
        if ( process_field_completion_nav( state, field_spec, current_value, hot_key, file_status,
                                           gotten_field_data,
                                           deficiency_message,
                                           validity_spec.base.is_valid_values_enabled,
                                           is_user_entered_a_valid_char ))
            continue;
        if ( !is_user_entered_a_valid_char &&
             validity_spec.base.is_valid_values_enabled &&
             process_valid_values_nav( current_valid_value_itr, valid_value_itr_begin, valid_value_itr_end,
                                       current_value, hot_key,
                                       gotten_field_data,
                                       deficiency_message )) // we don't allow user to use up arrow once they have started entering a value, unless it is a field completion navigation.
            continue;
        // *** End   Hot_key Handling ***  Hence we must have a character!  Here we reject bad or excess characters.  bool represents a problem which we handle by not using the user entered kb character.
        if ( check_prohibited_characters( validity_spec, kb_regular_value.at(0), deficiency_message ) )
            continue;
        if ( check_max_input_length( field_spec, current_value, deficiency_message )) //                        is_user_entered_a_valid_char ))
            continue;
        current_value += kb_regular_value;
        is_user_entered_a_valid_char = true;
    }
    /* todo: complete this: notify user about length requirements failure.
     *** output
        // not applicable? field_spec.trim; // trim already done above or not?
        // not applicable, only for alphanumeric: field_spec.alignment;
    */
    return gotten_field_data.value();
}
//Value_nup input_field_response( State_menu & state, IO_field_spec_character 	const & field_spec, std::optional<Kb_regular_value> const & existing_value) {
//Value_nup input_field_response( State_menu & state, IO_field_spec_uint64 		const & field_spec, std::optional<Kb_regular_value> const & existing_value) {
//Value_nup input_field_response( State_menu & state, IO_field_spec_time_point 	const & field_spec, std::optional<Kb_regular_value> const & existing_value) {
//Value_nup input_field_response( State_menu & state, IO_field_spec_tm 			const & field_spec, std::optional<Kb_regular_value> const & existing_value) {
Value_nup input_field_response( State_menu 								& state,
                                IO_field_spec_alphanumeric const 		& field_spec,
                                std::optional<Kb_regular_value> const 	& existing_value) {
    exclude_disallowed_fields( field_spec ); // todo: complete this: field_spec.lengths_input.max don't think we can/need to enforce min for an integer.
    pagination_reset( state, { 1, 40 } ); cout << '\n';  // create a new line on display and use that one line again and again as we reprompt for every character typed.// todo: complete this, is this reset true?  20 is an estimate!
    std::optional<Value_nup> gotten_field_data				{std::nullopt};
    std::string 			 deficiency_message 			{ field_spec.full_description };
    bool 					 is_editing_mode_insert 		{true};
    bool					 is_user_entered_a_valid_char 	{false};
    auto const 				 validity_spec 					= std::get<ValidityFieldSpecAlphanum>(
                                                                            *field_spec.validity_spec);  // todo: TODO can I replace this type with a decltype?
    auto const 				 my_set_variant 				= validity_spec.base.valid_values;
    auto const 				 valid_value_set 				= std::get<ValidValueAlphanum_set> (my_set_variant);  // todo: TODO can I replace this type with a decltype?
    auto const				 valid_value_itr 				= find_default_valid_value_itr( valid_value_set );
    auto const				 valid_value_itr_begin			= valid_value_set.begin();
    auto const 				 valid_value_itr_end 			= valid_value_set.end();  // std::set<ValidValueAlphanum>::const_iterator
    auto 				 	 current_valid_value_itr 		{ valid_value_itr != valid_value_itr_end ?
                                                              valid_value_itr : valid_value_itr_begin };  // set default if present, else at begining// std::set<ValidValueAlphanum>::iterator
    std::optional<Data_type_alphanumeric> const
                            default_typed_value 			{ retrieve_default( current_valid_value_itr, valid_value_set.end() ) };
    std::optional<Kb_regular_value>
                            default_value 					{ to_string_typed_value(
                                                                    default_typed_value ) };
    Kb_regular_value 	    current_value 					{ existing_o_default(
                                                                    default_value, existing_value ) };  // we build up the value we want to return character by character.

    while ( ! gotten_field_data.has_value() ) {
        prompt_for_field_data( state, field_spec, current_value, deficiency_message );
        deficiency_message = field_spec.full_description;  // let the user see the hint, unless of course we find a deficiency below.
        auto [kb_regular_value, hot_key, file_status] =
                get_kb_keys_raw( 1, false,
                    field_spec.prompt_field_spec.echo_mode==IO_field_echo_mode::normal ? true : false,
                    validity_spec.base.is_strip_control_chars,
                    validity_spec.is_password );

        // *** Start Hot_key Handling ***  Here we fully handle user intents expressed by hot_keys and loop or proceed to non-hot_key processing.  bool represents handling, else keep checking/handling.
        if ( auto hk_opt = process_hk_help( state, field_spec, hot_key.function_cat ) ) {
                hot_key = hk_opt.value();  // Reset to new hot_key, we now have a new user intent after displaying the help and the user's response.  // todo: not sure I don't just need function_cat, but this may evolve.
                //if ( hot_key.function_cat == HotKeyFunctionCat::help_popup )  // todo: which hot_keys would be just continue to top , versus those to be handled below?
                if ( hot_key.function_cat != HotKeyFunctionCat::nav_field_completion &&
                     hot_key.function_cat != HotKeyFunctionCat::navigation_esc )  // todo: which hot_keys would be just continue to top , versus those to be handled below?
                    continue;
        }
        if ( process_hk_editing_mode( state, hot_key.function_cat, is_editing_mode_insert ))
            continue;
        //************************if ( process_hk_cat( state, field_spec, hot_key.function_cat, is_editing_mode_insert ))
            //************************continue;
        if ( process_intra_field_nav( hot_key.intra_f_nav, current_value ))
            continue;
        if ( process_field_completion_nav( state, field_spec, current_value, hot_key, file_status,
                                           gotten_field_data,
                                           deficiency_message,
                                           validity_spec.base.is_valid_values_enabled,
                                           is_user_entered_a_valid_char ))
            continue;
        if ( !is_user_entered_a_valid_char &&
             validity_spec.base.is_valid_values_enabled &&
             process_valid_values_nav( current_valid_value_itr, valid_value_itr_begin, valid_value_itr_end,
                                       current_value, hot_key,
                                       gotten_field_data,
                                       deficiency_message )) // we don't allow user to use up arrow once they have started entering a value, unless it is a field completion navigation.
            continue;
        // *** End   Hot_key Handling ***  Hence we must have a character!  Here we reject bad or excess characters.  bool represents a problem which we handle by not using the user entered kb character.
        if ( check_prohibited_characters( validity_spec, kb_regular_value.at(0), deficiency_message ) )
            continue;
        if ( check_max_input_length( field_spec, current_value, deficiency_message ) )
            continue;
        current_value += kb_regular_value;
        is_user_entered_a_valid_char = true;
    }
    return gotten_field_data.value();
}

Value_nup input_field_response( State_menu & state, IO_field_spec_integer 		const & field_spec, std::optional<Kb_regular_value> const & existing_value ) {
    exclude_disallowed_fields( field_spec ); // todo: complete this: field_spec.lengths_input.max don't think we can/need to enforce min for an integer.
    std::string 		deficiency_message 		{ field_spec.full_description };
    bool 				is_editing_mode_insert 	{true};
    bool				is_user_entered_a_valid_char {false};
    auto const 			validity_spec 			= std::get<ValidityFieldSpecInteger>( *field_spec.validity_spec);  // todo: TODO can I replace this type with a decltype?
    auto const 			my_set_variant 			= validity_spec.base.valid_values;
    auto const 			valid_value_set 		= std::get<ValidValueInteger_set>(my_set_variant);  // todo: TODO can I replace this type with a decltype?
    auto const			valid_value_itr 		= find_default_valid_value_itr( valid_value_set );
    auto const			valid_value_itr_begin	= valid_value_set.begin();
    auto const			valid_value_itr_end		= valid_value_set.end();
    auto 				current_valid_value_itr { valid_value_itr != valid_value_itr_end ? valid_value_itr : valid_value_itr_begin };  // set default if present, else at begining
    //Kb_regular_value 	kludge 					{}; // std::optional<Kb_regular_value> default_value { /* retrieve_default( current_valid_value_itr, valid_value_set.end(), kludge )*/ std::nullopt };
    auto const 			default_typed_value 	{ retrieve_default( current_valid_value_itr, valid_value_set.end() ) };
    std::optional<Kb_regular_value>
                        default_value 			{ to_string_typed_value( default_typed_value )};
    Kb_regular_value 	current_value 			{ existing_o_default( default_value, existing_value ) };  // we build up the value we want to return character by character.
    pagination_reset( state, { 1, 40 } ); cout << '\n';  // create a new line on display and use that one line again and again as we reprompt for every character typed.// todo: complete this, is this reset true?  20 is an estimate!
    std::optional<Value_nup> gotten_field_data			{std::nullopt};
    while ( !gotten_field_data.has_value() ) {
        prompt_for_field_data( state, field_spec, current_value, deficiency_message );
        deficiency_message = field_spec.full_description;  // let the user see the hint, unless of course we find a deficiency below.
        auto [kb_regular_value, hot_key, file_status] =  get_kb_keys_raw( 1, false,
                    field_spec.prompt_field_spec.echo_mode==IO_field_echo_mode::normal ? true : false,
                    validity_spec.base.is_strip_control_chars, false );
        // auto [kb_regular_value, hot_key, file_status] = get_kb_keys_raw( 1, field_spec.prompt_field_spec.echo_mode==IO_field_echo_mode::normal ? true : false, validity_spec.base.is_strip_control_chars );
        // *** Start Hot_key Handling ***
        if ( auto hk_opt = process_hk_help( state, field_spec, hot_key.function_cat ) ) {
                hot_key = hk_opt.value();  // we may have a new user intent from the display of the help and the user's action.
                if ( hot_key.function_cat == HotKeyFunctionCat::help_popup )
                    continue;
        }
        if ( process_hk_editing_mode( state, hot_key.function_cat, is_editing_mode_insert ))
            continue;
        // *****************if ( process_hk_cat( state, field_spec, hot_key.function_cat, is_editing_mode_insert ))
            // *****************continue;
        if ( process_intra_field_nav( hot_key.intra_f_nav, current_value ))
            continue;
        if ( process_field_completion_nav( state, field_spec, current_value, hot_key, file_status , gotten_field_data, deficiency_message,
                                         validity_spec.base.is_valid_values_enabled, is_user_entered_a_valid_char))
            continue;
        if ( !is_user_entered_a_valid_char && validity_spec.base.is_valid_values_enabled
            && process_valid_values_nav( current_valid_value_itr, valid_value_itr_begin, valid_value_itr_end, current_value, hot_key, gotten_field_data, deficiency_message )) // we don't allow user to use up arrow once they have started entering a value, unless it is a field completion navigation.
            continue;
        // *** End   Hot_key Handling ***  hence we must have a character!
        if ( check_prohibited_characters( validity_spec, kb_regular_value.at(0), deficiency_message ) )
            continue;
        if ( process_numeric_decorators( current_value.length(), kb_regular_value, field_spec, deficiency_message ))
            continue;
        if ( check_max_input_length( field_spec, current_value, deficiency_message ) )
            continue;
        current_value += kb_regular_value;
        if ( auto error = check_numeric_invalidity( validity_spec, current_value ) ) {
            deficiency_message = error.value();
            continue;
        }
        is_user_entered_a_valid_char = true;
    }
    /* todo: complete this: notify user about length requirements failure.
     *** output
        // not applicable? field_spec.trim; // trim already done above or not?
        // not applicable, only for alphanumeric: field_spec.alignment;
    */
    return gotten_field_data.value();
}

Value_nup input_field_response( State_menu & state, IO_field_spec_decimal 		const & field_spec, std::optional<Kb_regular_value> const & existing_value ) {
    exclude_disallowed_fields( field_spec ); // todo: complete this: field_spec.lengths_input.max don't think we can/need to enforce min for an integer.
    std::string 		deficiency_message 		{ field_spec.full_description };
    bool 				is_editing_mode_insert 	{true};
    bool				is_user_entered_a_valid_char {false};
    auto const 			validity_spec 			= std::get<ValidityFieldSpecDecimal>( *field_spec.validity_spec);  // todo: TODO can I replace this type with a decltype?
    auto const 			my_set_variant 			= validity_spec.base.valid_values;
    auto const 			valid_value_set 		= std::get<ValidValueDecimal_set>(my_set_variant);  // todo: TODO can I replace this type with a decltype?
    auto const			valid_value_itr 		= find_default_valid_value_itr( valid_value_set );
    auto const			valid_value_itr_begin	= valid_value_set.begin();
    auto const			valid_value_itr_end		= valid_value_set.end();
    auto 				current_valid_value_itr { valid_value_itr != valid_value_itr_end ? valid_value_itr : valid_value_itr_begin };  // set default if present, else at begining
    auto const 			default_typed_value 	{ retrieve_default( current_valid_value_itr, valid_value_set.end() ) };
    std::optional<Kb_regular_value>
                        default_value 			{ to_string_typed_value( default_typed_value )};
    Kb_regular_value 	current_value 			{ existing_o_default( default_value, existing_value ) };  // we build up the value we want to return character by character.
    pagination_reset( state, { 1, 40 } ); cout << '\n';  // create a new line on display and use that one line again and again as we reprompt for every character typed.// todo: complete this, is this reset true?  20 is an estimate!
    std::optional<Value_nup> gotten_field_data			{std::nullopt};
    while ( !gotten_field_data.has_value() ) {
        prompt_for_field_data( state, field_spec, current_value, deficiency_message );
        deficiency_message = field_spec.full_description;  // let the user see the hint, unless of course we find a deficiency below.
        auto [kb_regular_value, hot_key, file_status] =  get_kb_keys_raw( 1, false,
                    field_spec.prompt_field_spec.echo_mode==IO_field_echo_mode::normal ? true : false,
                    validity_spec.base.is_strip_control_chars, false );
        // auto [kb_regular_value, hot_key, file_status] = get_kb_keys_raw( 1, field_spec.prompt_field_spec.echo_mode==IO_field_echo_mode::normal ? true : false, validity_spec.base.is_strip_control_chars );
        // *** Start Hot_key Handling ***
        if ( auto hk_opt = process_hk_help( state, field_spec, hot_key.function_cat ) ) {
                hot_key = hk_opt.value();  // we may have a new user intent from the display of the help and the user's action.
                if ( hot_key.function_cat == HotKeyFunctionCat::help_popup )
                    continue;
        }
        if ( process_hk_editing_mode( state, hot_key.function_cat, is_editing_mode_insert ))
            continue;
        // *****************if ( process_hk_cat( state, field_spec, hot_key.function_cat, is_editing_mode_insert ))  // todo: this.
            // *****************continue;
        if ( process_intra_field_nav( hot_key.intra_f_nav, current_value ))
            continue;
        if ( process_field_completion_nav( state, field_spec, current_value, hot_key, file_status , gotten_field_data, deficiency_message,
                                         validity_spec.base.is_valid_values_enabled, is_user_entered_a_valid_char))
            continue;
        if ( !is_user_entered_a_valid_char && validity_spec.base.is_valid_values_enabled
            && process_valid_values_nav( current_valid_value_itr, valid_value_itr_begin, valid_value_itr_end, current_value, hot_key, gotten_field_data, deficiency_message )) // we don't allow user to use up arrow once they have started entering a value, unless it is a field completion navigation.
            continue;
        // *** End   Hot_key Handling ***  hence we must have a character!
        if ( check_prohibited_characters( validity_spec, kb_regular_value.at(0), deficiency_message ) )
            continue;
        //if ( process_numeric_decorators( current_value.length(), kb_regular_value, deficiency_message, false ))
        if ( process_numeric_decorators( current_value.length(), kb_regular_value, field_spec, deficiency_message ))
            continue;
        if ( check_max_input_length( field_spec, current_value, deficiency_message ) )
            continue;
        current_value += kb_regular_value;
        if ( auto error = check_numeric_invalidity( validity_spec, current_value ) ) {
            deficiency_message = error.value();
            continue;
        }
        is_user_entered_a_valid_char = true;
    }
    return gotten_field_data.value();
}
//Value_nup input_field_response( State_menu & state, IO_field_spec_scientific	const & field_spec, std::optional<Kb_regular_value> const & existing_value ) // todo: this.

/* Value_nup input_dialog_response( State_menu & state, std::string const & prompt, ValidityCriteriaDialog const & validity ) {
    while ( true ) {
        prompt_for_dialog( state, prompt, validity );
        // todo: here we should a support selection from the valid values. complete this.
        auto [kb_regular_value, hot_key, file_status] = get_value_raw();  // todo: specify size??

        std::string kludge = kb_regular_value; // todo: TODO why do I need this or get compile error?

        switch (validity.default_handling) {
            case ValidityHandlingDialog::default_is_to_notify:
                return { {}, hot_key, {} }; // no problem, the end value will just be "".
            case ValidityHandlingDialog::default_is_to_approve:
                if ( kb_regular_value.length() == 0 )
                    //return { validity.value_default_approve_equivalents.at(0), hot_key, {} };
                //if ( std::find_if( validity.value_default_approve_equivalents.begin(), validity.value_default_approve_equivalents.end(),  [kludge] (std::string a) { return a == kludge; }) != validity.value_default_approve_equivalents.end() )
                    //return { validity.value_default_approve_equivalents.at(0), hot_key, {} };
                //if ( std::find_if( validity.value_default_deny_equivalents.begin(), validity.value_default_deny_equivalents.end(),  [kludge] (std::string a) { return a == kludge; }) != validity.value_default_deny_equivalents.end() )
                    //return { validity.value_default_deny_equivalents.at(0), hot_key, {} };
                break;
            case ValidityHandlingDialog::default_is_to_deny:
                if ( kb_regular_value.length() == 0)
                    //return { validity.value_default_deny_equivalents.at(0), hot_key, {} };
                //if ( std::find_if( validity.value_default_deny_equivalents.begin(), validity.value_default_deny_equivalents.end(),  [kludge] (std::string a) { return a == kludge; }) != validity.value_default_deny_equivalents.end() )
                    //return { validity.value_default_deny_equivalents.at(0), hot_key, {} };
                //if ( std::find_if( validity.value_default_approve_equivalents.begin(), validity.value_default_approve_equivalents.end(),  [kludge] (std::string a) { return a == kludge; }) != validity.value_default_approve_equivalents.end() )
                    //return { validity.value_default_approve_equivalents.at(0), hot_key, {} };
                break;
            case ValidityHandlingDialog::default_is_provided:
                if ( kb_regular_value.length() == 0 ) {
                    kb_regular_value = validity.value_default_provided;
                    return { kb_regular_value, hot_key, {} };
                }
                break;
            case ValidityHandlingDialog::check_valid_values:
            case ValidityHandlingDialog::no_specific_value_checking:
                break;
        }
        std::string user_error_prompt {};
        switch (validity.default_handling) {
            case ValidityHandlingDialog::check_valid_values:
                if ( validity.valid_values.size() <= 0) assert(false);
                else { // auto found2 = std::find_if( valid.values.begin(), valid.values.end(), "junk" //kb_regular_value//);  // todo: TODO why on earth do I get a strange iteration compile error on this?
                    if ( std::find_if( validity.valid_values.begin(), validity.valid_values.end(),  [kludge] (std::string a) { return a == kludge; }) != validity.valid_values.end() )
                        return { kb_regular_value, hot_key, {} };
                }
                user_error_prompt = "Is not one of the valid values(";
                pagination( state, {1, user_error_prompt.length()});
                cout << user_error_prompt ;
                for (auto & i: validity.valid_values ) {
                    pagination( state, {0, i.length() + 1} );
                    cout << i << ",";
                }
                break;
            case ValidityHandlingDialog::no_specific_value_checking:
            case ValidityHandlingDialog::default_is_to_notify:
            case ValidityHandlingDialog::default_is_to_approve:
            case ValidityHandlingDialog::default_is_to_deny:
            case ValidityHandlingDialog::default_is_provided:
                if (( validity.lengths.min || validity.lengths.max )   // if we have them then check them.
                       &&
                    ( kb_regular_value.length() < validity.lengths.min || kb_regular_value.length() > validity.lengths.max ))
                {
                    user_error_prompt = "Value is too ";
                    pagination( state, {1, user_error_prompt.length() + 5 + 1});
                    cout << user_error_prompt << (kb_regular_value.length() < validity.lengths.min ? "long" : "short");
                    cout << ".";
                    break;
                }
                else
                    return { kb_regular_value, hot_key, {} };
        }
        cout <<'\a'<<endl;  // just ring the bell? // todo: complete this: notify user about length requirements failure.
    }
}*/

/* InteractionResult action_dialog_modal_vals(State_menu & state, std::string const & prompt, ValidityCriteriaDialog const & validity) {  // different from process_menu() because we don't expect a major change in the flow of the program, ie. yes/no for a data save to file. BUT yes we do want to support _some_ hot_keys!
    Value_nup 		 	 value_plus 		= input_dialog_response( 	   state, "\n"+prompt+" <ENTER++> to continue.", validity );
    std::optional<Kb_regular_value>
        kb_regular_value 	                = std::get<std::optional<Kb_regular_value>>(  value_plus );
    InteractionResultNav irn 				= find_interaction_result_nav( std::get<Hot_key>(value_plus), InteractionCategory::dialog );
    return { std::get<Hot_key>(value_plus), {}, kb_regular_value.value_or(""), {}, irn }; // return { kb_regular_value, InteractionResultError {}, InteractionResultNav::retain_menu };
} */

bool is_store_value_intent( InteractionResultNav irn ) {
    switch ( irn  ) {
    case InteractionResultNav::down_one_field :
    case InteractionResultNav::up_one_block :  // essentially ignore it, assume equivalent to <Enter>
    case InteractionResultNav::continue_forward_pagination :  // essentially ignore it, assume equivalent to <Enter>
    case InteractionResultNav::up_one_field :
    case InteractionResultNav::continue_backward_pagination :
    case InteractionResultNav::down_one_block :
    case InteractionResultNav::save_form_as_is :  // is this true??
    case InteractionResultNav::retain_menu :
    case InteractionResultNav::main_menu :
    case InteractionResultNav::prior_menu :
        return true;
    case InteractionResultNav::skip_one_field :
    case InteractionResultNav::skip_to_end_of_fields :
    case InteractionResultNav::exit_fn_with_prompts :
    case InteractionResultNav::exit_fn_immediately :
    case InteractionResultNav::exit_pgm_immediately :
    case InteractionResultNav::exit_pgm_with_prompts :
    case InteractionResultNav::prior_menu_discard_value :
        return false;
    case InteractionResultNav::first_row :
    case InteractionResultNav::last_row :
    case InteractionResultNav::next_row :
    case InteractionResultNav::prior_row :
    case InteractionResultNav::na :
        cerr << static_cast<int>(irn) << endl; // crude debugging info.
        assert(false);  // todo: finish this:
    }
    return false;
}

/*******
 * Takes input from the user terminal for a data field defined by a field specification.
 * Input: 	'field' which optionally contains a partially validated value represented as a string.
 * Output:  'user navigation intent', ie. finished, or repeat operation. Currently we always assume "finished". also: error - not used yet, and value - not used and probably won't be.
 * Mutates: 'data value' specified by the field is set, or cleared, or untouched.
 * Calls:	- calls 'input_field_response()' which provides a partially validated value.
 * Callers:	- called directly by programmer
 * 			- called by an 'input a row' function.
 * The user input consists of an optional partially validated value and a manditory hot_key that designates storage intent of the user at the terminal. File_status is not used yet.
 * If the user approved storage, it converts from the string and fully validates the value, and stores it, and returns,
 * 		or else throws away the input, prints error, and takes input again (only case where it does not return.
 * It clears the field (AKA NULLs it) and returns, if the user provided no value and approved it for storage.
 * It just returns and does not modify the field if the user didn't approve it for storage (<ESC>).
 */
InteractionResult action_dialog_modal_io_field( State_menu & state, IO_field_spec_variant & field_spec) {  // different from process_menu() because we don't expect a major change in the flow of the program, ie. yes/no for a data save to file. BUT yes we do want to support _some_ hot_keys!
    std::variant<State_menu> state_kludge {state};  // WARNING: we are making a copy here! FIX it. todo: TODO it is too bad I have to wrap the first parameter with a std::variant to make it compile!?
    struct Input_a_AssignVisitor {
        InteractionResult operator() (State_menu & state, IO_field_spec_bool 			& field_spec ) {
            Value_nup 		 	 			value_nup {};
            InteractionResultNav 			irn  {};
            while ( true ) {
                value_nup 		    	= input_field_response( state, field_spec, field_spec.getData_value_str() );
                irn 					= find_interaction_result_nav( value_nup.hot_key, InteractionCategory::field );
                if ( is_store_value_intent( irn ) ) {
                    if ( !value_nup.kb_regular_value_opt.has_value() ) {
                        field_spec.clear_data_value();
                        return { value_nup.hot_key, {}, {}, {}, irn };
                    } else {
                        std::decay_t<decltype (field_spec.data_location)> typed_value {false};
                        if ( auto const itr = VALUE_DEFAULT_APPROVE_EQUIVALENTS.find( value_nup.kb_regular_value_opt.value() ); itr != VALUE_DEFAULT_APPROVE_EQUIVALENTS.end() )
                            typed_value = true;
                        else if ( auto const itr = VALUE_DEFAULT_DENY_EQUIVALENTS.find( value_nup.kb_regular_value_opt.value() ); itr != VALUE_DEFAULT_DENY_EQUIVALENTS.end() )
                            typed_value = false;
                        else
                            assert(false);
                        if ( std::optional<ValidationFieldError> vfr = field_spec.validate_data( typed_value ); !vfr.has_value() ) {
                            field_spec.setData_value( typed_value );
                            // return { value_nup.hot_key, {}, value_nup.kb_regular_value_opt.value(), {}, irn }; // return GOOD Typed VALUE
                            return { value_nup.hot_key, {}, typed_value, {}, irn }; // return GOOD Typed VALUE
                        } else {
                            cout << "Invalid boolean or (y/n) value."<< endl; // prompt final validation error ; throw away this value, get another value.
                        }
                    }
                }
                else {
                    return { value_nup.hot_key, {}, {}, {}, irn }; // return { Data_type_variant {} InteractionResultError {}, InteractionResultNav::retain_menu };
                }
            }
        }
        //InteractionResult operator() (State_menu & state, IO_field_spec_character		& field_spec ) {
        //InteractionResult operator() (State_menu & state, IO_field_spec_uint64			& field_spec ) {
        //InteractionResult operator() (State_menu & state, IO_field_spec_time_point		& field_spec ) {
        //InteractionResult operator() (State_menu & state, IO_field_spec_tm				& field_spec ) {
        InteractionResult operator() (State_menu & state, IO_field_spec_alphanumeric	& field_spec ) {
            Value_nup 		 	 			value_nup {};
            InteractionResultNav 			irn  {};
            while ( true ) {
                value_nup 		    	= input_field_response( state, field_spec, field_spec.getData_value_str() );
                irn 					= find_interaction_result_nav( value_nup.hot_key, InteractionCategory::field );
                if ( is_store_value_intent( irn ) ) {
                    if ( !value_nup.kb_regular_value_opt.has_value() ) {
                        field_spec.clear_data_value();
                        return { value_nup.hot_key, {}, {}, {}, irn };
                    } else if ( std::optional<ValidationFieldError> vfr = field_spec.validate_data( value_nup.kb_regular_value_opt.value() ); !vfr.has_value() ) {
                        field_spec.setData_value( value_nup.kb_regular_value_opt.value() );
                        return { value_nup.hot_key, {}, value_nup.kb_regular_value_opt.value(), {}, irn }; // return GOOD Typed VALUE { Data_type_variant {}, InteractionResultError {}, InteractionResultNav::retain_menu };
                    } else {
                        cout << "Number failed special validation function."<< endl; // prompt final validation error ; throw away this value, get another value.
                    }
                }
                else {
                    return { value_nup.hot_key, {}, {}, {}, irn };
                }
            }
        }
        InteractionResult operator() (State_menu & state, IO_field_spec_integer			& field_spec ) {
            Value_nup 		 	 			value_nup {};
            InteractionResultNav 			irn  {};
            while ( true ) {
                value_nup 		    	= input_field_response( state, field_spec, field_spec.getData_value_str() );
                irn 					= find_interaction_result_nav( value_nup.hot_key, InteractionCategory::field );
                if ( is_store_value_intent( irn ) ) {
                    if ( !value_nup.kb_regular_value_opt.has_value() ) {
                        field_spec.clear_data_value();
                        return { value_nup.hot_key, {}, {}, {}, irn };
                    } else {
                        std::decay_t<decltype (field_spec.data_location)> typed_value;
                        try {
                            typed_value = stoi( value_nup.kb_regular_value_opt.value() );
                        } catch (std::invalid_argument const & ia) {
                            field_spec.clear_data_value();
                            cout << "Invalid number."<<ia.what()<< endl; // prompt final validation error ; throw away this value, get another value.
                            continue;
                        } catch (std::out_of_range const & ia) {
                            field_spec.clear_data_value();
                            cout << "Number not in range."<< endl; // prompt final validation error ; throw away this value, get another value.
                            continue;
                        }
                        if ( std::optional<ValidationFieldError> vfr = field_spec.validate_data( typed_value ); !vfr.has_value() ) {
                            field_spec.setData_value( typed_value );
                            return { value_nup.hot_key, {}, value_nup.kb_regular_value_opt.value(), {}, irn }; // return GOOD Typed VALUE { Data_type_variant {}, InteractionResultError {}, InteractionResultNav::retain_menu };
                        } else {
                            field_spec.clear_data_value();
                            cout << "Number failed special validation function."<< endl; // prompt final validation error ; throw away this value, get another value.
                            continue;
                        }
                    }
                }
                else {
                    return { value_nup.hot_key, {}, {}, {}, irn }; // return { Data_type_variant {} InteractionResultError {}, InteractionResultNav::retain_menu };
                }
            }
        }
        InteractionResult operator() (State_menu & state, IO_field_spec_decimal			& field_spec ) {
            Value_nup 		 	 			value_nup {};
            InteractionResultNav 			irn  {};
            while ( true ) {
                value_nup 		    	= input_field_response( state, field_spec, field_spec.getData_value_str() );
                irn 					= find_interaction_result_nav( value_nup.hot_key, InteractionCategory::field );
                if ( is_store_value_intent( irn ) ) {
                    if ( !value_nup.kb_regular_value_opt.has_value() ) {
                        field_spec.clear_data_value();
                        return { value_nup.hot_key, {}, {}, {}, irn };
                    } else {
                        std::decay_t<decltype (field_spec.data_location)> typed_value;
                        try {
                            typed_value = stod( value_nup.kb_regular_value_opt.value() );
                        } catch (std::invalid_argument const &) {
                            field_spec.clear_data_value();
                            cout << "Invalid number."<< endl; // prompt final validation error ; throw away this value, get another value.
                            continue;
                        } catch (std::out_of_range const &) {
                            field_spec.clear_data_value();
                            cout << "Number not in range."<< endl; // prompt final validation error ; throw away this value, get another value.
                            continue;
                        }
                        if ( std::optional<ValidationFieldError> vfr = field_spec.validate_data( typed_value ); !vfr.has_value() ) {
                            field_spec.setData_value( typed_value );
                            return { value_nup.hot_key, {}, value_nup.kb_regular_value_opt.value(), {}, irn }; // return GOOD Typed VALUE { Data_type_variant {}, InteractionResultError {}, InteractionResultNav::retain_menu };
                        } else {
                            field_spec.clear_data_value();
                            cout << "Number failed special validation function."<< endl; // prompt final validation error ; throw away this value, get another value.
                        }
                    }
                }
                else {
                    return { value_nup.hot_key, {}, {}, {}, irn }; // return { Data_type_variant {} InteractionResultError {}, InteractionResultNav::retain_menu };
                }
            }
        }
        //InteractionResult operator() (State_menu & state, IO_field_spec_scientific		& field_spec ) {
    };
    return std::visit( Input_a_AssignVisitor(), state_kludge, field_spec );
}

InteractionResult action_dialog_modal_bool( State_menu & state, std::string const & prompt, bool is_default_approve ) {
    bool								data_location_value  	{};
    std::bitset<IO_ROW_FIELDS_NUM_MAX>	is_null_field		    {};
    ValidityFieldSpec_var 				validityFieldSpec_var { ValidityFieldSpecBool {
        ValidityFieldSpecBase {
            LengthRange {0, 15}, true, {'\a'},
            ValidValueBoolean_set {{true,true, is_default_approve? true: false }, {false,true, is_default_approve? false: true }},
            /*is_valid_values_enabled*/ true, /*is_default_value_enabled*/ true,
            ProhibitedValueAlphanum_set { },  // NOTE: USER_NOT_PROVIDED is prohibited because the field is NOT optional.  todo: complete this: make this an invariant.
            /*regex_pattern*/{},/*custom_validation_fn*/{}
        },
        /*value_default_approve_prompt*/ {}, /*value_default_deny_prompt*/ {},/*is_ignore_case_on_validation*/ {}
    }};
    IO_field_spec_variant 				field_spec_var { IO_field_spec_bool {
        0, 0,
        "name:modal bool dialog.", prompt, "Please enter yes or no, or press a Hot Key.", "This input only allows answering a question with a yes or no, this can also be interpreted as true or false, or proceed or halt.  It is generally used to get permission, or confirmation to proceed with a choice the user has just made. The user can use the up arrow or down arrow to make the selection. The user can also press a Hot Key, such as ESC, which may further alter the progress of the program.", "enter data here",
        LengthRange {0,15},
        &validityFieldSpec_var,	PromptFieldSpecification {}, super_user,
        /*is_primary_key_component*/ false, /*is_optional*/ true, /*is_initial_focus*/ {}, /*is_current_focus*/ {}, /*is_greyed_out*/ {}, /*is_programmer_only*/ {}, /*is_programmer_only*/ {},
        data_location_value,
        is_null_field
    }};
    std::get<IO_field_spec_bool>(field_spec_var).clear_data_value();
    // std::get<IO_field_spec_bool>(field_spec_var).clear_data_location();

    InteractionResult ir = action_dialog_modal_io_field( state, field_spec_var ) ;
    cout << endl;
    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::dialog );
    ir.navigation = nav;
    return ir;
}

InteractionResult action_dialog_modal_approve( State_menu & state, std::string const & prompt ) {
    return action_dialog_modal_bool( state, prompt, true );
}

InteractionResult action_dialog_modal_deny( State_menu & state, std::string const & prompt ) {
    return action_dialog_modal_bool( state, prompt, false );
}

InteractionResult action_dialog_modal_notify( State_menu & state, std::string const & prompt ) {
    std::string 	                    data_location_value  	{};
    std::bitset<IO_ROW_FIELDS_NUM_MAX>	is_null_field		    {};
    ValidityFieldSpec_var 				validityFieldSpec_var {
        ValidityFieldSpecAlphanum {
            ValidityFieldSpecBase {
                LengthRange {0, 0}, true, {'\a'},
                ValidValueAlphanum_set { },
                /* is_valid_values_enabled */false, /* is_default_value_enabled */false,
                ProhibitedValueAlphanum_set { },  // NOTE: USER_NOT_PROVIDED is prohibited because the field is NOT optional.  todo: complete this: make this an invariant.
                /*regex_pattern*/{},/*custom_validation_fn*/{}
            },
            LengthRange {0, 0}, /* is_ignore_case_on_validation */ false
        }
    };
    IO_field_spec_variant 				field_spec_var {
        IO_field_spec_alphanumeric {
            0, 0,
            "name:modal notify dialog", prompt ,"Please press the <ENTER> key to continue.", "This field only allows for the user to acknowledge the receipt of the dialog's message by pressing <ENTER>. The user can also press a Hot Key, such as ESC, which may further alter the progress of the program.", "enter data here",
                    LengthRange {0,0},
            &validityFieldSpec_var, 	PromptFieldSpecification {}, super_user,
                    /*is_primary_key_component*/ false, /*is_optional*/ true, /*is_initial_focus*/ {}, /*is_current_focus*/ {}, /*is_greyed_out*/ {}, /*is_programmer_only*/ {}, /*is_programmer_only*/ {},
            data_location_value,
                    is_null_field
        }
    };
    InteractionResult ir = action_dialog_modal_io_field( state, field_spec_var ) ;
    cout << endl;
    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::dialog );
    ir.navigation = nav;
    return ir;
}
