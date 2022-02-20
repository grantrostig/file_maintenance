#include <chrono>
#include <string>
#include <variant>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <cmath>
#include <cstdlib>
#include "io_field.h"
#include "io_table.h"

template<class T>  // https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::fabs(x-y) <= std::numeric_limits<T>::epsilon() * std::fabs(x+y) * ulp
           // unless the result is subnormal
           || std::fabs(x-y) < std::numeric_limits<T>::min();
}

/*IO_data_to_cpp_type_map convert_string_value_to_field_type_value(const IO_table &table, const unsigned short num, const std::string &val) {

//    if (        std::holds_alternative< IO_field_spec_bool >( table.spec.fields.at(num).field_spec ) ) {
//        return val == "Y" ? true : false;

//    } else if ( std::holds_alternative< IO_field_spec_character >( table.spec.fields.at(num).field_spec )) {
//        if ( val.length() == 0 ) return {};  // todo: should return std::monostate?
//        else if (val.length() == 1 ) return val[0];
//        else throw std::logic_error( "value_to_field():FATAL ERROR: expecting zero or one characters and got more than one in a string." );

//    } else if ( std::holds_alternative< IO_field_spec_uint64 >( table.spec.fields.at(num).field_spec )) {
//        throw std::logic_error( "value_to_field():FATAL ERROR: disallowed IO_field_spec_*" );

//    } else if ( std::holds_alternative< IO_field_spec_time_point >( table.spec.fields.at(num).field_spec )) {
//        throw std::logic_error( "value_to_field():FATAL ERROR: disallowed IO_field_spec_*" );

//    } else if ( std::holds_alternative< IO_field_spec_tm >( table.spec.fields.at(num).field_spec )) {
//        std::tm my_tm = {0,0,0,27,10,85
                         
//                         std::any IO_field_spec::getField_value() const
//                         {
//                             return field_value;
//                         }
                         
//                         void IO_field_spec::setField_value(const std::any &value)
//                         {
//                             field_value = value;
//                         }
                         
//                         
// // ,4,365,-1,0,""};  // todo: complete this: perform transformation of date string to tm.
//                         return my_tm;

//                        } else if ( std::holds_alternative< IO_field_spec_alphanumeric >( table.spec.fields.at(num).field_spec )) {
//        return val;

//    } else if ( std::holds_alternative< IO_field_spec_integer >( table.spec.fields.at(num).field_spec )) {
//        return stol( val );

//    } else if ( std::holds_alternative< IO_field_spec_decimal >( table.spec.fields.at(num).field_spec )) {
//        return stod( val );

//    } else
//        throw std::logic_error( "value_to_field():FATAL ERROR: unknown IO_field_spec_*" );
//}
*/

template <typename Spec_type, typename Candiate_type>
std::optional<ValidationFieldError> validate_scalar( Spec_type validity_spec, Candiate_type const candidate) {
    if ( validity_spec.range.min > candidate ) 		return ValidationFieldError::range_overflow;
    if ( validity_spec.range.max < candidate ) 		return ValidationFieldError::range_underflow;
    cerr <<"validate_scalar>: \n";
    if constexpr ( std::is_same<std::decay_t<Spec_type>, ValidityFieldSpecDecimal>::value ) {
        auto const inverse_precision = pow(10.0,validity_spec.range.precision);
        auto const shifted_value = candidate * inverse_precision;
        cerr <<"validate_scalar<decimal>:"<< ", inverse_precision: " << inverse_precision << ", shifted_value:" << shifted_value<< endl;
        cerr <<" shifted_value - trunc(shifted_value) "<< shifted_value - trunc( shifted_value)<< endl;
        if ( !almost_equal( shifted_value, trunc(shifted_value), 2 )) { // todo: TODO giving 2 as ULP units in the last place, what is that??
            return ValidationFieldError::precision_excess;
        }
    }
    return std::nullopt;
}

std::optional<ValidationFieldError>
IO_field_spec_bool::	validate_data(Data_type_boolean const candidate) {
    if ( is_system_calculated_read_only ) return ValidationFieldError::system_calculated;  // todo: note that is is also checked at data entry time.  one of them may be redundant.
    if ( is_programmer_only ) 			  return ValidationFieldError::system_calculated;
    return std::nullopt; }


std::optional<ValidationFieldError>
IO_field_spec_alphanumeric::	validate_data(Data_type_alphanumeric const & candidate) {
    if ( is_system_calculated_read_only ) return ValidationFieldError::system_calculated;  // todo: note that is is also checked at data entry time.  one of them may be redundant.
    if ( is_programmer_only ) 			  return ValidationFieldError::system_calculated;
    if ( candidate.length() > std::get<ValidityFieldSpecAlphanum>(*validity_spec).lengths_storage.max )	return ValidationFieldError::too_long;
    if ( candidate.length() < std::get<ValidityFieldSpecAlphanum>(*validity_spec).lengths_storage.min )	return ValidationFieldError::too_short;
    return std::nullopt; }

std::optional<ValidationFieldError>
IO_field_spec_integer::			validate_data(Data_type_integer const candidate) {
    if ( is_system_calculated_read_only ) return ValidationFieldError::system_calculated;  // todo: note that is is also checked at data entry time.  one of them may be redundant.
    if ( is_programmer_only ) 			  return ValidationFieldError::system_calculated;
    return validate_scalar( std::get<ValidityFieldSpecInteger>(* validity_spec), candidate );
    /* try {  // todo: previously did conversion from string to int, don't need try anymore?
        if ( range.min > candidate ) 		return ValidationFieldError::range_overflow;
        if ( range.max < candidate ) 		return ValidationFieldError::range_underflow;
        if (is_system_calculated_read_only) return ValidationFieldError::system_calculated;
        if (is_programmer_only) 			return ValidationFieldError::system_calculated;
    }
    catch (const std::invalid_argument & ){ return ValidationFieldError::unable_to_convert; }
    catch (const std::range_error & )     { return ValidationFieldError::unable_to_convert; }
    return {};
    */
}

std::optional<ValidationFieldError>
IO_field_spec_decimal::			validate_data(Data_type_decimal const candidate) {
    if ( is_system_calculated_read_only ) return ValidationFieldError::system_calculated;
    if ( is_programmer_only ) 			  return ValidationFieldError::system_calculated;
    return validate_scalar( std::get<ValidityFieldSpecDecimal>(* validity_spec), candidate );
}

std::optional<ValidationFieldError>
IO_field_spec_scientific::		validate_data(Data_type_scientific const candidate) {
    if ( is_system_calculated_read_only ) return ValidationFieldError::system_calculated;
    if ( is_programmer_only ) 			  return ValidationFieldError::system_calculated;
    // return validate_scalar( this, candidate );  // todo: complete this:
    return  {};
}

const Security_spec super_user {}; // yup, that is our default.
const Security_spec guest_user {
    { Security_role	  ::read_person },
    { Security_region ::all 		},
    { Security_level  ::public_info },
    { Security_action ::read 		}
};

void IO_field_spec_bool::			setData_value(const Data_type_boolean &value) 		{ data_location = value;  is_null_field_location[field_index_in_row] = false;
    cerr << " IO_field_spec_integer::setData_value().value:" << value << endl;
    cerr << "data_location:  " << data_location << endl;
    cerr << "&data_location: " << &data_location << endl;
}
void IO_field_spec_integer::		setData_value(const Data_type_integer & value) 			{  data_location = value;  is_null_field_location[field_index_in_row] = false;
    cerr << " IO_field_spec_integer::setData_value().value:" << value << endl;
    cerr << "data_location:  " << data_location << endl;
    cerr << "&data_location: " << &data_location << endl;
}
void IO_field_spec_character::		setData_value(const Data_type_character &value) 	{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_uint64::			setData_value(const Data_type_uint64 &value) 		{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_time_point::		setData_value(const Data_type_time_point &value) 	{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_tm::				setData_value(const Data_type_tm &value) 			{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_alphanumeric::	setData_value(const Data_type_alphanumeric &value)	{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_decimal::		setData_value(const Data_type_decimal &value) 		{  data_location = value;  is_null_field_location[field_index_in_row] = false;}
void IO_field_spec_scientific::		setData_value(const Data_type_scientific &value) 	{  data_location = value;  is_null_field_location[field_index_in_row] = false;}


std::optional<Kb_regular_value> IO_field_spec_bool::		getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return data_location ? YES : NO ; 	  				     else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_character::	getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return std::to_string( data_location ); 					 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_uint64::		getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return std::to_string( data_location ); 					 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_time_point::	getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return /* todo: std::to_string( data_value )*/ std::nullopt; 	 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_tm::			getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return /* todo: std::to_string( data_value )*/ std::nullopt; 	 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_alphanumeric::getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return data_location; 									 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_integer::		getData_value_str() const { if( !is_null_field_location[field_index_in_row]) return std::to_string( data_location ); 					 else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_decimal::		getData_value_str() const {
    if( !is_null_field_location[field_index_in_row]) {
        std::ostringstream oss {};
        oss << std::fixed << std::setprecision( std::get<ValidityFieldSpecDecimal>( *validity_spec ).range.precision )<< data_location;
        cerr<< "getData_value_str().value,precision,value.str():"<<data_location<<", "<<std::get<ValidityFieldSpecDecimal>( *validity_spec ).range.precision<<", "<<oss.str()<<endl;
        return oss.str();
    }
    else return std::nullopt; }
std::optional<Kb_regular_value> IO_field_spec_scientific::	getData_value_str() const { if( !is_null_field_location[field_index_in_row] ) return /*std::to_string( data_value );*/ std::nullopt; else return std::nullopt; }

std::optional<Data_type_boolean > IO_field_spec_bool::				getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_alphanumeric> IO_field_spec_alphanumeric::	getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_character> IO_field_spec_character::		getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_uint64> IO_field_spec_uint64::				getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_time_point> IO_field_spec_time_point::		getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_tm> IO_field_spec_tm::						getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_integer> IO_field_spec_integer::			getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_decimal> IO_field_spec_decimal::			getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }
std::optional<Data_type_scientific> IO_field_spec_scientific::		getData_value_location() const { if ( is_null_field_location[field_index_in_row] == false ) return data_location; else return std::nullopt; }

void IO_field_spec_bool::			clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_character::		clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_uint64::			clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_time_point::		clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_tm::				clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_alphanumeric::	clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_integer::		clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_decimal::		clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }
void IO_field_spec_scientific::		clear_data_value()    { data_location = {}; is_null_field_location[field_index_in_row] = true; }

void IO_field_spec_bool::			setData_range(const IO_field_spec_bool::Data_range &value) 		{ data_range = value; }
void IO_field_spec_integer::		setData_range(const IO_field_spec_integer::Data_range & value) 			{  data_range = value; }
void IO_field_spec_character::		setData_range(const IO_field_spec_character::Data_range &value) 	{  data_range = value; }
void IO_field_spec_uint64::			setData_range(const IO_field_spec_uint64::Data_range &value) 		{  data_range = value; }
void IO_field_spec_time_point::		setData_range(const IO_field_spec_time_point::Data_range &value) 	{  data_range = value; }
void IO_field_spec_tm::				setData_range(const IO_field_spec_tm::Data_range &value) 			{  data_range = value; }
void IO_field_spec_alphanumeric::	setData_range(const IO_field_spec_alphanumeric::Data_range &value)	{  data_range = value; }
void IO_field_spec_decimal::		setData_range(const IO_field_spec_decimal::Data_range &value) 		{  data_range = value; }
void IO_field_spec_scientific::		setData_range(const IO_field_spec_scientific::Data_range &value) 	{  data_range = value; }

IO_field_spec_bool::Data_range			IO_field_spec_bool::		getData_range() const { return data_range; }
IO_field_spec_integer::Data_range		IO_field_spec_integer::		getData_range() const { return data_range; }
IO_field_spec_character::Data_range		IO_field_spec_character::	getData_range() const { return data_range; }
IO_field_spec_uint64::Data_range		IO_field_spec_uint64::		getData_range() const { return data_range; }
IO_field_spec_time_point::Data_range	IO_field_spec_time_point::	getData_range() const { return data_range; }
IO_field_spec_tm::Data_range			IO_field_spec_tm::			getData_range() const { return data_range; }
IO_field_spec_alphanumeric::Data_range	IO_field_spec_alphanumeric::getData_range() const { return data_range; }
IO_field_spec_decimal::Data_range		IO_field_spec_decimal::		getData_range() const { return data_range; }
IO_field_spec_scientific::Data_range	IO_field_spec_scientific::	getData_range() const { return data_range; }

void IO_field_spec::print() const {
    cerr <<"***Spec: name: " << this->id_name <<", user_prompt:"<<user_prompt<<", full_desc:"<<full_description<<", help:"<<help<<", pre_fill:"<<pre_fill_value<<", lengths_input:"<<lengths_input;
    struct PrintVisitor {
        void operator() ( ValidityFieldSpecBool const  spec ) const {
            cerr <<", ValidityFieldSpecBool:";
            spec.base.print();
            //cerr << spec.value_default_approve_equivalents;
            //cerr << ", ";
            //cerr << spec.value_default_deny_equivalents;
            cerr << ", ";
            cerr << spec.value_default_approve_prompt;
            cerr << ", ";
            cerr << spec.value_default_deny_prompt;
            cerr << ", ";
            cerr << spec.is_ignore_case_on_validation;
            cerr << ". ";
        }
        void operator() ( ValidityFieldSpecAlphanum  const  spec ) const {
            cerr <<", ValidityFieldSpecAlphanum:";
            spec.base.print();
            cerr <<", ";
            spec.lengths_storage.print();
            cerr <<", is_ignore_case_on_validation:";
            cerr << spec.is_ignore_case_on_validation;
            cerr << ". ";
        }
        void operator() ( ValidityFieldSpecInteger const  spec ) const {
            cerr <<", ValidityFieldSpecDecimal:";
            spec.base.print();
            cerr <<", ";
            spec.range.print();
            cerr <<", is_signed:";
            cerr << spec.is_signed;
            cerr << ". ";
        }
        void operator() ( ValidityFieldSpecDecimal const & spec ) const {
            cerr <<", ValidityFieldSpecDecimal:";
            spec.base.print();
            cerr <<", ";
            spec.range.print();
            cerr <<", is_signed:";
            cerr << spec.is_signed;
            cerr << ". ";
        }
    };
    // prompt_field_spec, security spec
    cerr <<", is_optional:"<<is_optional<<", is_data_value_null:"<<is_data_value_null<<", is_initial_focus:"<<is_initial_focus<<", is_current_focus:"<< is_current_focus<<", is_greyed_out: "<<is_greyed_out<<", is_programmer_only:"<<is_programmer_only<<", is_system_calculated_read_only:"<<is_system_calculated_read_only<< ". ";
    std::visit( PrintVisitor(), *validity_spec );
}
void IO_field_spec_bool::			print() const { IO_field_spec::print(); cerr <<"data_location:"<<data_location<< endl; }
void IO_field_spec_character::		print() const { IO_field_spec::print(); cerr << endl; }  // todo: complete this: as above.
void IO_field_spec_uint64::			print() const { IO_field_spec::print(); cerr << endl; }
void IO_field_spec_time_point::		print() const { IO_field_spec::print(); cerr << endl; }
void IO_field_spec_tm::				print() const { IO_field_spec::print(); cerr << endl; }
void IO_field_spec_decimal::		print() const { IO_field_spec::print(); cerr << endl; }
void IO_field_spec_scientific::		print() const { IO_field_spec::print(); cerr << endl; }
void IO_field_spec_alphanumeric::	print() const { IO_field_spec::print(); cerr << endl; }

void IO_field_spec_integer::		print() const { IO_field_spec::print(); cerr << endl; }
