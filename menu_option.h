#ifndef MENU_OPTION_H
#define MENU_OPTION_H
#include <string>
#include <functional>
#include <variant>
//#include "lib_tty.h"
#include "state_menu.h"
#include "menu_actions.h"
#include "actions.h"
#include "action_io_row.h"
#include "process_menu.h"

// using my_test = decltype (action_program_exit_with_prompts);  // todo: used below for another todo:
using Menu_action_fn_variant = std::variant<
                // my_test, // BAD
                // decltype (action_program_exit),  // BAD todo: TODO why can't I decltype a function? Alternatives?
                // Menu_option, // BAD
                // std::function<Action_post_return_struct>,   // BAD
                // std::function<Action_post_return_error()>,  // OK
                // std::function< InteractionResult() >,  // NOTE: each of these appear in function_object_overload_set in process_menu_selection.
                std::function< InteractionResult( State_menu & )>,
                std::function< InteractionResult( State_menu &, Menu const & )>
                //std::function< InteractionResult( State_menu &, std::shared_ptr<Menu> )>
                //std::function< InteractionResult( State_menu &, IO_table & )>,
                //std::function< InteractionResult( State_menu &, const IO_table & )>

                //std::function<decltype( action_program_exit_with_prompts )>,
                //std::function<decltype( action_print_menu 	)>
                //std::function<decltype( action_io_row_print 	)>,
                //std::function<decltype( action_io_row_print_index 	)>,
                //std::function<decltype( action_io_row_list_rows 	)>,
                //std::function<decltype( action_io_row_create )>,
                //std::function<decltype( action_save_as_changes_to_disk )>
              >; // a function with the code for the action to be performed.

struct Menu_option {
    std::string input_token {}; // 'hot-key' when set by for universal_options.
    std::string name {};  // short name shown on menu.
    std::string description {}; // long name.
    std::string help {}; 	// full help explanation of this option and how this option relates to other options.
    std::string loading_message {};  // printed while loading the action. Shown until the first prompt from the action.

    // int  (* action_ptr2)(double) = nullptr;
    // std::any 					my_callable_any;

    Menu_action_fn_variant menu_action_fn_variant; // an optional (really?) function for the "action" to be performed.
    // int			option_value {0};  // todo: implement this, this has meaning only if above is null, else if 0 = stay on menu, otherwise return that value. // todo: need a way to formalize return codes, but meaning varies depending on function that return it.

    short		sort_order 				{0};  // optional, if zero then ordered by order of creation todo: not used.
    bool		is_advanced_menu_option {false}; // todo: not used.
    bool		is_enabled_option 		{true};  // todo: not used.
    bool		is_warning_enabled 		{false}; // todo: not used.

    InteractionResult print( State_menu &) const;
};

#endif // MENU_OPTION_H
