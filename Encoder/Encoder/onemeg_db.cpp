#include "pch.h"
#include "utils.h"
#include "onemeg_db.h"
#include <iostream>


int extonemeg_db::find_navaid(std::string ident, long double lat, long double lon, int elev)
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

int extonemeg_db::find_fix(int fix_type, std::string fix_ident, long double fix_lat, long double fix_lon)
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
				if (it->second.Latitude == fix_lat && it->second.Longitude == fix_lon)
					return it->second.recordIndex ;
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
				if (it->second.Latitude == fix_lat && it->second.Longitude == fix_lon)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding navaid !" << std::endl;
			throw (404);
		}
	}
	if (fix_type == 4)
	{
		auto it = ndbs_map.find(fix_ident);
		if (it == ndbs_map.end()) {
			std::cerr << "exception happend for finding ndb. no ndb with name: "  << fix_ident << std::endl;
			throw (404);
		}
		else {

			for (; it != ndbs_map.end() && (it->second.Ident == fix_ident); it++)
			{
				if (it->second.Latitude == fix_lat && it->second.Longitude == fix_lon)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding ndb.lat(" << fix_lat << ") or long(" << fix_lon << ") not found for " << fix_ident << std::endl;
			throw (404);
		}
	}
	if (fix_type == 5)
	{
		auto it = runways_map.find(fix_ident);
		if (it == runways_map.end()) {
			std::cerr << "exception happend for finding runway !" << std::endl;
			throw (404);
		}
		else {

			for (; it != runways_map.end() && (it->second.Ident == fix_ident); it++)
			{
				if (it->second.Latitude == fix_lat && it->second.Longitude == fix_lon)
					return it->second.recordIndex;
			}
			std::cerr << "exception happend for finding runway !" << std::endl;
			throw (404);
		}
	}

}

void extonemeg_db::create_airport()
{
	unsigned char * row = new unsigned char[airport_row_len];
	int count = 0;
	double freq;
	int rloc;
	bool runway_found;


	this->airport_table = new binary_table("airport", airport_row_len);
	if (this->airports.size() > 0)
	{
		for (auto const& x : airports)
		{
			memset(row, 0, airport_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);

			//set_uint(row, 0, 13, 1, x.ProceduresExist);
			set_uint(row, 0, 12, 1, x.RunwaysExist);
			set_uint(row, 0, 11, 1, x.SpeedLimitCoded);
			set_uint(row, 0, 10, 1, x.TransAltValid);
			set_uint(row, 0, 9, 1, x.GatesExist);

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



			set_uint(row, 7, 15, 16, (x.RunwayPtr));

			set_f_int(row, 8, 11, 12, 10.0, x.Elevation);
			set_uint(row, 9, 11, 12, x.SpeedLimit);
			set_f_int(row, 10, 11, 12, 10.0, x.SpeedLimitAltitude);
			set_f_int(row, 11, 11, 12, 10.0, x.TransAltitude);


			rloc = 0;
			runway_found = false;
			if (x.RunwaysExist) {
				for (auto const& r : runways)
				{
					if (r.Airport == x.Ident)
					{
						runway_found = true;
						set_uint(row, 7, 15, 16, rloc);
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

void extonemeg_db::create_airway()
{
	int cur_leg_ptr = 0;
	int fix_id, fix_type;
	unsigned char * index_row = new unsigned char[airway_index_row_len];
	unsigned char * fix_row = new unsigned char[airway_fix_row_len];

	this->airway_index_table = new binary_table("airway_index", airway_index_row_len);
	this->airway_fix_table = new binary_table("airway_fix", airway_fix_row_len);

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
				
			}
			else {
				set_ident(index_row, 1, 15, 16, x.Ident);
			}
			set_uint(index_row, 2, 15, 16, cur_leg_ptr);
			this->airway_index_table->add_row(index_row);

			for (int i = 0; i < x.fixes.size(); i += 4)
			{
				memset(fix_row, 0xFF, airway_fix_row_len);

				int num_fixes = x.fixes.size();

				for (int j = 0; i + j < num_fixes && j < 4; j++)
				{
					fix_type = x.fixes[i + j].fix_type;
					if (fix_type == 1 || fix_type == 3 || fix_type == 4)
						fix_id = find_fix(x.fixes[i + j].fix_type, x.fixes[i + j].fix_ident, x.fixes[i + j].fix_Latitude, x.fixes[i + j].fix_Longitude);
					else
						fix_id = 0xff;
					set_uint(fix_row, 0, 15-j*4, 4, fix_type);
					set_uint(fix_row, 1+j, 15, 16, fix_id);

				}
				cur_leg_ptr++;
				this->airway_fix_table->add_row(fix_row);
			}
		}
	}

}

void extonemeg_db::create_navaid()
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
				set_ident(row, 1, 15, 16, x.Ident.substr(0,3));
				set_ident(row, 0, 5, 6, x.Ident.substr(3,1));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 3, 7, 24, 0.000021457672119, x.Longitude);



			freq = (x.Freq - (double)108.0) * 20.0;

			set_double(row, 5, 15, 8, 1.0, freq);
			set_f_int(row, 5, 7, 12, 10.0, x.Elevation);
			set_double(row, 6, 11, 12, 0.087890625, x.StationDeclination);
			

			this->navaid_table->add_row(row);
			//this->navaid_table->print_row(count);
			
			count++;
		}
	}
	delete row;
}

void extonemeg_db::create_gridmora()
{
	unsigned char * row = new unsigned char[grid_row_len];
	int count = 0;
	double freq;


	this->gridmora_table = new binary_table("gridmora", grid_row_len);
	if (this->navaids.size() > 0)
	{
		for (auto const& x : gridmoras)
		{
			memset(row, 0, grid_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);
			set_double(row, 0, 11, 8, 1.0, x.Latitude);
			set_double(row, 0, 3, 4, 30.0 , x.Longitude);


			set_uint(row, 1, 15, 9, x.Altitude[0]);
			set_uint(row, 1, 6, 9, x.Altitude[1]);
			set_uint(row, 2, 13, 9, x.Altitude[2]);
			set_uint(row, 2, 4, 9, x.Altitude[3]);
			set_uint(row, 3, 11, 9, x.Altitude[4]);

			set_uint(row, 3, 2, 9, x.Altitude[5]);
			set_uint(row, 4, 9, 9, x.Altitude[6]);
			set_uint(row, 4, 0, 9, x.Altitude[7]);
			set_uint(row, 5, 7, 9, x.Altitude[8]);
			set_uint(row, 6, 14, 9, x.Altitude[9]);

			set_uint(row, 6, 5, 9, x.Altitude[10]);
			set_uint(row, 7, 12, 9, x.Altitude[11]);
			set_uint(row, 7, 3, 9, x.Altitude[12]);
			set_uint(row, 8, 10, 9, x.Altitude[13]);
			set_uint(row, 8, 1, 9, x.Altitude[14]);

			set_uint(row, 9, 8, 9, x.Altitude[15]);
			set_uint(row, 10, 15, 9, x.Altitude[16]);
			set_uint(row, 10, 6, 9, x.Altitude[17]);
			set_uint(row, 11, 13, 9, x.Altitude[18]);
			set_uint(row, 11, 4, 9, x.Altitude[19]);

			set_uint(row, 12, 11, 9, x.Altitude[20]);
			set_uint(row, 12, 2, 9, x.Altitude[21]);
			set_uint(row, 13, 9, 9, x.Altitude[22]);
			set_uint(row, 13, 0, 9, x.Altitude[23]);
			set_uint(row, 14, 7, 9, x.Altitude[24]);

			set_uint(row, 15, 14, 9, x.Altitude[25]);
			set_uint(row, 15, 5, 9, x.Altitude[26]);
			set_uint(row, 16, 12, 9, x.Altitude[27]);
			set_uint(row, 16, 3, 9, x.Altitude[28]);
			set_uint(row, 17, 10, 9, x.Altitude[29]);



			this->gridmora_table->add_row(row);
			//this->gridmora_table->print_row(count);

			count++;
		}
	}

	delete row;
}

void extonemeg_db::create_holdingpattern()
{
	unsigned char * row = new unsigned char[hop_row_len];
	int count = 0;
	int  alt_coeff;


	this->holdingpattern_table = new binary_table("holdingpattern", hop_row_len);
	if (this->holdingpatterns.size() > 0)
	{
		for (auto const& x : holdingpatterns)
		{
			memset(row, 0, hop_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);
			set_uint(row, 0, 13, 1, x.TurnDir);
			set_uint(row, 0, 12, 1, x.time_length_used);
			set_double(row, 0, 11, 12, 0.087890625, x.InBndCourse);

			int fix_id = find_fix(x.FixType, x.FilxIdent, x.Latitude, x.Longitude);
			set_uint(row, 3, 15, 4, x.FixType);
			set_uint(row, 1, 15, 16, fix_id);

			set_uint(row, 2, 15, 4, x.AltitudeLabel);
			if (x.AltitudeLabel == 0 || x.AltitudeLabel == 1 || x.AltitudeLabel == 4)
				alt_coeff = 10;
			else
				alt_coeff = 1;
			//if (x.AltitudeLabel == 2 || x.AltitudeLabel == 3 || x.AltitudeLabel == 5)
			//	alt_coeff = 100;

			if (x.Altitude_exist) {
				set_uint(row, 2, 11, 12, x.Altitude / alt_coeff);
			}


			if (x.time_length_used)
			{
				set_double(row, 3, 11, 12, 0.1 , x.LegLength);
			}
			else {
				set_double(row, 3, 11, 12, 1.0, x.LegTime);
			}
			this->holdingpattern_table->add_row(row);
			count++;
		}
	}

	delete row;
}

int extonemeg_db::proc_set_row(unsigned char * row, procedure x)
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

	set_uint(row, 0, 13, 4, x.header.ViaType);

	set_uint(row, 0, 9, 6, x.header.runway_num);
	set_uint(row, 0, 3, 4, x.header.runway_char);




	if (x.header.transalt_Exist)
	{
		set_uint(row, 3, 15, 1, 1);
		set_uint(row, 4, 15, 16, x.header.transAlt / 10);

	}



	//	std::cout << x.header.Ident << " | " << x.header.last_proc << "( ";

	for (auto const& y : x.fixes)
	{
		int satr_len = 4;
		bool p_fix_exist = false;
		bool p_cshd_exist = false;
		bool p_alt1_exist = false;
		bool p_distance_time_Exist = false;
		bool p_radial_exist = false;
		bool v8 = false;
		bool v9 = false;
		bool v10 = false;
		set_uint(row + row_len, 0, 15, 1, y.OverFly);
		set_uint(row + row_len, 0, 14, 1, y.MissedApproach);
		set_uint(row + row_len, 0, 13, 2, y.turn_dir);
		set_uint(row + row_len, 0, 11, 5, y.pathterm_num);
		set_uint(row + row_len, 0, 0, 1, 0);//FAF

		set_uint(row + row_len, 1, 12, 5, 0);//rnp_scale_flag
		//TODO: handle rnp_scale_flag rnp_coeff = (rnp_scale_flag)? 0.01 : 0.1. check in existing binary files for rnp_scale_flag stat.
		set_double(row + row_len, 1, 10, 7, 0.1, y.RNP);
		set_uint(row + row_len, 1, 3, 4, y.fix_type);

		if ((y.pathterm_num == 0) || (y.pathterm_num == 1) || (y.pathterm_num == 2) || (y.pathterm_num == 4) || (y.pathterm_num == 10) || (y.pathterm_num == 16))
			p_fix_exist = true;

		if ((y.pathterm_num == 0) || (y.pathterm_num == 7) || (y.pathterm_num == 13) || (y.pathterm_num == 14) || (y.pathterm_num == 9) || (y.pathterm_num == 12) || (y.pathterm_num == 11) || (y.pathterm_num == 15))
			p_cshd_exist = true;

		if ((y.pathterm_num == 3) || (y.pathterm_num == 6) || (y.pathterm_num == 11))//FA, HA
			p_alt1_exist = true;

		if ((y.pathterm_num == 1) || (y.pathterm_num == 9) || (y.pathterm_num == 12) || (y.pathterm_num == 17) || (y.pathterm_num == 18))
			p_distance_time_Exist = true;

		if ((y.pathterm_num == 3) || (y.pathterm_num == 5) || (y.pathterm_num == 6) || (y.pathterm_num == 8) || (y.pathterm_num == 7) || (y.pathterm_num == 9) || (y.pathterm_num == 17) || (y.pathterm_num == 18))
			v8 = true;

		//AF, VD/CD, VR/CR, CF, FM, PI, FC, FD
		if ((y.pathterm_num == 0) || (y.pathterm_num == 12) || (y.pathterm_num == 15) || (y.pathterm_num == 1) || (y.pathterm_num == 5) || (y.pathterm_num == 9) || (y.pathterm_num == 17) || (y.pathterm_num == 18))
			v9 = true;

		if ((y.pathterm_num == 1) || (y.pathterm_num == 3) || (y.pathterm_num == 5) || (y.pathterm_num == 8) || (y.pathterm_num == 17) || (y.pathterm_num == 18))
			v10 = true;

		if ((y.pathterm_num == 15))
			p_radial_exist = true;




		/*
		*p2
		*/

		if (p_fix_exist) {
			int fix_id = find_fix(y.fix_type, y.fix_ident, y.fix_Latitude, y.fix_Longitude);
			set_uint(row + row_len + satr_len, 0, 15, 16, fix_id);
			satr_len += 2;
		}

		if (p_cshd_exist) {
			if ((!y.pathterm.compare("VA")) || (!y.pathterm.compare("VD")) || (!y.pathterm.compare("VI")) || (!y.pathterm.compare("VR")))
				set_uint(row + row_len + satr_len, 0, 15, 1, 1);
			set_double(row + row_len + satr_len, 0, 12, 13, 0.0439453125, y.cs_hd);
			satr_len += 2;
		}
		if (p_alt1_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt1_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt1_num);
			satr_len += 2;

		}
		if (p_distance_time_Exist) {
			if (y.dist_exist)
			{
				set_double(row + row_len + satr_len, 0, 11, 12, 1.0, y.dist);
				set_uint(row + row_len + satr_len, 0, 15, 1, 1);
			}
			else if (y.time_exist) {
				set_uint(row + row_len + satr_len, 0, 11, 12, (int)y.time);
			}
			satr_len += 2;
		}

		if (p_radial_exist) {
			set_double(row + row_len + satr_len, 0, 12, 13, 0.043945312, y.Radial);
			satr_len += 2;
		}


		/*
		*p3
		*/

		//if (y.fix_type == 5)//runway
		if (!p_fix_exist && (y.fix_type != 0))
		{
			int fix_id = find_fix(y.fix_type, y.fix_ident, y.fix_Latitude, y.fix_Longitude);
			set_uint(row + row_len + satr_len, 0, 15, 16, fix_id);
			set_uint(row + row_len, 0, 6, 1, 1);//fixtype_is_runway
			satr_len += 2;
		}

		if (y.alt1_exist && !p_alt1_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt1_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt1_num);
			set_uint(row + row_len, 0, 5, 1, 1);//alt1_exist
			satr_len += 2;
		}
		if (y.alt2_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, y.alt2_char);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.alt2_num);
			set_uint(row + row_len, 0, 4, 1, 1);//alt1_exist
			satr_len += 2;
		}


		if (y.nav_exist) {
			int fix_id = find_fix(3, y.nav_ident, y.nav_Latitude, y.nav_Longitude);
			set_uint(row + row_len + satr_len, 0, 15, 16, fix_id);
			set_uint(row + row_len, 0, 3, 1, 1);//nav_exist
			satr_len += 2;
		}


		if (!p_cshd_exist && y.cs_hd_exist) {
			if ((!y.pathterm.compare("VA")) || (!y.pathterm.compare("VD")) || (!y.pathterm.compare("VI")) || (!y.pathterm.compare("VR")))
				set_uint(row + row_len + satr_len, 0, 15, 1, 1);
			set_double(row + row_len + satr_len, 0, 12, 13, 0.0439453125, y.cs_hd);
			set_uint(row + row_len, 0, 2, 1, 1);//cshd_exist
			satr_len += 2;
		}

		if (y.ndb_exist) {
			int fix_id = find_fix(4, y.ndb_ident, y.ndb_Latitude, y.ndb_Longitude);
			set_uint(row + row_len + satr_len, 0, 15, 16, fix_id);
			set_uint(row + row_len, 0, 1, 1, 1);//ndb_exist
			satr_len += 2;
		}


		/*
		if (y.arc_exist) {
			int fix_id = find_fix(4, y.arc_ident, y.arc_Latitude, y.arc_Longitude);
			set_uint(row + row_len + satr_len, 0, 15, 16, fix_id);
			set_uint(row + row_len, 0, 10, 1, 1);//arc_exist
			satr_len += 2;
		}
		*/


		/*
		* p4
		*/


		if (y.cas_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 0);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.CAS);
			satr_len += 2;
		}



		if (y.time_exist && !(p_distance_time_Exist && !y.dist_exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 1);
			set_uint(row + row_len + satr_len, 0, 11, 12, (int)(y.time * 60));
			satr_len += 2;
		}


		if (y.dist_exist && !(p_distance_time_Exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 2);
			set_uint(row + row_len + satr_len, 0, 11, 12, y.dist);
			satr_len += 2;
		}

		if (y.radial_exist && !(p_radial_exist)) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 3);
			set_double(row + row_len + satr_len, 0, 12, 13, 0.010986328, y.Radial);
			satr_len += 2;
		}


		if (y.fpa_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 4);
			set_double(row + row_len + satr_len, 0, 11, 12, 0.087890625, y.FPA);
			satr_len += 2;
		}


		if (y.gcsa_exist) {
			set_uint(row + row_len + satr_len, 0, 15, 4, 6);
			set_double(row + row_len + satr_len, 0, 11, 12, 10.0, y.GSCA);
			satr_len += 2;
		}

		/*
		*end of fix
		*/


		if (y.last_fix)
			set_uint(row + row_len, 1, 15, 4, 0);
		else
			set_uint(row + row_len, 1, 15, 4, satr_len / 2);


		row_len += satr_len;
		
	}


	if (x.header.last_proc)
	{
		set_uint(row, 3, 14, 15, 0);
		row_len += 2;
	}
	else {
		set_uint(row, 3, 14, 15, row_len / 2);
	}

	return row_len;
}

void extonemeg_db::create_procedure()
{
	int row_len;
	int i = 0;
	this->proc_table = new procedure_table();
	unsigned char * row = new unsigned char[512];

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
				int air_recordIndex  = airports_map[x.header.Airport].recordIndex;

				set_uint(this->airport_table->get_row(air_recordIndex), 5, 7, 8, (this->proc_table->cur_pos/2) % 256);//lsb
				set_uint(this->airport_table->get_row(air_recordIndex), 6, 11, 12, (this->proc_table->cur_pos / 2) / 256);//msb
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

void extonemeg_db::create_custom_procedure()
{
	int row_len;
	this->custom_proc_table = new procedure_table();
	unsigned char * row = new unsigned char[512];
	unsigned char * row_total = new unsigned char[20*512 + 4];
	int row_total_ptr = 0;
	int row_cnt = 0;

	if (this->custom_procedures.size() > 0)
	{
		for(int i = 0 ; i < custom_procedures.size(); i++)
		{
			procedure x = custom_procedures[i];
			memset(row, 0, 512);

			if (this->custom_proc_table->cur_pos > (2 * 1024 * 1024))
				continue;

			if (x.header.first_proc)
			{
				int air_recordIndex = airports_map[x.header.Airport].recordIndex;
				set_uint(this->airport_table->get_row(air_recordIndex), 0, 13, 1, 1);//procedure_exist

			}

			row_len = this->proc_set_row(row, x);
			/*
			for (int j = 0; j < row_len; j++) {
				printf("%02x, ", row[j]);
				if (((j + 1) % 32) == 0 && (j != (row_len - 1)))
					printf("\r\n");
			}
			printf("\r\n");
			*/


			memcpy(row_total + row_total_ptr + 4, row, row_len);
			row_total_ptr += row_len;

			if (x.header.last_proc)
			{
				int air_recordIndex = airports_map[x.header.Airport].recordIndex;
				set_uint(row_total, 0, 15, 16, air_recordIndex);

				if (i != (procedures.size() - 1))
					set_uint(row_total, 1, 15, 16, row_total_ptr/2);
				else
					set_uint(row_total, 1, 15, 16, 0);

				custom_proc_table->add_row(row_total, row_total_ptr+4);
				//custom_proc_table->print_row(row_cnt);
				row_cnt++;
				row_total_ptr = 0;

			}


		}

	}
	delete row;
	delete row_total;
}

void extonemeg_db::create_runway()
{
	unsigned char * row = new unsigned char[runway_row_len];
	int count = 0;
	double freq;
	


	this->runway_table = new binary_table("runway", runway_row_len);
	if (this->runways.size() > 0)
	{
		for (auto const& x : runways)
		{
			memset(row, 0, runway_row_len);
			set_uint(row, 0, 15, 2, x.RevCode);

			set_uint(row, 0, 5, 6, x.ident_num);
			if (x.Ident.length() > 4)
				set_ident(row, 0, 11, 6, x.Ident.substr(4, 1));

			set_double(row, 1, 12, 13, 0.0439453125, x.MagBearing);
			set_uint(row, 2, 15, 16, x.Length);
			set_double(row, 3, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 4, 7, 24, 0.000021457672119, x.Longitude);
			set_uint(row, 6, 15, 16, x.ThrDisp);
			set_f_int(row, 7, 11, 12, 10.0, x.Elevation);


			set_uint(row, 8, 15, 1, x.ILSExist);
			if (x.ILSExist) {
				set_uint(row, 8, 14, 2, x.ILSCategory);
				set_double(row, 8, 12, 13, 0.0439453125, x.ILSBearing);
				int navaid_row = find_navaid(x.ILSNavaid, x.Latitude, x.Longitude, x.Elevation);
				set_uint(row, 9, 15, 16, navaid_row);
			}



			set_uint(row, 0, 13, 1, x.last_flag);
			

			this->runway_table->add_row(row);
			//this->navaid_table->print_row(count);

			count++;
		}
	}

	delete row;
}

void extonemeg_db::create_waypoint()
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
			set_uint(row, 0, 12, 2, x.TerminalEnroute);

			if (x.Ident.length() > 3)
			{
				set_ident(row, 1, 15, 16, x.Ident.substr(0, 3));
				set_ident(row, 0, 10, 11, x.Ident.substr(3));
			}
			else {
				set_ident(row, 1, 15, 16, x.Ident);
			}
			set_double(row, 2, 15, 24, 0.000021457672119, x.Latitude);
			set_double(row, 3, 7, 24, 0.000021457672119, x.Longitude);


			this->waypoint_table->add_row(row);
			//this->waypoint_table->print_row(count);

			count++;
		}
	}
	delete row;
}

void extonemeg_db::create_ndb()
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

void extonemeg_db::create_fuel()
{
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
			set_double(row, 4, 7, 8, 0.1, x.ReserveMax);
			set_double(row, 5, 7, 8, 0.1, x.ReserveMin);

			set_uint(row, 6, 13, 7, x.FinalTimeInf);
			set_uint(row, 6, 6, 7, x.FinalTimePre);

			set_double(row, 7, 7, 8, 0.1, x.FinalFixInf);
			this->fuelpolicy_table->add_row(row);
			//this->fuelpolicy_table->print_row(count);
		}
	}

	delete row;
}

void extonemeg_db::create_header_sector() {
	memset(header_sector, 0, sizeof(header_sector));
	header_sector[0] = 0x48;
	memcpy(&header_sector[1], dinf.tag.c_str(), dinf.tag.length());
	memcpy(&header_sector[13], dinf.database_filename.c_str(), dinf.database_filename.length());

	int file_size = total_sector_num * SECTOR_SIZE;
	header_sector[64] = (file_size >> 24) & 0xFF;
	header_sector[65] = (file_size >> 16) & 0xFF;
	header_sector[66] = (file_size >> 8) & 0xFF;
	header_sector[67] = (file_size) & 0xFF;
	header_sector[72] = 0x04; header_sector[73] = 0x0d;
}

void extonemeg_db::create_info_sector() {

	memset(info_sector, 0, SECTOR_SIZE);


	if(dinf.sequence_name.length() > 0)
		set_ident(info_sector, 0, 11, 6, dinf.sequence_name.substr(0, 1));
	if (dinf.sequence_name.length() > 1)
		set_ident(info_sector, 0, 5, 6, dinf.sequence_name.substr(1, 1));

	if (dinf.sequence_name.length() > 2)
		set_ident(info_sector, 1, 11, 6, dinf.sequence_name.substr(2, 1));
	if (dinf.sequence_name.length() > 4)
		set_ident(info_sector, 1, 5, 6, dinf.sequence_name.substr(4, 1));

	if (dinf.sequence_name.length() > 5)
		set_ident(info_sector, 2, 11, 6, dinf.sequence_name.substr(5, 1));
	if (dinf.sequence_name.length() > 6)
		set_ident(info_sector, 2, 5, 6, dinf.sequence_name.substr(6, 1));

	if (dinf.sequence_name.length() > 7)
		set_ident(info_sector, 3, 11, 6, dinf.sequence_name.substr(7, 1));
	if (dinf.sequence_name.length() > 9)
		set_ident(info_sector, 3, 5, 6, dinf.sequence_name.substr(9, 1));

	if (dinf.sequence_name.length() > 10)
		set_ident(info_sector, 4, 11, 6, dinf.sequence_name.substr(10, 1));
	if (dinf.sequence_name.length() > 11)
		set_ident(info_sector, 4, 5, 6, dinf.sequence_name.substr(11, 1));


	set_uint(info_sector, 5, 15, 4, dinf.prev_start.mon);
	set_uint(info_sector, 5, 11, 5, dinf.prev_start.day);
	set_uint(info_sector, 5, 6, 7, dinf.prev_start.year);

	set_uint(info_sector, 6, 15, 4, dinf.prev_end.mon);
	set_uint(info_sector, 6, 11, 5, dinf.prev_end.day);
	set_uint(info_sector, 6, 6, 7, dinf.prev_end.year);

	set_uint(info_sector, 7, 15, 4, dinf.cur_start.mon);
	set_uint(info_sector, 7, 11, 5, dinf.cur_start.day);
	set_uint(info_sector, 7, 6, 7, dinf.cur_start.year);

	set_uint(info_sector, 8, 15, 4, dinf.cur_end.mon);
	set_uint(info_sector, 8, 11, 5, dinf.cur_end.day);
	set_uint(info_sector, 8, 6, 7, dinf.cur_end.year);


/*
alternate:					10, 0
fuelpolicy : 10, 3
gates : 10, 4
ftltsbus : 10, 10
msa : 9, 7
*/


	if(fuelpolicies.size() > 1)
		set_uint(info_sector, 10, 3, 1, 1);


	uint16_t * info_sector16 = (uint16_t *)info_sector;
	info_sector16[118] = total_sector_num;
	info_sector16[119] = 0x2000;
	info_sector16[121] = 0x0001;
	uint32_t val = infotable_sector_loc << 8;
	info_sector16[122] = (uint16_t)((val >> 16) & 0xFFFF);
	info_sector16[123] = (uint16_t)((val) & 0xFFFF);
	info_sector16[124] = 6;
	info_sector16[125] = 0x26;


	set_uint(info_sector, 255, 15, 16, this->total_sector_num);





}

void extonemeg_db::set_sector_header(int sector_num)
{
	sector_header[5] = (unsigned char)((sector_num) & 0xFF);
	sector_header[6] = (unsigned char)(((sector_num) >> 8) & 0xFF);
}

template <typename T>
void extonemeg_db::copy_table(T * table, int &sector_num) {


	for (int i = 0; i < table->sector_num; i++)
	{
		set_sector_header(table->sector_loc + i);
		memcpy(total_data + (table->sector_loc + i) * (SECTOR_SIZE + 8), sector_header, 8);
		memcpy(total_data + (table->sector_loc + i) * (SECTOR_SIZE + 8) + 8, table->data + i * SECTOR_SIZE, SECTOR_SIZE);
		sector_num++;

	}
}

void set_info_aux(unsigned char * data, binary_table * table, int index)
{
	if (table->sector_loc > 0)
	{

		set_uint(data, index*6 + 0, 15, 1, 1);//unk flag
		set_uint(data, index * 6 + 1, 15, 16, table->row_len / 2);
		set_uint(data, index * 6 + 2, 15, 32, table->sector_loc << 8);
		set_uint(data, index * 6 + 4, 15, 32, (table->row_num));
	}
}

void extonemeg_db::create_info_table() {


	memset(info_table, 0, SECTOR_SIZE);

	set_info_aux(info_table, navaid_table, 0);
	set_info_aux(info_table, waypoint_table, 1);
	set_info_aux(info_table, airway_index_table, 2);
	set_info_aux(info_table, airway_fix_table, 3);
	set_info_aux(info_table, airport_table, 4);
	set_info_aux(info_table, runway_table, 5);
	set_info_aux(info_table, holdingpattern_table, 6);
	set_info_aux(info_table, ndb_table, 7);
	
	set_info_aux(info_table, gridmora_table, 9);

	set_info_aux(info_table, fuelpolicy_table, 28);

	//procedures
	set_uint(info_table, 10 * 6 + 0, 15, 1, 1);
	set_uint(info_table, 10 * 6 + 1, 15, 16, 0);
	set_uint(info_table, 10 * 6 + 2, 15, 32, proc_table->sector_loc << 8);
	std::cout << "proc_table->sector_loc: " << proc_table->sector_loc << std::endl;
	set_uint(info_table, 10 * 6 + 4, 15, 32, (proc_table->cur_pos >> 1));
	//

	//custom procedures
	/*
	set_uint(info_table, 24 * 6 + 0, 15, 1, 1);
	set_uint(info_table, 24 * 6 + 1, 15, 16, 0);
	set_uint(info_table, 24 * 6 + 2, 15, 32, custom_proc_table->sector_loc << 8);
	set_uint(info_table, 24 * 6 + 4, 15, 32, (custom_proc_table->cur_pos >> 1));
	*/
	//
	


}

void extonemeg_db::create_crc_sector() {

	int res1 = 0, res2 = 0;

	memset(crc_sector, 0, SECTOR_SIZE + 8);

	crc_sector[0] = 0x45;
	


	crc_func3(header_sector, 520, &res1);


	for (int i = 0; i < total_sector_num; i++)
	{
		crc_func3(total_data + i * (SECTOR_SIZE + 8), 520, &res1);
		crc_func3(total_data + i * (SECTOR_SIZE + 8) + 8, 512, &res2);
	}

	*(uint32_t *)(crc_sector + 1) = res1;
	*(uint32_t *)(crc_sector + 5) = res2;





}

void extonemeg_db::set_loc_aux(binary_table * table, bool table_exist)
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

void extonemeg_db::aggregate_data() {
	total_sector_num = 1;//header_Sector, 


	set_loc_aux(navaid_table, this->navaids.size() > 0);
	set_loc_aux(waypoint_table, this->navaids.size() > 0);
	set_loc_aux(airway_index_table, this->airways.size() > 0);
	set_loc_aux(airway_fix_table, this->airways.size() > 0);
	set_loc_aux(airport_table, this->airports.size() > 0);
	set_loc_aux(ndb_table, this->ndbs.size() > 0);
	set_loc_aux(runway_table, this->runways.size() > 0);

	if (this->procedures.size() > 0)
		proc_table->sector_loc = total_sector_num;
	else
		proc_table->sector_loc = 0;

	proc_table->sector_num = proc_table->get_padded_len() / SECTOR_SIZE;
	total_sector_num += proc_table->sector_num;

	set_loc_aux(gridmora_table, this->gridmoras.size() > 0);
	set_loc_aux(holdingpattern_table, this->holdingpatterns.size() > 0);
	set_loc_aux(fuelpolicy_table, this->fuelpolicies.size() > 0);


	/*
	if (this->custom_procedures.size() > 0)
		custom_proc_table->sector_loc = total_sector_num;
	else
		custom_proc_table->sector_loc = 0;

	custom_proc_table->sector_num = custom_proc_table->get_padded_len() / SECTOR_SIZE;
	total_sector_num += custom_proc_table->sector_num;
	*/



	infotable_sector_loc = total_sector_num;
	total_sector_num += 1; //info_table


	total_data = new unsigned char[total_sector_num * (SECTOR_SIZE + 8)];



	this->create_info_sector();
	this->create_info_table();

	/*
	*writing total data
	*/
	int sector_num = 0;


	set_sector_header(sector_num);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8), sector_header, 8);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8) + 8, info_sector, SECTOR_SIZE);
	sector_num++;
		

	copy_table(navaid_table, sector_num);
	copy_table(waypoint_table, sector_num);
	copy_table(airway_index_table, sector_num);
	copy_table(airway_fix_table, sector_num);
	copy_table(airport_table, sector_num);
	copy_table(ndb_table, sector_num);
	copy_table(runway_table, sector_num);
	copy_table(proc_table, sector_num);
	copy_table(gridmora_table, sector_num);
	copy_table(holdingpattern_table, sector_num);
	copy_table(fuelpolicy_table, sector_num);
	//copy_table(custom_proc_table, sector_num);
	


	set_sector_header(sector_num);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8), sector_header, 8);
	memcpy(total_data + sector_num * (SECTOR_SIZE + 8) + 8, info_table, SECTOR_SIZE);
	sector_num++;




	this->create_header_sector();
	this->create_crc_sector();



	/*
	*
	*/


}

void extonemeg_db::write_file()
{
	FILE * fp;
	int ret;
	fp = fopen(dinf.result_filename.c_str(), "wb");
	if (!fp) {
		std::cerr << "failed to open result file" << std::endl;
		throw (1001);
	}

	fwrite(header_sector, 1, SECTOR_SIZE + 8, fp);
	fwrite(total_data, 1, total_sector_num * (SECTOR_SIZE + 8), fp);
	fwrite(crc_sector, 1, SECTOR_SIZE + 8, fp);


	fclose(fp);

}

