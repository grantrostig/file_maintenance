// defines the state variables used by the menu system.
#ifndef STATE_H
#define STATE_H
#include <vector>
#include <memory>
#include <stack>
#include "state_application.h"
struct Menu;
class State_menu {  // holds all state/data used by the menu system, but not data of the user's application.
    std::shared_ptr< Menu > 				menu_main_sp 	   		{};	// required, must have at least one menu.
    std::shared_ptr< Menu > 				menu_edit_sp     		{};	// optional, edit is shown as example, programmer defined meaning.
    std::shared_ptr< Menu > 				menu_file_sp     		{};	// optional, file is shown as example, programmer defined meaning.
    std::shared_ptr< Menu > 				menu_settings_sp 		{};	// optional, settings is shown as example, programmer defined meaning.
    std::stack< std::shared_ptr< Menu > >   menu_stack 				{}; // contains the current menu and menus navigated to get there, bottom of stack must be the main menu.  Used to return to prior menu.  Not sure of other uses?
    std::stack< InteractionCategory >       action_stack 			{}; // contains the current action and prior actions navigated via menu or other actions.
                                                                        // This allows us to check Navigation completion codes, during user input entry of the action.
                                                                        // We push a very specialized Interaction category before calling the action.
                                                                        // After checking the return from the action, we pop.
    std::shared_ptr< State_application > 	application_data_sp  	{}; // todo: I needed to store state for everything except the menu state, placed it in here for convience, perhaps it should be elsewhere?
    bool 				  				advanced_menu_enabled		{false}; // the user is being shown the additional menu options designated as 'advanced', if there are any.
    //bool 								is_data_saved 		   		{true};  // todo: not used, and worse, there is duplicate value in application_data.
public:
    std::shared_ptr<Menu> 	menu_top_sp()   const;
    void                  	push_menu_sp(   std::shared_ptr<Menu> const);  // todo: rename to be consistent.
    std::shared_ptr<Menu> 	menu_pop_top_sp();
    std::shared_ptr<Menu> 	menu_pop_to_sp( std::shared_ptr<Menu> const);

    InteractionCategory 	action_top()   	const;
    void                 	action_push(   	InteractionCategory const);
    InteractionCategory 	action_pop_top();

    bool is_menu_current_main (				  ) const;
    bool getAdvanced_menu_enabled(			  ) const;
    void setAdvanced_menu_enabled(  bool value);
    //bool getIs_data_unsaved(				  )	const;
    //void setIs_data_unsaved(  		bool value);

    std::shared_ptr<Menu> getMenu_main() const;
    void 				  setMenu_main(const std::shared_ptr<Menu> &);

    std::shared_ptr<Menu> getMenu_edit() const;
    void 				  setMenu_edit(const std::shared_ptr<Menu> &);

    std::shared_ptr<Menu> getMenu_file() const;
    void  				  setMenu_file(const std::shared_ptr<Menu> &);

    std::shared_ptr<Menu> getMenu_settings() const;
    void 				  setMenu_settings(const std::shared_ptr<Menu> &);

    std::shared_ptr<State_application> getApplication_data_sp() const;
    void 				               setApplication_data_sp( const std::shared_ptr<State_application> &);
};

#endif // STATE_H
