#include <vector>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "onemeg_db.h"


class isWhitespace {
public: bool operator()(unsigned c) {
	if (c == ' ' || c == '\t' || c == '\n' ||
		c == '\r' || c == '\f' || c == '\v') {
		return true;
	}
	else {
		return false;
	}
}
};

int month_to_int(std::string month);

struct procedure_raw {
	int recordIndex;
	procedure_header proc_header;
	procedure_fix fix;
};

class csv_parser {
public:
	std::ifstream file;
	int num_header_lines;
	std::vector<std::string> header_lines;
	std::vector<std::string> lines;
	db_info dinf;
	extonemeg_db * db;
	nononemeg_db * non_db;
	std::vector<int> table_locs;
public:

	csv_parser(const char * csv_filename, hon_time prev_start, hon_time prev_end, std::string tag, std::string result_filename, bool is_non);
	~csv_parser()
	{
		file.close();
	}

	void parse_airports(bool is_non);
	void parse_airways(bool is_non);
	void parse_gridmoras();
	void parse_holdingpatterns();
	void parse_navaids(bool is_non);
	void parse_ndbs(bool is_non);
	void parse_procedures(bool is_non);
	void parse_procs(std::vector<std::string> &lines, bool isCustom, bool is_non);
	void parse_runways(bool is_non);
	void parse_waypoints(bool is_non);
	void parse_fuelpolicies(bool is_non);
	void find_table(std::string name, int * start, int *end);

};