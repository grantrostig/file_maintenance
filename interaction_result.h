#ifndef INTERACTION_RESULT_H
#define INTERACTION_RESULT_H
#include <string>
#include <variant>
#include <chrono>
#include <global_entities.h>
#include "lib_tty.h"  // this one is needed
#include "valid_values.h"

enum class InteractionResultErrorCat {
    success,
    failed,
    re_try,  	// maybe?
    unavailable,  	// maybe?
    greyed_out,  // maybe?
    na
};

struct InteractionResultError {
    InteractionResultErrorCat category 	{InteractionResultErrorCat::na};
    std::string               message 	{};
};

enum class InteractionResultNav {  // todo: todo: would this be more compact if "char" or "byte"? What are downsides other than performance??
    no_result,			// there was no interaction, and so there is no result.  is this different from ::na?
    // universal navigation commands, this group operates at all levels
    exit_pgm_immediately,
    exit_pgm_with_prompts,
    exit_fn_immediately,
    exit_fn_with_prompts,

    // menu specific navigation commands  	- supplemented by universals'
    retain_menu,
    prior_menu,
    prior_menu_discard_value,
    main_menu,
    exit_all_menu,

    // dialog specific navigation commands  - supplemented by menus'
    continue_forward_pagination,
    continue_backward_pagination,

    // detail_row nav 						- supplemented by menus'
    up_one_field,
    down_one_field,
    up_one_block,   // top of block, or if there then top of prior block.
    down_one_block, // bottom of block, or if there then bottom of next block.
    save_form_as_is,
    skip_to_end_of_fields,
    skip_one_field,
    // inter_row nav
    next_row,
    prior_row,
    first_row,
    last_row,
    // *** field
    // store_value
    // todo: should we have a separate enum for dialFieldCompletionNavogs, menus, data_fields?
    na						// todo: NA not applicable. Does this indicate an error condidtion?
};

enum class InteractionCategory {
    // universal,
    menu,
    dialog,
    dialogn,
    field,
    detail_row,
    // master_row,
    na		// must be last element due to verify() on interaction map
};

struct InteractionResult {
        Hot_key							hot_key		{};  // interaction within interaction requires this value.  ie. help within a field interaction.
        size_t							index		{};  // points to most recent item processed
        InteractionResultData_Variant  	data 		{};  // the most important data value of the item processed. ie. name or code or ID.
        InteractionResultError		   	error 		{};	 // any error that occurred.
        InteractionResultNav 	       	navigation 	{InteractionResultNav::no_result};  // hint that is passed to the caller on user's intent for future processing, or that which is required due to an error.
      };

//struct ActionResultSpec {
    //InteractionCategory interaction_cat {};  // points to the category, which is associated with valid
                                             // and invalid InteractionCategories::s
//};

#endif // INTERACTION_RESULT_H
