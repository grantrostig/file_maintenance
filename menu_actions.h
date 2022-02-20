#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H
#include "state_menu.h"
#include "valid_values.h"
#include "interaction_result.h"

/* Following are a collection of actions associated with the general (or universal)
 * operations available to the user from the menu prompt.
 * The return value of these actions determines subsequent menu behaviour.
 * The invoking menu is retained and re-prompts the user if ::retain is returned.
 * Otherwise the prior menu re-gains control, if the ::prior_menu value is returned by an 'action'.
 *
 * The first action is called in the following file:
 * - main.cpp calls functions in process_menu.cpp
 *
 * The sets of actions are in the following files:
 * - menu_actions.cpp (this file set)
 * - actions.cpp
 * - actions_io_row.cpp
 * - actions_dialog.cpp
 *
 *
 */

InteractionResult action_save_work( 				State_menu &);
InteractionResult action_program_exit_with_prompts( State_menu &);
InteractionResult action_program_exit_immediately(  State_menu &);
InteractionResult action_go_back_up_menu( 			State_menu &);
InteractionResult action_print_menu_help( 			State_menu &, Menu const &);
InteractionResult action_print_menu( 				State_menu &, Menu const &);
InteractionResult action_home_menu( 				State_menu &);

//InteractionResult action_undo( State &);
//InteractionResult action_space_forward( State &);
//InteractionResult action_tab_forward( State &);
//InteractionResult action_null_forward( State &);
//InteractionResult action_menu_forward_pagination( State_menu &);
//InteractionResult action_menu_backward_pagination( State_menu &);

#endif // MENU_ACTIONS_H
