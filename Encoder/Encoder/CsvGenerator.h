#pragma once

#include <string>
#include <vector>
#include "onemeg_db.h" // Provides extonemeg_db and nononemeg_db, and their structs

// Helper function prototypes
std::string format_latitude_dms(long double degrees);
std::string format_longitude_dms(long double degrees);
std::string format_magvar_dms(double degrees, bool is_west);
std::string format_procedure_altitude(int alt_num, int alt_char_enum, bool is_non);
std::string navaid_class_to_string_lookup(int nav_class_enum, bool is_non);
std::string navaid_figureofmerit_to_string(int fom_enum);
std::string pathterm_enum_to_string(int pt_num, bool is_non);
std::string revcode_enum_to_string(int rc);
std::string turndir_enum_to_char(int td);
std::string waypoint_terminalenroute_enum_to_string(int te_enum);
char runway_index_to_char_csv(int index);
std::string fix_type_to_string(int fix_type_enum); // Combined for both, is_non not needed as values differ
std::string altitude_label_to_string(int alt_label_enum); // For AltitudeLabel columns

// Main CSV generation functions
bool generate_csv_from_onemeg_data(extonemeg_db* db, const std::string& output_csv_path);
bool generate_csv_from_nononemeg_data(nononemeg_db* non_db, const std::string& output_csv_path);
