#include "field_navigation_interaction_map.h"
#include <tuple>
#include <set>

bool FieldNavigationInteractionMap::verify() const {  // run once by the only function that uses this data structure.

    size_t radix = static_cast<int>(FieldCompletionNav::na) + 2; /* accounts for duplicate int */
    assert( mappings.size() % radix == 0 ); /* accounts for duplicate int */
    assert( mappings.size() % (static_cast<int>( InteractionCategory::na )) == 0 );

    struct Compare {
        bool operator() (const FieldNavInteractionRelation & lhs,
                         const FieldNavInteractionRelation & rhs ) const {
            return std::tie( lhs.category, lhs.field_nav ) < std::tie( rhs.category, rhs.field_nav );  // https://foonathan.net/2018/07/ordering-relations-programming/
        }
    };
    std::set< FieldNavInteractionRelation, Compare > temp { mappings.begin(), mappings.end() };  // todo: TODO: how do I capture radix, when this thing only takes a type?

    assert( mappings.size() == temp.size() + static_cast<int>( InteractionCategory::na ));     // checks for duplicate int suppressed by std::set
    return true;
}
