#ifndef FIELD_NAVIGATION_INTERACTION_MAP_H
#define FIELD_NAVIGATION_INTERACTION_MAP_H
#include <vector>
#include "interaction_result.h"

struct FieldNavInteractionRelation {
    InteractionCategory	 category 			{InteractionCategory::na};  // Type of interaction (action_) we are prior to completing and returning from.
    FieldCompletionNav 	 field_nav 			{FieldCompletionNav::na};  	// Result of last (user?) interaction, which will determine which InteractionResultNav that will be returned.
    InteractionResultNav interaction_nav 	{InteractionResultNav::na};	// Return value to be returned by the interaction that has just been executed. This began as what the menu system sees, but was extended to include moving between fields during data entry on a row, and then further.
};

class FieldNavigationInteractionMap {
public:
    std::vector<FieldNavInteractionRelation> mappings;
    bool verify() const;
};

/* This vector<> mapping defines what actions (InteractionResultNav) are taken on receipt of a
 * hot_key.FieldCompletionNav.  Different situations are differentiated by InteractionCategory.
 */
static const FieldNavigationInteractionMap fieldNavigationInteractionMap
{  // todo: get rid of global if possible, perhaps adding to the "statu" data structure.  ALSO: figure out if this level of detail, or any of this is necessary.  Seemed like a good idea at the time. LOL
std::vector<FieldNavInteractionRelation>
{
        /*	Type of interaction (action_) we are prior to completing and returning from.
                                        Result of last (user?) interaction, which will determine which InteractionResultNav that will be returned.
                                                                                Return value to be returned by the interaction that has just been executed. This began as what the menu system sees, but was extended to include moving between fields during data entry on a row, and then further.
*/
    { InteractionCategory::menu, 	    FieldCompletionNav::esc, 				InteractionResultNav::prior_menu_discard_value }, // for example, esc on menu, should take you back one.
    { InteractionCategory::menu, 	    FieldCompletionNav::exit_immediately, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::menu, 	    FieldCompletionNav::interrupt_signal, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::menu, 	    FieldCompletionNav::quit_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::menu, 	    FieldCompletionNav::kill_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::menu, 	    FieldCompletionNav::exit_with_prompts, 	InteractionResultNav::exit_pgm_with_prompts },
    { InteractionCategory::menu, 	    FieldCompletionNav::eof, 				InteractionResultNav::exit_pgm_immediately },
    //{ InteractionCategory::menu, 	    FieldCompletionNav::help, 				InteractionResultNav::retain_menu },
    { InteractionCategory::menu, 		FieldCompletionNav::browse_up, 			InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::browse_down, 		InteractionResultNav::retain_menu },  // normal response to menu selection prompt.
    { InteractionCategory::menu, 		FieldCompletionNav::up_one_field, 		InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::down_one_field, 	InteractionResultNav::retain_menu },  // normal response to menu selection prompt.
    { InteractionCategory::menu, 	    FieldCompletionNav::page_up, 			InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::page_down,		 	InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::save_form_as_is,	InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::skip_to_end_of_fields, InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::skip_one_field, 	InteractionResultNav::na },
    { InteractionCategory::menu, 	    FieldCompletionNav::no_result, 			InteractionResultNav::no_result },
    { InteractionCategory::menu, 	    FieldCompletionNav::na, 				InteractionResultNav::na },

    { InteractionCategory::dialog, 	    FieldCompletionNav::esc, 				InteractionResultNav::prior_menu },  // for example, if when <Return> to continue, then stop listing the items.
    { InteractionCategory::dialog, 	    FieldCompletionNav::exit_immediately, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialog, 	    FieldCompletionNav::interrupt_signal, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialog, 	    FieldCompletionNav::quit_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialog, 	    FieldCompletionNav::kill_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialog, 	    FieldCompletionNav::exit_with_prompts, 	InteractionResultNav::exit_pgm_with_prompts },
    { InteractionCategory::dialog, 	    FieldCompletionNav::eof, 				InteractionResultNav::exit_pgm_immediately },
    //{ InteractionCategory::dialog, 	    FieldCompletionNav::help, 				InteractionResultNav::na },
    { InteractionCategory::dialog, 		FieldCompletionNav::browse_up, 			InteractionResultNav::na },
    { InteractionCategory::dialog, 	    FieldCompletionNav::browse_down, 		InteractionResultNav::na },
    { InteractionCategory::dialog, 		FieldCompletionNav::up_one_field, 		InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::down_one_field, 	InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::page_up, 			InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::page_down,		 	InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::save_form_as_is,	InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::skip_to_end_of_fields, InteractionResultNav::retain_menu },
    { InteractionCategory::dialog, 	    FieldCompletionNav::skip_one_field, 	InteractionResultNav::na },
    { InteractionCategory::dialog, 	    FieldCompletionNav::no_result, 			InteractionResultNav::no_result },
    { InteractionCategory::dialog, 	    FieldCompletionNav::na, 				InteractionResultNav::na },

    { InteractionCategory::dialogn, 	    FieldCompletionNav::esc, 				InteractionResultNav::prior_menu },  // for example, if when <Return> to continue, then stop listing the items.
    { InteractionCategory::dialogn, 	    FieldCompletionNav::exit_immediately, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::interrupt_signal, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::quit_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::kill_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::exit_with_prompts, 	InteractionResultNav::exit_pgm_with_prompts },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::eof, 				InteractionResultNav::exit_pgm_immediately },
    //{ InteractionCategory::dialog, 	    FieldCompletionNav::help, 				InteractionResultNav::na },
    { InteractionCategory::dialogn, 		FieldCompletionNav::browse_up, 			InteractionResultNav::na },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::browse_down, 		InteractionResultNav::na },
    { InteractionCategory::dialogn, 		FieldCompletionNav::up_one_field, 		InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::down_one_field, 	InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::page_up, 			InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::page_down,		 	InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::save_form_as_is,	InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::skip_to_end_of_fields, InteractionResultNav::retain_menu },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::skip_one_field, 	InteractionResultNav::na },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::no_result, 			InteractionResultNav::no_result },
    { InteractionCategory::dialogn, 	    FieldCompletionNav::na, 				InteractionResultNav::na },

    { InteractionCategory::field, 	    FieldCompletionNav::esc, 				InteractionResultNav::prior_menu },  // for example, if when <Return> to continue, then stop listing the items.
    { InteractionCategory::field, 	    FieldCompletionNav::exit_immediately, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::field, 	    FieldCompletionNav::interrupt_signal, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::field, 	    FieldCompletionNav::quit_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::field, 	    FieldCompletionNav::kill_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::field, 	    FieldCompletionNav::exit_with_prompts, 	InteractionResultNav::exit_pgm_with_prompts },
    { InteractionCategory::field, 	    FieldCompletionNav::eof, 				InteractionResultNav::exit_pgm_immediately },
    //{ InteractionCategory::field, 	    FieldCompletionNav::help, 				InteractionResultNav::na },						// help is handled prior to this decision point.
    { InteractionCategory::field, 		FieldCompletionNav::browse_up, 			InteractionResultNav::up_one_field },  // NOTE: browse_up is also used to navigate between valid_values.
    { InteractionCategory::field, 	    FieldCompletionNav::browse_down, 		InteractionResultNav::down_one_field },  // NOTE: browse_down is also used to navigate between valid_values.
    { InteractionCategory::field, 		FieldCompletionNav::up_one_field, 		InteractionResultNav::up_one_field },
    { InteractionCategory::field, 	    FieldCompletionNav::down_one_field, 	InteractionResultNav::down_one_field },
    { InteractionCategory::field, 	    FieldCompletionNav::page_up, 			InteractionResultNav::up_one_block },
    { InteractionCategory::field, 	    FieldCompletionNav::page_down,		 	InteractionResultNav::down_one_block },
    { InteractionCategory::field, 	    FieldCompletionNav::save_form_as_is,	InteractionResultNav::save_form_as_is },
    { InteractionCategory::field, 	    FieldCompletionNav::skip_to_end_of_fields, InteractionResultNav::skip_to_end_of_fields },
    { InteractionCategory::field, 	    FieldCompletionNav::skip_one_field, 	InteractionResultNav::skip_one_field },
    { InteractionCategory::field, 	    FieldCompletionNav::no_result, 			InteractionResultNav::no_result },
    { InteractionCategory::field, 	    FieldCompletionNav::na, 				InteractionResultNav::na },

    { InteractionCategory::detail_row, 	FieldCompletionNav::esc, 				InteractionResultNav::prior_menu },  // for example, if when <Return> to continue, then stop listing the items.
    { InteractionCategory::detail_row, 	FieldCompletionNav::exit_immediately, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::detail_row, 	FieldCompletionNav::interrupt_signal, 	InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::detail_row, 	FieldCompletionNav::quit_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::detail_row, 	FieldCompletionNav::kill_signal,		InteractionResultNav::exit_pgm_immediately },
    { InteractionCategory::detail_row, 	FieldCompletionNav::exit_with_prompts, 	InteractionResultNav::exit_pgm_with_prompts },
    { InteractionCategory::detail_row, 	FieldCompletionNav::eof, 				InteractionResultNav::exit_pgm_immediately },
    //{ InteractionCategory::detail_row, 	FieldCompletionNav::help, 				InteractionResultNav::na },
    { InteractionCategory::detail_row, 	FieldCompletionNav::browse_up, 			InteractionResultNav::prior_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::browse_down, 		InteractionResultNav::next_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::up_one_field, 		InteractionResultNav::prior_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::down_one_field, 	InteractionResultNav::next_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::page_up, 			InteractionResultNav::up_one_block },
    { InteractionCategory::detail_row, 	FieldCompletionNav::page_down,		 	InteractionResultNav::down_one_block },
    { InteractionCategory::detail_row, 	FieldCompletionNav::save_form_as_is,	InteractionResultNav::prior_menu },
    { InteractionCategory::detail_row, 	FieldCompletionNav::skip_to_end_of_fields, InteractionResultNav::last_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::skip_one_field, 	InteractionResultNav::next_row },
    { InteractionCategory::detail_row, 	FieldCompletionNav::no_result, 			InteractionResultNav::no_result },
    { InteractionCategory::detail_row, 	FieldCompletionNav::na, 				InteractionResultNav::na }

    //  ::help - is similar to field, because it it probably called during field entry, so we just use ::field  // todo: complete this: OK so what if help is called from a menu or wizard?  Well then we can do ::field_help, ::menu_help ...
    // { InteractionCategory::master_row, 	FieldCompletionNav::na, 				InteractionResultNav::na }
}
};

#endif // FIELD_NAVIGATION_INTERACTION_MAP_H
