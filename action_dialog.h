#ifndef ACTION_DIALOG_H
#define ACTION_DIALOG_H
#include "state_menu.h"
#include "valid_values.h"
#include "interaction_result.h"
#include "io_field_specs_variant.h"

InteractionResultNav find_interaction_result_nav( Hot_key const & hot_key, InteractionCategory const cat );
bool	 			 verify_interaction_result_nav( Hot_key const & hot_key, InteractionCategory const cat );

InteractionResult action_dialog_modal_vals(    State_menu &, std::string   			const & prompt, ValidityCriteriaDialog const & valid);
InteractionResult action_dialog_modal_io_field( State_menu&, IO_field_spec_variant 		  & field_spec );
InteractionResult action_dialog_modal_approve( State_menu &, std::string   			const & prompt );
InteractionResult action_dialog_modal_deny(    State_menu &, std::string   			const & prompt );
InteractionResult action_dialog_modal_notify(  State_menu &, std::string   			const & prompt );

#endif // ACTION_DIALOG_H
