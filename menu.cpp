#include <iostream>
#include <cassert>
#include <set>
#include <functional>
#include <memory>
#include "menu.h"
#include "menu_option.h"
#include "state_menu.h"
#include "menu_actions.h"
#include "actions.h"
#include "action_io_row.h"
#include "window_panel.h"

using std::cout, std::cin, std::cerr, std::endl;

InteractionResult Menu::print( State_menu & state_menu) const {  // Note: we might use state in the future, for example to designate where to "print".
    // pagination_reset( state_menu, {1,0} );
    pagination_reset( state_menu, {.height = 1, .width = name.length() + 2 } );  // todo: TODO is it a bad idea to use this C99?
    cout << name << ": ";
    cout.flush();
    InteractionResult ir {};
    for (Menu_option const & option: options ) {
        ir = option.print( state_menu );
        ir = pagination( state_menu, {.height = 0, .width = 2} );  // todo: TODO is it a bad idea to use this C99?
        if ( ir.navigation != InteractionResultNav::continue_forward_pagination ) {
            cout << ", ";  // todo: complete this, it's a bug.
        }
        else {} // todo: complete this. should rollback the prior pagination!! chicken or the egg situation.
    }
    /* todo: complete this above, code to deal with replacing trailing ","
//    for (auto i = state.getMenu_current()->options.begin(); i != state.getMenu_current()->options.end(); i++) {
//       cout << i->name;
//       if (i+1 != state.getMenu_current()->options.end()) cout << ",";
//    }*/
    ir = pagination( state_menu, {.height = 1, .width = 1} );  // todo: TODO is it a bad idea to use this C99?
    cout << ".\n";
    return ir;
}

void initialize_universal_options(State_menu & state, std::shared_ptr<Menu> menu) {
    // menu->name has already been set by menu that these options are being added too.
    // menu->description
    menu->options.emplace_back (  Menu_option
    { "q",
      "quit",
      "Quit, exit the program, optionally with prompts to save data.",
      "",
      "",
      //Menu_action_function_1 {}
       //{ action_program_exit_with_prompts }
        [&state] ( State_menu & kludge_state ) { return action_program_exit_with_prompts( state ); }
                                    });
    menu->options.emplace_back ( Menu_option
    { "T",
       "Terminate",
       "Terminate, exit the program immediately, without saving data or freeing resources.",
       "",
       "",
        // { action_program_exit_immediately }
        [&state] ( State_menu & kludge_state ) { return action_program_exit_immediately( state ); }
                                      });
    menu->options.emplace_back (
        Menu_option
    { "h",
       "menu help",
       "Print instructions on how to use this menu, and the menu system in general.",
       "",
       "",
        //{ action_print_menu_help }
        [&state] ( State_menu & kludge_state, Menu const & menu ) { return action_print_menu_help( state, menu ); }
                                      });
    menu->options.emplace_back ( Menu_option
    { "m",
       "menu",
       "Print a list of the actions that can be selected from the current menu.",
       "",
       "",
        //{ action_print_menu }
        [&state] ( State_menu & kludge_state, Menu const & menu ) { return action_print_menu( state, menu ); }
                                      });
    menu->options.emplace_back ( Menu_option
    { "M",
       "main menu",
       "Go to the main menu, AKA home page for the application.",
       "",
       "",
        { action_home_menu }  // accesses nothing!
                                      });
    menu->options.emplace_back ( Menu_option
    { "b",
       "back menu",
       "Go to the prior menu, ie. back one level of the menu heirarchy.",
       "",
       "",
        //{ action_go_back_up_menu }
        [&state] ( State_menu & kludge_state ) { return action_go_back_up_menu( state ); }
                                      });
/*
//    menu->options.emplace_back ( Menu_option
//    { "F",
//       "forward",  // todo:  NEED TO CREATE a letter for BACKWARD ALSO!!
//       "Continue forward in printing the next page of information",
//       "",
//       "",
//        { action_menu_forward_pagination }
//                                      });
//    menu->options.emplace_back ( Menu_option
//    { "u",
//       "undo",
//       "",
//       "",
//       "",
//       std::bind( action_undo ) > { action_}
//                                      });
//    menu->options.emplace_back ( Menu_option
//    { "R",
//       "redo changes",
//       "",
//       "",
//       "",
//        { action_}
//                                      });
//    menu->options.emplace_back ( Menu_option
//    { " ",
//      "<SPACE>",
//      "",
//      "",
//      "",
//      std::bind( action_) > { action_}
//                                      });
//    menu->options.emplace_back ( Menu_option
//    { "T",
//      "<TAB>",
//      "",
//      "",
//      "",
//      std::bind( action_tab_forward ) > { action_}
//                                      });
//    menu->options.emplace_back ( Menu_option
//    {  "N",
//       "<NULL>",
//       "",
//       "",
//       "",
//       std::bind( action_null_forward ) > { action_}
//                                      });
*/

    // test for duplicate input_tokens, or action names, using sets.
    cerr << "Menu construction sanity check: " << endl;
    std::set<decltype(Menu_option::input_token)> t {};
    std::set<decltype(Menu_option::name)> n {};
    for (Menu_option & m : menu->options) {
        auto[tmp1,t_inserted] = t.insert( m.input_token );
        if (t_inserted == false) cerr << "duplicate menu option input token: " << m.name << endl;
        auto[tmp2,n_inserted] = n.insert( m.name );
        if (n_inserted == false) cerr << "duplicate menu option name:        " << m.name << endl;
    }
}

// --------------------------------------------------------------------------------

void initialize_menu_main_options(State_menu & state, std::shared_ptr<Menu> menu ) { // this this the top level menu, ie. the first menu entered on running the program.
            menu->name = "Main Menu";
            menu->description = "The first menu shown: Main or Home menu.";
            menu->options.emplace_back( Menu_option
            { "f",
              "file menu",
              "Access the file menu to allow saving, or loading data from the disk." ,
              "help.",
              "preparing...",
              [&state] (State_menu & kludge_state ) { return process_menu( state, state.getMenu_file() ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "e",
              "edit menu",
              "Access the edit menu to allow " ,
              "help.",
              "preparing...",
              [&state] (State_menu & kludge_state  ) { return process_menu( state, state.getMenu_edit() ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "g",
              "settings menu",
              "Access the settigns menu to allow " ,
              "settings help.",
              "preparing...",
              [&state] (State_menu & kludge_state  ) { return process_menu( state, state.getMenu_settings() ); }
                                             });
            menu->options.emplace_back( Menu_option
                 { "l",
                  "list entries",
                  "Print all the account-password entries to terminal." ,
                  "help.",
                  "preparing...",
                  //[] ( State_menu & state /*ignored*/, IO_table const & placeholder_1, Row_range const & placeholder_2 ) { return action_io_row_list_rows( state, placeholder_1, placeholder_2 ); }
                  [&state] ( State_menu & kludge_state ) { return action_io_row_list_rows( state, io_table_persons ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "r",
              "search entries",
              "Print all entries that match search criteria to terminal." ,
              "help.",
              "preparing...",
              // { action_print_search_entries }
              [&state] (State_menu & kludge_state ) { return action_io_row_search_list_rows( state, io_table_persons ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "t",
              "select entry",
              "Select an id for a specific entry to view or change it." ,
              "help.",
              "preparing...",
              [&state] (State_menu & kludge_state ) { return action_io_row_select( state, io_table_persons ); }
                                             });
            menu->options.emplace_back( Menu_option
            {  "X",
               "delete ALL",
               "WARNING:Deletes ALL ENTRIES, nothing left!, empty!" ,
               "help.",
               "preparing...",
              [&state] (State_menu & kludge_state ) { return action_delete_all( state ); }
                                             });
            menu->options.emplace_back( Menu_option
            {  "s",
               "save all",
               "Save all work to disk." ,
               "help.",
               "preparing...",
              [&state] (State_menu & kludge_state ) { return action_save_work( state ); }
                                             });
            initialize_universal_options( state, menu );
        }

void initialize_menu_file_options(State_menu & state, std::shared_ptr<Menu> menu) {
            // this this the top level menu, ie. the first menu entered on running the program.
            menu->name = "File Menu";
            menu->description = "File Menu to allow saving of user data.";
            menu->options.emplace_back
                ( Menu_option
            { "s",
              "save",
              "save changes to disk" ,
              "save help.",
              "preparing...",
              //{ action_save_as_changes_to_disk }
              [&state] (State_menu & kludge_state ) { return action_save_changes_to_disk( state ); }
                                             });
            menu->options.emplace_back
                ( Menu_option
            { "a",
              "save as",
              "save changes to disk as different file" ,
              "save help.",
              "preparing...",
               //{ action_save_as_changes_to_disk }
              [&state] (State_menu & kludge_state ) { return action_save_as_changes_to_disk( state ); }
                                             });
            menu->options.emplace_back
                ( Menu_option
            { "l",
              "load",
              "load data from disk." ,
              "load help text.",
              "preparing...",
              // { action_load_data_from_disk }
              [&state] (State_menu & kludge_state ) { return action_load_data_from_disk( state ); }
                                             });
            menu->options.emplace_back
                ( Menu_option
            { "L",
              "load from",
              "load data from a different file on the disk." ,
              "load as help text.",
              "preparing...",
              // { action_load_data_from_disk }
              [&state] (State_menu & kludge_state ) { return action_load_as_data_from_disk( state ); }
                                             });
            menu->options.emplace_back
                ( Menu_option
                 { "S",
                  "backup",
                  "Backup changes to disk, a big (S)ave that make a backup or archive copy, as opposed to a regular save which saves changes to the current working copy of the data." ,
                  "backup help.",
                  "preparing...",
                  // { action_backup_data }
                  [&state] ( State_menu & kludge_state ) { return action_backup_data( state ); }
                 });
            initialize_universal_options( state, menu );
        }

void initialize_menu_edit_options(State_menu & state, std::shared_ptr<Menu> menu) {
            menu->name = 		"Edit Menu";
            menu->description = "Edit Menu to allow editing/changing/deleting user data.";
            menu->options.emplace_back( Menu_option
            { "l",
              "list entries",
              "Print all the account-password entries to terminal." ,
              "help.",
              "preparing...",
              //[] ( State_menu & state /*ignored*/, IO_table const & placeholder_1, Row_range placeholder_2 ) { return action_io_row_list_rows( state, placeholder_1, placeholder_2 ); }
              [&state] ( State_menu & kludge_state ) { return action_io_row_list_rows( state, io_table_persons ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "r",
              "search entries",
              "Print all entries that match search criteria to terminal." ,
              "help.",
              "preparing...",
               //{ action_print_search_entries }
              [&state] (State_menu & kludge_state ) { return action_io_row_search_list_rows( state, io_table_persons ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "t",
              "select entry",
              "Select an id for a specific entry to view or change it." ,
              "help.",
              "preparing...",
              //[] (State_menu & state, IO_table & placeholder_1 ) { return action_io_row_select( state, placeholder_1); }
              [&state] (State_menu & kludge_state ) { return action_io_row_select( state, io_table_persons ); }
                                             });
            menu->options.emplace_back(
                Menu_option
            { "v",
              "view entry",
              "To view an entry, enter its 'id'.",
              "view help.",
              "preparing...",
              // { action_entry_viewing }
            [&state] (State_menu & kludge_state ) { return action_io_row_list_row( state, io_table_persons ); }
                                             });
            menu->options.emplace_back(
                Menu_option
                { "u",
              "update/edit entry",
              "To edit/update an entry, enter its 'id'.",
              "edit help.",
              "preparing...",
              [&state] (State_menu & kludge_state ) { return action_io_row_update( state, io_table_persons );}
                                             });
            menu->options.emplace_back(
                Menu_option
            { "a",
              "add new entry",
              "Create a whole new user and password entry.",
              "help.",
              "preparing...",
              //std::function< decltype(action_io_row_create ) > { std::bind( action_io_row_create, std::ref(state), std::placeholders::_1 )}  // todo: TODO why doesn't placeholder work here?
              //{ std::bind( action_io_row_create, std::ref(state), std::placeholders::_1 )}  // todo: TODO why doesn't placeholder work here?
              //std::function< decltype(action_io_row_create ) > { std::bind( action_io_row_create, std::ref(state), io_table_persons )}
              //std::function< decltype(action_io_row_create ) > { action_io_row_create }
              //[] ( State_menu & state /*ignored*/, IO_table & placeholder_1 ) { return action_io_row_create( state, placeholder_1); }
              [&state] ( State_menu & kludge_state ) { return action_io_row_create( state, io_table_persons); }
                                             });
            menu->options.emplace_back(
                Menu_option
            { "d",
              "delete selected",
              "Print all the types of entries to terminal." ,
              "help.",
              "preparing...",
               //{ action_delete_entry }
              [&state] (State_menu & kludge_state ) { return action_io_row_delete( state, io_table_persons ); }
                                             });
            menu->options.emplace_back(
                Menu_option
            {  "X",
               "Delete ALL",
               "WARNING:Deletes ALL ENTRIES, nothing left!, empty!" ,
               "help.",
               "preparing...",
                //{ action_delete_all }
              [&state] (State_menu & kludge_state ) { return action_delete_all( state ); }
                                             });

            // todo: complete this. what about (l)ist?
            initialize_universal_options( state, menu );
        }

void initialize_menu_settings_options(State_menu & state, std::shared_ptr<Menu> menu) {
            menu->name = "Settings Menu";
            menu->description = "Program Settings Menu to change overall options.";
            menu->options.emplace_back( Menu_option
            { "z",
              "screen size",
              "to set screen size." ,
              "help.",
              "preparing...",
               //{ action_screen_size }
              [&state] (State_menu & kludge_state ) { return action_screen_size( state ); }
                                             });
            menu->options.emplace_back( Menu_option
            { "A",
              "advanced settings",
              "show advanced settings switch (y/N)",
              "Enter <y> or <Y> to cause the more advanced settings and menu options to be visible and accessible during the operation of the program.",
              "preparing...",
              // { action_advanced_settings_selection }
              [&state] (State_menu & kludge_state ) { return action_advanced_settings_selection( state ); }
                                             });
            initialize_universal_options( state, menu );
        }
