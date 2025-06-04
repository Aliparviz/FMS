#include "pch.h"
#include "csv_parser.h"
#include <sstream>
#include "dms.h"
#include "onemeg_db.h"
#include <iomanip>
#include <algorithm>

using namespace std;






/*
int __cdecl sub_96B570(int a1, int a2)
{
	int result; // eax

	result = a2;
	switch (a1)
	{
	case 0xFFFFFFFE:
		strcpy((char *)a2, "DESTIN");
		break;
	case 0xFFFFFFFF:
		strcpy((char *)a2, "ORIGIN");
		break;
	case 0:
		*(_DWORD *)a2 = &loc_444953;
		break;
	case 1:
		strcpy((char *)a2, "STAR");
		break;
	case 2:
		strcpy((char *)a2, "PROF DESC");
		break;
	case 3:
		strcpy((char *)a2, "APPR");
		break;
	case 4:
		*(_DWORD *)a2 = 1414747459;
		*(_DWORD *)(a2 + 4) = &loc_444953;
		break;
	case 5:
		*(_DWORD *)a2 = 1413761363;
		*(_DWORD *)(a2 + 4) = &loc_4E4152;
		break;
	case 6:
		strcpy((char *)a2, "STARTRAN");
		break;
	case 7:
		strcpy((char *)a2, "APPRTRAN");
		break;
	case 8:
		strcpy((char *)a2, "CRZALT");
		break;
	case 9:
		*(_DWORD *)a2 = 1346720851;
		*(_DWORD *)(a2 + 4) = (char *)&loc_544C3D + 4;
		break;
	case 0xA:
		strcpy((char *)a2, "STRNYTRN");
		break;
	case 0xB:
		strcpy((char *)a2, "INITFIX");
		break;
	case 0xC:
		strcpy((char *)a2, "CUSTSTAR");
		break;
	case 0xD:
		strcpy((char *)a2, "CUSTPRODES");
		break;
	case 0xE:
		strcpy((char *)a2, "AIRWAY");
		break;
	case 0xF:
		strcpy((char *)a2, "DIRECT");
		break;
	case 0x10:
		strcpy((char *)a2, "CUSTAPPR");
		break;
	case 0x11:
		strcpy((char *)a2, "CSTSIDTRAN");
		break;
	case 0x12:
		strcpy((char *)a2, "CSTSTRTRAN");
		break;
	case 0x13:
		strcpy((char *)a2, "CSTAPPTRAN");
		break;
	case 0x14:
		strcpy((char *)a2, "CUSTSTARRW");
		break;
	case 0x15:
		strcpy((char *)a2, "SIDRNYTRN");
		goto LABEL_2;
	default:
	LABEL_2:
		*(_BYTE *)a2 = 0;
		break;
	}
	return result;
}*/










int viatype_string_to_int(std::string viatype)
{
		if (!viatype.compare("SIDCR"))
			return 0;
		else if (!viatype.compare("SIDET"))
			return 1;
		else if (!viatype.compare("SIDRT"))
			return 2;
		else if (!viatype.compare("STARCR"))
			return 3;
		else if (!viatype.compare("STARET"))
			return 4;
		else if (!viatype.compare("STARRT"))
			return 5;
		else if (!viatype.compare("APP"))
			return 6;
		else if (!viatype.compare("APPTRN"))
			return 7;
		else if (!viatype.compare("PRF_DSC"))
			return 8;
		else if (!viatype.compare("EOSID"))
			return 9;
		else if (!viatype.compare("PRF_DSC_ENR_TRNS"))
			return 10;
		else if (!viatype.compare("PRF_DSC_RWY_TRNS"))
			return 11;
		else if (!viatype.compare("UNK"))
			return 12;
		else {
			std::cerr << "invalid viatype" << std::endl;
			throw(1001);
		}
}

int non_viatype_to_vianum(int viatype)
{
	switch (viatype)
	{
		case 0: return 0;//SIDCR
		case 3: return 1;//STARCR
		case 8: return 2;//PRF_DSC
		case 6: return 3;//APP
		case 1: return 4;
		case 4: return 7;// or return 4 ||| STARCR 
		case 2: return 5;//SIDRT
		case 7: return 5;//APPTRN
		case 9: return 6;//APPTRN
		//
		//case 5: return 9;//STARRT
		default: 
			std::cerr << "invalid viatype" << std::endl;
			throw(1001);		
	}
}



int viatype_to_int(std::string viatype, int &viastate)
{



	if (viastate == 0 && ((!viatype.compare("SIDCR")) || (!viatype.compare("SIDET")) || (!viatype.compare("SIDRT")) || (!viatype.compare("EOSID"))))
	{
		viastate = 1;
	}
	else if (viastate == 1 && ((!viatype.compare("STARCR")) || (!viatype.compare("PRF_DSC")) || (!viatype.compare("STARRT")) || (!viatype.compare("PRF_DSC"))))
	{
		viastate = 2;
	}
	else if (!viatype.compare("APP"))
		viastate = 3;

	int viad = 0;
	if (!viatype.compare("SIDCR"))//0
		viad = 0;
	else if (!viatype.compare("SIDET"))//1
	{
		if (viastate == 2)
		{
			std::cerr << "unhandled viatype" << std::endl;
			throw(1001);
		}
		viad = 4;//*if(viastate == 2) throw error
	}
	else if (!viatype.compare("SIDRT"))//2
	{
		if (viastate != 1)
		{
			std::cerr << "unhandled viatype" << std::endl;
			throw(1001);
		}
		viad = 5;//*if(viastate != 1) throw error
	}
		
	else if (!viatype.compare("STARCR"))//3
		viad = 1;
	else if (!viatype.compare("STARET"))//4
	{
		if (viastate == 2) 
			
			viad = 4;
		else 
			viad = 7;
	}
	else if (!viatype.compare("STARRT"))//5
		viad = 9;
	else if (!viatype.compare("APP"))//6
		viad = 3;
	else if (!viatype.compare("APPTRN"))//7
	{
		if (viastate != 2 && viastate != 1) 
			viad = 5;
		else 
			viad = 10;
	}
	else if (!viatype.compare("PRF_DSC"))//8
		viad = 2;
	else if (!viatype.compare("EOSID"))//9
		viad = 6;
	else if (!viatype.compare("PRF_DSC_ENR_TRNS"))//10
		viad = 8;
	else {
		std::cerr << "invalid viatype" << std::endl;
		throw(1001);
	}
	//viatype	| viatyperes
	//0		|	0											|		SIDCR
	//1		|	3											|		STARCR
	//2		|	8											|		PRF_DSC
	//3		|	6											|		APP
	//4		| (via_state != 2)? 1 : 4;						|		SIDET/STARET
	//5		| (via_state == 1)? 2 : (via_state != 2)? 7		|		SIDRT/APPTRN
	//6		|	9											|		EOSID
	//7		|	4											|		STARET
	//8		|	10											|		PRF_DSC_ENR_TRNS
	//9		|	5											|		STARRT
	//10	|	7											|		APPTRN
	//11	|	8											|		PRF_DSC


	//12,13,14,

	//cout << viatype << ", state: " << viastate << " viad: " << viad << endl;
	return viad;
}

int runway_char_to_index(char index)
{
	switch (index)
	{
		case ' ': return 0;
		case 'L': return 1;
		case 'R': return 2;
		case 'C': return 3;
		case 'B': return 4;
		case 'A': return 5;
		case 'O': return 6;
		case 'M': return 7;
		default: return 0;
	}
}

int turndir_char_to_index(char t)
{
	if (t == 'R')
		return 1;
	else if (t == 'L')
		return 2;
	else if (t == 'E')
		return 0;
	else
		return 3;
}

int paththerm_string_to_int(std::string pathterm) {
	if(!pathterm.compare("AF"))
		return 0;
	if (!pathterm.compare("CF"))
		return 1;
	if (!pathterm.compare("DF"))
		return 2;
	if (!pathterm.compare("FA"))
		return 3;
	if (!pathterm.compare("IF"))
		return 4;
	if (!pathterm.compare("FM"))
		return 5;
	if (!pathterm.compare("HA"))
		return 6;
	if (!pathterm.compare("HF"))
		return 7;
	if (!pathterm.compare("HM"))
		return 8;
	if (!pathterm.compare("PI"))
		return 9;
	if (!pathterm.compare("TF"))
		return 10;
	if (!pathterm.compare("VA") || !pathterm.compare("CA"))
		return 11;
	if (!pathterm.compare("VD") || !pathterm.compare("CD"))
		return 12;
	if (!pathterm.compare("VI") || !pathterm.compare("CI"))
		return 13;
	if (!pathterm.compare("VM"))
		return 14;
	if (!pathterm.compare("VR") || !pathterm.compare("CR"))
		return 15;
	if (!pathterm.compare("RF"))
		return 16;
	if (!pathterm.compare("FC"))
		return 17;
	if (!pathterm.compare("FD"))
		return 18;
	return 19;
}

int non_paththerm_string_to_int(std::string pathterm)
{

		if (!pathterm.compare("AF"))
			return 0; 
		else if (!pathterm.compare("CA") || !pathterm.compare("VA/CA"))
			return 11; //path_select = 1;
		else if (!pathterm.compare("CD") || !pathterm.compare("VD/CD"))
			return 12; //path_select = 1;
		else if (!pathterm.compare("CF"))
			return 1;
		else if (!pathterm.compare("CI") || !pathterm.compare("VI/CI"))
			return 13; //path_select = 1;
		else if (!pathterm.compare("CR"))
			return 15;
		else if (!pathterm.compare("DF"))
			return 2;
		else if (!pathterm.compare("FA"))
			return 3;
		else if (!pathterm.compare("FC"))
			return 17;
		else if (!pathterm.compare("FD"))
			return 18;
		else if (!pathterm.compare("FM"))
			return 5;
		else if (!pathterm.compare("HA"))
			return 6;
		else if (!pathterm.compare("HF"))
			return 7;
		else if (!pathterm.compare("HM"))
			return 8;
		else if (!pathterm.compare("IF"))
			return 4;
		else if (!pathterm.compare("PI"))
			return 9;
//		else if (!pathterm.compare("RF"))
		else if (!pathterm.compare("TF"))
			return 10;
		else if (!pathterm.compare("VA"))
			return 11; //path_select = 0;
		else if (!pathterm.compare("VD"))
			return 12; //path_select = 0;
		else if (!pathterm.compare("VI"))
			return 13; //path_select = 0;
		else if (!pathterm.compare("VM"))
			return 14;
		//else if (!pathterm.compare("VR"))
		//{
		//	std::cout << "VR" << std::endl;
		//}
		else {
			std::cerr << "unhandled patherm: " << pathterm << std::endl;
			throw(1001);
		}


}





int month_to_int(std::string month)
{

	if (!month.compare("JAN"))
		return 0;
	else if (!month.compare("FEB"))
		return 1;
	else if (!month.compare("MAR"))
		return 2;
	else if (!month.compare("APR"))
		return 3;
	else if (!month.compare("MAY"))
		return 4;
	else if (!month.compare("JUN"))
		return 5;
	else if (!month.compare("JUL"))
		return 6;
	else if (!month.compare("AUG"))
		return 7;
	else if (!month.compare("SEP"))
		return 8;
	else if (!month.compare("OCT"))
		return 9;
	else if (!month.compare("NOV"))
		return 10;
	else if (!month.compare("DEC"))
		return 11;
	else {
		std::cerr << "invalid month" << std::endl;
		throw(1001);

	}

}

int navaid_navclass_to_int(std::string navclass)
{
	if (!navclass.compare("VOR"))
		return 0;
	else if (!navclass.compare("DME"))
		return 1;
	else if (!navclass.compare("DMEPAIR"))
		return 2;
	else if (!navclass.compare("TACAN"))
		return 3;
	else if (!navclass.compare("TACANPAIR"))
		return 4;
	else if (!navclass.compare("LOC"))
		return 5;
	else if (!navclass.compare("ILS"))
		return 6;
	else if (!navclass.compare("MLS"))
		return 7;
	else if (!navclass.compare("ILSDME"))
		return 8;
	else if (!navclass.compare("VORTAC"))
		return 9;
	else if (!navclass.compare("VORDME"))
		return 10;
	else if (!navclass.compare("MLSDME"))
		return 13;
	else if (!navclass.compare("RES15"))
		return 15;
	else {
		std::cerr << "unsupprted navclass: " << navclass << std::endl;
		throw(1001);

	}

}

int non_navaid_navclass_to_int(std::string navclass)
{


	if (!navclass.compare("VOR"))
		return 0;
	else if (!navclass.compare("DME"))
		return 1;
	else if (!navclass.compare("DMEPAIR"))
		return 2;
	else if (!navclass.compare("TACAN"))
		return 3;
	else if (!navclass.compare("TACANPAIR"))
		return 4;
	else if (!navclass.compare("LOC"))
		return 5;
	else if (!navclass.compare("ILS"))
		return 6;
	else if (!navclass.compare("MLS"))
		return 7;
	else if (!navclass.compare("ILSDME"))
		return 8;
	else if (!navclass.compare("VORTAC"))
		return 9;
	else if (!navclass.compare("VORDME"))
		return 10;
	else if (!navclass.compare("CVORTAC"))
		return 11;
	else if (!navclass.compare("CVORDME"))
		return 12;
	else if (!navclass.compare("MLSDME"))
		return 13;
	else if (!navclass.compare("RES15"))
		return 15;
	else if (!navclass.compare("LOCDME"))
		return 17;
	else if (!navclass.compare("LOCTAC"))
		return 18;
	else if (!navclass.compare("LDADME"))
		return 19;
	else if (!navclass.compare("LDATAC"))
		return 20;
	else if (!navclass.compare("ILSTAC"))
		return 21;
	else if (!navclass.compare("NDB"))
		return 22;
	else if (!navclass.compare("NDB-DME"))
		return 23;
	else if (!navclass.compare("LMLSCAT1"))
		return 24;
	else if (!navclass.compare("LMLSCAT2"))
		return 25;
	else if (!navclass.compare("LMLSCAT3"))
		return 26;
	else if (!navclass.compare("LDAIGS"))
		return 27;
	else if (!navclass.compare("LDANIGS"))
		return 28;
	else if (!navclass.compare("SDFIGS"))
		return 29;
	else if (!navclass.compare("SDFNIGS"))
		return 30;
	else if (!navclass.compare("IGS"))
		return 31;
	else {
		std::cerr << "unsupprted navclass: " << navclass << std::endl;
		throw(1001);

	}

}

void extract_alt_info(std::string alt, int * alt_char, int * alt_num, bool is_non)
{
	bool last_a, last_b, first_fl;

	if (alt.substr(alt.length() - 1).c_str()[0] == 'A')
		last_a = true;
	else
		last_a = false;

	if (alt.substr(alt.length() - 1).c_str()[0] == 'B')
		last_b = true;
	else
		last_b = false;

	if (alt.substr(0, 1).c_str()[0] == 'F' && alt.substr(1, 1).c_str()[0] == 'L')
		first_fl = true;
	else
		first_fl = false;
	if (last_b && !first_fl)
	{
		if(is_non)
			*alt_char = 8;
		else
			*alt_char = 0;
		*alt_num = std::stoi(alt.substr(0, alt.length() - 1)) / 10;
	}
	if (last_a && !first_fl)
	{
		if (is_non)
			*alt_char = 9;
		else
			*alt_char = 1;
		*alt_num = std::stoi(alt.substr(0, alt.length() - 1)) / 10;
	}
	if (last_b && first_fl)
	{
		if (is_non)
			*alt_char = 10;
		else
			*alt_char = 2;
		*alt_num = std::stoi(alt.substr(2, alt.length() - 3));
	}
	if (last_a && first_fl)
	{
		if (is_non)
			*alt_char = 11;
		else
			*alt_char = 3;
		*alt_num = std::stoi(alt.substr(2, alt.length() - 3));
	}
	if (first_fl && !last_a && !last_b)
	{
		if (is_non)
			*alt_char = 13;
		else
			*alt_char = 5;
		*alt_num = std::stoi(alt.substr(2, alt.length() - 2));
	}

	if (!first_fl && !last_a && !last_b)
	{
		if (is_non)
			*alt_char = 12;
		else
			*alt_char = 4;
		*alt_num = std::stoi(alt) / 10;
	}

}

int  navaid_figureofmerit_to_string(std::string figureofmerit)
{
	if (!figureofmerit.compare("T"))
		return 0;
	else if (!figureofmerit.compare("L"))
		return 1;
	else if (!figureofmerit.compare("UNC"))
		return 2;
	else if (!figureofmerit.compare("H"))
		return 3;
	else if (!figureofmerit.compare("U"))
		return 4;
	else if (!figureofmerit.compare("TU"))
		return 5;
	else if (!figureofmerit.compare("LU"))
		return 6;
	else if (!figureofmerit.compare("HU"))
		return 7;
	else {
		std::cerr << "invalid FigureofMerit: " << figureofmerit << std::endl;
		throw(1001);
	}
}

int  non_navaid_figureofmerit_to_string(std::string figureofmerit)
{
	if (!figureofmerit.compare("T"))
		return 0;
	else if (!figureofmerit.compare("L"))
		return 1;
	else if (!figureofmerit.compare("UNC"))
		return 2;
	else if (!figureofmerit.compare("H"))
		return 3;
	else if (!figureofmerit.compare("U"))
		return 4;
	else if (!figureofmerit.compare("TU"))
		return 5;
	else if (!figureofmerit.compare("LU"))
		return 6;
	else if (!figureofmerit.compare("HU"))
		return 7;
	else if (!figureofmerit.compare("O"))
		return 9;
	else {
		std::cerr << "invalid FigureofMerit: " << figureofmerit << std::endl;
		throw(1001);
	}
}

int waypoint_tenroute_to_int(std::string ten)
{

	if (!ten.compare(" "))
		return 0;
	else if (!ten.compare("E"))
		return 1;
	else if (!ten.compare("T"))
		return 2;
	else if (!ten.compare("B"))
		return 3;
	else {
		std::cerr << "invalid TerminalEnroute" << std::endl;
		throw(1001);
	}

}

int revcode_to_int(std::string revcode)
{
	if (revcode.length() != 1) {
		cerr << "revcode must be single character " << endl;
		throw (1001);
	}
	const char *c = revcode.c_str();
	switch (c[0])
	{
	case 'B': return 3;
	case '1': return 1;
	case '2': return 2;
	default:
		cerr << "valid revcodes are: 'B' , '1', '2' " << endl;
		throw (1001);

	}
}

csv_parser::csv_parser(const char * csv_filename, hon_time prev_start, hon_time prev_end, std::string tag, std::string result_filename, bool is_non)
{
	file.open(csv_filename);
	string line;
	if (is_non)
		this->num_header_lines = 33;
	else
		this->num_header_lines = 40;
	if (!file.is_open())
	{
		cerr << "Unable to open file!" << endl;
		throw (401);
	}

	for (int i = 0; i < this->num_header_lines; i++)
	{
		if (!getline(file, line))
		{
			cerr << "failed to read summary and header lines!" << endl;
			throw (401);
		}
		header_lines.push_back(line);
	}
	//for(int i=0; i<40; i++)
	//	cout << header_lines[i] << endl;

	dinf.database_name = header_lines[1].substr(10);
	dinf.sequence_name = header_lines[2].substr(10);
	dinf.database_filename = string("       0202HONEYWELL           ") + dinf.database_name + string(".BIN");//used only in extonemeg
	dinf.tag = tag;
	dinf.result_filename = result_filename;


	/*
	if ((dinf.database_name.length() < 12 && !is_non) || dinf.sequence_name.length() < 12 && !is_non) {
		cerr << "database and sequence name must be 12 character but are " << dinf.database_name.length() << ", " <<  dinf.sequence_name.length() << endl;
		throw (402);
	}
	*/



	dinf.prev_start = prev_start;
	dinf.prev_end = prev_end;
	/*
	dinf.prev_start.day = std::stoi(std::string("08"));
	dinf.prev_start.mon = month_to_int(std::string("DEC"));
	dinf.prev_start.year = std::stoi(std::string("2016")) - 85 - 1900;
	*/

	dinf.cur_start.day = std::stoi(header_lines[4].substr(24, 2));
	dinf.cur_start.mon = month_to_int(header_lines[4].substr(27, 3));
	dinf.cur_start.year = std::stoi(header_lines[4].substr(31, 4)) - 85 - 1900;

	dinf.cur_end.day = std::stoi(header_lines[4].substr(39, 2));
	dinf.cur_end.mon = month_to_int(header_lines[4].substr(42, 3));
	dinf.cur_end.year = std::stoi(header_lines[4].substr(46, 4)) - 85 - 1900;



	int count = 0;

	
	while (getline(file, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		lines.push_back(line);
		count++;
		if (count == 2) {
			table_locs.push_back(1);
		}
		else {
			if (line.find(std::string("<<< Record Count")) != std::string::npos) {
				table_locs.push_back(count);
				 //cout << count << endl;
			}
		}
	}


	/*
	for (int i = 0; i < table_locs.size() - 1; i++)
	{
		cout << lines[table_locs[i]] <<  endl;
	}
	*/



	if (is_non) {
		non_db = new nononemeg_db(dinf);

		parse_airports(true);
		parse_navaids(true);
		parse_waypoints(true);
		parse_runways(true);
		parse_ndbs(true);
		parse_airways(true);
		parse_fuelpolicies(true);
		parse_procedures(true);

		non_db->create_tables();
		non_db->write_file();
	}
	else {

		db = new extonemeg_db(dinf);

		parse_airports(false);
		parse_airways(false);
		parse_gridmoras();
		parse_holdingpatterns();
		parse_navaids(false);
		parse_ndbs(false);
		parse_procedures(false);
		parse_runways(false);
		parse_waypoints(false);
		parse_fuelpolicies(false);




		db->create_navaid();
		db->create_waypoint();
		db->create_airway();
		db->create_ndb();
		db->create_runway();
		db->create_airport();
		db->create_procedure();
		db->create_gridmora();
		db->create_holdingpattern();
		db->create_fuel();
		db->create_custom_procedure();
		db->aggregate_data();
		db->write_file();
	}
	

}

void  csv_parser::find_table(std::string name, int * start, int *end)
{
	for (int i = 0; i < table_locs.size() - 1; i++) {
		//cout << table_locs[i] << endl;
		if (lines[table_locs[i]].substr(0, name.length()).compare(name) == 0)
		{
			*start = table_locs[i] + 3;
			*end = table_locs[i+1] - 2;
			return;
		}
	}
}

void csv_parser::parse_airports(bool is_non)
{
	int start =0, end = 0;
	int rloc;
	bool runway_found;
	find_table("Airports", &start, &end);
	if( start != 0)
		cout << "found " << end - start +1 << " rows in Airports (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	airport air;
	int record_index = 0;
	int offset1 = (is_non) ? 1 : 0;


	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		air.Ident = token_lines[0];
		air.RevCode = revcode_to_int(token_lines[1]);
		air.Latitude = DMS::DecimalDegrees(token_lines[3 - offset1]);
		air.Longitude = DMS::DecimalDegrees(token_lines[4 - offset1]);
		if (token_lines[5 - offset1].length() > 0)
		{
			if (token_lines[5 - offset1].c_str()[0] == 'W')
				air.MagVar_Neg = true;
			else
				air.MagVar_Neg = false;

			try {
				air.MagVar = DMS::Declanation(token_lines[5 - offset1]);
			}
			catch (...) {
				air.MagVar = 0;
			}
		}
		else {
			air.MagVar_Neg = false;
			air.MagVar = 0;
		}




		try {
			air.Elevation = std::stoi(token_lines[6 - offset1]);
		}
		catch (...) {
			air.Elevation = 0;
		}

		if (!token_lines[7 - offset1].compare("Y")) {
			air.SpeedLimitCoded = true;
		}
		else {
			air.SpeedLimitCoded = false;
		}

		if (air.SpeedLimitCoded) {
			air.SpeedLimit = std::stoi(token_lines[8 - offset1]);
			air.SpeedLimitAltitude = std::stoi(token_lines[9 - offset1]);
		}
		else {
			air.SpeedLimit = 0;
			air.SpeedLimitAltitude = 0;
		}

		try {
			air.TransAltitude = std::stoi(token_lines[10 - offset1]);
			air.TransAltValid = true;
		}
		catch (...) {
			air.TransAltitude = 0;
			air.TransAltValid = false;
		}

		if (!token_lines[11 - offset1].compare("Y")) {
			air.ProceduresExist = true;
		}
		else {
			air.ProceduresExist = false;
		}

		if (!token_lines[12 - offset1].compare("Y")) {
			air.RunwaysExist = true;
		}
		else {
			air.RunwaysExist = false;
		}

		if (!token_lines[13 - offset1].compare("Y")) {
			air.GatesExist = true;
		}
		else {
			air.GatesExist = false;
		}


		if (is_non) {
			if (!token_lines[14 - offset1].compare("Y")) {
				air.GatesExist = true;
			}
			else {
				air.GatesExist = false;
			}
		}

		try {
			air.MaxRunwayLength = std::stoi(token_lines[15]);
		}
		catch (...) {
			air.MaxRunwayLength = 0;
		}

		if (!token_lines[16].compare("Y")) {
			air.SidExist = true;
		}
		else {
			air.SidExist = false;
		}

		if (!token_lines[17].compare("Y")) {
			air.StarExist = true;
		}
		else {
			air.StarExist = false;
		}

		if (!token_lines[18].compare("Y")) {
			air.ApproachExist = true;
		}
		else {
			air.ApproachExist = false;
		}

		if (!token_lines[19].compare("Y")) {
			air.EosidExist = true;
		}
		else {
			air.EosidExist = false;
		}


		air.recordIndex = record_index;
		record_index++;

		if (is_non)
		{
			non_db->airports.push_back(air);
			non_db->airports_map.insert({ air.Ident, air });
		}
		else {
			db->airports.push_back(air);
			db->airports_map.insert({ air.Ident, air });
		}
		
	}

}

void csv_parser::parse_airways(bool is_non)
{
	int start = 0, end = 0;
	find_table("AirwayFixes", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in Airways (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	airway air;
	airway_fix fix;
	std::string last_ident("first");
	int last_revcode = -1;
	int rawIndex = 0;

	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);



		std::string ident = token_lines[0];
		int revcode = revcode_to_int(token_lines[1]);
		if ((last_ident.compare(ident) || revcode != last_revcode) && (rawIndex != 0))
		{
			air.Ident = last_ident;
			air.revcode = last_revcode;
			//std::cout << " new airway " << air.Ident << ", len: " << air.fixes.size() << std::endl;
			if(is_non)
				non_db->airways.push_back(air);
			else
				db->airways.push_back(air);
			air.fixes.clear();		
		}
		last_ident = ident;
		last_revcode = revcode;

		fix.fix_ident = token_lines[2];
		std::string type = token_lines[3];
		if (fix.fix_ident.length() >= 5 && !fix.fix_ident.substr(0, 5).compare("*DIS*")) {
			fix.fix_type = 14;
		}
		else if (fix.fix_ident.length() >= 5 && !fix.fix_ident.substr(0, 5).compare("*EOA*")) {
			fix.fix_type = 15;
		}
		else if (!type.compare("NAV") || !type.compare("VOR")) {
			fix.fix_type = 3;
		}
		else if (!type.compare("WPT")) {
			fix.fix_type = 1;
		}
		else if (!type.compare("NDB")) {
			fix.fix_type = 4;
		}
		else {
			cerr << "unsupprted airway fix type(" << rawIndex << "): " << type << endl;
			throw (1001);
		}

		if (fix.fix_type == 1 || fix.fix_type == 3 || fix.fix_type == 4) {
			//try {
				fix.fix_Latitude = DMS::DecimalDegrees(token_lines[4]);
				fix.fix_Longitude = DMS::DecimalDegrees(token_lines[5]);
			//}
			//catch (...) {
			//	std::cout << " fix.fix_ident : " << fix.fix_ident << ", lat " << token_lines[4] << ", lon: " << token_lines[5];
			//}
		}
		air.fixes.push_back(fix);
		rawIndex++;
	

	}

	air.Ident = last_ident;
	air.revcode = last_revcode;
	//std::cout << " new airway " << air.Ident << ", len: " << air.fixes.size();
	if(is_non)
		non_db->airways.push_back(air);
	else
		db->airways.push_back(air);
	air.fixes.clear();

}

void csv_parser::parse_gridmoras()
{
	int start = 0, end = 0;
	find_table("GridMORA", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in GridMORAs (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	gridmora grid;
	int record_index = 0;

	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		grid.RevCode = revcode_to_int(token_lines[0]);
		grid.Latitude = DMS::DecimalDegrees(token_lines[1]);
		grid.Longitude = DMS::DecimalDegrees(token_lines[2]);
		for (int i = 0; i < 30; i++)
		{
			grid.Altitude[i] = std::stoi(token_lines[3+i]) / 100;
		}
	
		grid.recordIndex = record_index;
		record_index++;
		db->gridmoras.push_back(grid);
	}
}

void csv_parser::parse_holdingpatterns()
{
	int start = 0, end = 0;
	find_table("HoldingPatterns", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in HoldingPatterns (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	holdingpattern hop;
	int record_index = 0;
	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		hop.FilxIdent = token_lines[0];
		hop.FixTypeString = token_lines[1];
		hop.RevCode = revcode_to_int(token_lines[2]);

		if (!hop.FixTypeString.compare("NAV") || !hop.FixTypeString.compare("VOR")) {
			hop.FixType = 3;
		}
		else if (!hop.FixTypeString.compare("WPT")) {
			hop.FixType = 1;
		}
		else if (!hop.FixTypeString.compare("NDB")) {
			hop.FixType = 4;
		}
		else {
			cerr << "unsupprted holdingpattern fix type(" << record_index << "): " << hop.FixTypeString << endl;
			throw (1001);
		}

		hop.Latitude = DMS::DecimalDegrees(token_lines[3]);
		hop.Longitude = DMS::DecimalDegrees(token_lines[4]);

		

		hop.AltitudeLabel = std::stoi(token_lines[5]);
		hop.Altitude_exist = false;

		std::string alt_str = token_lines[6];

		alt_str.erase(std::remove_if(alt_str.begin(), alt_str.end(), isWhitespace()), alt_str.end());
		if (alt_str.length() > 0)
		{ 
			if (alt_str.substr(alt_str.length() - 1).c_str()[0] == 'A')
				alt_str = alt_str.substr(0, alt_str.length() - 1);

			if (alt_str.substr(alt_str.length() - 1).c_str()[0] == 'B')
				alt_str = alt_str.substr(0, alt_str.length() - 1);

			if (alt_str.substr(0, 1).c_str()[0] == 'F' && alt_str.substr(1, 1).c_str()[0] == 'L')
			{
				alt_str = alt_str.substr(2, alt_str.length() - 2);
			}
		
			hop.Altitude_exist = true;
			hop.Altitude = std::stoi(alt_str);
		}

		hop.InBndCourse = std::stod(token_lines[7]);



		token_lines[8].erase(std::remove_if(token_lines[8].begin(), token_lines[8].end(), isWhitespace()), token_lines[8].end());
		token_lines[9].erase(std::remove_if(token_lines[9].begin(), token_lines[9].end(), isWhitespace()), token_lines[9].end());
		if (token_lines[8].length() > 0) {
			hop.LegTime = std::stoi(token_lines[8].substr(0, token_lines[8].length()-1));
			hop.time_length_used = false;
		}
		else {
			if (token_lines[9].length() > 0)
			{
				hop.LegLength = std::stoi(token_lines[9]);
				hop.time_length_used = true;

			}
			else {
				cerr << "unsupprted holdingpattern leg time\leg length" << endl;
				throw (1001);
			}
		}

		hop.TurnDirString = token_lines[11];
		if (hop.TurnDirString.c_str()[0] == 'L')
			hop.TurnDir = 1;
		else
			hop.TurnDir = 0;
		hop.HoldTypeString = token_lines[12];;
		
		record_index++;

		hop.recordIndex = record_index;
		db->holdingpatterns.push_back(hop);

	}

}

void csv_parser::parse_navaids(bool is_non)
{
	int start = 0, end = 0;
	find_table("Navaids", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in Navaids (" << start << "|" << end << ")" << endl;
	navaid nav;
	int record_index = 0;

	if ((start == 0) && (end == 0)) {
		return;
	}


	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);
		
		nav.Ident = token_lines[0];
		nav.RevCode = revcode_to_int(token_lines[1]);
		nav.Freq = std::stod(token_lines[2]);
		nav.NavClassString = token_lines[3];
		if (is_non)
		{
			nav.NavClass = non_navaid_navclass_to_int(nav.NavClassString);
		}
		else {
			nav.NavClass = navaid_navclass_to_int(nav.NavClassString);
		}
		nav.Latitude = DMS::DecimalDegrees(token_lines[5]);
		nav.Longitude = DMS::DecimalDegrees(token_lines[6]);
		nav.StationDeclination = DMS::Declanation(token_lines[7]);
		
		try {
			nav.Elevation = std::stoi(token_lines[8]);
		}
		catch(...){
			nav.Elevation = 0;
		}
		
		nav.FigureOfMeritString = token_lines[9];
		nav.FigureOfMerit = navaid_figureofmerit_to_string(nav.FigureOfMeritString);
		//TODO:isCustomized, Bearing, Glidescope



		//cout << nav.Ident << ", " << nav.RevCode << ", " << nav.NavClassString << ", "  << nav.Freq << ", " << nav.Latitude  << ", " << nav.Longitude << ", " << nav.StationDeclination << ", " << nav.Elevation << ", " << nav.FigureOfMeritString << endl;


		nav.recordIndex = record_index;
		record_index++;


		/*
		if (db->navaids.find(nav.Ident) != db->navaids.end())
		{
			cerr << " duplicate navaid ident" << endl;
			throw (1001);
		}
		*/
		if (is_non)
		{
			non_db->navaids.push_back(nav);
			non_db->navaids_map.insert({ nav.Ident, nav });
		}
		else {
			db->navaids.push_back(nav);
			db->navaids_map.insert({ nav.Ident, nav });
		}
		
	}

}

void csv_parser::parse_ndbs(bool is_non)
{
	int start = 0, end = 0;
	find_table("NDBs", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in NDBs (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}


	ndb ndb;
	int record_index = 0;


	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);




		ndb.Ident = token_lines[0];
		ndb.RevCode = revcode_to_int(token_lines[1]);

		/*
		if (!ndb.Ident.compare("ZNS")) {
			std::cout << "wait Mf" << std::endl;
		}
		*/
		ndb.Latitude = DMS::DecimalDegrees(token_lines[2]);
		ndb.Longitude = DMS::DecimalDegrees(token_lines[3]);
		ndb.Frequency = std::stod(token_lines[4]);

		ndb.recordIndex = record_index;
		record_index++;
		if (is_non)
		{
			non_db->ndbs.push_back(ndb);
			non_db->ndbs_map.insert({ ndb.Ident, ndb });
		}
		else {
			db->ndbs.push_back(ndb);
			db->ndbs_map.insert({ ndb.Ident, ndb });
		}
	}
}

void csv_parser::parse_procedures(bool is_non)
{
	int start = 0, end = 0;
	std::vector<std::string> proc_lines;
	std::vector<std::string> custom_proc_lines;

	find_table("Procedures", &start, &end);
	if (start != 0)
		cout << "found " << end - start + 1 << " rows in Procedures (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	for (int i = start; i <= end; i++)
	{
		
		


		std::istringstream iss(lines[i]);
		std::string token_line;
		std::getline(iss, token_line, ',');
		std::getline(iss, token_line, ',');
		std::getline(iss, token_line, ',');
		std::getline(iss, token_line, ',');
		if (!token_line.compare("Y")) {
			custom_proc_lines.push_back(lines[i]);
		}
		else {
			proc_lines.push_back(lines[i]);
		}
	
	}
	this->parse_procs(proc_lines, false, is_non);
	this->parse_procs(custom_proc_lines, true, is_non);

}

void csv_parser::parse_procs(std::vector<std::string> &lines, bool isCustom, bool is_non)
{

	procedure_raw proc_raw;
	std::vector <procedure_raw> procedure_raws;
	int record_index = 0;
	int viaState = 0;
	int revState;


	std::string last_airport("first");
	std::string last_ident("first");
	std::string last_runway("first");
	std::string last_via("first");
	int last_revcode = 5;
	std::vector <int> ident_lens;
	std::vector <int> ident_indexes;
	int ident_count = 0;
	int debug_i = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		proc_raw.proc_header.Airport = token_lines[0];
		proc_raw.proc_header.Ident = token_lines[1];
		proc_raw.proc_header.RevCode = revcode_to_int(token_lines[2]);
		proc_raw.proc_header.RunwayString = token_lines[5];
		proc_raw.proc_header.ViaTypeString = token_lines[4];
		proc_raw.proc_header.last_proc = false;
		proc_raw.proc_header.first_proc = false;

		//if (debug_i >= 168316 && debug_i < 168335) {
		//std::cout << i << ": " << proc_raw.proc_header.Airport << ", " << proc_raw.proc_header.Ident << std::endl;
		//}



		if (proc_raw.proc_header.Airport.compare(last_airport))
		{

			viaState = 0;
			proc_raw.proc_header.first_proc = true;
			if (record_index != 0) {
				procedure_raws[record_index - 1].proc_header.last_proc = true;
			}

			//cout << "*****************************************************************new Airport: " << proc_raw.proc_header.Airport << endl;
		}
		//cout << "Ident: " << proc_raw.proc_header.Ident << endl;

		if (proc_raw.proc_header.Ident.compare(last_ident) || proc_raw.proc_header.RunwayString.compare(last_runway) ||
			proc_raw.proc_header.ViaTypeString.compare(last_via) || proc_raw.proc_header.RevCode != last_revcode ||
			proc_raw.proc_header.Airport.compare(last_airport))
		{
			ident_indexes.push_back(record_index);
			if (record_index != 0)
				ident_lens.push_back(ident_count);
			ident_count = 0;
		}

		last_airport = proc_raw.proc_header.Airport;
		last_ident = proc_raw.proc_header.Ident;
		last_runway = proc_raw.proc_header.RunwayString;
		last_revcode = proc_raw.proc_header.RevCode;
		last_via = proc_raw.proc_header.ViaTypeString;


		proc_raw.proc_header.ViaType = viatype_to_int(proc_raw.proc_header.ViaTypeString, viaState);

		if (!proc_raw.proc_header.RunwayString.compare("ALL"))
		{
			proc_raw.proc_header.runway_num = 0;
			proc_raw.proc_header.runway_char = 5;
		}
		else if (!proc_raw.proc_header.RunwayString.compare(" "))
		{
			proc_raw.proc_header.runway_num = 0;
			proc_raw.proc_header.runway_char = 0;
		}
		else {
			if ((proc_raw.proc_header.RunwayString.length() == 2) || (proc_raw.proc_header.RunwayString.length() == 1))
			{
				proc_raw.proc_header.runway_num = std::stoi(proc_raw.proc_header.RunwayString);
				proc_raw.proc_header.runway_char = 0;
			}
			else {
				proc_raw.proc_header.runway_num = std::stoi(proc_raw.proc_header.RunwayString.substr(0, 2));
				proc_raw.proc_header.runway_char = runway_char_to_index(proc_raw.proc_header.RunwayString.substr(2, 1).c_str()[0]);
			}
		}

		if (token_lines[6].length() > 0)
		{
			proc_raw.proc_header.transalt_Exist = true;
			proc_raw.proc_header.transAlt = std::stoi(token_lines[6]);
		}
		else {
			proc_raw.proc_header.transalt_Exist = false;
		}

		//
		//end of header
		//
		if(is_non)
			proc_raw.fix.TurnDirString = token_lines[9];
		else
			proc_raw.fix.TurnDirString = token_lines[11];
		proc_raw.fix.turn_dir = turndir_char_to_index(proc_raw.fix.TurnDirString.substr(0, 1).c_str()[0]);


		if (is_non)
		{
			proc_raw.fix.pathterm = token_lines[10];
			proc_raw.fix.pathterm_num = non_paththerm_string_to_int(proc_raw.fix.pathterm);
		}
		else
		{
			proc_raw.fix.pathterm = token_lines[12];
			proc_raw.fix.pathterm_num = paththerm_string_to_int(proc_raw.fix.pathterm);
		}

			

		//if (!proc_raw.proc_header.Airport.compare("ANYN") && !proc_raw.proc_header.Ident.compare("NDB12")) {
		//	printf("salam\r\n");
		//}


		if (!token_lines[8].compare("RWY"))
		{
			if(is_non)
				proc_raw.fix.fix_type = 2;
			else
				proc_raw.fix.fix_type = 5;
		}
		else if (!token_lines[8].compare("NAV") || !token_lines[8].compare("VOR"))
		{
			proc_raw.fix.fix_type = 3;
		}
		else if (!token_lines[8].compare("WPT"))
		{
			proc_raw.fix.fix_type = 1;
		}
		else if (!token_lines[8].compare("NDB"))
		{
			proc_raw.fix.fix_type = 4;
		}
		else if (!token_lines[8].compare("APT") && is_non)
		{
			proc_raw.fix.fix_type = 6;
		}
		else {
			proc_raw.fix.fix_type = 0;
		}
		if (proc_raw.fix.fix_type)
		{
			proc_raw.fix.fix_ident = token_lines[7];
			try {

				if (is_non)
				{
					proc_raw.fix.fix_Latitude = DMS::DecimalDegrees(token_lines[26]);
					proc_raw.fix.fix_Longitude = DMS::DecimalDegrees(token_lines[27]);
				}
				else {
					proc_raw.fix.fix_Latitude = DMS::DecimalDegrees(token_lines[9]);
					proc_raw.fix.fix_Longitude = DMS::DecimalDegrees(token_lines[10]);
				}
			}
			catch (...) {
				proc_raw.fix.fix_type = 0;
				std::cout << token_lines[0] << ", " << token_lines[1] << ", " << token_lines[9] << ", " << token_lines[10] << ", " << std::endl;
			}
		}

		if (is_non)
		{
			if (!token_lines[12].compare("Y"))
				proc_raw.fix.OverFly = true;
			else
				proc_raw.fix.OverFly = false;

			if (!token_lines[11].compare("Y"))
				proc_raw.fix.MissedApproach = true;
			else
				proc_raw.fix.MissedApproach = false;
		}
		else {
			if (!token_lines[13].compare("Y"))
				proc_raw.fix.OverFly = true;
			else
				proc_raw.fix.OverFly = false;

			if (!token_lines[14].compare("Y"))
				proc_raw.fix.MissedApproach = true;
			else
				proc_raw.fix.MissedApproach = false;
		}

		if (!is_non)
		{
			token_lines[28].erase(std::remove_if(token_lines[28].begin(), token_lines[28].end(), isWhitespace()), token_lines[28].end());
			if (token_lines[28].length() > 0) {
				proc_raw.fix.RNP = std::stod(token_lines[28]);
			}
			else {
				proc_raw.fix.RNP = 0;
			}
		}



		//
		//end of satr header
		//


		std::string cs_hd;
		if(is_non)
			cs_hd = token_lines[13];
		else
			cs_hd = token_lines[15];
		if (cs_hd.length() > 0)
		{
			proc_raw.fix.cs_hd = std::stod(cs_hd);
			proc_raw.fix.cs_hd_exist = true;
		}
		else {
			proc_raw.fix.cs_hd_exist = false;
		}

		std::string alt1;
		if (is_non)
			alt1 = token_lines[15];
		else
			alt1 = token_lines[17];


		if (alt1.length() > 0)
		{
			proc_raw.fix.alt1_exist = true;


			extract_alt_info(alt1, &proc_raw.fix.alt1_char, &proc_raw.fix.alt1_num, is_non);


		}
		else {
			proc_raw.fix.alt1_exist = false;
		}


		std::string alt2;
		if (is_non)
			alt2 = token_lines[17];
		else
			alt2 = token_lines[19];


		if (alt2.length() > 0) {
			proc_raw.fix.alt2_exist = true;
			extract_alt_info(alt2, &proc_raw.fix.alt2_char, &proc_raw.fix.alt2_num, is_non);

		}
		else {
			proc_raw.fix.alt2_exist = false;
		}

		std::string dist;
		if (is_non)
			dist = token_lines[18];
		else
			dist = token_lines[20];


		if (dist.length() > 1)
		{
			proc_raw.fix.dist_exist = true;
			proc_raw.fix.dist = std::stod(dist) * 10;
		}
		else {
			proc_raw.fix.dist_exist = false;
		}


		std::string nav, navlat, navlon;
		if (is_non)
		{
			nav = token_lines[20];
			navlat = token_lines[28];
			navlon = token_lines[29];
		}
		else
		{
			nav = token_lines[22];
			navlat = token_lines[31];
			navlon = token_lines[32];
		}


		nav.erase(std::remove_if(nav.begin(), nav.end(), isWhitespace()), nav.end());
		if (nav.length() > 0) {
			proc_raw.fix.nav_exist = true;
			proc_raw.fix.nav_ident = nav;
			try {
				proc_raw.fix.nav_Latitude = DMS::DecimalDegrees(navlat);
				proc_raw.fix.nav_Longitude = DMS::DecimalDegrees(navlon);
			}
			catch (...) {
				proc_raw.fix.nav_exist = false;
			}
		}
		else {
			proc_raw.fix.nav_exist = false;
		}

		std::string ndb, ndblat, ndblon;
		if (is_non)
		{
			ndb = token_lines[21];
			ndb.erase(std::remove_if(ndb.begin(), ndb.end(), isWhitespace()), ndb.end());
			
			if (ndb.length() > 0)
			{
				
				ndblat = token_lines[34];
				ndblon = token_lines[35];

				//std::cout << "debug: " << ndb << ", " << ndblat << ", " << ndblon << std::endl;
			}
		}
		else
		{
			ndb = token_lines[23];
			ndb.erase(std::remove_if(ndb.begin(), ndb.end(), isWhitespace()), ndb.end());
			ndblat = token_lines[39];
			ndblon = token_lines[40];
		}



		
		if (ndb.length() > 0) {
			proc_raw.fix.ndb_exist = true;
			proc_raw.fix.ndb_ident = ndb;
			try {
				proc_raw.fix.ndb_Latitude = DMS::DecimalDegrees(ndblat);
				proc_raw.fix.ndb_Longitude = DMS::DecimalDegrees(ndblon);
			}
			catch (...) {
				proc_raw.fix.ndb_exist = false;
			}
		}
		else {
			proc_raw.fix.ndb_exist = false;
		}




		std::string radial;
		if (is_non)
			radial = token_lines[19];
		else
			radial = token_lines[21];
		
		radial.erase(std::remove_if(radial.begin(), radial.end(), isWhitespace()), radial.end());
		if (radial.length() > 0) {
			proc_raw.fix.Radial = std::stod(radial);
			proc_raw.fix.radial_exist = true;
		}
		else {
			proc_raw.fix.Radial = 0;
			proc_raw.fix.radial_exist = false;
		}





		if (!is_non)
		{
			token_lines[29].erase(std::remove_if(token_lines[29].begin(), token_lines[29].end(), isWhitespace()), token_lines[29].end());
			if (token_lines[29].length() > 1) {
				proc_raw.fix.arc_exist = true;
				proc_raw.fix.arc_ident = token_lines[29];
				try {
					proc_raw.fix.arc_Latitude = DMS::DecimalDegrees(token_lines[35]);
					proc_raw.fix.arc_Longitude = DMS::DecimalDegrees(token_lines[36]);
				}
				catch (...) {
					proc_raw.fix.arc_exist = false;
				}
			}
			else {
				proc_raw.fix.arc_exist = false;
			}
		}



		std::string fpa;
		if (is_non)
			fpa = token_lines[22];
		else
			fpa = token_lines[24];



		fpa.erase(std::remove_if(fpa.begin(), fpa.end(), isWhitespace()), fpa.end());
		if (fpa.length() > 0)
		{
			proc_raw.fix.fpa_exist = true;
			proc_raw.fix.FPA = std::stod(fpa);
		}
		else {
			proc_raw.fix.fpa_exist = false;
		}


		std::string cas;
		if (is_non)
			cas = token_lines[23];
		else
			cas = token_lines[25];


		cas.erase(std::remove_if(cas.begin(), cas.end(), isWhitespace()), cas.end());
		if (cas.length() > 0)
		{
			proc_raw.fix.cas_exist = true;
			proc_raw.fix.CAS = std::stoi(cas);
		}
		else {
			proc_raw.fix.cas_exist = false;
		}

		if (!is_non)
		{
			token_lines[26].erase(std::remove_if(token_lines[26].begin(), token_lines[26].end(), isWhitespace()), token_lines[26].end());
			if (token_lines[26].length() > 0)
			{
				proc_raw.fix.gcsa_exist = true;
				proc_raw.fix.GSCA = std::stod(token_lines[26]);
			}
			else {
				proc_raw.fix.gcsa_exist = false;
			}
		}


		std::string time;
		if (is_non)
			time = token_lines[25];
		else
			time = token_lines[30];


		time.erase(std::remove_if(time.begin(), time.end(), isWhitespace()), time.end());
		if (time.length() > 1)
		{
			proc_raw.fix.time_exist = true;
			proc_raw.fix.time = std::stod(time);
		}
		else {
			proc_raw.fix.time_exist = false;
		}


		debug_i++;
		ident_count++;
		record_index++;
		procedure_raws.push_back(proc_raw);
	}
	if (!record_index)
		return;
	procedure_raws[record_index - 1].proc_header.last_proc = true;
	ident_lens.push_back(ident_count);

	for (int i = 0; i < ident_indexes.size(); i++)
	{
		int loc = ident_indexes[i];
		procedure proc;
		procedure_fix fix;

		proc.header = procedure_raws[loc].proc_header;
		proc.fix_num = ident_lens[i];



		for (int j = 0; j < ident_lens[i]; j++)
		{
			fix = procedure_raws[loc + j].fix;
			fix.last_fix = false;
			if (j == (ident_lens[i] - 1))
			{
				fix.last_fix = true;
				if (procedure_raws[loc + j].proc_header.last_proc)
					proc.header.last_proc = true;


			}
			proc.fixes.push_back(fix);
		}
		if (is_non)
		{
			if (!isCustom)
				non_db->procedures.push_back(proc);
			else
				non_db->custom_procedures.push_back(proc);
		}
		else
		{
			if (!isCustom)
				db->procedures.push_back(proc);
			else
				db->custom_procedures.push_back(proc);
		}
		//cout << ident_indexes[i] << ", " << ident_lens[i] << endl;
	}

	for (int i = 0; i < ident_indexes.size(); i++)
		for (int j = 0; j < ident_lens[i]; j++)
			if (procedure_raws[ident_indexes[i] + j].fix.MissedApproach)
			{


				if (is_non)
				{
					if (!isCustom) {
						non_db->procedures[i].fixes[j].MissedApproach = false;
						non_db->procedures[i].fixes[j + 1].MissedApproach = true;
					}
					else {
						non_db->custom_procedures[i].fixes[j].MissedApproach = false;
						non_db->custom_procedures[i].fixes[j + 1].MissedApproach = true;
					}
				}
				else
				{
					if (!isCustom) {
						db->procedures[i].fixes[j].MissedApproach = false;
						db->procedures[i].fixes[j + 1].MissedApproach = true;
					}
					else {
						db->custom_procedures[i].fixes[j].MissedApproach = false;
						db->custom_procedures[i].fixes[j + 1].MissedApproach = true;
					}
				}
			}
	

}

void csv_parser::parse_runways(bool is_non)
{
	int start = 0, end = 0;
	find_table("Runways", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in Runways (" << start << "|" << end << ")" << endl;
	

	if ((start == 0) && (end == 0)) {
		return;
	}

	runway rwy;
	rwy.last_flag = false;
	int record_index = 0;

	std::vector<runway> rwys;
	std::vector<int> last_locs;

	std::string last_airport("dummy");


	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		rwy.Airport = token_lines[0];
		rwy.Ident = token_lines[1];
		rwy.ident_num = std::stoi(rwy.Ident.substr(2, 2));
		rwy.RevCode = revcode_to_int(token_lines[2]);

		if (is_non)
		{
			rwy.MagBearing = std::stod(token_lines[3]);
			if (rwy.MagBearing < 0) {
				cerr << "negative magbearing is not sipported" << endl;
				throw(1001);
			}
			rwy.Length = std::stoi(token_lines[4]);
			rwy.Latitude = DMS::DecimalDegrees(token_lines[5]);
			rwy.Longitude = DMS::DecimalDegrees(token_lines[6]);
		}
		else {
			rwy.MagBearing = std::stod(token_lines[6]);
			rwy.Length = std::stoi(token_lines[7]);
			rwy.Latitude = DMS::DecimalDegrees(token_lines[8]);
			rwy.Longitude = DMS::DecimalDegrees(token_lines[9]);
		}

		if (is_non)
		{
			try {
				rwy.ThrDisp = std::stoi(token_lines[8]);
			}
			catch (...) {
				rwy.ThrDisp = 0;
			}

			try {
				rwy.Elevation = std::stoi(token_lines[7]);
			}
			catch (...) {
				rwy.Elevation = 0;
			}
		}
		else {
			try {
				rwy.ThrDisp = std::stoi(token_lines[10]);
			}
			catch (...) {
				rwy.ThrDisp = 0;
			}

			try {
				rwy.Elevation = std::stoi(token_lines[11]);
			}
			catch (...) {
				rwy.Elevation = 0;
			}
		}




		if (is_non)
		{
			if (token_lines[11].compare("    ")) {
				rwy.ILSExist = true;
				rwy.ILSNavaid = token_lines[11];
				try {
					rwy.ILSCategory = std::stoi(token_lines[9]);
				}
				catch (...)
				{
					rwy.ILSCategory = 0;
				}
				rwy.ILSBearing = std::stod(token_lines[10]);

			}
			else {
				rwy.ILSExist = false;
			}
		}
		else {
			if (token_lines[12].compare("    ")) {
				rwy.ILSExist = true;
				rwy.ILSNavaid = token_lines[12];
				rwy.ILSCategory = std::stoi(token_lines[13]);
				rwy.ILSBearing = std::stod(token_lines[14]);

			}
			else {
				rwy.ILSExist = false;
			}
		}



		rwy.recordIndex = record_index;

		if (rwy.Airport.compare(last_airport))
		{

			if (record_index != 0)
			{
				if(is_non)
					non_db->runways[record_index - 1].last_flag = true;
				else
					db->runways[record_index - 1].last_flag = true;
			}
		}

		last_airport = rwy.Airport;


		record_index++;
		if (is_non)
			non_db->runways.push_back(rwy);
		else
			db->runways.push_back(rwy);
		
	}

	if(is_non)
		non_db->runways[record_index - 1].last_flag = true;
	else
		db->runways[record_index - 1].last_flag = true;


	int num = (is_non) ? non_db->runways.size() : db->runways.size();
	for (int i = 0; i < num; i++)
	{
		if(is_non)
			non_db->runways_map.insert({ non_db->runways[i].Ident, non_db->runways[i] });
		else
			db->runways_map.insert({ db->runways[i].Ident, db->runways[i] });
	}
	
}

void csv_parser::parse_waypoints(bool is_none)
{
	int start = 0, end = 0;
	find_table("Waypoints", &start, &end);
	if (start != 0)
		cout << "found " << end - start +1 << " rows in Waypoints (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}


	waypoint way;
	int record_index = 0;


	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		way.Ident = token_lines[0];
		way.RevCode = revcode_to_int(token_lines[1]);

		if (is_none)
		{
			way.Latitude = DMS::DecimalDegrees(token_lines[3]);
			way.Longitude = DMS::DecimalDegrees(token_lines[4]);
			way.TerminalEnrouteString = token_lines[2];

		}
		else {
			way.Latitude = DMS::DecimalDegrees(token_lines[2]);
			way.Longitude = DMS::DecimalDegrees(token_lines[3]);
			way.TerminalEnrouteString = token_lines[6];
		}
		//std::cout << way.TerminalEnrouteString << ", " << way.Ident << std::endl;
		//way.TerminalEnrouteString.erase(std::remove(way.TerminalEnrouteString.begin(), way.TerminalEnrouteString.end(), '\r'), way.TerminalEnrouteString.end());
		way.TerminalEnroute = waypoint_tenroute_to_int(way.TerminalEnrouteString);


		//TODO:isCustomized, ICAO



		//cout << nav.Ident << ", " << nav.RevCode << ", " << nav.NavClassString << ", "  << nav.Freq << ", " << nav.Latitude  << ", " << nav.Longitude << ", " << nav.StationDeclination << ", " << nav.Elevation << ", " << nav.FigureOfMeritString << endl;


		way.recordIndex = record_index;
		record_index++;

		if (is_none)
		{
			non_db->waypoints.push_back(way);
			non_db->waypoints_map.insert({ way.Ident, way });
		}
		else {
			db->waypoints.push_back(way);
			db->waypoints_map.insert({ way.Ident, way });
		}

	}



	/*
	struct waypoint {
		int RevCode;
		int TerminalEnroute;
		std::string TerminalEnrouteString;
		std::string Ident;
		double Latitude;
		double Longitude;
		bool IsCustom;
	};
	*/


}

void csv_parser::parse_fuelpolicies(bool is_non)
{
	int start = 0, end = 0;
	find_table("FuelPolicy", &start, &end);
	if (start != 0)
		cout << "found " << end - start + 1 << " rows in FuelPolicy (" << start << "|" << end << ")" << endl;

	if ((start == 0) && (end == 0)) {
		return;
	}

	fuelpolicy fuel;
	int record_index = 0;


	struct fuelpolicy {
		int recordIndex;

		bool RI;
		bool RA;
		bool FD;
		int FinalAlt;
		double TaxiFuel;
		double ReserveFuel;
		double ReserveMax;
		double ReserveMin;
		int FinalTimeInf;
		int FinalTimePre;
		double FinalFixInf;
	};
	for (int i = start; i <= end; i++)
	{
		std::istringstream iss(lines[i]);
		std::string token_line;
		std::vector<std::string> token_lines;
		while (std::getline(iss, token_line, ','))
			token_lines.push_back(token_line);

		fuel.RI = (!token_lines[0].compare("y") || !token_lines[0].compare("Y"))? true : false;
		fuel.RA = (!token_lines[1].compare("y") || !token_lines[1].compare("Y")) ? true : false;
		fuel.FD = (!token_lines[2].compare("y") || !token_lines[2].compare("Y")) ? true : false;



		fuel.FinalAlt = std::stoi(token_lines[3]);

		fuel.TaxiFuel = std::stod(token_lines[4]);
		fuel.ReserveFuel = std::stod(token_lines[5]);
		fuel.ReserveMax = std::stod(token_lines[6]);
		fuel.ReserveMin = std::stod(token_lines[7]);

		fuel.FinalTimeInf = std::stoi(token_lines[8]);
		fuel.FinalTimePre = std::stoi(token_lines[9]);

		fuel.FinalFixInf = std::stod(token_lines[10]);


		record_index++;

		fuel.recordIndex = record_index;

		if (is_non)
		{
			non_db->fuelpolicies.push_back(fuel);
		}
		else
		{
			db->fuelpolicies.push_back(fuel);
		}

	}

}



