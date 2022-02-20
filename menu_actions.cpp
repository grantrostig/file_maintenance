#include <iostream>
#include <algorithm>
//#include "lib_tty.h"
#include "menu.h"
#include "menu_actions.h"
#include "window_panel.h"
#include "interaction_result.h"
#include "action_dialog.h"

// #include "menu_option.h"

using std::cin, std::cout, std::endl, std::cerr;

// ---- Actions called by menu which are part of the menuing control system, not the general application.

InteractionResult action_save_work( State_menu & state) {  // always save the data if the user requests it, even if we KNOW that there was not change.
    // We could be saving many things here.  Currently we assume only one thing to save, that being the entire database, which comprises all our "work"
    InteractionResult ir { action_save_changes_to_disk( state )};

    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    assert( verify_interaction_result_nav( ir.hot_key, InteractionCategory::menu ));

    // we don't need to do any more processing based on how the user responded.  //if ( nav == InteractionResultNav::retain_menu )  //  User didn't ask to exit or esc.
    return { {}, {}, {}, {}, nav };
}

InteractionResult action_program_exit_with_prompts( State_menu & state) {
                                                    cerr << "int action_program_exit_with_prompts( State & state) {" << endl;
    if ( state.getApplication_data_sp()->getIs_data_unsaved() ) {
        InteractionResult ir_modal = action_dialog_modal_approve(state, "Save unsaved data?");
        if ( std::get<Data_type_boolean>(ir_modal.data) )
            action_save_changes_to_disk( state );  // discard IR we are getting out anyway, ie. not matter what. //todo: process ir: What if user now doesn't want to exit anymore?
        else {
            std::string message {"Work not saved, as per your request."};
            pagination_reset( state, { 1, message.length() });  // we IGNORE any user input on this output, we are getting out!
            cout << "\n" << message << endl;
        }
    }
    return { {}, {}, {}, {}, InteractionResultNav::exit_all_menu };
}

InteractionResult action_program_exit_immediately( State_menu & state ) {  // we might use it in later versions?
                                    cerr << "int action_program_exit_immediately( State & state) {" << endl;
    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::exit_all_menu };  //std::exit(0);  // todo: is this really correct in the context of c++ as opposed to Unix??
}

InteractionResult action_go_back_up_menu( State_menu & state) {
    // go back up one levels of menu, only if we are not already at main menu.
    if ( state.is_menu_current_main() ) {
        cerr << "int action_go_back( State &): retain menu." << endl;
        return { {}, {}, {}, {}, InteractionResultNav::retain_menu };
    }
    cerr << "int action_go_back( State &): prior menu." << endl;
    return { {}, {}, {}, {}, InteractionResultNav::prior_menu };  // note: going to PRIOR!
}

InteractionResult action_print_menu_help( State_menu & state, Menu const & menu) {
    menu.print( state );
                                    cerr << "int action_print_menu_help( State &) "<<endl;
    InteractionResult ir 		{ pagination( state, { .height= 1, .width= menu.help.length() })};  // we assume that menu.help() will fit on one screen so we check once and them print entire help in one statement below.
    InteractionResultNav nav = 	find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    assert( verify_interaction_result_nav( ir.hot_key, InteractionCategory::menu ));

    if ( nav == InteractionResultNav::retain_menu )  //  User didn't ask to exit or esc.
        cout << menu.help <<endl;  // print entire message without further pagination.  todo: probably sub-optimal.
    return { {}, {}, {}, {}, nav };
}

InteractionResult action_print_menu( State_menu & state, Menu const & menu) {
    InteractionResult ir 		{ menu.print( state )};
    InteractionResultNav nav = 	find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    assert( verify_interaction_result_nav( ir.hot_key, InteractionCategory::menu ));
    return { {}, {}, {}, {}, nav };
}

InteractionResult action_home_menu( State_menu & ) {
    cerr << "int action_home_menu( State & state):" << endl;
    return { {},{}, {}, {}, InteractionResultNav::main_menu };  // note: going to MAIN.
}

/*/InteractionResult action_menu_forward_pagination( State_menu & ) {  // todo: complete this: need a "pagination menu" that can be used anytime that a variable size list is printed.
//    cerr << "int action_list_forward( State & state) { " << endl;
//    return { {}, {}, InteractionResultNav::continue_forward_pagination };
//}

//InteractionResult action_menu_backward_pagination( State_menu & ) {  // todo: need a "pagination menu" that can be used anytime that a variable size list is printed.
//    cerr << "int action_list_backward( State & state) { " << endl;
//    return { {}, {}, InteractionResultNav::continue_backward_pagination };
//}

//Action_post_return_struct action_undo( State &) {  // todo: complete this
//    cerr << "int action_undo( State &) {" << endl;

//    return 0;
//}
//Action_post_return_struct action_space_forward( State &) {  // todo: does this call action_menu_forward_navigation?
//    cerr << "int action_space_forward( State &) {" << endl;

//    return 0;
//}
//Action_post_return_struct action_tab_forward( State &) {  // todo: does this call action_menu_forward_navigation?
//    cerr << "int action_tab_forward( State &) {" << endl;

//    return 0;
//}
//Action_post_return_struct action_null_forward( State &) {  // todo: does this call action_menu_forward_navigation?
//    cerr << "int action_null_forward( State &) {" << endl;
//    return 1;
//}
*/

