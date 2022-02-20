#ifndef ACTION_IO_ROW_H
#define ACTION_IO_ROW_H
#include <string>
#include <vector>
#include <any>
#include <variant>
#include <filesystem>
#include "state_menu.h"
#include "state_application.h"
#include "interaction_result.h"
#include "io_table.h"

InteractionResult action_io_row_print(   		State_menu & state, IO_row_variant const & row );
InteractionResult action_io_row_print_index( 	State_menu & state, IO_table const & table, size_t const & row_index );
InteractionResult action_io_row_list_row(      State_menu & state, IO_table const & table);
InteractionResult action_io_row_list_rows( 		State_menu & state, IO_table const & table, Row_range const & row_range ); // if Row_range is ommitted then, do whole container.
InteractionResult action_io_row_list_rows(     State_menu & state, IO_table const & table);
InteractionResult action_io_search_rows(        State_menu & state, IO_table const & table );
InteractionResult action_io_row_search_list_rows(State_menu & state, IO_table const & table);
InteractionResult action_io_browse_rows(		State_menu &state, const IO_table &table);

InteractionResult action_io_row_select( 		State_menu & state, IO_table & table );
InteractionResult action_io_row_create( 		State_menu & state, IO_table & table );
InteractionResult action_io_row_update( 		State_menu & state, IO_table & table );
InteractionResult action_io_row_delete( 		State_menu & state, IO_table & table );

#endif // ACTION_IO_ROW_H
