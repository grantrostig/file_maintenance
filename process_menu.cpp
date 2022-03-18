#include "process_menu.h"
#include <iostream>
#include <cassert>
#include "state_menu.h"
#include "menu.h"
#include "menu_option.h"
#include "window_panel.h"
#include "lib_tty.h"  // todo: don't seem to need it due to other headers, but I should have it here.
#include "interaction_result.h"
#include "io_table.h"
#include "actions.h"
#include "action_io_row.h"
#include "action_dialog.h"
using std::endl, std::cin, std::cerr, std::cout;

Menu_option const & input_menu_selection( State_menu & state_menu, std::shared_ptr<Menu> const menu_sp ) {  // loop until we return either a menu selection or a number representing the entry to be selected
    while (true) {
        auto r = pagination( state_menu, {.height= 1, .width= menu_sp->name.length() + 3 + 1} );  // +1 for cursor position.
        // todo: complete this above.
        cout << menu_sp->name<<">> ";
        cout.flush();

        pagination_reset( state_menu, {0,0});
        auto const [kb_regular_value, hot_key, file_status] =
                                        get_kb_keys_raw( 1, false, true /* echo skc */, true, false );
        cout << endl;  									// We finish our prompt and user input.

        // *** handle file_status ***
                // todo: this

        // *** handle hot_keys ***
        //Kb_regular_value user_menu_selection { kb_regular_value };
        //if ( HotKeyFunctionCat::help_popup == hot_key.function_cat  )
            //user_menu_selection = 'h';  // WARNING NOTE: todo: improve this! Here we link the hot_key to the regular key for getting "help", ie. F1 key. The mapping is found in lib_tty::consider_hot_key()::hot_keys.

        // WARNING NOTE: todo: improve this! Here we link the hot_key to the regular key for getting "help", ie. F1 key. The mapping is found in lib_tty::consider_hot_key()::hot_keys.
        Kb_regular_value user_menu_selection { hot_key.function_cat == HotKeyFunctionCat::help_popup
                                                ? "h" : kb_regular_value };
        // *** handle kb_regular_value ***
        for ( Menu_option const & menu_option: menu_sp->options )
            if ( user_menu_selection == menu_option.input_token )
                return menu_option;

        cout << "\aInvalid menu selection or key press, try again, or press the single <h> key for (h)elp.\n";
    }
    assert(false);  // infinute loop that is exited via a menu selection.  Should never get here, obviously not needed, but placed here for clarity. todo:  fix this up.
}

// The 'Overloaded' pattern _could_ have been standardized in c++20 but wasn't, as per Stroustup in A Tour of C++ 2nd Edition, Page 176.
template<class... Ts>  				// variadic ie. any number of any type are handled. Below we use several in the contructor of our one object. // ... is a parameter pack.
struct Overloaded : Ts... { 		// inherit from all those types  todo:  how does this work??
    using Ts::operator()...;  		// all of those operator()s.  // new in c++17.
};

template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;  // deduction guide.  -> = returns.  todo: explain: what returns what to whom??

InteractionResult const process_menu_selection( State_menu & state, Menu_option const & menu_selection ) {
    // get some general data, even though it is not used for every overload.
    std::shared_ptr<Menu>   current_menu_sp 	{ state.menu_top_sp() };  	// todo: I don't think I need a value in the lambda, because it is overridden at std::visit?
    Menu 		    current_menu 	{ *current_menu_sp };
    State_menu		    kludge_state	{};
    /* 								//Menu 					kludge_menu			{};
    // std::shared_ptr<Menu> 	kludge_menu_sp 		{};
    // IO_table  				kludge_table   		{}; 			// state.io_table_sp();  // todo: I don't think I need a value in the lambda, because it is overridden at std::visit?A
    // IO_row_variant 			kludge_row_var 		{};
    // Row_range				kludge_row_range 	{};
    // size_t					kludge_row_index 	{};
    // std::filesystem::path	kludge_file_path 	{};
    // if ( std::get<>(menu_selection.menu_action_fn) != nullptr) {
        // auto r = std::get<std::function<Action_post_return_struct(State&)>>(menu_selection.menu_action_fn_variant)(state);
    */

    auto function_object_overload_set = Overloaded {
        [&kludge_state ]  			   ( std::function< InteractionResult(State_menu &              ) > &af)
            { auto r = af( kludge_state );
              cerr << "\n [] state overload.\n";
              return r; }, 						// to be invoked via operator() on the function object (AKA lambda) or any callable object.
        [&kludge_state, &current_menu] ( std::function< InteractionResult(State_menu &, Menu const &) > &af)
            { auto r = af( kludge_state, current_menu );
              cerr << "\n [] state,menu overload.\n";
              return r; }, 						// to be invoked via operator() on the function object (AKA lambda) or any callable object.
     /*   							//[]  						(std::function<InteractionResult()> 									        &af) { auto r = af(); cerr<<"\nvoid overload.\n";        			    return r; }, // to be invoked via operator() on the function object (AKA lambda) or any callable object.
        // [&kludge_state, &kludge_menu_sp](std::function<InteractionResult(State_menu &, std::shared_ptr<Menu>)> 	&af) { auto r = af( kludge_state, kludge_menu_sp ); cerr<<"\nmenu_sp overload.\n";    			    return r; }, // to be invoked via operator() on the function object (AKA lambda) or any callable object.
        //[&kludge_state, &kludge_table]	(std::function<InteractionResult(State_menu &, IO_table &)> 			&af) { auto r = af( kludge_state, kludge_table );cerr<<"\ntable overload.\n";    			    return r; }, // to be invoked via operator() on the function object (AKA lambda) or any callable object.
        //[&kludge_state, &kludge_table]	(std::function<InteractionResult(State_menu &, IO_table const &)> 			&af) { auto r = af( kludge_state, kludge_table );cerr<<"\ntable overload.\n";    			    return r; }, // to be invoked via operator() on the function object (AKA lambda) or any callable object.
      //[&state ]  				(std::function<decltype(action_program_exit_with_prompts)> &af) { auto r = af(state);          			return r; }, // to be invoked via operator() on the function object (AKA lambda) or any callable object.
      //[&state, &menu]    		(std::function<decltype(action_print_menu)> &af) 				{ auto r = af(state,menu);      		return r; }, // NOTE: each of these must also appear in Menu_action_fn_variant type!
      //[&state, &menu_sp] 		(std::function<decltype(process_menu)> &af) 					{ auto r = af(state,menu_sp); 			return r; },
      //[&state, &kludge_table]  	(std::function<decltype(action_io_row_create)> &af)				{ auto r = af(state,kludge_table);    	return r; },
      //[&state, &kludge_row_var] (std::function<decltype(action_io_row_print)> &af)				{ auto r = af(state,kludge_row_var);    return r; },
      //[&state, &kludge_table, &kludge_row_index]   (std::function<decltype(action_io_row_print_index)> &af)	{ auto r = af(state,kludge_table,kludge_row_index);    	return r; },
      // [&state, &kludge_table, &kludge_row_range]   (std::function<decltype(action_io_row_list_rows)> &af)	{ auto r = af(state,kludge_table,kludge_row_range);    	return r; },
      // [&state, &kludge_file_path]   (std::function<decltype(action_save_as_changes_to_disk)> &af)		        { auto r = af(state,kludge_file_path);  return r; }
      */
    };
    auto fo_return_type_overloaded_set = [ & ] (auto... args) -> InteractionResult
                                                    { return function_object_overload_set( args...); };  // not needed if all return types are the same, which is the current case and that is used in the return type of this function.
    InteractionResult action_result = std::visit( fo_return_type_overloaded_set,
                                                    menu_selection.menu_action_fn_variant );
    return action_result;
    // } else  // return menu_selection.option_value;  todo: this may or may not be applicable depending on the use of menus to select values to be used by other? functions.
}

InteractionResult const process_menu( State_menu & state, 		// needed for menu and application, like a global variable/singleton.  todo: improve this?
                                      std::shared_ptr<Menu> menu_sp 	// the menu we will run/process
                                      ) {
    assert( menu_sp != nullptr );
    state.push_menu_sp( menu_sp ); // our currently active menu is stored at top of stack
    while (true) {  /* input menu selection and run it */
        Menu_option const menu_option  	{ input_menu_selection( state, menu_sp ) };
        InteractionResult const ir     	= process_menu_selection( state, menu_option );
        //irn = verify_interaction_result_nav( state, process_menu_selection( state, input_menu_selection( state, menu_sp )).hot_key,  InteractionCategory::menu );

        // switch ( irn ) { 					// post processing: load the correct menu to be shown to user, or exit.
        switch ( ir.navigation ) { 					// post processing: load the correct menu to be shown to user, or exit.
        case InteractionResultNav::retain_menu:  	// we did something on this menu and we stay on current menu and just loop.
            break;
        case InteractionResultNav::prior_menu:   	// we have been asked to go back one level of menu so we get and set that menu, and loop.
            menu_sp = state.menu_pop_top_sp();
            break;
        case InteractionResultNav::main_menu:   	// we have been asked to go directly to main menu so we set that menu and loop.
            menu_sp = state.menu_pop_to_sp(state.getMenu_main());
            break;
        case InteractionResultNav::exit_all_menu:   	// we have been asked to go directly to main menu so we set that menu and loop.
            return { {}, {}, {}, {}, InteractionResultNav::exit_all_menu };  // returning to main to get out. return result is probably not needed?

        // *** above are valid return values from running an "action_", *** below are simply debugging code.
        case InteractionResultNav::exit_pgm_immediately:
            // return { {}, {}, {}, {}, InteractionResultNav::exit_pgm_immediately };  // returning to main to get out. return result is probably not needed?
        case InteractionResultNav::exit_pgm_with_prompts:
            // return { {}, {}, {}, {}, InteractionResultNav::exit_pgm_with_prompts };  // returning to main to get out. return result is probably not needed?
        case InteractionResultNav::continue_forward_pagination:
        case InteractionResultNav::continue_backward_pagination:
        case InteractionResultNav::exit_fn_immediately:
        case InteractionResultNav::exit_fn_with_prompts:
        case InteractionResultNav::prior_menu_discard_value:
        case InteractionResultNav::up_one_field:
        case InteractionResultNav::down_one_field:
        case InteractionResultNav::up_one_block:
        case InteractionResultNav::down_one_block:
        case InteractionResultNav::save_form_as_is:
        case InteractionResultNav::skip_one_field:
        case InteractionResultNav::skip_to_end_of_fields:
        case InteractionResultNav::next_row:
        case InteractionResultNav::prior_row:
        case InteractionResultNav::first_row:
        case InteractionResultNav::last_row:
        case InteractionResultNav::na:
            assert(( "Logic error: process_menu: invalid InteractionResultNav after doing menu selection.", false));
            break;
        }
    }
}

InteractionResult const process_main_menu(State_menu & state ) {  // cannot overload process_menu, or Overload become ambiguous.  todo: TODO Is there a fix/cast?
    action_print_menu_help( state, *state.getMenu_main());
    return process_menu( state, state.getMenu_main());
}
