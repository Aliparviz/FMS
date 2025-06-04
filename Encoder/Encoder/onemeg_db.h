#pragma once
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cmath>
#include <limits>
#include "binary_table.h"
#include "procedure_table.h"


struct hon_time {
	int year;
	int mon;
	int day;
};


struct db_info {
	std::string database_name;
	std::string sequence_name;
	std::string database_filename;//used only in extonemeg
	std::string tag;//serial number in nononemeg
	std::string result_filename;

	//
	hon_time prev_start;
	hon_time prev_end;
	hon_time cur_start;
	hon_time cur_end;

	/*
	int airports_len;
	int airways_len;
	int airway_legs_len;
	int alternates_len;
	int airline_policy_len;
	int company_routes_len;
	int company_route_legs_len;
	int flttstbus_len;
	int fuelpolicy_len;
	int gates_len;
	int gridmora_len;
	int holdingpatterns_len;
	int msa_len;
	int navaids_len;
	int custom_navaids_len;
	int ndbs_len;
	int procedures_len;
	int procedure_legs_len;
	int custom_procedures_len;
	int custom_procedure_legs_len;
	int runways_len;
	int waypoints_len;
	int custom_waypoints_len;
	int weightVar_len;
	*/
};



struct navaid {
	std::string Ident;
	int RevCode;
	double Freq;
	int NavClass;
	std::string NavClassString;
	bool TrueNorth;
	long double Latitude;
	long double Longitude;
	double StationDeclination;
	int Elevation;
	int FigureOfMerit;
	std::string FigureOfMeritString;
	bool IsCustomized;
	double Bearing;
	double GlideSlope;
	int recordIndex;
};

struct waypoint {
	int RevCode;
	int TerminalEnroute;
	std::string TerminalEnrouteString;
	std::string Ident;
	long double Latitude;
	long double Longitude;
	bool IsCustom;
	int recordIndex;
};

struct airway_index {
	int RevCode;
	std::string Ident;
	int FixCount;
};

struct airport
{

	std::string Ident;
	int RevCode;
	bool ProceduresExist;
	bool RunwaysExist;
	bool SpeedLimitCoded;
	bool TransAltValid;
	//TrueNorth BOOL, 
	long double Latitude;
	long double Longitude;
	double MagVar;
	int Elevation;
	int SpeedLimit;
	int SpeedLimitAltitude;
	int TransAltitude;
	bool GatesExist;
	bool AlternatesExist;
	int MaxRunwayLength;
	bool SidExist;
	bool StarExist;
	bool ApproachExist;
	bool EosidExist;

	bool MagVar_Neg;
	/*
	RevState INT,
	CC TEXT,
	*/
	int recordIndex;
	//
	int RunwayPtr;
};

struct runway {
	std::string  Ident;
	int RevCode;
	double MagBearing;
	int Length;
	long double Latitude;
	long double Longitude;
	int ThrDisp;
	int Elevation;
	int ILSCategory;
	double ILSBearing;
	std::string ILSNavaid;
	bool ILSExist;
	bool FMSVFR;
	bool BiDir;
	std::string Airport;
	bool last_flag;
	/*
		MLSNavaid TEXT,
		MLSAzimuthBrng REAL, 
		IsCustomized BOOL, 
		RevState INT, 
		MLSMinGPA REAL, 
	*/	
	int ident_num;
	int recordIndex;
};

struct ndb {
	int recordIndex;
	int RevCode;
	std::string Ident;
	long double Latitude;
	long double Longitude;
	double Frequency;
};

struct gridmora {
	int recordIndex;
	int RevCode;
	long double Latitude;
	long double Longitude;
	int Altitude[30];
};

struct procedure_header {
	int RevCode;
	std::string Ident;
	std::string Airport;
	std::string ViaTypeString;
	int ViaType;
	std::string RunwayString;
	int runway_num;
	int runway_char;
	bool transalt_Exist;
	int transAlt;
	bool last_proc;
	bool first_proc;
	
};

struct procedure_fix {
	std::string fix_ident;
	int fix_type;
	double fix_Latitude;
	double fix_Longitude;

	std::string TurnDirString;
	int turn_dir;
	bool OverFly;
	bool MissedApproach;
	std::string pathterm;
	int pathterm_num;
	double RNP;
	double cs_hd;
	bool cs_hd_exist;
	int alt1_num;
	int alt1_char;
	bool alt1_exist;
	int alt2_num;
	int alt2_char;
	bool alt2_exist;
	double dist;
	bool dist_exist;
	double Radial;
	bool radial_exist;
	//
	bool nav_exist;
	std::string nav_ident;
	long double nav_Latitude;
	long double nav_Longitude;
	//
	bool ndb_exist;
	std::string ndb_ident;
	long double ndb_Latitude;
	long double ndb_Longitude;
	//
	bool arc_exist;
	std::string arc_ident;
	long double arc_Latitude;
	long double arc_Longitude;
	//////////////////////////////////////
	int CAS;
	bool cas_exist;
	double GSCA;
	bool gcsa_exist;
	double FPA;
	bool fpa_exist;
	double time;
	bool time_exist;
	//////////////////////////////////////
	bool last_fix;

	//
};

struct procedure {
	int binary_byte_len;
	int fix_num;
	procedure_header header;
	std::vector< procedure_fix> fixes;
};


struct airway_fix {
	std::string fix_ident;
	int fix_type;
	long double fix_Latitude;
	long double fix_Longitude;

};


struct airway {
	int recordIndex;
	int revcode;
	std::string Ident;
	std::vector < airway_fix> fixes;
};

struct holdingpattern {
	int RevCode;
	std::string FilxIdent;
	std::string FixTypeString;
	int FixType;
	long double Latitude;
	long double Longitude;
	int Altitude;
	int Altitude_exist;
	int AltitudeLabel;
	double InBndCourse;
	int time_length_used;
	double LegTime;
	double LegLength;
	std::string TurnDirString;
	int TurnDir;
	std::string HoldTypeString;
	int HoldType;
	int recordIndex;
};


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

/*


create table Navaids(RecordIndex INT, Key INT, 
	RevCode INT, TrueNorth BOOL,NavClass INT,NavClassString TEXT,FigureOfMerit INT,FigureOfMeritString TEXT,Ident TEXT,Latitude REAL,Longitude REAL,
	Frequency REAL,Elevation INT,StationDeclination REAL,IsCustomized BOOL,RevState INT,Bearing REAL,GlideSlope REAL,CC TEXT);

create table Waypoints(RecordIndex INT, Key INT, RevCode INT, TerminalEnroute INT,TerminalEnrouteString TEXT,Ident TEXT,Latitude REAL,Longitude REAL,IsCustomized BOOL,RevState INT,CC TEXT);

create table AirwayIndexes(RecordIndex INT, Key INT, Airway TEXT,
	Extended BOOL,FixCount INT,FixRadTurnAlt INT,FRTAltAbove REAL,FRTAltBelow REAL,RevCode INT,RevState INT);

create table AirwayFixes(RecordIndex INT, Key INT, Airway TEXT, ParentKey INT,
	FixType INT,FixTypeString TEXT,Fix TEXT,Latitude REAL,Longitude REAL,RNP REAL,AwyFixIndex INT,RevCode INT,RevState INT,CTR REAL);


create table Airports(RecordIndex INT, Key INT, Ident TEXT, RevCode INT,ProceduresExist BOOL,RunwaysExist BOOL,SpeedLimitCoded BOOL,TransAltValid BOOL,TrueNorth BOOL,Latitude REAL,Longitude REAL,
	MagVar INT,Elevation INT,SpeedLimit INT,SpeedLimitAltitude INT,TransAltitude INT,GatesExist BOOL,AlternatesExist BOOL,MaxRunwayLength INT,RevState INT,CC TEXT,SidExist TEXT,
	StarExist TEXT,ApproachExist TEXT,EosidExist TEXT);

create table Runways(RecordIndex INT, Key INT, Airport TEXT, 
Ident TEXT,RevCode INT,TrueNorth BOOL,MagBearing REAL,Length INT,Latitude REAL,Longitude REAL,ThrDisp INT,Elevation INT,ILSCategory INT,ILSBearing INT,
	MLSNavaid TEXT,MLSAzimuthBrng REAL,IsCustomized BOOL,RevState INT,MLSMinGPA REAL,FMSVFR BOOL,ILSNavaid TEXT,BiDir BOOL)

create table NDBs(RecordIndex INT, Key INT, RevCode INT, Ident TEXT, Latitude REAL,Longitude REAL,Frequency REAL,RevState INT,CC TEXT)


create table ProcedureHeaders(RecordIndex INT,Key INT,Airport TEXT,
 Ident TEXT,BaseProcedure TEXT,Runway TEXT,TransAltitude INT,RevCode INT,ViaType INT,ViaTypeString TEXT,
 IsCustomized BOOL,Extended BOOL,RevState INT);


 create table Procedures(RecordIndex INT,Key INT,ParentKey INT,Airport TEXT,
 Ident TEXT,BaseProcedure TEXT,Runway TEXT,TransAltitude INT,RevCode INT,ViaType INT,ViaTypeString TEXT,IsCustomized BOOL,Extended BOOL,
 Fix TEXT,FixLatitude REAL,FixLongitude REAL,FixType INT,FixTypeString TEXT,
 TurnDir INT,TurnDirString TEXT,PathTerm TEXT,OverFly BOOL,
 MissedApproach BOOL,Course REAL,Heading REAL,Radial REAL,Alt1Lable INT,Alt2Lable INT,Alt1 INT,Alt2 INT,CombinedAlt1 TEXT,CombinedAlt2 TEXT,Distance REAL,Time REAL,
 Navaid TEXT,NavaidFrequency REAL,NavaidLatitude REAL,NavaidLongitude REAL,NavaidDeclination REAL,NDB TEXT,NDBFrequency REAL,NDBLatitude REAL,
 NDBLongitude REAL,ArcCtr TEXT,ArcCtrLatitude REAL,ArcCtrLongitude REAL,FPA REAL,RNP REAL,GSCA REAL,CondAlt REAL,ARC BOOL,ProcIndex BOOL,RevState INT,CAS REAL,FAF BOOL);


 create table  HoldingPatterns(RecordIndex INT,Key INT,Fix TEXT,RevCode INT,HoldType INT,FixType INT,FixTypeString TEXT,Latitude REAL,Longitude REAL,
 Altitude INT,AltCombined TEXT,AltitudeLabel INT,InBndCourse REAL,LegLength REAL,LegTime REAL,TrueNorth BOOL,TurnDir INT,TurnDirString TEXT,RevState INT);

 create table GridMORA(RecordIndex INT,Key INT,RevCode INT,Latitude REAL,Longitude REAL,Altitude1 INT,Altitude2 INT,Altitude3 INT,Altitude4 INT,Altitude5 INT,Altitude6 INT,Altitude7 INT,
 Altitude8 INT,Altitude9 INT,Altitude10 INT,Altitude11 INT,Altitude12 INT,Altitude13 INT,Altitude14 INT,Altitude15 INT,Altitude16 INT,Altitude17 INT,Altitude18 INT,Altitude19 INT,
 Altitude20 INT,Altitude21 INT,Altitude22 INT,Altitude23 INT,Altitude24 INT,Altitude25 INT,Altitude26 INT,Altitude27 INT,Altitude28 INT,Altitude29 INT,Altitude30 INT,RevState INT);

 create table FuelPolicy(RecordIndex INT,Key INT,RI BOOL,RA BOOL,FD BOOL,FinalAlt INT,TaxiFuel REAL,ReserveFuel REAL,ReserveMax REAL,ReserveMin REAL,FinalTimeInf INT,
 FinalTimePre INT,FinalFixInf REAL);




 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 nononemeg

create table Airports(RecordIndex INT,Key INT,Ident TEXT,RevCode INT,ProceduresExist BOOL,RunwaysExist BOOL,GatesExist BOOL,AlternatesExist BOOL,SpeedLimitCoded INT,TransAltValid BOOL,
Latitude REAL,Longitude REAL,Elevation INT,SpeedLimit INT,SpeedLimitAltitude INT,TransAltitude INT,MaxRunwayLength INT,RevState INT,TrueNorth BOOL,MagVar INT,CC TEXT,SidExist TEXT,StarExist TEXT,ApproachExist TEXT,EosidExist TEXT);



create table Runways(RecordIndex INT,Key INT,Airport TEXT,Ident TEXT,RevCode INT,RevState INT,FMSVFR BOOL,MagBearing REAL,Length INT,Latitude REAL,Longitude REAL,
ThrDisp INT,Elevation INT,ILSCategory INT,ILSBearing REAL,ILSNavaid TEXT);




create table AirwayIndexes(RecordIndex INT,Key INT,Airway TEXT,FixCount INT,RevCode INT,RevState INT);


 create table AirwayFixes(RecordIndex INT,Key INT,Airway TEXT,RevCde INT,RevState INT,ParentKey INT,
 FixType INT,FixTypeString TEXT,Fix TEXT,Latitude REAL,Longitude REAL,AwyFixIndex INT,RNP REAL,CTR REAL);


create table Alternates(RecordIndex INT,Key INT,Airport TEXT,Ident TEXT,RevCode INT,Distance REAL,
Type TEXT,RevState INT);


create table Navaids(RecordIndex INT,Key INT,RevCode INT,RevState INT,Ident TEXT,Frequency REAL,
Latitude REAL,Longitude REAL,StationDeclination REAL,Elevation INT,FigureOfMerit INT,
FigureOfMeritString TEXT,CC TEXT,NavClass INT,NavClassString TEXT,IsCustomized BOOL,
Bearing REAL,GlideSlope REAL,TrueNorth BOOL);


create table NDBs(RecordIndex INT,Key INT,RevCode INT,Ident TEXT,Latitude REAL,Longitude REAL,
Frequency REAL,RevState INT,CC TEXT);

create table Gates(RecordIndex INT,Key INT,Ident TEXT,Airport TEXT,RevCode INT,
Latitude REAL,Longitude REAL,RevState INT);


create table ProcedureHeaders(RecordIndex INT,Key INT,Airport TEXT,Ident TEXT,BaseProcedure TEXT,
Runway TEXT,TransAltitude INT,RevCode INT,ViaType INT,ViaTypeString TEXT,RevState INT,
IsCustomized BOOL);

create table Procedures(
RecordIndex INT,Key INT,Airport TEXT,Ident TEXT,BaseProcedure TEXT,Runway TEXT,
TransAltitude INT,ViaType INT,ViaTypeString TEXT,RevCode INT,Fix TEXT,FixLatitude REAL,
FixLongitude REAL,FixType INT,FixTypeString TEXT,TurnDir INT,TurnDirString TEXT,PathTerm TEXT,
Course REAL,Alt1 REAL,Alt2 REAL,CombinedAlt1 TEXT,CombinedAlt2 TEXT,Distance REAL,RevState INT,
Navaid TEXT,NDB TEXT,OverFly BOOL,MissedApproach BOOL,IsCustomized BOOL,FPA REAL,Time REAL,
NavaidFrequency REAL,NavaidDeclination REAL,NavaidLatitude REAL,NavaidLongitude REAL,
NDBFrequency REAL,NDBLatitude REAL, NDBLongitude REAL,Alt1Lable INT,Alt2Lable INT,
Radial REAL,CAS REAL,GSCA REAL,ARC TEXT);



create table Waypoints(RecordIndex INT,Ident TEXT,Key INT,RevCode INT,RevState INT,Latitude REAL,
Longitude REAL,TerminalEnroute INT,TerminalEnrouteString TEXT,IsCustomized BOOL,CC TEXT);

create table HoldingPatterns(RecordIndex INT,Key INT,Fix TEXT,RevCode INT,FixType INT,
FixTypeString TEXT,Latitude REAL,Longitude REAL,Altitude INT,AltitudeLabel INT,AltCombined TEXT,
InBndCourse REAL,LegLength REAL,LegTime REAL,TurnDir INT,TurnDirString TEXT,RevState INT,
HoldType INT,TrueNorth BOOL,ArcRNP REAL,ArcRadius REAL);

create table FuelPolicy(RecordIndex INT,Key INT,RI BOOL,RA BOOL,FD BOOL,FinalAlt INT,TaxiFuel REAL,ReserveFuel REAL,ReserveMax REAL,ReserveMin REAL,FinalTimeInf INT,FinalTimePre INT,FinalFixInf REAL);


create table CompanyRouteIndexes(RecordIndex INT,Key INT,Route TEXT,RevCode INT,CostIndex INT,
Origin TEXT,OriginLatitude REAL,OriginLongitude REAL,Destination TEXT,DestinationLatitude REAL,
DestinationLongitude REAL,RevState INT)


create table CompanyRouteVias(RecordIndex INT,Key INT,ParentKey INT,Route TEXT,RevCode INT,
RevState INT,CostIndex INT,Fix TEXT,FixType INT,FixTypeString TEXT,ViaIdent TEXT,ViaType INT,
ViaTypeString TEXT,Latitude REAL,Longitude REAL,AltitudeLabel INT,Altitude INT,AltCombined TEXT,
StepAltitude INT,ViaIndex INT);


create table APFHeader(RecordIndex INT,AircraftID TEXT,EngineAirframe TEXT,EngineType TEXT,APFIdentifier TEXT,APFParentIdent TEXT,ApfSpecVersion REAL);

create table APFData(RecordIndex INT,APFParentIdent TEXT,ApfDataFieldName TEXT,Data TEXT);

create table GridMORA(RecordIndex INT,Key INT,RevCode INT,Latitude REAL,Longitude REAL,n8 INT,Altitude2 INT,Altitude3 INT,Altitude4 INT,Altitude5 INT,Altitude6 INT,Altitude7 INT,Altitude8 
INT,Altitude9 INT,Altitude10 INT,Altitude11 INT,Altitude12 INT,Altitude13 INT,Altitude14 INT,Altitude15 INT,Altitude16 INT,Altitude17 INT,Altitude18 INT,Altitude19 INT,Altitude20 INT,Altitude21 I
NT,Altitude22 INT,Altitude23 INT,Altitude24 INT,Altitude25 INT,Altitude26 INT,Altitude27 INT,Altitude28 INT,Altitude29 INT,Altitude30 INT,RevState INT);







create table CompanyRouteVias(RecordIndex INT,Key INT,ParentKey INT,Route TEXT,RevCode INT,RevState INT,CostIndex INT,Fix TEXT,FixType INT,FixTypeString TEXT,ViaIdent TEXT,
ViaType INT,ViaTypeString TEXT,Latitude REAL,Longitude REAL,AltitudeLabel INT,Altitude INT,AltCombined TEXT,StepAltitude INT,ViaIndex INT);



create table CompanyRouteIndexes(RecordIndex INT,Key INT,Route TEXT,RevCode INT,CostIndex INT,Origin TEXT,OriginLatitude REAL,OriginLongitude REAL,Destination TEXT,DestinationLatitude REAL,DestinationLongitude REAL,RevState INT);













create table A320(IsA320Database BOOL);
*/



#define navaid_row_len 14
#define waypoint_row_len 10
#define ndb_row_len 12
#define runway_row_len 28
#define airport_row_len 24
#define grid_row_len 36
#define hop_row_len 10
#define airway_index_row_len 6
#define airway_fix_row_len 10
#define fuel_row_len 16


#define non_airport_row_len 26
#define non_runway_row_len 20
#define non_airway_fix_row_len 2




class extonemeg_db {
public:
	db_info dinf;
	unsigned char sector_header[8];
	//
	int total_sector_num;
	unsigned char * total_data;
	//
	std::vector <navaid> navaids;
	std::multimap <std::string,  navaid> navaids_map;
	binary_table *navaid_table;
	//
	std::vector <waypoint> waypoints;
	std::multimap <std::string, waypoint> waypoints_map;
	binary_table *waypoint_table;
	//
	std::map <int, airway_index> airway_indexes;
	binary_table *airway_indexe_table;
	//
	std::vector <ndb> ndbs;
	std::multimap <std::string, ndb> ndbs_map;
	binary_table *ndb_table;
	//
	std::vector <runway> runways;
	std::multimap <std::string, runway> runways_map;
	binary_table *runway_table;
	//
	std::vector <airport> airports;
	std::map <std::string, airport> airports_map;
	binary_table *airport_table;
	//
	std::vector <procedure> procedures;
	procedure_table *proc_table;
	//
	std::vector <procedure> custom_procedures;
	procedure_table *custom_proc_table;
	//
	std::vector <gridmora> gridmoras;
	binary_table *gridmora_table;
	//
	std::vector <airway> airways;
	binary_table *airway_index_table;
	binary_table *airway_fix_table;
	//
	std::vector <holdingpattern> holdingpatterns;
	binary_table *holdingpattern_table;
	//
	std::vector <fuelpolicy> fuelpolicies;
	binary_table *fuelpolicy_table;
	/*
	std::map <int, airway_fixe> airway_fixes;
	binary_table *airway_fixe_table;
	int airway_fixe_sector_loc;
	int airway_fixe_sector_num;
	*/
	//
	int infotable_sector_loc;
	//


	

	extonemeg_db(db_info dinf) {
		this->dinf = dinf;
		const unsigned char sh[] = { 0x44, 0x02, 0x00, 0x02, 0x00, 0xa5, 0x16, 0x00 };
		memcpy(sector_header, sh, sizeof(sh));

	}
	

	unsigned char header_sector[SECTOR_SIZE+8];
	unsigned char info_sector[SECTOR_SIZE];
	unsigned char info_table[SECTOR_SIZE];
	unsigned char crc_sector[SECTOR_SIZE+8];
	

	void set_sector_header(int sector_num);
	template <typename T> 
	void copy_table(T * table, int &sector_num);

	void create_header_sector();
	void create_info_sector();
	void create_info_table();
	void create_crc_sector();
	void aggregate_data();
	void write_file();

	void set_loc_aux(binary_table * table, bool table_exist);

	void create_navaid();
	void create_waypoint();
	void create_ndb();
	void create_runway();
	void create_airport();
	int proc_set_row(unsigned char * row, procedure x);
	void create_procedure();
	void create_custom_procedure();
	void create_gridmora();
	void create_airway();
	void create_holdingpattern();
	void create_fuel();

	int find_navaid(std::string ident, long double lat, long double lon, int elev);
	int find_fix(int fix_type, std::string fix_ident, long double fix_lat, long double fix_lon);


};



class nononemeg_db {
public:
	db_info dinf;
	unsigned char sector_header[7];
	//const char const_ff = 0xff;
	int total_sector_num;
	unsigned char * total_data;
	unsigned char header_data[2*SECTOR_SIZE];
	//
	std::vector <navaid> navaids;
	std::multimap <std::string, navaid> navaids_map;
	binary_table *navaid_table;
	//
	std::vector <waypoint> waypoints;
	std::multimap <std::string, waypoint> waypoints_map;
	binary_table *waypoint_table;
	//
	std::vector <airport> airports;
	std::map <std::string, airport> airports_map;
	binary_table *airport_table;
	//
	std::vector <runway> runways;
	std::multimap <std::string, runway> runways_map;
	binary_table *runway_table;
	//
	std::vector <airway> airways;
	binary_table *airway_index_table;
	binary_table *airway_fix_table;
	//
	std::vector <ndb> ndbs;
	std::multimap <std::string, ndb> ndbs_map;
	binary_table *ndb_table;
	//
	std::vector <fuelpolicy> fuelpolicies;
	binary_table *fuelpolicy_table;
	//
	std::vector <procedure> procedures;
	procedure_table *proc_table;
	//
	std::vector <procedure> custom_procedures;
	procedure_table *custom_proc_table;



	nononemeg_db(db_info dinf) {
		this->dinf = dinf;
	}


	void create_navaid();
	void create_waypoint();
	void create_airport();
	void create_runway();
	void create_ndb();
	void create_airway();
	void create_fuel();
	int proc_set_row(unsigned char * row, procedure x);
	void create_procedure(int base_sector);
	void create_tables();
	//
	void create_header();
	void copy_header(int &sector_num);
	template <typename T>
	void copy_table(T * table, int &sector_num);
	void set_loc_aux(binary_table * table, bool table_exist);
	void set_sector_header(int sector_num);
	void calc_crc();
	void aggregate_data();
	void write_file();

	int find_navaid(std::string ident, long double lat, long double lon, int elev);
	int find_fix(int fix_type, std::string fix_ident, long double fix_lat, long double fix_lon);


	/*
	//
	//
	std::map <int, airway_index> airway_indexes;
	binary_table *airway_indexe_table;
	//
	std::vector <procedure> procedures;
	procedure_table *proc_table;
	//
	std::vector <procedure> custom_procedures;
	procedure_table *custom_proc_table;
	//
	std::vector <gridmora> gridmoras;
	binary_table *gridmora_table;
	//
	std::vector <holdingpattern> holdingpatterns;
	binary_table *holdingpattern_table;
	
	//
	int infotable_sector_loc;
	//




	unsigned char header_sector[SECTOR_SIZE + 8];
	unsigned char info_sector[SECTOR_SIZE];
	unsigned char info_table[SECTOR_SIZE];
	unsigned char crc_sector[SECTOR_SIZE + 8];




	void create_header_sector();
	void create_info_sector();
	void create_info_table();
	
	

	


	int proc_set_row(unsigned char * row, procedure x);
	void create_procedure();
	void create_custom_procedure();
	void create_gridmora();
	void create_holdingpattern();
	
	*/

};