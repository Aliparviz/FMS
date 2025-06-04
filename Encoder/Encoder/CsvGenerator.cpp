#include "CsvGenerator.h"
#include "csv_parser.h" // For month_to_string, potentially other enum lookups if not reverse engineered
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath> // For std::abs, std::fmod, std::floor
#include <cstring> // For strncpy

// Helper function to convert month number to string (from csv_parser.cpp)
std::string month_to_string_csv(int mon) {
	switch (mon) {
	case 1: return "JAN"; case 2: return "FEB"; case 3: return "MAR";
	case 4: return "APR"; case 5: return "MAY"; case 6: return "JUN";
	case 7: return "JUL"; case 8: return "AUG"; case 9: return "SEP";
	case 10: return "OCT"; case 11: return "NOV"; case 12: return "DEC";
	default: return "UNK";
	}
}

char runway_index_to_char_csv(int index) {
    switch (index) {
    case 0: return ' '; case 1: return 'L'; case 2: return 'R'; case 3: return 'C';
    case 4: return 'B'; case 5: return 'A'; case 6: return 'O'; case 7: return 'M';
    default: return 'X';
    }
}

std::string format_latitude_dms(long double degrees) {
    std::stringstream ss;
    char hemisphere = (degrees >= 0) ? 'N' : 'S';
    degrees = std::abs(degrees);
    int deg = static_cast<int>(std::floor(degrees));
    long double minutes_decimal = (degrees - deg) * 60.0;
    int min = static_cast<int>(std::floor(minutes_decimal));
    long double seconds_decimal = (minutes_decimal - min) * 60.0;

    ss << hemisphere
       << std::setw(2) << std::setfill('0') << deg << "*"
       << std::setw(2) << std::setfill('0') << min << "'"
       << std::fixed << std::setprecision(2) << std::setw(5) << std::setfill('0') << seconds_decimal << "\"";
    return ss.str();
}

std::string format_longitude_dms(long double degrees) {
    std::stringstream ss;
    char hemisphere = (degrees >= 0) ? 'E' : 'W';
    degrees = std::abs(degrees);
    int deg = static_cast<int>(std::floor(degrees));
    long double minutes_decimal = (degrees - deg) * 60.0;
    int min = static_cast<int>(std::floor(minutes_decimal));
    long double seconds_decimal = (minutes_decimal - min) * 60.0;

    ss << hemisphere
       << std::setw(3) << std::setfill('0') << deg << "*"
       << std::setw(2) << std::setfill('0') << min << "'"
       << std::fixed << std::setprecision(2) << std::setw(5) << std::setfill('0') << seconds_decimal << "\"";
    return ss.str();
}

std::string format_magvar_dms(double degrees, bool is_west_explicit_flag) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << degrees;
    return ss.str();
}

std::string format_procedure_altitude(int alt_num, int alt_char_enum, bool is_non) {
    std::stringstream ss;
    if (alt_num == 0 && alt_char_enum == 0) return "";

    bool is_flight_level = false;
    int display_alt = alt_num;

    // FL determination based on 'is_non'
    if (is_non) { // non-onemeg specific FL handling (e.g. FL180 is just 180 in some CSVs)
        if (alt_num >= 180 && alt_num < 1000) { // Heuristic for non-onemeg FLs (e.g. 180, 240)
             // Check if this is intended to be FLXXX or just XXX hundred feet.
             // csv_parser for non-onemeg: if (alt_str.rfind("FL", 0) == 0) { alt = std::stoi(alt_str.substr(2, alt_str.length() - 2)); }
             // This implies non-onemeg CSV uses "FL180" which is then converted to 180.
             // So, to reverse, if alt_num is 180, output should be "FL180".
            ss << "FL" << display_alt; // display_alt is already 180, 240 etc.
        } else { // Not a FL or a very high absolute altitude
            ss << display_alt;
        }
    } else { // onemeg
        if (alt_num >= 18000) { // Standard FL representation
            display_alt = alt_num / 100;
            is_flight_level = true;
            ss << "FL" << display_alt;
        } else {
            ss << display_alt;
        }
    }

    // Suffixes: Based on AltLable_to_int from csv_parser.cpp
    // "A":1, "B":2, " ":0, "+A":1, "+B":2, "-A":1, "-B":2, "G":3, "H":4, "S":5
    // The alt_char_enum in procedure_fix struct should correspond to these.
    switch (alt_char_enum) {
        case 1: ss << "A"; break;
        case 2: ss << "B"; break;
        case 3: ss << "G"; break;
        case 4: ss << "H"; break;
        case 5: ss << "S"; break;
        default: break;
    }

    if (alt_num == 0 && alt_char_enum == 1 && !is_flight_level && !is_non) { // onemeg "0A"
        return "0A";
    }
    if (alt_num == 0 && alt_char_enum == 1 && is_non ) { // non-onemeg "0A" (FL0A is not typical)
        return "0A";
    }


    return ss.str();
}

std::string navaid_class_to_string_lookup(int nav_class_enum, bool is_non) {
    if (is_non) {
        switch (nav_class_enum) {
            case 0: return "VOT";
            case 1: return "TAC";
            case 2: return "NDB";
            case 3: return "ILS";
            default: return std::to_string(nav_class_enum);
        }
    } else {
        switch (nav_class_enum) {
            case 0: return "VOR"; case 1: return "VORDME"; case 2: return "VORTAC";
            case 3: return "TACAN"; case 5: return "ILS"; case 6: return "ILSDME";
            case 7: return "LOC"; case 8: return "LOCDME";
            // Less common ones, add if necessary based on actual data range for nav_class_enum
            case 13: return "UNAV"; case 14: return "RNAV"; case 15: return "GPS";
            default: return std::to_string(nav_class_enum);
        }
    }
}

std::string navaid_figureofmerit_to_string(int fom_enum) {
    switch (fom_enum) {
        case 0: return "NONE"; case 1: return "ONE"; case 2: return "TWO";
        case 3: return "THREE"; case 4: return "FOUR"; case 5: return "FIVE";
        case 6: return "SIX"; case 7: return "SEVEN";
        default: return std::to_string(fom_enum);
    }
}

void paththerm_num_to_string_local(int pathterm_num, char * pathterm) { // Renamed to avoid conflict if linked with Encoder.o
	switch (pathterm_num) {
	    case 0: strncpy(pathterm, "AF", 3); break; case 1: strncpy(pathterm, "CF", 3); break;
	    case 2: strncpy(pathterm, "DF", 3); break; case 3: strncpy(pathterm, "FA", 3); break;
	    case 4: strncpy(pathterm, "IF", 3); break; case 5: strncpy(pathterm, "FM", 3); break;
	    case 6: strncpy(pathterm, "HA", 3); break; case 7: strncpy(pathterm, "HF", 3); break;
	    case 8: strncpy(pathterm, "HM", 3); break; case 9: strncpy(pathterm, "PI", 3); break;
	    case 10: strncpy(pathterm, "TF", 3); break;case 11: strncpy(pathterm, "CA", 3); break;
	    case 12: strncpy(pathterm, "CD", 3); break;case 13: strncpy(pathterm, "CI", 3); break;
	    case 14: strncpy(pathterm, "VM", 3); break;case 15: strncpy(pathterm, "CR", 3); break;
	    case 16: strncpy(pathterm, "RF", 3); break;case 17: strncpy(pathterm, "FC", 3); break;
	    case 18: strncpy(pathterm, "FD", 3); break;default: strncpy(pathterm, "  ", 3); break;
	}
   pathterm[2] = '\0';
}

std::string pathterm_enum_to_string(int pt_num, bool is_non) {
    char pathterm_str[3];
    paththerm_num_to_string_local(pt_num, pathterm_str);
    return std::string(pathterm_str);
}

std::string revcode_enum_to_string(int rc) {
    switch (rc) {
        case 0: return "UNCHANGED"; case 1: return "NEW";
        case 2: return "CHANGED"; case 3: return "DELETED";
        default: return std::to_string(rc);
    }
}

std::string turndir_enum_to_char(int td) {
    switch (td) {
        case 1: return "L"; case 2: return "R"; case 3: return "E";
        default: return " ";
    }
}

std::string waypoint_terminalenroute_enum_to_string(int te_enum) {
    switch (te_enum) {
        case 1: return "TERMINAL"; case 2: return "ENROUTE"; case 3: return "BOTH";
        default: return std::to_string(te_enum);
    }
}

std::string fix_type_to_string(int fix_type_enum) {
    // Based on FixType_to_int in csv_parser.cpp (seems common for onemeg/non-onemeg)
    // "WAYPOINT":1, "NDB":2, "VOR":3, "AIRPORT":4, "LOCATOR":5, "FIX":6
    switch (fix_type_enum) {
        case 1: return "WAYPOINT"; case 2: return "NDB"; case 3: return "VOR";
        case 4: return "AIRPORT";  case 5: return "LOCATOR"; case 6: return "FIX";
        default: return std::to_string(fix_type_enum);
    }
}

std::string altitude_label_to_string(int alt_label_enum) {
    // For columns like "Alt1Lable", "Alt2Lable", "AltitudeLabel" (holding)
    // Based on AltLable_to_int: " ":0, "A":1, "B":2, "G":3, "H":4, "S":5
    switch (alt_label_enum) {
        case 0: return "";  // Empty string for "AT" usually
        case 1: return "A"; case 2: return "B"; case 3: return "G";
        case 4: return "H"; case 5: return "S";
        default: return std::to_string(alt_label_enum);
    }
}


bool generate_csv_from_onemeg_data(extonemeg_db* db, const std::string& output_csv_path) {
    if (!db) return false;
    std::ofstream outfile(output_csv_path);
    if (!outfile.is_open()) { return false; }
    outfile << std::fixed << std::setprecision(8);

    outfile << "Database Name = " << db->dinf.database_name << std::endl;
    outfile << "Sequence = " << db->dinf.sequence_name << std::endl;
    outfile << "Previous Start Date = " << std::setw(2) << std::setfill('0') << db->dinf.prev_start.day << "-" << month_to_string_csv(db->dinf.prev_start.mon) << "-" << db->dinf.prev_start.year << std::endl;
    outfile << "Previous End Date = " << std::setw(2) << std::setfill('0') << db->dinf.prev_end.day << "-" << month_to_string_csv(db->dinf.prev_end.mon) << "-" << db->dinf.prev_end.year << std::endl;
    outfile << "Current Start Date = " << std::setw(2) << std::setfill('0') << db->dinf.cur_start.day << "-" << month_to_string_csv(db->dinf.cur_start.mon) << "-" << db->dinf.cur_start.year << std::endl;
    outfile << "Current End Date = " << std::setw(2) << std::setfill('0') << db->dinf.cur_end.day << "-" << month_to_string_csv(db->dinf.cur_end.mon) << "-" << db->dinf.cur_end.year << std::endl;
    outfile << "Tag = " << db->dinf.tag << std::endl;
    outfile << std::endl;

    outfile << "Airports <<< Record Count = " << db->airports.size() << std::endl;
    outfile << "Ident,RevCode,ProceduresExist,RunwaysExist,SpeedLimitCoded,TransAltValid,TrueNorth,Latitude,Longitude,MagVar,Elevation,SpeedLimit,SpeedLimitAltitude,TransAltitude,GatesExist,AlternatesExist,MaxRunwayLength,SIDExist,STARExist,ApproachExist,EOSIDExist,CC" << std::endl;
    for (const auto& rec : db->airports) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << (rec.ProceduresExist ? "TRUE" : "FALSE") << "," << (rec.RunwaysExist ? "TRUE" : "FALSE") << "," << (rec.SpeedLimitCoded ? "TRUE" : "FALSE") << "," << (rec.TransAltValid ? "TRUE" : "FALSE") << "," << "TRUE" << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << std::fixed << std::setprecision(1) << rec.MagVar << "," << rec.Elevation << "," << rec.SpeedLimit << "," << rec.SpeedLimitAltitude << "," << rec.TransAltitude << "," << (rec.GatesExist ? "TRUE" : "FALSE") << "," << (rec.AlternatesExist ? "TRUE" : "FALSE") << "," << rec.MaxRunwayLength << "," << (rec.SidExist ? "TRUE" : "FALSE") << "," << (rec.StarExist ? "TRUE" : "FALSE") << "," << (rec.ApproachExist ? "TRUE" : "FALSE") << "," << (rec.EosidExist ? "TRUE" : "FALSE") << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "Navaids <<< Record Count = " << db->navaids.size() << std::endl;
    outfile << "Ident,RevCode,TrueNorth,NavClass,FigureOfMerit,Latitude,Longitude,Frequency,Elevation,StationDeclination,IsCustomized,Bearing,GlideSlope,CC" << std::endl;
    for (const auto& rec : db->navaids) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << (rec.TrueNorth ? "TRUE" : "FALSE") << "," << (rec.NavClassString.empty() ? navaid_class_to_string_lookup(rec.NavClass, false) : rec.NavClassString) << "," << (rec.FigureOfMeritString.empty() ? navaid_figureofmerit_to_string(rec.FigureOfMerit) : rec.FigureOfMeritString) << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << std::fixed << std::setprecision(3) << rec.Freq << "," << rec.Elevation << "," << std::fixed << std::setprecision(1) << rec.StationDeclination << "," << (rec.IsCustomized ? "TRUE" : "FALSE") << "," << std::fixed << std::setprecision(1) << rec.Bearing << "," << std::fixed << std::setprecision(1) << rec.GlideSlope << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "Waypoints <<< Record Count = " << db->waypoints.size() << std::endl;
    outfile << "Ident,RevCode,TerminalEnroute,Latitude,Longitude,IsCustomized,CC" << std::endl;
     for (const auto& rec : db->waypoints) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << (rec.TerminalEnrouteString.empty() ? waypoint_terminalenroute_enum_to_string(rec.TerminalEnroute) : rec.TerminalEnrouteString) << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << (rec.IsCustom ? "TRUE" : "FALSE") << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "NDBs <<< Record Count = " << db->ndbs.size() << std::endl;
    outfile << "Ident,RevCode,Latitude,Longitude,Frequency,CC" << std::endl;
    for (const auto& rec : db->ndbs) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << std::fixed << std::setprecision(1) << rec.Frequency << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "Runways <<< Record Count = " << db->runways.size() << std::endl;
    outfile << "Airport,Ident,RevCode,TrueNorth,MagBearing,Length,Latitude,Longitude,ThrDisp,Elevation,ILSCategory,ILSBearing,MLSNavaid,MLSAzimuthBrng,IsCustomized,MLSMinGPA,FMSVFR,ILSNavaid,BiDir" << std::endl;
    for (const auto& rec : db->runways) {
        outfile << rec.Airport << "," << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << "TRUE" << "," << std::fixed << std::setprecision(1) << rec.MagBearing << "," << rec.Length << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << rec.ThrDisp << "," << rec.Elevation << "," << rec.ILSCategory << "," << std::fixed << std::setprecision(1) << rec.ILSBearing << "," << "" << "," << "" << "," << "FALSE" << "," << "" << "," << (rec.FMSVFR ? "TRUE" : "FALSE") << "," << (rec.ILSExist ? rec.ILSNavaid : "") << "," << (rec.BiDir ? "TRUE" : "FALSE")  << std::endl;
    }
    outfile << std::endl;

    outfile << "Procedures <<< Record Count = " << db->procedures.size() << std::endl;
    outfile << "Airport,Ident,BaseProcedure,Runway,TransAltitude,RevCode,ViaType,IsCustomized,Extended,Fix,FixLatitude,FixLongitude,FixType,TurnDir,PathTerm,OverFly,MissedApproach,Course,Heading,Radial,Alt1Lable,Alt2Lable,Alt1,Alt2,CombinedAlt1,CombinedAlt2,Distance,Time,Navaid,NavaidFrequency,NavaidLatitude,NavaidLongitude,NavaidDeclination,NDB,NDBFrequency,NDBLatitude,NDBLongitude,ArcCtr,ArcCtrLatitude,ArcCtrLongitude,FPA,RNP,GSCA,CondAlt,ARC,ProcIndex,CAS,FAF" << std::endl;
    for (const auto& proc : db->procedures) {
        bool first_fix = true;
        if (proc.fixes.empty() && proc.header.Ident.length() > 0) {
             outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << revcode_enum_to_string(proc.header.RevCode) << "," << proc.header.ViaTypeString << "," << "FALSE" << "," << "FALSE" << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
        } else {
            for (const auto& fix : proc.fixes) {
                if (first_fix) {
                    outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << revcode_enum_to_string(proc.header.RevCode) << "," << proc.header.ViaTypeString << "," << "FALSE" << "," << "FALSE" << ",";
                    first_fix = false;
                } else {
                     outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << ",";
                }
                outfile << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << fix_type_to_string(fix.fix_type) << "," << turndir_enum_to_char(fix.turn_dir) << "," << pathterm_enum_to_string(fix.pathterm_num, false) << "," << (fix.OverFly ? "TRUE" : "FALSE") << "," << (fix.MissedApproach ? "TRUE" : "FALSE") << "," << (fix.cs_hd_exist ? std::to_string(fix.cs_hd) : "") << "," << "" << "," << (fix.radial_exist ? std::to_string(fix.Radial) : "") << "," << altitude_label_to_string(fix.alt1_char) << "," << altitude_label_to_string(fix.alt2_char) << "," << (fix.alt1_exist ? std::to_string(fix.alt1_num) : "") << "," << (fix.alt2_exist ? std::to_string(fix.alt2_num) : "") << "," << format_procedure_altitude(fix.alt1_num, fix.alt1_char, false) << "," << format_procedure_altitude(fix.alt2_num, fix.alt2_char, false) << "," << (fix.dist_exist ? std::to_string(fix.dist) : "") << "," << (fix.time_exist ? std::to_string(fix.time) : "") << "," << (fix.nav_exist ? fix.nav_ident : "") << "," << "" << "," << (fix.nav_exist ? format_latitude_dms(fix.nav_Latitude) : "") << "," << (fix.nav_exist ? format_longitude_dms(fix.nav_Longitude) : "") << "," << "" << "," << (fix.ndb_exist ? fix.ndb_ident : "") << "," << "" << "," << (fix.ndb_exist ? format_latitude_dms(fix.ndb_Latitude) : "") << "," << (fix.ndb_exist ? format_longitude_dms(fix.ndb_Longitude) : "") << "," << (fix.arc_exist ? fix.arc_ident : "") << "," << (fix.arc_exist ? format_latitude_dms(fix.arc_Latitude) : "") << "," << (fix.arc_exist ? format_longitude_dms(fix.arc_Longitude) : "") << "," << (fix.fpa_exist ? std::to_string(fix.FPA) : "") << "," << std::fixed << std::setprecision(2) << fix.RNP << "," << (fix.gcsa_exist ? std::to_string(fix.GSCA) : "") << "," << "" << "," << (fix.arc_exist ? "TRUE" : "FALSE") << "," << "" << "," << (fix.cas_exist ? std::to_string(fix.CAS) : "") << "," << "FALSE"  << std::endl;
            }
        }
    }
    outfile << std::endl;

    outfile << "CustomProcedures <<< Record Count = " << db->custom_procedures.size() << std::endl;
    outfile << "Airport,Ident,BaseProcedure,Runway,TransAltitude,RevCode,ViaType,IsCustomized,Extended,Fix,FixLatitude,FixLongitude,FixType,TurnDir,PathTerm,OverFly,MissedApproach,Course,Heading,Radial,Alt1Lable,Alt2Lable,Alt1,Alt2,CombinedAlt1,CombinedAlt2,Distance,Time,Navaid,NavaidFrequency,NavaidLatitude,NavaidLongitude,NavaidDeclination,NDB,NDBFrequency,NDBLatitude,NDBLongitude,ArcCtr,ArcCtrLatitude,ArcCtrLongitude,FPA,RNP,GSCA,CondAlt,ARC,ProcIndex,CAS,FAF" << std::endl;
     for (const auto& proc : db->custom_procedures) {
        bool first_fix = true;
        if (proc.fixes.empty() && proc.header.Ident.length() > 0) {
             outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << revcode_enum_to_string(proc.header.RevCode) << "," << proc.header.ViaTypeString << ","  << "TRUE" << "," << "FALSE" << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
        } else {
            for (const auto& fix : proc.fixes) {
                if (first_fix) {
                    outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << revcode_enum_to_string(proc.header.RevCode) << "," << proc.header.ViaTypeString << "," << "TRUE" << "," << "FALSE" << ",";
                    first_fix = false;
                } else {
                     outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << "," << "TRUE" << "," << "" << ",";
                }
                outfile << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << fix_type_to_string(fix.fix_type) << "," << turndir_enum_to_char(fix.turn_dir) << "," << pathterm_enum_to_string(fix.pathterm_num, false) << "," << (fix.OverFly ? "TRUE" : "FALSE") << "," << (fix.MissedApproach ? "TRUE" : "FALSE") << "," << (fix.cs_hd_exist ? std::to_string(fix.cs_hd) : "") << "," << "" << "," << (fix.radial_exist ? std::to_string(fix.Radial) : "") << "," << altitude_label_to_string(fix.alt1_char) << "," << altitude_label_to_string(fix.alt2_char) << "," << (fix.alt1_exist ? std::to_string(fix.alt1_num) : "") << "," << (fix.alt2_exist ? std::to_string(fix.alt2_num) : "") << "," << format_procedure_altitude(fix.alt1_num, fix.alt1_char, false) << "," << format_procedure_altitude(fix.alt2_num, fix.alt2_char, false) << "," << (fix.dist_exist ? std::to_string(fix.dist) : "") << "," << (fix.time_exist ? std::to_string(fix.time) : "") << "," << (fix.nav_exist ? fix.nav_ident : "") << "," << "" << "," << (fix.nav_exist ? format_latitude_dms(fix.nav_Latitude) : "") << "," << (fix.nav_exist ? format_longitude_dms(fix.nav_Longitude) : "") << "," << "" << "," << (fix.ndb_exist ? fix.ndb_ident : "") << "," << "" << "," << (fix.ndb_exist ? format_latitude_dms(fix.ndb_Latitude) : "") << "," << (fix.ndb_exist ? format_longitude_dms(fix.ndb_Longitude) : "") << "," << (fix.arc_exist ? fix.arc_ident : "") << "," << (fix.arc_exist ? format_latitude_dms(fix.arc_Latitude) : "") << "," << (fix.arc_exist ? format_longitude_dms(fix.arc_Longitude) : "") << "," << (fix.fpa_exist ? std::to_string(fix.FPA) : "") << "," << std::fixed << std::setprecision(2) << fix.RNP << "," << (fix.gcsa_exist ? std::to_string(fix.GSCA) : "") << "," << "" << "," << (fix.arc_exist ? "TRUE" : "FALSE") << "," << "" << "," << (fix.cas_exist ? std::to_string(fix.CAS) : "") << "," << "FALSE" << std::endl;
            }
        }
    }
    outfile << std::endl;

    outfile << "GridMORA <<< Record Count = " << db->gridmoras.size() << std::endl;
    outfile << "RevCode,Latitude,Longitude,Altitude1,Altitude2,Altitude3,Altitude4,Altitude5,Altitude6,Altitude7,Altitude8,Altitude9,Altitude10,Altitude11,Altitude12,Altitude13,Altitude14,Altitude15,Altitude16,Altitude17,Altitude18,Altitude19,Altitude20,Altitude21,Altitude22,Altitude23,Altitude24,Altitude25,Altitude26,Altitude27,Altitude28,Altitude29,Altitude30" << std::endl;
    for (const auto& mora : db->gridmoras) {
        outfile << revcode_enum_to_string(mora.RevCode) << "," << std::fixed << std::setprecision(2) << mora.Latitude << "," << std::fixed << std::setprecision(2) << mora.Longitude;
        for (int i = 0; i < 30; ++i) { outfile << "," << mora.Altitude[i]; }
        outfile << std::endl;
    }
    outfile << std::endl;

    outfile << "AirwayIndexes <<< Record Count = " << db->airways.size() << std::endl;
    outfile << "Airway,Extended,FixCount,FixRadTurnAlt,FRTAltAbove,FRTAltBelow,RevCode" << std::endl;
    for (const auto& awy_idx : db->airways) {
        outfile << awy_idx.Ident << "," << "FALSE" << "," << awy_idx.fixes.size() << "," << "" << "," << "" << "," << "" << "," << revcode_enum_to_string(awy_idx.revcode) << std::endl;
    }
    outfile << std::endl;

    outfile << "AirwayFixes <<< Record Count = ";
    size_t total_airway_fixes = 0;
    for (const auto& awy : db->airways) total_airway_fixes += awy.fixes.size();
    outfile << total_airway_fixes << std::endl;
    outfile << "Airway,FixType,Fix,Latitude,Longitude,RNP,AwyFixIndex,RevCode,CTR" << std::endl;
    for (const auto& awy : db->airways) {
        int fix_idx_in_airway = 0;
        for (const auto& fix : awy.fixes) {
            outfile << awy.Ident << "," << fix_type_to_string(fix.fix_type) << ","  << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << "" << "," << fix_idx_in_airway++ << "," << revcode_enum_to_string(awy.revcode) << "," << "" << std::endl;
        }
    }
    outfile << std::endl;

    outfile << "HoldingPatterns <<< Record Count = " << db->holdingpatterns.size() << std::endl;
    outfile << "Fix,RevCode,HoldType,FixType,Latitude,Longitude,Altitude,AltCombined,AltitudeLabel,InBndCourse,LegLength,LegTime,TrueNorth,TurnDir" << std::endl;
    for (const auto& hp : db->holdingpatterns) {
        outfile << hp.FilxIdent << "," << revcode_enum_to_string(hp.RevCode) << "," << hp.HoldTypeString << "," << fix_type_to_string(hp.FixType) << "," << format_latitude_dms(hp.Latitude) << "," << format_longitude_dms(hp.Longitude) << "," << (hp.Altitude_exist ? std::to_string(hp.Altitude) : "") << "," << format_procedure_altitude(hp.Altitude, hp.AltitudeLabel, false) << "," << altitude_label_to_string(hp.AltitudeLabel) << ","  << std::fixed << std::setprecision(1) << hp.InBndCourse << "," << std::fixed << std::setprecision(1) << (hp.time_length_used == 0 ? hp.LegLength : 0.0) << "," << std::fixed << std::setprecision(1) << (hp.time_length_used == 1 ? hp.LegTime : 0.0) << "," << "TRUE" << "," << turndir_enum_to_char(hp.TurnDir) << std::endl;
    }
    outfile << std::endl;

    outfile << "FuelPolicy <<< Record Count = " << db->fuelpolicies.size() << std::endl;
    outfile << "RI,RA,FD,FinalAlt,TaxiFuel,ReserveFuel,ReserveMax,ReserveMin,FinalTimeInf,FinalTimePre,FinalFixInf" << std::endl;
    for (const auto& fp : db->fuelpolicies) {
        outfile << (fp.RI ? "TRUE" : "FALSE") << "," << (fp.RA ? "TRUE" : "FALSE") << "," << (fp.FD ? "TRUE" : "FALSE") << "," << fp.FinalAlt << "," << std::fixed << std::setprecision(1) << fp.TaxiFuel << "," << std::fixed << std::setprecision(1) << fp.ReserveFuel << "," << std::fixed << std::setprecision(1) << fp.ReserveMax << "," << std::fixed << std::setprecision(1) << fp.ReserveMin << "," << fp.FinalTimeInf << "," << fp.FinalTimePre << "," << std::fixed << std::setprecision(1) << fp.FinalFixInf << std::endl;
    }
    outfile << std::endl;

    outfile.close();
    return true;
}

bool generate_csv_from_nononemeg_data(nononemeg_db* non_db, const std::string& output_csv_path) {
    if (!non_db) return false;
    std::ofstream outfile(output_csv_path);
    if (!outfile.is_open()) { return false; }
    outfile << std::fixed << std::setprecision(8);

    outfile << "Airports <<< Record Count = " << non_db->airports.size() << std::endl;
    outfile << "Ident,RevCode,ProceduresExist,RunwaysExist,GatesExist,AlternatesExist,SpeedLimitCoded,TransAltValid,Latitude,Longitude,Elevation,SpeedLimit,SpeedLimitAltitude,TransAltitude,MaxRunwayLength,TrueNorth,MagVar,CC,SIDExist,STARExist,ApproachExist,EOSIDExist" << std::endl;
    for (const auto& rec : non_db->airports) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << (rec.ProceduresExist ? "TRUE" : "FALSE") << "," << (rec.RunwaysExist ? "TRUE" : "FALSE") << "," << (rec.GatesExist ? "TRUE" : "FALSE") << "," << (rec.AlternatesExist ? "TRUE" : "FALSE") << "," << rec.SpeedLimitCoded << "," << (rec.TransAltValid ? "TRUE" : "FALSE") << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << rec.Elevation << "," << rec.SpeedLimit << "," << rec.SpeedLimitAltitude << "," << rec.TransAltitude << "," << rec.MaxRunwayLength << "," << "FALSE" << "," << std::fixed << std::setprecision(1) << rec.MagVar << "," << " " << "," << (rec.SidExist ? "TRUE" : "FALSE") << "," << (rec.StarExist ? "TRUE" : "FALSE") << "," << (rec.ApproachExist ? "TRUE" : "FALSE") << "," << (rec.EosidExist ? "TRUE" : "FALSE") << std::endl;
    }
    outfile << std::endl;

    outfile << "Navaids <<< Record Count = " << non_db->navaids.size() << std::endl;
    outfile << "Ident,RevCode,Frequency,Latitude,Longitude,StationDeclination,Elevation,FigureOfMerit,CC,NavClass,IsCustomized,Bearing,GlideSlope,TrueNorth" << std::endl;
    for (const auto& rec : non_db->navaids) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << std::fixed << std::setprecision(3) << rec.Freq << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << std::fixed << std::setprecision(1) << rec.StationDeclination << "," << rec.Elevation << "," << (rec.FigureOfMeritString.empty() ? navaid_figureofmerit_to_string(rec.FigureOfMerit) : rec.FigureOfMeritString) << "," << " " << "," << (rec.NavClassString.empty() ? navaid_class_to_string_lookup(rec.NavClass, true) : rec.NavClassString) << "," << (rec.IsCustomized ? "TRUE" : "FALSE") << "," << std::fixed << std::setprecision(1) << rec.Bearing << "," << std::fixed << std::setprecision(1) << rec.GlideSlope << "," << (rec.TrueNorth ? "TRUE" : "FALSE") << std::endl;
    }
    outfile << std::endl;

    outfile << "Waypoints <<< Record Count = " << non_db->waypoints.size() << std::endl;
    outfile << "Ident,RevCode,Latitude,Longitude,TerminalEnroute,IsCustomized,CC" << std::endl;
    for (const auto& rec : non_db->waypoints) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << (rec.TerminalEnrouteString.empty() ? waypoint_terminalenroute_enum_to_string(rec.TerminalEnroute) : rec.TerminalEnrouteString) << "," << (rec.IsCustom ? "TRUE" : "FALSE") << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "NDBs <<< Record Count = " << non_db->ndbs.size() << std::endl;
    outfile << "Ident,RevCode,Latitude,Longitude,Frequency,CC" << std::endl;
     for (const auto& rec : non_db->ndbs) {
        outfile << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << std::fixed << std::setprecision(1) << rec.Frequency << "," << " " << std::endl;
    }
    outfile << std::endl;

    outfile << "Runways <<< Record Count = " << non_db->runways.size() << std::endl;
    outfile << "Airport,Ident,RevCode,FMSVFR,MagBearing,Length,Latitude,Longitude,ThrDisp,Elevation,ILSCategory,ILSBearing,ILSNavaid" << std::endl;
    for (const auto& rec : non_db->runways) {
        outfile << rec.Airport << "," << rec.Ident << "," << revcode_enum_to_string(rec.RevCode) << "," << (rec.FMSVFR ? "TRUE" : "FALSE") << "," << std::fixed << std::setprecision(1) << rec.MagBearing << "," << rec.Length << "," << format_latitude_dms(rec.Latitude) << "," << format_longitude_dms(rec.Longitude) << "," << rec.ThrDisp << "," << rec.Elevation << "," << rec.ILSCategory << "," << std::fixed << std::setprecision(1) << rec.ILSBearing << "," << (rec.ILSExist ? rec.ILSNavaid : "")  << std::endl;
    }
    outfile << std::endl;

    outfile << "Procedures <<< Record Count = " << non_db->procedures.size() << std::endl;
    outfile << "Airport,Ident,BaseProcedure,Runway,TransAltitude,ViaType,RevCode,Fix,FixLatitude,FixLongitude,FixType,TurnDir,PathTerm,Course,Alt1,Alt2,CombinedAlt1,CombinedAlt2,Distance,Navaid,NDB,OverFly,MissedApproach,IsCustomized,FPA,Time,NavaidFrequency,NavaidDeclination,NavaidLatitude,NavaidLongitude,NDBFrequency,NDBLatitude,NDBLongitude,Alt1Lable,Alt2Lable,Radial,CAS,GSCA,ARC" << std::endl;
    for (const auto& proc : non_db->procedures) {
        bool first_fix = true;
        if (proc.fixes.empty() && proc.header.Ident.length() > 0) {
            outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << proc.header.ViaTypeString << "," << revcode_enum_to_string(proc.header.RevCode) << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
        } else {
            for (const auto& fix : proc.fixes) {
                if (first_fix) {
                    outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << proc.header.ViaTypeString << "," << revcode_enum_to_string(proc.header.RevCode) << ",";
                    first_fix = false;
                } else {
                    outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << ",";
                }
                outfile << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << fix_type_to_string(fix.fix_type) << ","  << turndir_enum_to_char(fix.turn_dir) << "," << pathterm_enum_to_string(fix.pathterm_num, true) << "," << (fix.cs_hd_exist ? std::to_string(fix.cs_hd) : "") << "," << (fix.alt1_exist ? std::to_string(fix.alt1_num) : "") << ","   << (fix.alt2_exist ? std::to_string(fix.alt2_num) : "") << ","   << format_procedure_altitude(fix.alt1_num, fix.alt1_char, true) << "," << format_procedure_altitude(fix.alt2_num, fix.alt2_char, true) << "," << (fix.dist_exist ? std::to_string(fix.dist) : "") << "," << (fix.nav_exist ? fix.nav_ident : "") << ","   << (fix.ndb_exist ? fix.ndb_ident : "") << ","   << (fix.OverFly ? "TRUE" : "FALSE") << "," << (fix.MissedApproach ? "TRUE" : "FALSE") << "," << "FALSE" << "," << (fix.fpa_exist ? std::to_string(fix.FPA) : "") << "," << (fix.time_exist ? std::to_string(fix.time) : "") << ","       << "" << "," << "" << "," << (fix.nav_exist ? format_latitude_dms(fix.nav_Latitude) : "") << "," << (fix.nav_exist ? format_longitude_dms(fix.nav_Longitude) : "") << "," << "" << "," << (fix.ndb_exist ? format_latitude_dms(fix.ndb_Latitude) : "") << "," << (fix.ndb_exist ? format_longitude_dms(fix.ndb_Longitude) : "") << "," << altitude_label_to_string(fix.alt1_char) << "," << altitude_label_to_string(fix.alt2_char) << "," << (fix.radial_exist ? std::to_string(fix.Radial) : "") << ","   << (fix.cas_exist ? std::to_string(fix.CAS) : "") << "," << (fix.gcsa_exist ? std::to_string(fix.GSCA) : "") << ","       << (fix.arc_exist ? "TRUE" : "FALSE")  << std::endl;
            }
        }
    }
    outfile << std::endl;

    if (non_db->custom_procedures.size() > 0) {
        outfile << "CustomProcedures <<< Record Count = " << non_db->custom_procedures.size() << std::endl;
        outfile << "Airport,Ident,BaseProcedure,Runway,TransAltitude,ViaType,RevCode,Fix,FixLatitude,FixLongitude,FixType,TurnDir,PathTerm,Course,Alt1,Alt2,CombinedAlt1,CombinedAlt2,Distance,Navaid,NDB,OverFly,MissedApproach,IsCustomized,FPA,Time,NavaidFrequency,NavaidDeclination,NavaidLatitude,NavaidLongitude,NDBFrequency,NDBLatitude,NDBLongitude,Alt1Lable,Alt2Lable,Radial,CAS,GSCA,ARC" << std::endl;
        for (const auto& proc : non_db->custom_procedures) {
            bool first_fix = true;
            if (proc.fixes.empty() && proc.header.Ident.length() > 0) {
                 outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << proc.header.ViaTypeString << "," << revcode_enum_to_string(proc.header.RevCode) << ",,,,,,,,,,,,,,,,,,,,,,,TRUE,,,,,,,,,,,,,," << std::endl;
            } else {
                for (const auto& fix : proc.fixes) {
                    if (first_fix) {
                        outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << proc.header.RunwayString << "," << (proc.header.transalt_Exist ? std::to_string(proc.header.transAlt) : "") << "," << proc.header.ViaTypeString << "," << revcode_enum_to_string(proc.header.RevCode) << ",";
                        first_fix = false;
                    } else {
                        outfile << proc.header.Airport << "," << proc.header.Ident << "," << "" << "," << "" << "," << "" << "," << "" << "," << "" << ",";
                    }
                    outfile << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << fix_type_to_string(fix.fix_type) << ","  << turndir_enum_to_char(fix.turn_dir) << "," << pathterm_enum_to_string(fix.pathterm_num, true) << "," << (fix.cs_hd_exist ? std::to_string(fix.cs_hd) : "") << "," << (fix.alt1_exist ? std::to_string(fix.alt1_num) : "") << "," << (fix.alt2_exist ? std::to_string(fix.alt2_num) : "") << "," << format_procedure_altitude(fix.alt1_num, fix.alt1_char, true) << "," << format_procedure_altitude(fix.alt2_num, fix.alt2_char, true) << "," << (fix.dist_exist ? std::to_string(fix.dist) : "") << "," << (fix.nav_exist ? fix.nav_ident : "") << "," << (fix.ndb_exist ? fix.ndb_ident : "") << "," << (fix.OverFly ? "TRUE" : "FALSE") << "," << (fix.MissedApproach ? "TRUE" : "FALSE") << "," << "TRUE" << "," << (fix.fpa_exist ? std::to_string(fix.FPA) : "") << "," << (fix.time_exist ? std::to_string(fix.time) : "") << "," << "" << "," << "" << "," << (fix.nav_exist ? format_latitude_dms(fix.nav_Latitude) : "") << "," << (fix.nav_exist ? format_longitude_dms(fix.nav_Longitude) : "") << "," << "" << "," << (fix.ndb_exist ? format_latitude_dms(fix.ndb_Latitude) : "") << "," << (fix.ndb_exist ? format_longitude_dms(fix.ndb_Longitude) : "") << "," << altitude_label_to_string(fix.alt1_char) << "," << altitude_label_to_string(fix.alt2_char) << "," << (fix.radial_exist ? std::to_string(fix.Radial) : "") << "," << (fix.cas_exist ? std::to_string(fix.CAS) : "") << "," << (fix.gcsa_exist ? std::to_string(fix.GSCA) : "") << "," << (fix.arc_exist ? "TRUE" : "FALSE") << std::endl;
                }
            }
        }
        outfile << std::endl;
    }

    outfile << "Airways <<< Record Count = ";
    size_t total_non_airway_fixes = 0;
    for (const auto& awy : non_db->airways) total_non_airway_fixes += awy.fixes.size();
    outfile << total_non_airway_fixes << std::endl;
    outfile << "Airway,RevCde,FixType,Fix,Latitude,Longitude,AwyFixIndex,RNP,CTR" << std::endl;
    for (const auto& awy : non_db->airways) {
        int fix_idx_in_airway = 0;
        for (const auto& fix : awy.fixes) {
            outfile << awy.Ident << "," << revcode_enum_to_string(awy.revcode) << "," << fix_type_to_string(fix.fix_type) << ","  << fix.fix_ident << "," << format_latitude_dms(fix.fix_Latitude) << "," << format_longitude_dms(fix.fix_Longitude) << "," << fix_idx_in_airway++ << "," << "" << "," << ""  << std::endl;
        }
    }
    outfile << std::endl;

    if (non_db->fuelpolicies.size() > 0) {
        outfile << "FuelPolicy <<< Record Count = " << non_db->fuelpolicies.size() << std::endl;
        outfile << "RI,RA,FD,FinalAlt,TaxiFuel,ReserveFuel,ReserveMax,ReserveMin,FinalTimeInf,FinalTimePre,FinalFixInf" << std::endl;
        for (const auto& fp : non_db->fuelpolicies) {
            outfile << (fp.RI ? "TRUE" : "FALSE") << "," << (fp.RA ? "TRUE" : "FALSE") << "," << (fp.FD ? "TRUE" : "FALSE") << "," << fp.FinalAlt << "," << std::fixed << std::setprecision(1) << fp.TaxiFuel << "," << std::fixed << std::setprecision(1) << fp.ReserveFuel << "," << std::fixed << std::setprecision(1) << fp.ReserveMax << "," << std::fixed << std::setprecision(1) << fp.ReserveMin << "," << fp.FinalTimeInf << "," << fp.FinalTimePre << "," << std::fixed << std::setprecision(1) << fp.FinalFixInf << std::endl;
        }
        outfile << std::endl;
    }

    outfile.close();
    return true;
}

// Renamed from paththerm_num_to_string to avoid potential linkage conflicts if Encoder.cpp also defines it globally.
// If these files are part of the same compilation unit or if Encoder.cpp's version is static/namespaced, original name is fine.
void paththerm_num_to_string_local(int pathterm_num, char * pathterm) { // Renamed to avoid conflict if linked with Encoder.o
	switch (pathterm_num)
	{
	    case 0: strncpy(pathterm, "AF", 3); break; // Use strncpy for safety
	    case 1: strncpy(pathterm, "CF", 3); break;
	    case 2: strncpy(pathterm, "DF", 3); break;
	    case 3: strncpy(pathterm, "FA", 3); break;
	    case 4: strncpy(pathterm, "IF", 3); break;
	    case 5: strncpy(pathterm, "FM", 3); break;
	    case 6: strncpy(pathterm, "HA", 3); break;
	    case 7: strncpy(pathterm, "HF", 3); break;
	    case 8: strncpy(pathterm, "HM", 3); break;
	    case 9: strncpy(pathterm, "PI", 3); break;
	    case 10: strncpy(pathterm, "TF", 3); break;
	    case 11: strncpy(pathterm, "CA", 3); break; // Simplified from original conditional in Encoder.cpp
	    case 12: strncpy(pathterm, "CD", 3); break; // Simplified
	    case 13: strncpy(pathterm, "CI", 3); break; // Simplified
	    case 14: strncpy(pathterm, "VM", 3); break;
	    case 15: strncpy(pathterm, "CR", 3); break; // Simplified
	    case 16: strncpy(pathterm, "RF", 3); break;
	    case 17: strncpy(pathterm, "FC", 3); break;
	    case 18: strncpy(pathterm, "FD", 3); break;
	    default: strncpy(pathterm, "  ", 3); break; // Ensure 2 spaces for unknown to maintain column alignment
	}
   pathterm[2] = '\0'; // Ensure null termination
}
