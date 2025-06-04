#include "pch.h"
#include "utils.h"
#include "onemeg_db.h"
#include <iostream>


int * nononemeg_crc_funtion_(uint8_t * data, uint16_t len, int *crc_result)
{
	int v4; // ecx
	int *result; // eax
	int v6; // edx
	int v7; // esi
	int i; // ebx
	unsigned int v9; // eax

	result = (int *)(len >> 1);
	if (len >> 1)
	{
		v6 = *crc_result;
		v7 = 0;
		do
		{
			v4 = *(uint16_t *)(data + 2 * v7);
			for (i = 0; i != 16; ++i)
			{
				while (1)
				{
					v9 = (unsigned int)(int16_t)v4 >> 31;
					v4 *= 2;
					if (v6 < 0)
						break;
					v6 = v9 + 2 * v6;
					if (++i == 16)
						goto LABEL_9;
				}
				v6 = (v9 + 2 * v6) ^ 0x400007;
			}
		LABEL_9:
			++v7;
		} while (v7 < len >> 1);
		result = crc_result;
		*crc_result = v6;
	}
	return result;
}

int  nononemeg_crc_function(int *cnt, uint8_t * data, uint16_t len, int *result_crc)
{
	char v4; // cl
	char v6; // [esp+10h] [ebp-10h]

	if (*(int32_t *)(cnt))
	{
		nononemeg_crc_funtion_(data, len, result_crc);
	}
	else
	{
		v4 = *(uint8_t *)(data + 1);
		*(uint8_t *)(data + 1) = v4 | 0x80;
		v6 = v4;
		nononemeg_crc_funtion_(data, len, result_crc);
		*(uint8_t *)(data + 1) = v6;
	}
	++*(int32_t *)(cnt);
	return 0;
}


int nononemeg_db::find_navaid(std::string ident, long double lat, long double lon, int elev)
{

	auto it = navaids_map.find(ident);
	if (it == navaids_map.end()) {
		std::cerr << "while creating runways table exception happend for finding ILS navaid row!" << std::endl;
		throw (404);
	}
	else {

		int best_match = 0;
		double best_dist = std::numeric_limits<double>::infinity();
		for (; it != navaids_map.end() && (it->second.Ident == ident); it++)
		{
			double d = std::pow((it->second.Latitude - lat), 2.0) + std::pow((it->second.Longitude - lon), 2.0);
			if (d < best_dist)
			{
				best_match = it->second.recordIndex;
				best_dist = d;
			}
		}
		return best_match;
	}

}


int nononemeg_db::find_fix(int fix_type, std::string fix_ident, long double fix_lat, long double fix_lon)
{
	if (fix_type == 0)
		return 0;

	if (fix_type == 1)
	{
		auto it = waypoints_map.find(fix_ident);
		if (it == waypoints_map.end()) {
			std::cerr << "exception happend for finding waypoint !" << std::endl;
			throw (404);
		}
		else {

			for (; it != waypoints_map.end() && (it->second.Ident == fix_ident); it++)
			{
				if (std::abs(it->second.Latitude - fix_lat) < 0.001 && std::abs(it->second.Longitude - fix_lon) < 0.001)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding waypoint !" << std::endl;
			throw (404);
		}
	}

	if (fix_type == 3)
	{
		auto it = navaids_map.find(fix_ident);
		if (it == navaids_map.end()) {
			std::cerr << "exception happend for finding navaid !" << std::endl;
			throw (404);
		}
		else {

			for (; it != navaids_map.end() && (it->second.Ident == fix_ident); it++)
			{
				//std::cout << std::abs(it->second.Latitude - fix_lat) << ", " << std::abs(it->second.Longitude == fix_lon) <<  ", " << it->second.Latitude  << ", " << it->second.Longitude << std::endl;
				if (std::abs(it->second.Latitude - fix_lat) < 0.001 && std::abs(it->second.Longitude - fix_lon) < 0.001)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding navaid.lat(" << fix_lat << ") or long(" << fix_lon << ") not found for " << fix_ident << std::endl;
			throw (404);
		}
	}
	if (fix_type == 4)
	{
		auto it = ndbs_map.find(fix_ident);
		if (it == ndbs_map.end()) {
			std::cerr << "exception happend for finding ndb. no ndb with name: " << fix_ident << std::endl;
			throw (404);
		}
		else {

			for (; it != ndbs_map.end() && (it->second.Ident == fix_ident); it++)
			{
				//std::cout << it->second.Latitude << ", " << it->second.Longitude << std::endl;
				if (std::abs(it->second.Latitude - fix_lat) < 0.001 && std::abs(it->second.Longitude - fix_lon) < 0.001)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding ndb.lat(" << fix_lat << ") or long(" << fix_lon << ") not found for " << fix_ident << std::endl;
			throw (404);
		}
	}
	if (fix_type == 2)//TODO: what about 5?
	{
		auto it = runways_map.find(fix_ident);
		if (it == runways_map.end()) {
			std::cerr << "exception happend for finding runway !" << std::endl;
			throw (404);
		}
		else {

			for (; it != runways_map.end() && (it->second.Ident == fix_ident); it++)
			{
				if (std::abs(it->second.Latitude - fix_lat) < 0.001 && std::abs(it->second.Longitude - fix_lon) < 0.001)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding runway !" << std::endl;
			throw (404);
		}
	}
	if (fix_type == 6)
	{
		auto it = airports_map.find(fix_ident);
		if (it == airports_map.end()) {
			std::cerr << "exception happend for finding apt. no apt with name: " << fix_ident << std::endl;
			throw (404);
		}
		else {

			for (; it != airports_map.end() && (it->second.Ident == fix_ident); it++)
			{
				//std::cout << it->second.Latitude << ", " << it->second.Longitude << std::endl;
				if (std::abs(it->second.Latitude - fix_lat) < 0.001 && std::abs(it->second.Longitude - fix_lon) < 0.001)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding apt.lat(" << fix_lat << ") or long(" << fix_lon << ") not found for " << fix_ident << std::endl;
			throw (404);
		}
	}

}

void nononemeg_db::calc_crc()
{
	int cnt = 0;
	int crc = 0;
	int crc_loc;
	int i = 0;
	memset((total_data + (total_sector_num - 1) * 520 + 7 + 508), 0, 5);


	for(i = 0; i < total_sector_num; i++)
		nononemeg_crc_function(&cnt, (uint8_t *)(total_data + i * 520 + 7), 512, &crc);


	//printf("calc_crc: %08x\r\n", crc);

	crc_loc = total_sector_num * 520 - 5;

	total_data[crc_loc] = (crc >> 16) & 0xFF; total_data[crc_loc + 1] = (crc >> 24) & 0xFF; total_data[crc_loc + 2] = (crc >> 0) & 0xFF; total_data[crc_loc + 3] = (crc >> 8) & 0xFF;



}


void nononemeg_db::create_navaid()
{
	unsigned char * row = new unsigned char[navaid_row_len];
	int count = 0;
	double freq;


	this->navaid_table = new binary_table("navaid", navaid_row_len);
	if (this->navaids.size() > 0)
	{
		for (auto const& x : navaids)
		{
			memset(row, 0, navaid_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);
			set_uint(row, 0, 12, 4, x.NavClass);
			set_uint(row, 0, 8, 3, x.FigureOfMerit);
			if (x.Ident.length() > 3)
			{
				set_ident(row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(row, 0, 5, 6, x.Ident.substr(3, 1));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 3, 7, 24, 0.000021457672119, x.Longitude);


			if (x.NavClass == 7 || x.NavClass == 13)
				freq = x.Freq * (double)0.05;
			else
				freq = x.Freq - (double)108.0;

			set_double(row, 5, 15, 8, 0.05, freq);
			set_f_int(row, 5, 7, 12, 10.0, x.Elevation);
			set_double(row, 6, 11, 12, 0.087890625, x.StationDeclination);


			this->navaid_table->add_row(row);
			//this->navaid_table->print_row(count);

			count++;
		}
	}
	delete row;
}

void nononemeg_db::create_waypoint()
{
	unsigned char * row = new unsigned char[waypoint_row_len];
	int count = 0;


	this->waypoint_table = new binary_table("waypoint", waypoint_row_len);
	if (this->waypoints.size() > 0)
	{
		for (auto const& x : waypoints)
		{

			memset(row, 0, waypoint_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);
			//set_uint(row, 0, 13, 1, x.unk);
			set_uint(row, 0, 12, 2, x.TerminalEnroute);

			if (x.Ident.length() > 3)
			{
				set_ident(row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(row, 0, 10, 11, x.Ident.substr(3));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 2.14576721190000005900968410577E-5, x.Latitude);
			set_double(row, 3, 7, 24, 2.14576721190000005900968410577E-5, x.Longitude);


			this->waypoint_table->add_row(row);
			//this->waypoint_table->print_row(count);

			count++;
		}
	}
	delete row;
}

void nononemeg_db::create_airport()
{
	unsigned char * row = new unsigned char[non_airport_row_len];
	int count = 0;
	double freq;
	int rloc;
	bool runway_found;


	this->airport_table = new binary_table("airport", non_airport_row_len);
	if (this->airports.size() > 0)
	{
		for (auto const& x : airports)
		{
			memset(row, 0, non_airport_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);

			//set_uint(row, 0, 13, 1, x.ProceduresExist);
			set_uint(row, 0, 12, 1, x.RunwaysExist);
			//set_uint(row, 0, 11, 1, x.GatesExist);
			//set_uint(row, 0, 10, 1, x.AlternatesExist);

			set_uint(row, 0, 9, 1, x.SpeedLimitCoded);
			set_uint(row, 0, 8, 1, x.TransAltValid);


			if (x.Ident.length() > 3)
			{
				set_ident(row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(row, 0, 5, 6, x.Ident.substr(3, 1));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 3, 7, 24, 0.000021457672119, x.Longitude);

			/*
			if (x.MagVar_Neg && (x.MagVar == 0))
			{
				set_uint(row, 5, 15, 8, 0xFF);//msb
				set_uint(row, 6, 15, 4, 0xF);//lsb
			}
			else {
				int32_t magvar_int = double_to_int(0.087890625, x.MagVar);
				set_uint(row, 5, 15, 8, (magvar_int >> 4) & 0xFF);//msb
				set_uint(row, 6, 15, 4, (magvar_int) & 0xF);//lsb
			}
			*/



			//set_uint(row, 7, 15, 16, (x.RunwayPtr));

			set_f_int(row, 8, 11, 12, 10.0, x.Elevation);
			set_uint(row, 10, 11, 12, x.SpeedLimit);
			set_f_int(row, 11, 11, 12, 10.0, x.SpeedLimitAltitude);
			set_f_int(row, 12, 11, 12, 10.0, x.TransAltitude);


			rloc = 0;
			runway_found = false;
			if (x.RunwaysExist) {
				for (auto const& r : runways)
				{
					if (r.Airport == x.Ident)
					{
						runway_found = true;
						set_uint(row, 6, 11, 12, (rloc >> 2) & 0XFFF);
						set_uint(row, 7, 15, 2, (rloc) & 0x3);
						//x.second.RunwayPtr = rloc;
						break;
					}
					rloc++;
				}
				if (!runway_found) {
					std::cerr << "airport runwayexist but not found " << std::endl;
					throw (404);
				}
			}

			this->airport_table->add_row(row);
			//this->airport_table->print_row(count);
			count++;
		}
	}
	delete row;
}

void nononemeg_db::create_runway()
{
	unsigned char * row = new unsigned char[non_runway_row_len];
	int count = 0;
	double freq;



	this->runway_table = new binary_table("runway", non_runway_row_len);
	if (this->runways.size() > 0)
	{
		for (auto const& x : runways)
		{
			memset(row, 0, non_runway_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);

			set_uint(row, 0, 5, 6, x.ident_num);
			if (x.Ident.length() > 4)
				set_ident(row, 0, 11, 6, x.Ident.substr(4, 1));

			set_double(row, 1, 15, 16, 0.0054931641, x.MagBearing);
			set_uint(row, 2, 15, 16, x.Length);
			set_double(row, 3, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 4, 7, 24, 0.000021457672119, x.Longitude);

			set_uint(row, 6, 15, 16, x.ThrDisp);
			set_f_int(row, 7, 15, 16, 10.0, x.Elevation);


			set_uint(row, 8, 15, 1, x.ILSExist);
			if (x.ILSExist) {
				set_uint(row, 8, 13, 2, x.ILSCategory);
				set_double(row, 9, 15, 16, 0.0054931640625, x.ILSBearing);
				int navaid_row = find_navaid(x.ILSNavaid, x.Latitude, x.Longitude, x.Elevation);
				set_uint(row, 8, 11, 12, navaid_row);
			}



			set_uint(row, 0, 13, 1, x.last_flag);


			this->runway_table->add_row(row);
			//this->runway_table->print_row(count);

			count++;
		}
	}

	delete row;
}

void nononemeg_db::create_ndb()
{
	unsigned char * row = new unsigned char[ndb_row_len];
	int count = 0;


	this->ndb_table = new binary_table("ndb", ndb_row_len);
	if (this->ndbs.size() > 0)
	{
		for (auto const& x : ndbs)
		{
			memset(row, 0, ndb_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);
			/*
			if (!x.Ident.compare("ZNS")) {
				std::cout << "wait Mf" << std::endl;
			}
			*/

			if (x.Ident.length() > 3)
			{
				set_ident(row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(row, 0, 5, 6, x.Ident.substr(3));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 3, 7, 24, 0.000021457672119, x.Longitude);
			set_double(row, 5, 11, 12, 0.5, (x.Frequency - 190));


			this->ndb_table->add_row(row);
			//this->ndb_table->print_row(count);
			count++;
		}
	}

	delete row;
}


void nononemeg_db::create_airway()
{

	int cur_leg_ptr = 0;
	int cnt = 0;
	int fix_id, fix_type;
	unsigned char * index_row = new unsigned char[airway_index_row_len];
	unsigned char * fix_row = new unsigned char[non_airway_fix_row_len];

	this->airway_index_table = new binary_table("airway_index", airway_index_row_len);
	this->airway_fix_table = new binary_table("airway_fix", non_airway_fix_row_len);

	if (this->airways.size() > 0)
	{
		for (auto const& x : airways)
		{
			memset(index_row, 0, airway_index_row_len);

			set_uint(index_row, 0, 15, 2, x.revcode);

			if (x.Ident.length() > 3)
			{
				set_ident(index_row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(index_row, 0, 10, 11, x.Ident.substr(3));
				//set_uint(index_row, 0, 10, 11, get_char_int(x.Ident.substr(3)[0]) * 40);

			}
			else {
				set_ident(index_row, 1, 15, 16, x.Ident);
			}
			set_uint(index_row, 2, 15, 16, cur_leg_ptr);
			this->airway_index_table->add_row(index_row);
			//this->airway_index_table->print_row(cnt);
			cnt++;

			for (int i = 0; i < x.fixes.size(); i++)
			{
				memset(fix_row, 0x00, non_airway_fix_row_len);
				fix_type = x.fixes[i].fix_type;
				if (fix_type == 1 || fix_type == 3 || fix_type == 4)
					fix_id = find_fix(x.fixes[i].fix_type, x.fixes[i].fix_ident, x.fixes[i].fix_Latitude, x.fixes[i].fix_Longitude);
				else
					fix_id = 0xff;
				if (fix_type == 1) {//WPT
					set_uint(fix_row, 0, 15, 2, 0);
					set_uint(fix_row, 0, 13, 14, fix_id);//A
				}
				if (fix_type == 3) {//VOR
					set_uint(fix_row, 0, 15, 2, 2);
					set_uint(fix_row, 0, 12, 13, fix_id);//C
				}
				if (fix_type == 4) {//NDB
					set_uint(fix_row, 0, 15, 4, 15);
					set_uint(fix_row, 0, 11, 12, fix_id);//E
				}
				if (fix_type == 14) {//DIS
					set_uint(fix_row, 0, 15, 4, 13);
				}
				if (fix_type == 15) {//EOA
					set_uint(fix_row, 0, 15, 4, 14);
				}
				cur_leg_ptr++;
				this->airway_fix_table->add_row(fix_row);
			}
		}
	}
}

void nononemeg_db::create_fuel()
{
	//int count = 0;
	unsigned char * row = new unsigned char[fuel_row_len];


	this->fuelpolicy_table = new binary_table("fuelpolicy", fuel_row_len);
	if (this->fuelpolicies.size() > 0)
	{
		for (auto const& x : fuelpolicies)
		{
			memset(row, 0, fuel_row_len);
			set_uint(row, 0, 15, 1, x.RI);
			set_uint(row, 0, 14, 1, x.RA);
			set_uint(row, 0, 13, 1, x.FD);

			set_f_int(row, 0, 7, 8, 100, x.FinalAlt);

			set_double(row, 2, 5, 6, 0.1, x.TaxiFuel);
			set_double(row, 3, 7, 8, 0.1, x.ReserveFuel);
			set_double(row, 4, 6, 7, 0.1, x.ReserveMax);
			set_double(row, 5, 6, 7, 0.1, x.ReserveMin);

			set_uint(row, 6, 13, 7, x.FinalTimeInf);
			set_uint(row, 6, 6, 7, x.FinalTimePre);

			set_double(row, 7, 6, 7, 0.1, x.FinalFixInf, true);
			this->fuelpolicy_table->add_row(row);
			//this->fuelpolicy_table->print_row(count++);
		}
	}

	delete row;
}


int nononemeg_db::proc_set_row(unsigned char * row, procedure x)
{
	int row_len = 0;

	row_len = (x.header.transalt_Exist) ? 10 : 8;

	set_uint(row, 0, 15, 2, x.header.RevCode);

	if (x.header.Ident.length() > 3)
	{
		set_ident(row, 1, 15, 16, x.header.Ident.substr(0, 3));
		set_ident(row, 2, 15, 16, x.header.Ident.substr(3));
	}
	else {
		set_ident(row, 1, 15, 16, x.header.Ident);
	}

	set_uint(row, 0, 13, 3, x.header.ViaType);

	set_uint(row, 0, 10, 6, x.header.runway_num);
	set_uint(row, 0, 4, 4, x.header.runway_char);




	if (x.header.transalt_Exist)
	{
		set_uint(row, 0, 0, 1, 1);
		set_uint(row, 4, 15, 16, x.header.transAlt / 10);

	}



	//	std::cout << x.header.Ident << " | " << x.header.last_proc << "( ";

	for (auto const& y : x.fixes)
	{
		int satr_len = 2;
		bool p_fix_exist = false;
		bool p_cshd_exist = false;
		bool p_alt1_exist = false;
		bool p_distance_time_Exist = false;
		bool p_radial_exist = false;
		bool v56 = false;//second_fix_exist must
		bool v57 = false;//not p_fix_exist
		bool v58 = false;


		set_uint(row + row_len, 0, 14, 1, y.OverFly);

		set_uint(row + row_len, 0, 13, 1, y.MissedApproach);
		set_uint(row + row_len, 0, 12, 2, y.turn_dir);
		set_uint(row + row_len, 0, 10, 5, y.pathterm_num);


		/*
		if ((!y.pathterm.compare("CA")) || (!y.pathterm.compare("CD")) || (!y.pathterm.compare("CI")))
			set_uint(row + row_len, 0, 1, 1, 1);
		*/

		set_uint(row + row_len, 0, 0, 1, y.last_fix);

		
			



		int pathterm_num = y.pathterm_num;
		if ((pathterm_num == 0) || (pathterm_num == 1) || (pathterm_num == 2) || (pathterm_num == 4) || (pathterm_num == 10) || (pathterm_num == 16))
			p_fix_exist = true;

		if ((pathterm_num == 0) || (pathterm_num == 7) || (pathterm_num == 13) || (pathterm_num == 14) || (pathterm_num == 9) || (pathterm_num == 12) || (pathterm_num == 11) || (pathterm_num == 15))
			p_cshd_exist = true;

		if ((pathterm_num == 3) || (pathterm_num == 6) || (pathterm_num == 11))//FA, HA
			p_alt1_exist = true;

		if ((pathterm_num == 1) || (pathterm_num == 9) || (pathterm_num == 12) || (pathterm_num == 17) || (pathterm_num == 18))
			p_distance_time_Exist = true;

		if ((pathterm_num == 15))
			p_radial_exist = true;


		//printf("%d|%d|%d|%d|%d|\r\n", p_fix_exist, p_cshd_exist, p_alt1_exist, p_distance_time_Exist, p_radial_exist);

		if ((pathterm_num == 0) || (pathterm_num == 12) || (pathterm_num == 15) || (pathterm_num == 1) || (pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
			v56 = true; //no mandatory fix exist

		if ((pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 6) || (pathterm_num == 8) || (pathterm_num == 7) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
			v57 = true;


		if ((pathterm_num == 1) || (pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 6) || (pathterm_num == 8) || (pathterm_num == 17) || (pathterm_num == 18))
			v58 = true;//no mandatory cshd exist



		if (p_fix_exist)
		{
			int fix_id = find_fix(y.fix_type, y.fix_ident, y.fix_Latitude, y.fix_Longitude);
			uint16_t val = 0;
			if (y.fix_type == 1)
			{//waypoint
				val = 0x8000;
				val |= (fix_id & 0x3FFF);
			}
			else if (y.fix_type == 3)
			{
				val = 0xC000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 2)
			{//runway
				val = 0xD000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 4)
			{
				val = 0xF000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 6)
			{
				val = 0xE000;
				val |= (fix_id & 0x0FFF);
			}
			set_uint(row + row_len + satr_len, 0, 15, 16, val);
			satr_len += 2;
		}

		if (p_cshd_exist)
		{
			set_double(row + row_len + satr_len, 0, 14, 12, 0.087890625, y.cs_hd);

			if ((!y.pathterm.compare("VA")) || (!y.pathterm.compare("VD")) || (!y.pathterm.compare("VI")))
				set_uint(row + row_len + satr_len, 0, 0, 1, 1);
			satr_len += 2;
		}
			

		if (p_alt1_exist)
		{
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt1_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt1_num);
			satr_len += 2;
		}

		if (p_distance_time_Exist)
		{
			if (p_distance_time_Exist) {
				if (y.dist_exist)
				{
					set_double(row + row_len + satr_len, 0, 13, 14, 1.0, y.dist);
				}
				else if (y.time_exist) {
					set_uint(row + row_len + satr_len, 0, 15, 2, 2);
					set_double(row + row_len + satr_len, 0, 13, 14, 1, y.time * 60.0);
				}
				satr_len += 2;
			}
		}


		if (p_radial_exist)
		{
			set_double(row + row_len + satr_len, 0, 14, 15, 0.010986328, y.Radial);
			satr_len += 2;
		}


		if (v57 && (y.fix_type != 0))
		{
			int fix_id = find_fix(y.fix_type, y.fix_ident, y.fix_Latitude, y.fix_Longitude);
			uint16_t val = 0;
			if (y.fix_type == 1)
			{//waypoint
				val = 0x8000;
				val |= (fix_id & 0x3FFF);
			}
			else if (y.fix_type == 3)
			{
				val = 0xC000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 2)
			{
				val = 0xD000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 4)
			{
				val = 0xF000;
				val |= (fix_id & 0x0FFF);
			}
			else if (y.fix_type == 6)
			{
				val = 0xE000;
				val |= (fix_id & 0x0FFF);
			}
			set_uint(row + row_len + satr_len, 0, 15, 16, val);
			set_uint(row + row_len, 0, 5, 1, 1);
			satr_len += 2;

		}




		if (y.alt1_exist && !p_alt1_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt1_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt1_num);
			set_uint(row + row_len, 0, 4, 1, 1);//alt1_exist
			satr_len += 2;
		}
		if (y.alt2_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt2_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt2_num);
			set_uint(row + row_len, 0, 3, 1, 1);//alt2_exist
			satr_len += 2;
		}



		if (y.nav_exist && y.ndb_exist)
		{
			std::cerr << "ndb and nav fix cannot both exist at same time for a procedure" << std::endl;
			throw (1001);
		}
		else {
			if (y.nav_exist) {
				int fix_id = find_fix(3, y.nav_ident, y.nav_Latitude, y.nav_Longitude);
				uint16_t val = 0;
				val = 0x8000;
				val |= (fix_id & 0x0FFF);
				set_uint(row + row_len + satr_len, 0, 15, 16, val);
				set_uint(row + row_len, 0, 2, 1, 1);//
				satr_len += 2;
			}


			if (y.ndb_exist) {
				int fix_id = find_fix(4, y.ndb_ident, y.ndb_Latitude, y.ndb_Longitude);
				uint16_t val = 0;
				val = 0xF000;
				val |= (fix_id & 0x0FFF);
				set_uint(row + row_len + satr_len, 0, 15, 16, val);
				set_uint(row + row_len, 0, 2, 1, 1);//
				satr_len += 2;
			}

		}


		if (v58 && !(p_cshd_exist) && (y.cs_hd_exist))
		{
			set_double(row + row_len + satr_len, 0, 14, 12, 0.087890625, y.cs_hd);
			set_uint(row + row_len, 0, 1, 1, 1);
			if ((!y.pathterm.compare("VA")) || (!y.pathterm.compare("VD")) || (!y.pathterm.compare("VI")))
				set_uint(row + row_len + satr_len, 0, 0, 1, 1);

			satr_len += 2;

		}


		
		if (y.cas_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0x8);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.CAS);
			satr_len += 2;
		}



		if (y.time_exist && !(p_distance_time_Exist && !y.dist_exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0x9);
			set_double(row + row_len + satr_len, 0, 11, 12, 1, y.time * 60.0);
			satr_len += 2;
		}


		if (y.dist_exist && !(p_distance_time_Exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0xA);
			set_double(row + row_len + satr_len, 0, 11, 12, 1.0, y.dist);
			satr_len += 2;
		}





		if (y.radial_exist && !(p_radial_exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0xB);
			set_double(row + row_len + satr_len, 0, 11, 12, 0.010986328, y.Radial);
			satr_len += 2;
		}




		if (y.fpa_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0xC);
			set_double(row + row_len + satr_len, 0, 11, 12, 0.087890625, y.FPA);
			satr_len += 2;
		}


		/*
		if (y.gcsa_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 6);
			set_double(row + row_len + satr_len, 0, 11, 12, 10.0, y.GSCA);
			satr_len += 2;
		}
		*/
		row_len += satr_len;

	}


	if (x.header.last_proc)
	{
		set_uint(row, 3, 15, 16, 0);
		row_len += 2;
	}
	else {
		set_uint(row, 3, 15, 16, row_len / 2);
	}

	return row_len;
}

void nononemeg_db::create_procedure(int base_sector)
{
	int row_len;
	int i = 0;
	this->proc_table = new procedure_table();
	unsigned char * row = new unsigned char[512];
	base_sector *= 256;

	if (this->procedures.size() > 0)
	{
		for (auto const& x : procedures)
		{
			memset(row, 0, 512);

			if (this->proc_table->cur_pos > (2 * 1024 * 1024))
				continue;

			//if(!x.header.Airport.compare("DTTJ") && !x.header.Ident.compare("ZEKR2") /*&& !x.header.RunwayString.compare("09")*/)
			//	std::cout << x.header.Airport << ", " << x.header.Ident << std::endl;
			//if (!x.header.Airport.compare("ZWSH") && !x.header.Ident.compare("D03"))
			//	std::cout << x.header.Airport << ", " << x.header.Ident << std::endl;

			if (x.header.first_proc)
			{
				int air_recordIndex = airports_map[x.header.Airport].recordIndex;
				int proc_loc = base_sector + this->proc_table->cur_pos / 2;
				int lsb = proc_loc % 256;
				int msb = proc_loc / 256;
				/*
				fd00
				1004d
				100550
				*/
				/*
				FD00
				10124
				10782
				10963
				*/


				//set_uint(this->airport_table->get_row(air_recordIndex), 5, 15, 8, lsb);//lsb
				set_double(this->airport_table->get_row(air_recordIndex), 5, 15, 8, 1.0, lsb);
				set_uint(this->airport_table->get_row(air_recordIndex), 5, 7, 8, msb >> 4);//msb1
				set_uint(this->airport_table->get_row(air_recordIndex), 6, 15, 4, msb & 0xF);//msb1


				set_uint(this->airport_table->get_row(air_recordIndex), 0, 13, 1, 1);//procedure_exist
				//airport_table->print_row(air_recordIndex);


			}

			row_len = this->proc_set_row(row, x);


			proc_table->add_row(row, row_len);
			if (!x.header.Airport.compare("BIAR") && !x.header.Ident.compare("ASKU1D") /*&& !x.header.RunwayString.compare("09")*/)
				proc_table->print_row(i);

			i++;

			//proc_table->print_row(i);
		}
	}
}


void nononemeg_db::create_tables()
{
	create_navaid();
	create_waypoint();
	create_airport();
	create_runway();
	create_fuel();
	create_ndb();
	create_airway();
	//create_procedure();
}




void nononemeg_db::create_header() {

	memset(header_data, 0, 2*SECTOR_SIZE);

	if (dinf.sequence_name.length() > 0)
		set_ident(header_data, 0, 11, 6, dinf.sequence_name.substr(0, 1));
	if (dinf.sequence_name.length() > 1)
		set_ident(header_data, 0, 5, 6, dinf.sequence_name.substr(1, 1));

	if (dinf.sequence_name.length() > 2)
		set_ident(header_data, 1, 11, 6, dinf.sequence_name.substr(2, 1));
	if (dinf.sequence_name.length() > 4)
		set_ident(header_data, 1, 5, 6, dinf.sequence_name.substr(4, 1));

	if (dinf.sequence_name.length() > 5)
		set_ident(header_data, 2, 11, 6, dinf.sequence_name.substr(5, 1));
	if (dinf.sequence_name.length() > 6)
		set_ident(header_data, 2, 5, 6, dinf.sequence_name.substr(6, 1));

	if (dinf.sequence_name.length() > 7)
		set_ident(header_data, 3, 11, 6, dinf.sequence_name.substr(7, 1));
	if (dinf.sequence_name.length() > 9)
		set_ident(header_data, 3, 5, 6, dinf.sequence_name.substr(9, 1));

	if (dinf.sequence_name.length() > 10)
		set_ident(header_data, 4, 11, 6, dinf.sequence_name.substr(10, 1));
	if (dinf.sequence_name.length() > 11)
		set_ident(header_data, 4, 5, 6, dinf.sequence_name.substr(11, 1));


	set_uint(header_data, 5, 15, 4, dinf.prev_start.mon);
	set_uint(header_data, 5, 11, 5, dinf.prev_start.day);
	set_uint(header_data, 5, 6, 7, dinf.prev_start.year);

	set_uint(header_data, 6, 15, 4, dinf.prev_end.mon);
	set_uint(header_data, 6, 11, 5, dinf.prev_end.day);
	set_uint(header_data, 6, 6, 7, dinf.prev_end.year);

	set_uint(header_data, 7, 15, 4, dinf.cur_start.mon);
	set_uint(header_data, 7, 11, 5, dinf.cur_start.day);
	set_uint(header_data, 7, 6, 7, dinf.cur_start.year);

	set_uint(header_data, 8, 15, 4, dinf.prev_end.mon);
	set_uint(header_data, 8, 11, 5, dinf.prev_end.day);
	set_uint(header_data, 8, 6, 7, dinf.prev_end.year);


	set_uint(header_data, 9, 12, 1, waypoint_table->sector_num > 0);
	set_uint(header_data, 9, 11, 12, waypoint_table->sector_loc);

	set_uint(header_data, 10, 12, 1, airway_index_table->sector_num > 0);
	set_uint(header_data, 10, 11, 12, airway_index_table->sector_loc);

	set_uint(header_data, 12, 12, 1, airport_table->sector_num > 0);
	set_uint(header_data, 12, 11, 12, airport_table->sector_loc);

	set_uint(header_data, 13, 12, 1, runway_table->sector_num > 0);
	set_uint(header_data, 13, 11, 12, runway_table->sector_loc);

	set_uint(header_data, 15, 12, 1, fuelpolicy_table->sector_num > 0);
	set_uint(header_data, 15, 11, 12, fuelpolicy_table->sector_loc);

	//set_uint(header_data, 16, 12, 1, airway_fix_table->sector_num > 0);
	set_uint(header_data, 16, 11, 12, airway_fix_table->sector_loc);

	set_uint(header_data, 506, 12, 1, ndb_table->sector_num > 0);
	set_uint(header_data, 506, 11, 12, ndb_table->sector_loc);

	set_uint(header_data, 118, 15, 16, this->total_sector_num);
	set_uint(header_data, 511, 15, 16, this->total_sector_num);


}

void nononemeg_db::copy_header(int &sector_num)
{
	set_sector_header(sector_num);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8), sector_header, 7);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8) + 7, header_data, SECTOR_SIZE);
	*(total_data + sector_num * (SECTOR_SIZE + 8) + 7 + SECTOR_SIZE) = 0xff;
	sector_num++;
	set_sector_header(sector_num);
	memcpy(total_data + sector_num  * (SECTOR_SIZE + 8), sector_header, 7);
	memcpy(total_data + sector_num  * (SECTOR_SIZE + 8) + 7, header_data + SECTOR_SIZE, SECTOR_SIZE);
	*(total_data + sector_num * (SECTOR_SIZE + 8) + 7 + SECTOR_SIZE) = 0xff;
	sector_num++;
}


template <typename T>
void nononemeg_db::copy_table(T * table, int &sector_num) {


	for (int i = 0; i < table->sector_num; i++)
	{
		set_sector_header(table->sector_loc + i);
		memcpy(total_data + (table->sector_loc + i) * (SECTOR_SIZE + 8), sector_header, 7);
		memcpy(total_data + (table->sector_loc + i) * (SECTOR_SIZE + 8) + 7, table->data + i * SECTOR_SIZE, SECTOR_SIZE);
		*(total_data + (table->sector_loc + i) * (SECTOR_SIZE + 8) + 7 + SECTOR_SIZE) = 0xff;
		sector_num++;

	}
}



void nononemeg_db::set_sector_header(int sector_num)
{
	sector_header[0] = 0x00;
	sector_header[1] = 0x00;
	sector_header[2] = 0x41;
	sector_header[3] = (unsigned char)((sector_num) & 0xFF);
	sector_header[4] = (unsigned char)(((sector_num) >> 8) & 0xFF);
	sector_header[5] = 0x00;
	sector_header[6] = 0x00;
}



void nononemeg_db::set_loc_aux(binary_table * table, bool table_exist)
{
	if (table_exist)
	{
		table->sector_loc = total_sector_num;
		table->sector_num = table->get_padded_len() / SECTOR_SIZE;
		total_sector_num += table->sector_num;
	}
	else {
		table->sector_loc = 0;
		table->sector_num = 0;
	}
}


void nononemeg_db::aggregate_data()
{
	total_sector_num = 2;//header_Sector, 



	set_loc_aux(navaid_table, this->navaids.size() > 0);
	set_loc_aux(waypoint_table, this->waypoints.size() > 0);
	set_loc_aux(airport_table, this->airports.size() > 0);
	set_loc_aux(runway_table, this->runways.size() > 0);


	


	if (this->procedures.size() > 0)
	{
		create_procedure(total_sector_num);
		proc_table->sector_loc = total_sector_num;
	}
	else
		proc_table->sector_loc = 0;

	proc_table->sector_num = proc_table->get_padded_len() / SECTOR_SIZE;
	total_sector_num += proc_table->sector_num;



	set_loc_aux(airway_fix_table, this->airways.size() > 0);
	set_loc_aux(airway_index_table, this->airways.size() > 0);
	set_loc_aux(fuelpolicy_table, this->fuelpolicies.size() > 0);
	set_loc_aux(ndb_table, this->ndbs.size() > 0);
	printf("navaid:   %d|%d|%04x\r\n", navaid_table->row_num, navaid_table->sector_num, navaid_table->sector_loc);
	printf("waypoint: %d|%d|%04x\r\n", waypoint_table->row_num, waypoint_table->sector_num, waypoint_table->sector_loc);
	printf("airport: %d|%d|%04x\r\n", airport_table->row_num, airport_table->sector_num, airport_table->sector_loc);
	printf("runway: %d|%d|%04x\r\n", runway_table->row_num, runway_table->sector_num, runway_table->sector_loc);
	printf("proc: %d|%d|%04x\r\n", proc_table->row_num, proc_table->sector_num, proc_table->sector_loc);
	printf("fuelpolicy:	  %d|%d|%04x\r\n", fuelpolicy_table->row_num, fuelpolicy_table->sector_num, fuelpolicy_table->sector_loc);
	printf("ndb:	  %d|%d|%04x\r\n", ndb_table->row_num, ndb_table->sector_num, ndb_table->sector_loc);
	printf("airway_fix:	  %d|%d|%04x\r\n", airway_fix_table->row_num, airway_fix_table->sector_num, airway_fix_table->sector_loc);
	printf("airway_index:	  %d|%d|%04x\r\n", airway_index_table->row_num, airway_index_table->sector_num, airway_index_table->sector_loc);

	total_data = new unsigned char[total_sector_num * (SECTOR_SIZE + 8)];

	int sector_num = 0;

	create_header();
	copy_header(sector_num);
	copy_table(navaid_table, sector_num);
	copy_table(waypoint_table, sector_num);
	copy_table(airport_table, sector_num);
	copy_table(runway_table, sector_num);
	copy_table(proc_table, sector_num);
	copy_table(airway_index_table, sector_num);
	copy_table(airway_fix_table, sector_num);
	copy_table(fuelpolicy_table, sector_num);
	copy_table(ndb_table, sector_num);
	

	calc_crc();
}

void nononemeg_db::write_file()
{
	FILE * fp;
	int ret;


	aggregate_data();

	fp = fopen(dinf.result_filename.c_str(), "wb");
	if (!fp) {
		std::cerr << "failed to open result file" << std::endl;
		throw (1001);
	}



	fwrite(total_data, 1, total_sector_num * (SECTOR_SIZE + 8), fp);



	fclose(fp);

}


