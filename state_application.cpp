#include "state_application.h"

size_t State_application::getSize_t_data() const
{
    return size_t_data;
}

void State_application::setSize_t_data(size_t value)
{
    size_t_data = value;
}

Row_range State_application::getRow_range() const
{
    return row_range;
}

void State_application::setRow_range(const Row_range &value)
{
    row_range = value;
}

int State_application::getInt_data() const
{
    return int_data;
}

void State_application::setInt_data(int value)
{
    int_data = value;
}

bool State_application::getIs_data_unsaved() const
{
    return is_data_unsaved;
}

void State_application::setIs_data_unsaved(bool value)
{
    is_data_unsaved = value;
}

std::ostream &operator<<(std::ostream &os, const Panel_dimensions &pd) {
    os << "pd{"<< pd.height<<"," << pd.width<<"}";
    return os;
}
