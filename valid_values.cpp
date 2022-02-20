#include "valid_values.h"

std::ostream &operator<<(std::ostream &os, const LengthRange &pd) {
    os << "LenghtRange{min: "<<pd.min<<", max:"<<pd.max<<"}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const DecimalValueRange &THIS_)
{
    os << "DecimalValueRange{min: "<<THIS_.min<< ", max:"<<THIS_.max<<", precision: "<<THIS_.precision<<", significant values: "<<THIS_.significant_values<<",  step: "<<THIS_.step<<", is_not_between: "<<THIS_.is_check_value_between<<"}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ScientificValueRange &THIS_)
{
    //os << "DecimalValueRange{min: "<<THIS_.min<< ", max:"<<THIS_.max<<", precision: "<<THIS_.precision<<", significant values: "<<THIS_.significant_values<<",  step: "<<THIS_.step<<", is_not_between: "<<THIS_.is_check_value_between<<"}";
    os << "DecimalValueRange{min: "<<THIS_.min<< ", max:"<<THIS_.max<<", significant values: "<<THIS_.significant_values<<",  step: "<<THIS_.step<<", is_not_between: "<<THIS_.is_check_value_between<<"}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const IntegerValueRange &THIS_)
{
    os << "IntegerValueRange{min: "<<THIS_.min<<", max:"<<THIS_.max<<", step: "<<THIS_.step<<", is_check_value_between: "<<THIS_.is_check_value_between<<"}";
    return os;
}
