#include "window_panel.h"
#include <iostream>
#include <cassert>
//#include "lib_tty.h"
#include "optional"
#include "state_menu.h"
#include "state_application.h"
#include "menu_option.h"
#include "menu_actions.h"
#include "interaction_result.h"
#include "action_dialog.h"

void pagination_reset(State_menu &state, const Panel_dimensions &used_content) {
    cerr << "pagination_reset():\n";
    // auto & used_d = state.getApplication_data()->used_dimensions;
    auto & pnl_d = state.getApplication_data_sp()->panel_dimensions;
    assert( pnl_d.height >= used_content.height); // todo: probably wrong.
    assert( pnl_d.width  >= used_content.width);
/*    { // some fun
//    // auto [calc_height, calc_width] = *pagination_effect( state, used_xy, false); // todo: TODO seriously a joke, optional with structured binding? If nothing in optional, then undefined behaviour?  Better way to do this?
//    Panel_dimensions r {};
//    Panel_dimensions * rp {};
//    std::optional<Panel_dimensions> ro {};
//    auto [calc_height3, calc_width3] =
//            (  ro = pagination_effect( state, used_content, false),
//               ro.has_value() ? *ro
//                              : Panel_dimensions { 9999, 9999}  // sentinel value.
//            ); // todo: TODO seriously joke, optional with structured binding? If nothing in optional, then undefined behaviour?  Better way to do this?

//    auto [calc_height2, calc_width2] = pagination_effect( state, used_content, false).value_or(Panel_dimensions {9999,9999});  // sentinel value
//    }
    // getting serious
*/
    state.getApplication_data_sp()->used_dimensions = used_content;
    cerr << "pagination_reset(): used_content:"<<used_content<<endl;
}

Panel_dimensions oversized_by( State_menu const & state, Panel_dimensions const & wrapped_content ){
    Panel_dimensions const used_d = state.getApplication_data_sp()->used_dimensions;
    Panel_dimensions const panl_d = state.getApplication_data_sp()->panel_dimensions;
    Panel_dimensions const avail_d { panl_d.height - used_d.height,
                                     0 };
   // Panel_dimensions const avail_d { panl_d - used_d}; // todo:  TODO why is this not automaticly supplied by compiler, since it is a POD?
    Panel_dimensions const over_d = { wrapped_content.height > avail_d.height ? wrapped_content.height - avail_d.height : 0,  // note we make it a positive value.
                                      0};
    cerr << "oversized_by(): panl_d:"<< panl_d  <<", avail_d:"<< avail_d<<", wrapped_d:"<< wrapped_content <<", over_d:"<< over_d << ".\n";
    return over_d;
}

// calculates new propopsed_content with extra height lines due to wrapping of a lengthening horizonatal line. todo: forgot why the bool is usefull.
std::optional<Panel_dimensions> pagination_wrap_effect(const State_menu &state, const Panel_dimensions &proposed_content, const bool is_printed_dimensions_considered = true) {
    // cerr << "pagination_wrap_effect():\n";
    auto const & panl_d = state.getApplication_data_sp()->panel_dimensions;
    auto const & used_d = state.getApplication_data_sp()->used_dimensions;

    Panel_dimensions const existing_d = { is_printed_dimensions_considered ? used_d.height : 0, is_printed_dimensions_considered ? used_d.width : 0 };

    auto calc_wrapped_height = (proposed_content.width + existing_d.width) / (panl_d.width ); // We are wrapping the line. Note: integer math

    calc_wrapped_height = std::max( calc_wrapped_height, proposed_content.height /* + existing_d.height */);  // add at least our proposed height, but if calc_add_height is greater then use that to add.

    if (( panl_d.height < proposed_content.height) || ( panl_d.height < calc_wrapped_height )) {  // todo: don't think I need to check width, but could be wrong.
        cerr <<"SYSTEM WARNING: Screen panel is too small to accept the proposed content even if we clear the screen."<<endl;
        // todo: complete this: draw something smaller, which will fit.
        return {};
    }
    auto const calc_wrapped_width  = (proposed_content.width + existing_d.width) % panl_d.width;

    Panel_dimensions wrapped_d = {.height = calc_wrapped_height, .width = calc_wrapped_width};
    cerr << "pagination_wrap_effect(): panl_d:"<< panl_d <<", proposed_d:"<< proposed_content <<", used_d:"<< used_d <<", wrapped_d:"<< wrapped_d << ".\n";

    return Panel_dimensions {.height = calc_wrapped_height, .width = calc_wrapped_width};
}

/* The goal of pagination in general is to determine if one command's output to the screen will fit
 * and if not, to prompt the user to accept new output to the screen.  This allows the user to examine
 * the output which would have scrolled away with the printing of more than one screen full.
 * Some or all previously displayed data may scroll out of sight after the required prompting.  This is
 * like the unix more or less commands.
 *
 * We test for size of content to be printed to verify it will fit on screen
 * If it does then accumulate size info,
 * Else prompt and reset size characteristics.
 * Currently we return, presuming it worked.
*/
InteractionResult pagination(State_menu &state, const Panel_dimensions &proposed_content) {
                                        cerr << "pagination()START.\n";
    InteractionResult ir { {}, {}, {}, {}, InteractionResultNav::retain_menu };
    if (std::optional<Panel_dimensions> proposed_content_wrapped = pagination_wrap_effect( state, proposed_content, true )) {	// content should fit
        Panel_dimensions over_d = oversized_by( state, proposed_content_wrapped.value() );
                                        cerr << "pagination()1: proposed_d:"<< proposed_content <<", proposed_wrapped_d:" << proposed_content_wrapped.value()<< ", over_d:"<< over_d << ".\n";
        if ( over_d.height > 0 ) {  // width is within line, because we wrapped it just above.
            // cout << ">>Enter <Enter> to continue displaying data.";
            // Kb_value_plus hot_key = get_kb_keys_raw( 1 );
            InteractionResult ir_modal = action_dialog_modal_notify( state, "...more... Press <Enter> to continue" ); // todo: URGENT put back menu_system function for this!
            if ( ir.navigation == InteractionResultNav::exit_pgm_immediately ||
                 ir.navigation == InteractionResultNav::exit_pgm_with_prompts )
                ir.navigation = ir_modal.navigation;
            pagination_reset( state, {0,0} );
        }
        else {
            state.getApplication_data_sp()->used_dimensions.height += proposed_content_wrapped.value().height;
            state.getApplication_data_sp()->used_dimensions.width   = proposed_content_wrapped.value().width;
                                            cerr << "pagination()2: proposed_d:"<< proposed_content<<", proposed_wrapped_d:"<< proposed_content_wrapped.value()<<", over_d:"<<over_d<<" used_d:"<<state.getApplication_data_sp()->used_dimensions<< ".\n";
        }
                                        cerr << "pagination()END.\n";
        return ir;
    }
    else
        assert(false); // can't fit according to pagination_wrap_effect().
}
