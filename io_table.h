#ifndef IO_TABLE_H
#define IO_TABLE_H
#include <algorithm>
#include <variant>
#include <map>
#include "global_entities.h"
#include "io_field.h"
#include "io_field_specs_variant.h"
#include "io_row.h"
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/string.hpp>

/* some helper functions to do integrity checks between the vector of values against the struct Row of values
template< typename Field_spec_t, typename Data_row_t >
class Field_check {
public:
    Data_row_t data_row {};
    Field_check( Data_row_t const & data_row_) : data_row (data_row_) {}
    bool operator()( Field_spec_t const & field_spec ) {
        if ( field_spec.is_primary_key_component) {
            auto var_of_data_type_kludge = field_spec.data_location;  // todo TODO: I'm just stripping off the ref so I can use decltype().  Is there a better way?
            auto the_field_in_data_row = *(reinterpret_cast< decltype( var_of_data_type_kludge ) const *>(&data_row + field_spec.memory_offset_from_row_begin));
            if ( field_spec.getData_value() == the_field_in_data_row )
                return true;
        }
        return false;
    }
};

template< typename Field_specs_t, typename Data_row_t >
class Data_row_check {
public:
    Data_row_t 		data_row {};
    Field_specs_t   field_specs {};
    Data_row_check( Field_specs_t const & field_specs_) : field_specs (field_specs_) {}
    bool operator()( Data_row_t const & data_row ) {
        //Field_check field_check<IO_field_spec_alphanumeric, Data_row_t>( Data_row_t const & data_row);
        //std::find_if( field_specs.begin(), field_specs.end(), field_check );
        return false;
    }
}; */

class IO_table_spec {
public:
    std::string 			   name				{"MANDATORY: FILL ME IN!"};
    std::string				   full_description {"FILL full description in!"};
    std::string				   help 			{"FILL help in!"};
    std::vector<IO_field_spec_variant> fields 	{};
/*  OR std::array<IO_field_spec, //IO_ROW_FIELDS_NUM_MAX >// todo: use X_MACROs to replace counts  https://stackoverflow.com/questions/12194793/how-do-i-get-the-number-of-members-in-a-structure https://www.embedded.com/reduce-c-language-coding-errors-with-x-macros-part-1/
//  bool				       is_allow_duplicate_full_rows {false};  // todo: complete this.
// 	std::vector<field_compound_key_component>	unique_key // todo: complete this, this feature disallows duplicate rows based on a subset of columns, ie. not the full row as in is_allow_duplicate_full_rows.
//  std::shared_ptr<IO_table_spec> 			    parent_table {};  // todo: complete this: work out the pointers
//  std::vector<std::shared_ptr<IO_table_spec>> child_tables {};  // todo: complete this: work out the pointers
//  Row_Field_loc_a_val_row_mappings field_validation_fn_mapping {}; // not needed in OOAD version? must be initialize later!!
//  void create_map();
//  void assign_field_validation(unsigned short const field_index, IO_field_spec const &);  // obsolete?
*/
    IO_row_ptr_variant		   io_candidate_row			{};
public:
    void clear_row_fields_data();
    void clear_candidate_row_locations();
    void print() const;
};

bool is_check_pk_component_duplicate( IO_field_spec_alphanumeric * fs, IO_row_variant const & data_row );

class IO_table {
public:
    IO_table_spec  			        spec {};  // todo: should I init it with {}?
    std::vector< IO_row_variant > 	rows {};
    // std::map< IO_row_index_variant, IO_row_variant, IO_row_index_compare_variant>	rows_ordered {};

    std::optional<IO_row_variant> retrieve_row( size_t const row_index );
    std::optional<std::vector<ValidationReport>> check_rows_for_duplicate_pk( std::optional<size_t> exempted_row_index );
    std::optional<std::vector<ValidationReport>> validate_candidate_row_infull( std::optional<size_t> exempted_row_index );

    void read_candidate_row( size_t const row_index ) {
        /*std::get<IO_row_person *>(spec.io_candidate_row) = std::get<IO_row_person>(rows.at( row_index ));
        IO_row_variant row_kludge {rows.at(row_index)};  // todo: warning: raw_pointer.
        std::visit( [] (auto & row, auto candidate_row ) ->void {
            *candidate_row = row;
        }, rows.at(row_index), spec.io_candidate_row );
        }, row_kludge, spec.io_candidate_row ); */
        struct CandidateAssignVisitor {
            void operator()(IO_row_company row, IO_row_company *  c ) {
                *c = row;
            }
            void operator()(IO_row_person row, IO_row_person *  c ) {
                *c = row;
            }
            void operator()(IO_row_person, IO_row_company * ) {}
            void operator()(IO_row_company, IO_row_person * ) {}
        };
        std::visit( CandidateAssignVisitor(), rows.at(row_index), spec.io_candidate_row );
        /*
        struct Data_struct_var {
            IO_row_variant 		 r;
            IO_row_ptr_variant c;
        };
        struct CandidateAssignVisitor3 {
            void operator()(IO_row_company row, IO_row_company *  c ) {
                *c = row;
            }
            void operator()(IO_row_person row, IO_row_person *  c ) {
                *c = row;
            }
        };
        std::visit( CandidateAssignVisitor(), rows.at(row_index), spec.io_candidate_row );

        struct Data_struct_company {
            IO_row_company r;
            IO_row_company * c;
        };
        struct Data_struct_person {
            IO_row_person r;
            IO_row_person * c;
        };
        using Data_variant = std::variant<
            Data_struct_company,
            Data_struct_person >;
        struct CandidateAssignVisitor2 {
            void operator()(Data_struct_company & data_pair ) {
                *(data_pair.c) = data_pair.r;
            }
            void operator()(Data_struct_person & data_pair ) {
                *(data_pair.c) = data_pair.r;
            }
        };

        //Data_variant data_variant2 { rows.at(row_index), spec.io_candidate_row };
        //Data_variant data_variant3 { std::get<IO_row_person>(rows.at(row_index)), std::get<IO_row_person *>(spec.io_candidate_row) };
        // *** the challenge is to cleanly construct data_variant with the correct variant types.  See special constructors for variants.
        Data_variant data_variant;
        data_variant = { Data_struct_person { std::get<IO_row_person>(rows.at(row_index)), std::get<IO_row_person *>(spec.io_candidate_row)} };
        // if (spec.io_candidate_row is IO_row_person) data_variant = { Data_struct_person { std::get<IO_row_person>(rows.at(row_index)), std::get<IO_row_person *>(spec.io_candidate_row)} };  // todo: another strategy to load the variant?
        //Data_variant data_pair_var { std::pair( rows.at(row_index), spec.io_candidate_row ) };
        std::visit( CandidateAssignVisitor2(), data_variant );

        return true;
        */
    }

    auto insert_candidate_row() {
        std::variant< std::vector<IO_row_variant> * > rows_kludge {&rows};  // todo: warning: raw_pointer.
        auto r = std::visit( [] (auto & rows, auto row ) {
            //cerr << __PRETTY_FUNCTION__<<endl;
            //cerr << ", size:"<<rows->size()<<endl;
            auto r = rows ->emplace_back( *row ); // cerr << "insert_candidate_row()-rows.emplace_back(row), size:"<<rows->size()<<"\n"; cerr << "insert_candidate_row()-rows.emplace_back(row), row.print(): "; row->print();
            return r;
        }, rows_kludge, spec.io_candidate_row );
        return true;
    }
    // **** assumes the pk and data are ready to update.
    void update_candidate_row( size_t const row_index ) {
        std::variant< std::vector<IO_row_variant> * > rows_kludge {&rows};  // todo: warning: raw_pointer.
        std::visit( [row_index] (auto & rows, auto row ) {
            //cerr << ", size:"<<rows->size()<<endl;
            rows->at( row_index ) = *row; // cerr << "update_candidate_row()-rows->at()=row, size:"<<rows->size()<<"\n"; cerr << "insert_candidate_row()-rows.emplace_back(row), row.print(): "; row->print();
            return;
        }, rows_kludge, spec.io_candidate_row );
    }

    // **** assumes the pk and data are ready to update.
    void delete_candidate_row( size_t const row_index ) {
        // rows.erase(rows.begin()*row_index);
        auto itr = rows.begin();
        std::advance( itr, row_index );
        rows.erase(itr);
        /* //auto r2 = std::iterator_traits<
        if ( std::holds_alternative< std::vector<IO_row_person> >(rows.at(0))) {
        }
        if ( std::holds_alternative< IO_row_person >(rows.at(0))) {
            rows.erase( std::get<IO_row_person>(rows.at(0))   * row_index )
        }
        std::variant< std::vector<IO_row_variant> * > rows_kludge {&rows};  // todo: warning: raw_pointer.
        std::visit( [row_index] (auto & rows, auto itr_begin ) {
            cerr << ", size:"<<rows->size()<<endl;
            //rows->erase( itr_begin * row_index ); cerr << "delete_candidate_row()-rows->at()=row, size:"<<rows->size()<<"\n";
            return;
        }, rows_kludge, rows.begin() );*/
    }

    void print_rows() const;
    void print_spec() const;
    void print() const;
    template< class Archive >
    void serialize( Archive & archive ) {
        archive( rows );
    }
};

using IO_field_spec_variants = std::vector< IO_field_spec_variant >;
extern IO_field_spec_variants io_table_persons_field_specs;
extern IO_table 		      io_table_persons;

std::optional<std::vector<size_t>> get_pk_components( IO_table const & table );

#endif // IO_TABLE_H
