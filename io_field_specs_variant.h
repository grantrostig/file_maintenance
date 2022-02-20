#ifndef IO_FIELD_SPECS_VARIANT_H
#define IO_FIELD_SPECS_VARIANT_H

#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <unordered_set>
#include "state_menu.h"
#include "io_field.h"

using State_menu_variant = std::variant<State_menu>;

using  IO_field_spec_variant = std::variant<
    // IO_field_spec,  // todo: is this a good idea?
    IO_field_spec_bool,
    //IO_field_spec_character,
    //IO_field_spec_uint64,
    //IO_field_spec_tm,
    //IO_field_spec_time_point,
    IO_field_spec_alphanumeric,
    IO_field_spec_integer,
    IO_field_spec_decimal
    //IO_field_spec_scientific
>;



#endif // IO_FIELD_SPECS_VARIANT_H
