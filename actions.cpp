#include <arpa/inet.h>
#include <cassert>
#include <fstream>
#include <iostream>

#include "global_entities.h"
#include "actions.h"
#include "action_dialog.h"
#include "window_panel.h"
#include "interaction_result.h"
//#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
using std::cin, std::cout, std::endl, std::cerr, std::clog;

using namespace std::string_literals;

/*InteractionResult action_print_categories( State_menu &) {
//    cerr << "int action_print_categories( State &) {" << endl;
//    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::retain_menu };
//}

//InteractionResult action_print_search_entries( State_menu &) {
//    cerr << "int action_print_search_entries( State &) {" << endl;
//    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::retain_menu };
//}

//InteractionResult action_entry_viewing( State_menu &) {
//    cerr << "int action_entry_viewing( State &) {" << endl;
//    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::retain_menu };
//}*/

InteractionResult action_delete_all( State_menu & state ) {
    cerr << "int action_delete_all( State &) {" << endl;
    // todo: delete all
    state.getApplication_data_sp()->setIs_data_unsaved(true);
    InteractionResult ir = action_dialog_modal_notify( state, "All user data have been removed from the memory of the application.  If you save now, the saved file will be empty.");
    return ir;
}

InteractionResult action_save_as_changes_to_disk( State_menu & state ) {
    cerr << "int action_save_changes_to_disk( State &) " << endl;
    //cerr << "saving application data: "<<state.getApplication_data()->getInt_data()<<endl;
    // todo: get file name
    InteractionResult ir = action_save_changes_to_disk( state );
    return ir;
}

std::streamoff size_of_data( std::iostream & stream ) {
    auto old_exceptions { stream.exceptions() };
    stream.exceptions(stream.exceptions()|(std::ios_base::failbit & std::ios_base::badbit));
    stream.seekg( 0, std::ios_base::end );  // https://stackoverflow.com/questions/4432793/size-of-stringstream/4432843#4432843
    std::streamoff size { stream.tellg() };
    stream.seekg( 0, std::ios_base::beg );
    stream.exceptions(old_exceptions);
    return size;
}

InteractionResult action_save_changes_to_disk( State_menu & state ) {
    // disk file layout:  	version_size char, version char*,
    //					  	adata_size uint16, adata_serialized char*,
    //						pdata_size uint32, pdata_serialized_char*,
    InteractionResult 		ir  	{};  // to be returned
    InteractionResult 		ir_temp {};  // temporary for handling intermediate values.
    InteractionResultNav 	nav  	{};
                                    cerr << "int action_save_changes_to_disk( State &) " << endl;
    // if ( io_table_persons.rows.size() == 0 ) ir = action_dialog_modal_deny( state, "Warning: there are no data records to save, do you wish to proceed?: ");
    // if ( ir.hot_key != proceed ) {  todo: //ir = ?; //return ir; //}

    //  ****** Build and Write ADATA ******
    std::ofstream ofstream_adata { DEFAULT_FILE_NAME, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc };  // adata means additional text data
    if ( ofstream_adata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: no such data file exists, or you don't have permission to read it."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
    ofstream_adata.exceptions( ofstream_adata.exceptions() | std::ios_base::badbit );  // so we don't have to check for bad() on every call.

    //  *** write length of and file version to disk ***
    Version file_version { state.getApplication_data_sp()->data_file_state_adata.file_version };
    ofstream_adata.put( static_cast<char> (sizeof file_version) );
    if ( ofstream_adata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: unable to write data to file.");
        return ir;
    }
                                    cerr<< ofstream_adata.tellp() << endl;
    ofstream_adata.write( reinterpret_cast<char *>(&file_version), sizeof (file_version));  // struct of all chars so don't need to worry about endianess.
    if ( ofstream_adata.fail() || ofstream_adata.bad() ) {  // > clause is redundant, but why not.
        ir = action_dialog_modal_notify( state, "Error: unable to write adequate data to the data file.");
        return ir;
    }
                                    cerr<< ofstream_adata.tellp() << endl;
    ofstream_adata.flush();  // DEBUG for debugging of what data was written to the disk.

    //  *** serialize pdata and get its length but don't write it yet. ***  // doing this here because I would like to know PDATA's length to store in adata, but have chicken and egg problem.
    std::stringstream pdata_serialized {};  // pdata means plaint text data
    { //cereal::PortableBinaryOutputArchive
        cereal::JSONOutputArchive oarchive( pdata_serialized );
        oarchive( io_table_persons.rows );
    } // flushes
    //  *** calculate length of pdata ***
    auto old_exceptions { pdata_serialized.exceptions() };
    pdata_serialized.exceptions(pdata_serialized.exceptions()|(std::ios_base::failbit & std::ios_base::badbit));
    pdata_serialized.seekg( 0, std::ios_base::end );  			// https://stackoverflow.com/questions/4432793/size-of-stringstream/4432843#4432843
    std::streamoff pdata_length { pdata_serialized.tellg() };
    pdata_serialized.seekg( 0, std::ios_base::beg );
    pdata_serialized.exceptions(old_exceptions);
    if ( pdata_length > PDATA_MAX_LENGTH ) {
        ir = action_dialog_modal_notify( state, "Error: Attempting to store too much data in the data file.");
        return ir;
    }
                                    cerr<< "pdata_length:" << pdata_length << endl;

    //  *** prepare ADATA using PDATA's length and more ***
    state.getApplication_data_sp()->data_file_state_adata.application_product.application_version = file_version;  // already also have this written to disk directly.
    // state.getApplication_data()->data_file_state_adata.adata_length = static_cast<uint32_t>(adata_length);  chicken before the egg problem
    state.getApplication_data_sp()->data_file_state_adata.pdata_length = static_cast<uint32_t>(pdata_length);

    //  *** write length of and adata to disk ***
    std::stringstream adata_serialized {};
    {
        cereal::JSONOutputArchive oarchive( adata_serialized );
        oarchive( state.getApplication_data_sp()->data_file_state_adata );
    }

    std::streamoff const adata_length = size_of_data( adata_serialized );
    assert( static_cast<uint_fast64_t>( adata_length ) <= ADATA_MAX_LENGTH );  // arbitrary limit
    uint16_t adata_length_network_order { htons( static_cast<uint16_t>(adata_length) ) };
    ofstream_adata.write( reinterpret_cast<char *>(&adata_length_network_order), sizeof adata_length_network_order );  // todo: TODO how I can be sure I get an unsigned char?
    if ( ofstream_adata.fail() || ofstream_adata.bad() ) {  // > clause is redundant, but why not.
        ir = action_dialog_modal_notify( state, "Error: failed to write data to file.");
        return ir;
    }
                                    cerr<< ofstream_adata.tellp() << endl;
    ofstream_adata << adata_serialized.str();
    if ( ofstream_adata.fail() || ofstream_adata.bad() ) {  // todo: is fail() necessary?
        ir = action_dialog_modal_notify( state, "Error: failed to write date to file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
                                    cerr<< "adata_length:" << adata_length << endl;
                                    cerr<< ofstream_adata.tellp() << endl;

    //  *** write length of pdata to disk ***
    uint32_t pdata_length_network_order { htonl( static_cast<uint32_t>(pdata_length) ) };
    ofstream_adata.write( reinterpret_cast<char *>(&pdata_length_network_order), sizeof pdata_length_network_order );
    if ( ofstream_adata.fail() || ofstream_adata.bad() ) {  // todo: is fail() necessary?
        ir = action_dialog_modal_notify( state, "Error: failed to write date to file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
                                    cerr<< ofstream_adata.tellp() << endl;

    //  *** write PDATA to disk using ofstream_adata (yes, it is bad naming) ***
    ofstream_adata.exceptions( (std::ios_base::failbit | std::ios_base::badbit) );
    ofstream_adata.flush(); // DEBUG to see what has been written.
    ofstream_adata << pdata_serialized.str();
    if ( ofstream_adata.fail() || ofstream_adata.bad() ) {  // todo: is fail() necessary?
        ir = action_dialog_modal_notify( state, "Error: failed to write date to file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
    state.getApplication_data_sp()->setIs_data_unsaved( false );

    state.action_push( InteractionCategory::dialogn );  // Next 3 lines are one unit.
    ir_temp 		= action_dialog_modal_notify( state, "All user and relevant program data have been saved to disk." );
    nav 		= find_interaction_result_nav( ir_temp.hot_key, state.action_top() );
    ir.navigation 	= nav;
    return ir;
}

InteractionResult action_load_data_from_disk( State_menu & state) {
    InteractionResult ir {};

    if ( not state.getApplication_data_sp()->getIs_data_unsaved() ) {
        // todo:  verify with user if unsaved data existing.
    }

    // try {  // to catch IO bad() errors.
                                    cerr << "int action_load_data_from_disk( State &) " << endl;
    std::ifstream ifstream_adata_pdata { DEFAULT_FILE_NAME, std::ios_base::binary | std::ios_base::in };
    if ( ifstream_adata_pdata.fail() || ifstream_adata_pdata.bad() ) {
        ir = action_dialog_modal_notify( state, "Error: cannot open file, no data such file exists, or you don't have permission to read it."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
    ifstream_adata_pdata.exceptions( ifstream_adata_pdata.exceptions() | std::ios_base::badbit );  // so we don't have to check for bad() on every call. todo: TODO can this throw, where documented?

    //  *** read length and version from disk ***
    const int version_length { ifstream_adata_pdata.get() };  // one char
    if ( ifstream_adata_pdata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: unable to read adequate data from open data file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
    Version file_version {};
    assert( version_length == sizeof( file_version ));
    ifstream_adata_pdata.read( reinterpret_cast<char *>(&file_version), sizeof (file_version));
    if ( ifstream_adata_pdata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: unable to read adequate data from open data file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
                                    cerr<< ifstream_adata_pdata.tellg() << endl;
    // validate_version_compatibility(); // todo: complete this.

    //  *** read length and adata from disk ***
    uint16_t adata_length_network_order { };
    ifstream_adata_pdata.read( reinterpret_cast<char *>(&adata_length_network_order), sizeof adata_length_network_order );  // todo: TODO how I can be sure I get and unsigned char?
    if ( ifstream_adata_pdata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: failed to read data from file.");
        return ir;
    }
                                    cerr<< ifstream_adata_pdata.tellg() << endl;
    uint16_t adata_length { ntohs( adata_length_network_order ) };
    std::string adata ( static_cast<size_t>(adata_length), 0x00 ); //adata.reserve( static_cast<size_t>(adata_length) );  // grostig added std::
    ifstream_adata_pdata.read( adata.data(), adata_length );
    if ( ifstream_adata_pdata.eof() ) {  // todo:  TODO check eof before fail?
        ir = action_dialog_modal_notify( state, "Error: premature end of file on data file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
    if ( ifstream_adata_pdata.fail() ) {  // is there a fail other than eof()?
        ir = action_dialog_modal_notify( state, "Error: failed to read from data file."); // todo: I just return after failure? Is there a subtle issue with any returned hot key?  Should it be cleared?
        return ir;
    }
                                    cerr<< "adata_length: " << adata_length << endl;
                                    cerr<< ifstream_adata_pdata.tellg() << endl;

    std::basic_stringstream<char> sstream_serialized_adata_in { adata };  // todo: TODO don't want to have to take the file stream, convert to string and then to stringstream! crazy!
    sstream_serialized_adata_in.exceptions( std::ios_base::failbit & std::ios_base::badbit );  // probably not necessary and could break the cereal library?
    {
        cereal::JSONInputArchive adata_iarchive( sstream_serialized_adata_in );
        adata_iarchive( state.getApplication_data_sp()->data_file_state_adata );
    }

    //  *** read only the length of the PDATA from disk ***

    uint32_t pdata_length_network_order { };  // todo: complete this: LATER TO BE STORED IN ADATA
    ifstream_adata_pdata.read( reinterpret_cast<char *>(&pdata_length_network_order), sizeof pdata_length_network_order );  // todo: TODO how I can be sure I get and unsigned char?
    if ( ifstream_adata_pdata.fail() ) {
        ir = action_dialog_modal_notify( state, "Error: failed to read data from file.");
        return ir;
    }
    uint32_t pdata_length { ntohl( pdata_length_network_order ) };
    if ( pdata_length > PDATA_MAX_LENGTH ) {
        ir = action_dialog_modal_notify( state, "Too much pdata to be able to read from the data file.");
        return ir;
    }
                                    cerr<< ifstream_adata_pdata.tellg() << endl;
                                    cerr << "pdata_length: " << pdata_length << endl;
    std::string sink_string {};
    //ifstream_adata_pdata.read( sink_string, pdata_length   );

    std::stringstream kludge { sink_string };
    {
        cereal::JSONInputArchive iarchive( kludge );
        io_table_persons.rows.clear();
        iarchive( io_table_persons.rows );
    }
    state.getApplication_data_sp()->setIs_data_unsaved(false);
    ir = action_dialog_modal_notify( state, "All user data have been loaded from disk, replacing any prior data.");
    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    ir.navigation 			 = nav;
    return ir;
}

//also need: InteractionResult action_save_as_data_from_disk( State_menu & state) {  // save to a new or existing file, which is not from the current name.
InteractionResult action_load_as_data_from_disk( State_menu & state) {  // load from an existing file, which is not from the current name.
    cerr << "int action_load_as_data_from_disk( State &) " << endl;
    //todo: get file name, also decide if new name and location is new default?
    InteractionResult ir = action_load_data_from_disk( state );
    ir = action_dialog_modal_notify( state, "All user data have been loaded from disk. Note: Please take care where you will save the data.  TODO TODO!!!! !");
    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    ir.navigation 			 = nav;
    return ir;
}

InteractionResult action_backup_data( State_menu & state ) {
    cerr << "int action_backup_data( State &) " << endl;
    // todo: do something.
    InteractionResult 	 ir  = action_dialog_modal_notify( state, "All user data have been backed up to disk in a backup format. Note: data have not been saved in the normal format in the normal location.  Please also normally save any data you wish to save for normal use of the application!");
    InteractionResultNav nav = find_interaction_result_nav( ir.hot_key, InteractionCategory::menu );
    ir.navigation 			 = nav;
    return ir;
}

InteractionResult action_screen_size( State_menu &) {
    cerr << "int action_screen_size( State &) " << endl;
    // todo: do something
    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::retain_menu };
}

InteractionResult action_advanced_settings_selection( State_menu &) {
    cerr << "int action_advanced_settings_selection:" << endl;
    // todo: do something
    return InteractionResult { {}, {}, {}, {}, InteractionResultNav::retain_menu };
}
