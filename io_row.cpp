#include <chrono>
#include "io_row.h"
#include "io_table.h"

void IO_row_person::print() const {
    cout << "name:"<<name<<", "<<is_null_field[0]<<", balance:"<<balance<<", "<<is_null_field[1]<<", age:"<<age<<", "<<is_null_field[2]<<", is_cpp:"<<is_cpp_programmer<<", "<<is_null_field[3]<<"...more..."<<endl;
    cerr << "&name:"<<&name<<", &balance:"<<&balance<<", &age:"<<&age<<", is_cpp:"<<&is_cpp_programmer<<"...more..."<<endl;
    cerr << "is_null_name:"<<is_null_field[0]<<", is_null_balance:"<<is_null_field[1]<<", is_null_age:"<<is_null_field[2]<<", is_null_is_cpp:"<<is_null_field[3]<<"...more..."<<endl;
}

IO_row_person io_candidate_row_person { "candidate_name", 11.11, 11, true};
