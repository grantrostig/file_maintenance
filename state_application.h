#ifndef STATE_APPLICATION_H
#define STATE_APPLICATION_H
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include "global_entities.h"
#include "io_row.h"
//#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
// used to determine when to perform pagination.
struct Panel_dimensions {
    size_t height {};   // NOTE: usually the actual height/lines.
                        // BUT when 'proposed_dimensions' it designates the minimum number of
                        // new lines to be added for pagination calculation,
                        // also takinginto account wrapping when exceeding the width.
    size_t width {};  	// NOTE: usually the actual number of characters on a line,
                        // or the max width of any of the multiple lines.
                        // BUT when 'proposed_dimensions' it may add enough characters
                        // to wrap the line, resulting in more height.
};

struct Row_range {
    size_t first {}; // points at first
    size_t end {};  // like the iterator end(), so it point past the last element in the range.
};
std::ostream& operator<< ( std::ostream& os, Panel_dimensions const & pd);

struct Data_file_state_adata {  // NOTE: this UDT should no be more than 255 bytes once serialized, due to the file format which uses unsigned char to store length on disk.
    //Version 				file_version 		{ '\x00', 0x1, 0, 0, 0, 0, 0, 0  };  // note: a file version may fall behind the application version. This would require the application version to support prior file versions.
    Version 				file_version 		{ version_file };  // note: a file version may fall behind the application version. This would require the application version to support prior file versions.
    // uint16_t				adata_length			{};  // chicken before the egg problem.
    uint32_t				pdata_length			{};
    Data_type_time_point	date_file_saved 		  {};
    Data_type_time_point	date_file_modified     {};
    size_t					file_size_data {};  // the size of the data as it is stored on the disk, ie after serialization . Used to limit the size of the read during loading of the file.
    uint64_t				product_customer_key {};
    Software_product 		application_product {};
    template< class Archive >
    void serialize( Archive & archive ) {
        archive( file_version, /* adata_length,*/ pdata_length, date_file_saved, date_file_modified, file_size_data, product_customer_key, application_product );
    }
};

class State_application { // holds all data used by the user's application, but not state for the menu system.
    bool 			        is_data_unsaved {false};  // the user has unsaved data input/editing that has not been committed to disk.
    int 				    int_data 		  {};  // used for passing parameters through the menu system for use by the application.
    size_t 				    size_t_data  	  {};  // used for passing parameters through the menu system for use by the application.
    Row_range 				row_range 		  {};  // todo: not used, is it needed?
public:
    Data_file_state_adata 	data_file_state_adata 	{};
    Panel_dimensions        panel_dimensions 		{.height = 15, .width = 80};
    Panel_dimensions        used_dimensions  		{0,0};
    bool getIs_data_unsaved(				)  const;
    void setIs_data_unsaved(   bool value );
    size_t  getSize_t_data(		        ) const;
    void setSize_t_data(	 size_t value);
    Row_range getRow_range() const;
    void setRow_range(const Row_range &value);
    int getInt_data() const;
    void setInt_data(int value);
};
#endif // STATE_APPLICATION_H
