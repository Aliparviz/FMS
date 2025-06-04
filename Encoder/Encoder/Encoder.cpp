#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <sstream>
#include <math.h> 
#include <iostream>
#include <fstream>
#include "csv_parser.h"
using namespace std;


#define SECTOR_SIZE 512
#define SECTOR_id_SIZE 8 
#define MAX_BINARY_SIZE (2097152*4)
char alphabets[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '-', '+', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void print_hex(unsigned char *data, int len, const char * desc)
{
	// printf("%s: \r\n", desc);
	for (int i = 0; i < len; i++) {
		printf("%02x, ", data[i]);
		if(((i+1) % 32) == 0  && (i != (len - 1)))
			printf("\r\n");
	}
	printf("\r\n");
}


char runway_index_to_char(int index)
{
	switch (index)
	{
	case 0: return ' ';
	case 1: return 'L';
	case 2: return 'R';
	case 3: return 'C';
	case 4: return 'B';
	case 5: return 'A';
	case 6: return 'O';
	case 7: return 'M';
	default: return 'X';
	}

}


void paththerm_num_to_string(int pathterm_num, char * pathterm) {
	switch (pathterm_num)
	{
	case 0:
		strcpy(pathterm, "AF");
		break;
	case 1:
		strcpy( pathterm, "CF");
		break;
	case 2:
		strcpy( pathterm, "DF");
		break;
	case 3:
		strcpy( pathterm, "FA");
		break;
	case 4:
		strcpy( pathterm, "IF");
		break;
	case 5:
		strcpy( pathterm, "FM");
		break;
	case 6:
		strcpy( pathterm, "HA");
		break;
	case 7:
		strcpy( pathterm, "HF");
		break;
	case 8:
		strcpy( pathterm, "HM");
		break;
	case 9:
		strcpy( pathterm, "PI");
		break;
	case 10:
		strcpy( pathterm, "TF");
		break;
	case 11:
		if (false)
			strcpy(pathterm, "VA");
		else
			strcpy(pathterm, "CA");
		
		break;
	case 12:
		if (false)
			strcpy(pathterm, "VD");
		else
			strcpy(pathterm, "CD");
		break;
	case 13:
		if (false)
			strcpy(pathterm, "VI");
		else
			strcpy(pathterm, "CI");
		break;
	case 14:
		strcpy(pathterm, "VM");
	case 15:
		if (false)
			strcpy(pathterm, "VR");
		else
			strcpy(pathterm, "CR");
		break;
	case 16:
		strcpy( pathterm, "RF");
		break;
	case 17:
		strcpy( pathterm, "FC");
		break;
	case 18:
		strcpy( pathterm, "FD");
		break;
	default:
		strcpy(pathterm, " ");
		break;
	}
}
void procedure_satr_process(uint8_t * satr_ptr, int satr_len) {
	int rnp_int = (int)(satr_ptr[3] & 0x7) * 16 + (int)(satr_ptr[2] >> 4);
	bool rnp_scale_Flag = (satr_ptr[3] & 0x8);
	double rnp = (double)rnp_int;
	if (rnp_scale_Flag)
		rnp = rnp * 0.01;
	else
		rnp = rnp * 0.1;
	int pathterm_num = (int)(satr_ptr[1] & 0xF) * 2 + (int)(satr_ptr[0] >> 7);
	char pathterm[3];
	paththerm_num_to_string(pathterm_num, pathterm);
	bool unk_flag = (satr_ptr[1] & 0x40);


	int fix_type = (int)(satr_ptr[2] & 0xF);

	bool fixtype_is_runway = (satr_ptr[0] & 0x40);
	bool alt1_exist = (satr_ptr[0] & 0x20);
	bool alt2_exist = (satr_ptr[0] & 0x10);
	bool navaid_exist = (satr_ptr[0] & 0x08);
	bool cshd_exist = (satr_ptr[0] & 0x40);
	bool ndb_exist = (satr_ptr[0] & 0x20);




/////////////////////////////////////////////////////////////////////////////
	bool p_fix_exist = false;
	bool p_cshd_exist = false;
	bool p_alt1_exist = false;
	bool p_distance_time_Exist = false;
	bool p_radial_exist = false;

	bool v8 = false;
	bool v9 = false;
	bool v10 = false;

	if ((pathterm_num == 0) || (pathterm_num == 1) || (pathterm_num == 2) || (pathterm_num == 4) || (pathterm_num == 10) || (pathterm_num == 16))
		p_fix_exist = true;

	if ((pathterm_num == 0) || (pathterm_num == 7) || (pathterm_num == 13) || (pathterm_num == 14) || (pathterm_num == 9) || (pathterm_num == 12) || (pathterm_num == 11) || (pathterm_num == 15))
		p_cshd_exist = true;

	if ((pathterm_num == 3) || (pathterm_num == 6) || (pathterm_num == 11))//FA, HA
		p_alt1_exist = true;

	if ((pathterm_num == 1) || (pathterm_num == 12) || (pathterm_num == 18))
		p_distance_time_Exist = true;

	if ((pathterm_num == 15))
		p_radial_exist = true;


	if ((pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 6) || (pathterm_num == 8) || (pathterm_num == 7) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
		v8 = true;

	//AF, VD/CD, VR/CR, CF, FM, PI, FC, FD
	if ((pathterm_num == 0) || (pathterm_num == 12) || (pathterm_num == 15) || (pathterm_num == 1) || (pathterm_num == 5) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
		v9 = true;

	if ((pathterm_num == 1) || (pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 8)  || (pathterm_num == 17) || (pathterm_num == 18))
		v10 = true;


	satr_ptr += 4;
	printf("rnp: %f, pathterm: %s, fix_type: %d, unk_flag = %d, ", rnp, pathterm, fix_type, unk_flag);
	
	int fixid_int = 0;
	int alt1label_num = 0, alt1labe2_num;
	int alt1_num = 0, alt2_num = 0;
	int nav_row = 0, ndb_row = 0;
	double cshd;
	if (p_fix_exist)
	{
		fixid_int = (int)(satr_ptr[1] & 0x1F) * 256 + (int)(satr_ptr[0]);
		satr_ptr += 2;
		printf("fixid_int = %d, ", fixid_int);
	}

	if (p_cshd_exist)
	{
		int cshd_num_int = (int)(satr_ptr[1] & 0x1F) * 256 + (int)(satr_ptr[0]);
		cshd = (double)cshd_num_int;
		cshd *= 0.0439453125;
		satr_ptr += 2;
		printf("cshd = %.2f, ", cshd);
	}

	if (p_alt1_exist)
	{
		alt1label_num = (int)(satr_ptr[1] >> 4);
		alt1_num = (int)(satr_ptr[1] & 0xF) * 256 + (int)(satr_ptr[0]);
		satr_ptr += 2;
		printf("alt1_num = %d, alt1label_num = %d, ", alt1_num, alt1label_num);
	}

	if (p_distance_time_Exist)
	{
		satr_ptr += 2;
	}

	if (p_radial_exist)
	{
		satr_ptr += 2;
	}


	if (fixtype_is_runway)
	{
		fixid_int = (int)(satr_ptr[1] & 0x1F) * 256 + (int)(satr_ptr[0]);
		satr_ptr += 2;
		printf("fixid_int = %d, ", fixid_int);

	}

	if (!v8 || fixtype_is_runway) {
		if (alt1_exist)
		{
			alt1label_num = (int)(satr_ptr[1] >> 4);
			alt1_num = (int)(satr_ptr[1] & 0xF) * 256 + (int)(satr_ptr[0]);
			satr_ptr += 2;
			printf("alt1_num = %d, alt1label_num = %d, ", alt1_num, alt1label_num);
		}
		if (alt2_exist)
		{
			alt1labe2_num = (int)(satr_ptr[1] >> 4);
			alt2_num = (int)(satr_ptr[1] & 0xF) * 256 + (int)(satr_ptr[0]);
			satr_ptr += 2;
			printf("alt2_num = %d, ", alt2_num);
		}
	}

	if (navaid_exist) {
		nav_row = (int)(satr_ptr[1] ) * 256 + (int)(satr_ptr[0]);
		satr_ptr += 2;
		printf("nav_row = %d, ", nav_row);
	}

	if (cshd_exist) {
		int cshd_num_int = (int)(satr_ptr[1] & 0x1F) * 256 + (int)(satr_ptr[0]);
		cshd = (double)cshd_num_int;
		cshd *= 0.0439453125;
		satr_ptr += 2;
		printf("cshd = %.2f, ", cshd);
	}


	printf("fixtype_is_runway: %d\r\n", fixtype_is_runway);



	
}

void parse_procedures_table(unsigned char *data, int table_size, int table_start_sector, int table_row_size)
{

	unsigned char *table_ptr = data + table_start_sector * SECTOR_SIZE;
	float lat, lon;
	int first = 1;
	int bogus = 0;
	int procedures_byte_len = 0;
	int total_len = 0;
	uint16_t trans_alt;
	uint8_t * satr_ptr;
	int header_len;
	int satr_len;

	unsigned char *row_ptr = table_ptr;
	int cnt = 0;
	int airport_num = 0;
	for (int i = 0; i < 23289; i++) {


		uint16_t ad2 = *(uint16_t *)(row_ptr + 4);
		int d02 = ad2 / 1600;
		int d12 = (ad2 % 1600) / 40;
		int d22 = (ad2 % 1600) % 40;
		char a3 = (d02 > 0) ? alphabets[d02 - 1] : ' ';
		char a4 = (d12 > 0) ? alphabets[d12 - 1] : ' ';
		char a5 = (d22 > 0) ? alphabets[d22 - 1] : ' ';

		uint16_t ad = *(uint16_t *)(row_ptr + 2);
		int d0 = ad / 1600;
		int d1 = (ad % 1600) / 40;
		int d2 = (ad % 1600) % 40;
		char a0 = (d0 > 0) ? alphabets[d0 - 1] : ' ';
		char a1 = (d1 > 0) ? alphabets[d1 - 1] : ' ';
		char a2 = (d2 > 0) ? alphabets[d2 - 1] : ' ';



		bool transAltExist = (row_ptr[7] & 0x80);
		if (transAltExist) {
			trans_alt = *(uint16_t *)(row_ptr + 8);
		}
		uint8_t runway_num = ((row_ptr[1] & 0x3) << 4) | ((row_ptr[0] & 0xF0) >> 4);
		uint8_t runway_char_index = (row_ptr[0] & 0x0F);

		char runway_str[4] = { '\0' };
		char runway_char;
		if(runway_num)
			sprintf(runway_str, "%02d%c", runway_num, runway_index_to_char(runway_char_index));
		else {
			if (runway_char_index == 5) {
				sprintf(runway_str, "%s", "ALL");
			}
			else {
				sprintf(runway_str, "%s", " ");
			}
		}
		if (transAltExist)
		{
			satr_ptr = row_ptr + 10;
			header_len = 10;
		}
		else
		{
			satr_ptr = row_ptr + 8;
			header_len = 8;
		}
		uint16_t header_row_len = *(uint16_t *)(&row_ptr[6]) & 0x7F;
		header_row_len *= 2;
		int satr_sum_len = 0;


		printf("ident:\t%c%c%c%c%c%c,\trunway:\t%s,\theader_row_len: %d\t\r\n*****************************************************************************************\r\n", a0, a1, a2, a3, a4, a5, runway_str, header_row_len);

		do {
			satr_len = (satr_ptr[3] & 0xF0) >> 4;
			if (satr_len != 0)
			{
				print_hex(satr_ptr, 2 * satr_len, "starts");
				procedure_satr_process(satr_ptr, 2 * satr_len);
			}
			satr_ptr += satr_len * 2;
			satr_sum_len += satr_len*2;
			
		} while (satr_len != 0);

		if (header_row_len != 0)
		{
			satr_len = header_row_len - header_len - satr_sum_len;
			print_hex(satr_ptr, satr_len, "starts");
			procedure_satr_process(satr_ptr, satr_len);
			satr_sum_len += satr_len;
			satr_ptr += satr_len;
			
		}
		else {
			satr_len = 0;
			uint16_t * satr16 = (uint16_t *)satr_ptr;
			int j = 0;
			while (satr16[j++] != 0) {
				satr_len += 2;
			}
			satr_len += 2;
			print_hex(satr_ptr, satr_len, "starts");
			procedure_satr_process(satr_ptr, satr_len);
			satr_sum_len += satr_len;
			satr_ptr += satr_len;
			
		}
		
		row_ptr = satr_ptr;

		



		
		//print_hex(satr_ptr, 2*header_row_len - header_len, "starts");
		if (header_row_len == 0)
		{
			airport_num++;
			if(airport_num == 4)
				break;
		}
			



	}



}


void parse_airports_table(unsigned char *data, int table_size, int table_start_sector, int table_row_size)
{
	unsigned char *table_ptr = data + table_start_sector * SECTOR_SIZE;
	char ident[5];
	int mav_var_msb = 0, mav_var_lsb = 0;


	for (int i = 0; i < table_size; i++) {
		unsigned char *row_ptr = table_ptr + i * table_row_size;
		char a3 = alphabets[*(row_ptr + 0) - 1];
		uint16_t ad = *(uint16_t *)(row_ptr + 2);
		int d0 = ad / 1600;
		int d1 = (ad % 1600) / 40;
		int d2 = (ad % 1600) % 40;
		char a0 = alphabets[d0 - 1];
		char a1 = alphabets[d1 - 1];
		char a2 = alphabets[d2 - 1];
		snprintf(ident, sizeof(ident), "%c%c%c%c", a0, a1, a2, a3);

		mav_var_msb = (int)row_ptr[11];
		mav_var_lsb = (int)row_ptr[13],
		printf("%s: %02x|%02x\r\n", ident, mav_var_msb, mav_var_lsb);
	}

}

int decode(const char * filename)
{
	int ret;
	int sector_num = 0;
	unsigned char file_hdr[SECTOR_SIZE];
	unsigned char file_hdr_sector_i[SECTOR_id_SIZE];
	unsigned char sector_id[SECTOR_id_SIZE];
	unsigned char sector[SECTOR_SIZE + SECTOR_id_SIZE];
	unsigned char *data = (unsigned char * ) malloc(4096 * 1024);
	FILE * fp;
	
	std::cout << "Hello World!\n"; 

	fp = fopen(filename, "rb");
	if (!fp) {
		printf("failed to open binary file\r\n");
		return -1;
	}

	ret = fread(file_hdr, 1, SECTOR_SIZE, fp);
	fread(file_hdr_sector_i, 1, SECTOR_id_SIZE, fp);
	print_hex(file_hdr, SECTOR_SIZE, "hdr");


	int i = 0;
	while (fread(sector, 1u, 520u, fp) == 520)
	{
		memcpy(data + i*SECTOR_SIZE, sector + 8, 512);
		i++;
	}

	//parse_airports_table(data, 0x05e5, 0x183a, 24);
	parse_procedures_table(data, 0, 0x0617, 0);

	return 0;

}


unsigned char reverse_byte(unsigned char x)
{
	static const unsigned char table[] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
	};
	return table[x];
}



int * nononemeg_crc_funtion(uint8_t * data, uint16_t len, int *crc_result)
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




int * nononemeg_crc_funtion_reverse(uint8_t * data, uint16_t len, int *crc_result)
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
			v4 = *(uint16_t *)(data + 2 * ((len/2)-v7-1));
			uint32_t v4a = reverse_byte(v4 & 0xFF);
			uint32_t v4b = reverse_byte((v4 >> 8) & 0xFF);
			uint32_t v4_r = ((v4a << 8) & 0xFF00) | (v4b & 0XFF);
			//uint32_t v4_r = ((v4b << 8) & 0xFF00) | (v4a & 0XFF);
			//printf("%d: %04x|%04x\r\n", ((len / 2) - v7 - 1), v4, v4_r);
			v4 = v4_r;
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
				v6 = (v9 + 2 * v6) ^ 0xE0000200;
			}
		LABEL_9:
			++v7;
		} while (v7 < len >> 1);
		result = crc_result;
		*crc_result = v6;
	}
	return result;
}

int  nononemeg_check_crc_(int *cnt, uint8_t * data, uint16_t len, int *result_crc)
{
	char v4; // cl
	char v6; // [esp+10h] [ebp-10h]

	if (*(int32_t *)(cnt))
	{
		nononemeg_crc_funtion(data, len, result_crc);
	}
	else
	{
		v4 = *(uint8_t *)(data + 1);
		*(uint8_t *)(data + 1) = v4 | 0x80;
		v6 = v4;
		nononemeg_crc_funtion(data, len, result_crc);
		*(uint8_t *)(data + 1) = v6;
	}
	++*(int32_t *)(cnt);
	return 0;
}

void non_decode_waypoints(unsigned char * file, int start_Sector, int num)
{
	unsigned char *data = file + start_Sector * 512;
	for (int i = 0; i < 240; i++)
	{
		unsigned char *row = data + i * waypoint_row_len;
		printf("%02x\r\n", i);
		

		uint16_t ad2 = *(uint16_t *)(row + 2);
		int d02 = ad2 / 1600;
		int d12 = (ad2 % 1600) / 40;
		int d22 = (ad2 % 1600) % 40;
		char a0 = (d02 > 0) ? alphabets[d02 - 1] : ' ';
		char a1 = (d12 > 0) ? alphabets[d12 - 1] : ' ';
		char a2 = (d22 > 0) ? alphabets[d22 - 1] : ' ';

		uint16_t ad = *(uint16_t *)(row) & 0x7FF;
		int d1 = (ad) / 40;
		int d2 = (ad) % 40;
		char a3 = (d1 > 0) ? alphabets[d1 - 1] : ' ';
		char a4 = (d2 > 0) ? alphabets[d2 - 1] : ' ';

		printf("%c%c%c%c%c: \r\n", a0, a1, a2, a3, a4);
		print_hex(row, waypoint_row_len, "waypoint_row");


		uint32_t lat_d = ((uint32_t)(*((uint16_t *)row + 2) & 0xFFFF) << 8) | ((uint32_t)(*((uint16_t *)row + 3) & 0xFF00) >> 8);

		printf("%08x\r\n", lat_d);


		
	}
}

void non_decode_proc(unsigned char * data, int start_Sector, int proc_num)
{
	unsigned char *proc_data = data + start_Sector * 512;
	//printf("%02x,%02x,%02x,%02x\r\n", proc_data[0], proc_data[1], proc_data[2], proc_data[3]);
	uint16_t proc_len;
	//for (int i = 0; i < 60; i++)
	unsigned char * satr_ptr;
	for (int i = 0; i < 75; i++)
	{
		do
		{
			uint16_t ad2 = *(uint16_t *)(proc_data + 4);
			int d02 = ad2 / 1600;
			int d12 = (ad2 % 1600) / 40;
			int d22 = (ad2 % 1600) % 40;
			char a3 = (d02 > 0) ? alphabets[d02 - 1] : ' ';
			char a4 = (d12 > 0) ? alphabets[d12 - 1] : ' ';
			char a5 = (d22 > 0) ? alphabets[d22 - 1] : ' ';

			uint16_t ad = *(uint16_t *)(proc_data + 2);
			int d0 = ad / 1600;
			int d1 = (ad % 1600) / 40;
			int d2 = (ad % 1600) % 40;
			char a0 = (d0 > 0) ? alphabets[d0 - 1] : ' ';
			char a1 = (d1 > 0) ? alphabets[d1 - 1] : ' ';
			char a2 = (d2 > 0) ? alphabets[d2 - 1] : ' ';


			int vianum = (proc_data[1] >> 3) & 0x7;


			proc_len = *(uint16_t *)(proc_data + 6);

			//satr_ptr = ()proc_data

			bool transalt_exit = proc_data[0] & 0x01;


			satr_ptr = (transalt_exit) ? (proc_data + 10) : (proc_data + 8);


			printf("%c%c%c%c%c%c: %d | vianum = %d\r\n", a0, a1, a2, a3, a4, a5, proc_len, vianum);
			//print_hex(proc_data, proc_len * 2, "proc_data");












			bool last = false;




			while (!last)
			{


				//
				//for each satr

				int pathterm_num = (((uint16_t *)(satr_ptr))[0] >> 6) & 0x1F;
				last = satr_ptr[0] & 0x01;
				bool path_select = satr_ptr[0] & 0x02;
				bool fix_exist = satr_ptr[0] & 0x20;
				bool alt1_exist = satr_ptr[0] & 0x10;
				bool alt2_exist = satr_ptr[0] & 0x08;
				bool field_17 = satr_ptr[0] & 0x04;

				//printf("satr start: %02x,%02x\r\n", satr_ptr[0], satr_ptr[1]);
				//printf("last: %d, fix_exist: %d, pathterm_num: %d, path_select: %d\r\n", last, fix_exist, pathterm_num, path_select);


				satr_ptr += 2;


				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				bool p_fix_exist = false;
				bool p_cshd_exist = false;
				bool p_alt1_exist = false;
				bool p_distance_time_Exist = false;
				bool p_radial_exist = false;



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

				bool v56 = false;
				bool v57 = false;
				bool v58 = false;


				if ((pathterm_num == 0) || (pathterm_num == 12) || (pathterm_num == 15) || (pathterm_num == 1) || (pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
					v56 = true; //no mandatory fix exist

				if ((pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 6) || (pathterm_num == 8) || (pathterm_num == 7) || (pathterm_num == 9) || (pathterm_num == 17) || (pathterm_num == 18))
					v57 = true;


				if ((pathterm_num == 1) || (pathterm_num == 3) || (pathterm_num == 5) || (pathterm_num == 6) || (pathterm_num == 8) || (pathterm_num == 17) || (pathterm_num == 18))
					v58 = true;




				if (p_fix_exist)
					satr_ptr += 2;

				if (p_cshd_exist)
					satr_ptr += 2;

				if (p_alt1_exist)
					satr_ptr += 2;

				if (p_distance_time_Exist)
					satr_ptr += 2;

				if (p_radial_exist)
					satr_ptr += 2;


				if (v57 && fix_exist)
					satr_ptr += 2;

				if (alt1_exist)
					satr_ptr += 2;

				if (alt2_exist)
					satr_ptr += 2;


				if (v56 && field_17)
					satr_ptr += 2;


				if (v58 && path_select)
					satr_ptr += 2;


				//printf("%d|%d|%d|%d|%d|\r\n", v57 & fix_exist, alt1_exist, alt2_exist, v56 && field_17, v58 & path_select);



				//printf("satr end: %02x,%02x\r\n", satr_ptr[0], satr_ptr[1]);
				while ((satr_ptr[1] & 0x80) && (satr_ptr != proc_data + proc_len * 2)) {
					satr_ptr += 2;
					//printf("adding not necessary\r\n");
				}



				//printf("we reached here: %02x, %02x\r\n", satr_ptr[0], satr_ptr[1]);
			}


			if (satr_ptr != (proc_data + proc_len * 2))
			{
				printf("***** something is wrong*************** %08x | %08x\r\n", satr_ptr, (proc_data + proc_len * 2));
				if (satr_ptr[0] != 0 || satr_ptr[1] != 0)
					printf("real problem\r\n");
			}




			//print_hex(proc_data, satr_ptr - (proc_data + proc_len * 2) + 4, "proc_data");












			//















			if (proc_len != 0)
				proc_data = proc_data + proc_len * 2;
			else
				proc_data = satr_ptr + 2;






		} while (proc_len != 0);
	}

}

int non_decode(const char * filename)
{
	int ret;
	unsigned char const_ff;
	unsigned sec0_hdr[7], sec1_hdr[7];
	unsigned char sector0[1024];
	unsigned char *data = (unsigned char *)malloc(4096 * 1024);
	FILE * fp;

	std::cout << "Hello World!\n";

	fp = fopen(filename, "rb");
	if (!fp) {
		printf("failed to open binary file\r\n");
		return -1;
	}

	fread(sec0_hdr, 1, 7, fp);
	fread(sector0, 1, 512, fp);
	fread(&const_ff, 1, 1, fp);

	fread(sec1_hdr, 1, 7, fp);
	fread(sector0 + 512, 1, 512, fp);
	fread(&const_ff, 1, 1, fp);







	//print_hex(sector0, 512, "sector0");
	printf("\r\n********************\r\n");
	//print_hex(sector0 + 512, 512, "sector1");

	uint16_t sector_num = *(uint16_t *)(sector0 + 118 * 2);
	printf("sector_num: %04x\r\n", sector_num);



	char serial_number[11] = { '\0' };
	for (int i = 0; i < 5; i++)
	{

		uint16_t ad = *(uint16_t *)(sector0 + i*2);
		int d0 = (ad >> 6) & 0x3F;
		int d1 = (ad) & 0x3F;
		
		serial_number[2 * i] = (d0 > 0) ? alphabets[d0 - 1] : ' ';
		serial_number[2 * i + 1] = (d1 > 0) ? alphabets[d1 - 1] : ' ';		
	}
	printf("serial_number: %s\r\n", serial_number);
	for (int i = 0; i < 4; i++)
	{
		uint16_t t = *(uint16_t *)(sector0 + 10 +  i * 2);
		uint8_t d1 = (t >> 12) & 0xF;
		uint8_t d2 = (t >> 7) & 0x1F;
		uint8_t d3 = (t) & 0x7F;
		printf("date: %d-%d-%d\r\n", d1, d2, d3);
	}



	uint16_t df;

	df = *(uint16_t *)(sector0 + 9*2);
	bool flag1 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table1 = df & 0xFFF;
	printf("table1: %d|%04x\r\n", flag1, table1);
	//
	df = *(uint16_t *)(sector0 + 10 * 2);
	bool flag2 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table2 = df & 0xFFF;
	printf("table2: %d|%04x\r\n", flag2, table2);
	//
	df = *(uint16_t *)(sector0 + 11 * 2);
	bool flag3 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table3 = df & 0xFFF;
	printf("table3: %d|%04x\r\n", flag3, table3);
	//
	df = *(uint16_t *)(sector0 + 12 * 2);
	bool flag4 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table4 = df & 0xFFF;
	printf("table4: %d|%04x\r\n", flag4, table4);
	//
	df = *(uint16_t *)(sector0 + 13 * 2);
	bool flag5 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table5 = df & 0xFFF;
	printf("table5: %d|%04x\r\n", flag5, table5);
	//
	df = *(uint16_t *)(sector0 + 14 * 2);
	bool flag6 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table6 = df & 0xFFF;
	printf("table6: %d|%04x\r\n", flag6, table6);
	//
	df = *(uint16_t *)(sector0 + 15 * 2);
	bool flag7 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table7a = df & 0xFFF;
	df = *(uint16_t *)(sector0 + 16 * 2);
	uint16_t table7b = df & 0xFFF;
	printf("table6: %d|%04x-%04x\r\n", flag6, table7a, table7b);
	//
	df = *(uint16_t *)(sector0 + 17 * 2);
	bool flag8 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table8a = df & 0xFFF;
	df = *(uint16_t *)(sector0 + 18 * 2);
	uint16_t table8b = df & 0xFFF;
	printf("table8: %d|%04x-%04x\r\n", flag6, table8a, table8b);
	//
	//
	//
	df = *(uint16_t *)(sector0 + 506 * 2);
	bool flag9 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table9 = df & 0xFFF;
	printf("table9: %d|%04x\r\n", flag9, table9);
	//
	df = *(uint16_t *)(sector0 + 505 * 2);
	bool flag10 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table10 = (df & 0xFFF)*256;
	printf("table10: %d|%04x\r\n", flag10, table10);
	//
	df = *(uint16_t *)(sector0 + 507 * 2);
	bool flag11 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table11 = (df & 0xFFF);
	printf("table11: %d|%04x\r\n", flag11, table11);
	//
	df = *(uint16_t *)(sector0 + 509 * 2);
	bool flag12 = ((df >> 12) & 0x1) ? true : false;
	uint16_t table12 = (df & 0xFFF);
	printf("table12: %d|%04x\r\n", flag12, table12);
	//
	uint8_t unk0 = (*(uint16_t *)(sector0 + 470 * 2)) >> 8;
	printf("unk0: %02x\r\n", unk0);
	uint8_t unk1 = (*(uint16_t *)(sector0 + 470 * 2)) & 0xFF;
	printf("unk1: %02x\r\n", unk1);


	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
	print_hex(sector0 + 38, 56, "disputed");
	int l = 0;
	while(1)
	{
		uint16_t ad1 = *(uint16_t *)(sector0 + (19 + l*2) * 2);
		uint16_t ad2 = *(uint16_t *)(sector0 + (20 + l*2) * 2);
		if ((ad1 == 0) && (ad2 == 0))
			break;
		int d02 = ad2 / 1600;
		int d12 = (ad2 % 1600) / 40;
		int d22 = (ad2 % 1600) % 40;
		char a2 = ((ad1 & 0x3F) > 0) ? alphabets[(ad1 & 0x3F) - 1] : ' ';
		char a3 = (d02 > 0) ? alphabets[d02 - 1] : ' ';
		char a4 = (d12 > 0) ? alphabets[d12 - 1] : ' ';
		char a5 = (d22 > 0) ? alphabets[d22 - 1] : ' ';
		printf("%c%c%c%c\r\n", a2, a3, a4, a5);
		l++;
	}
	printf("*********************************\r\n");
	l = 0;
	while (1)
	{
		uint16_t ad1 = *(uint16_t *)(sector0 + (256 + l * 2) * 2);
		uint16_t ad2 = *(uint16_t *)(sector0 + (257 + l * 2) * 2);
		if ((ad1 == 0) && (ad2 == 0))
			break;
		int d02 = ad2 / 1600;
		int d12 = (ad2 % 1600) / 40;
		int d22 = (ad2 % 1600) % 40;
		printf("*: %d\r\n", (ad1 & 0x3F));
		char a2 = ((ad1 & 0x3F) > 0) ? alphabets[(ad1 & 0x3F) - 1] : ' ';
		char a3 = (d02 > 0) ? alphabets[d02 - 1] : ' ';
		char a4 = (d12 > 0) ? alphabets[d12 - 1] : ' ';
		char a5 = (d22 > 0) ? alphabets[d22 - 1] : ' ';
		printf("%c%c%c%c\r\n", a2, a3, a4, a5);
		l++;
	}
	*/
	int cnt = 0;
	int result_crc = 0;
	uint8_t * sector_data = (uint8_t *)malloc(sector_num*512);

	memcpy(sector_data, sector0, 1024);
	for (int i = 2; i < sector_num; i++)
	{
		fread(sec0_hdr, 1, 7, fp);
		fread(sector_data + i *512, 1, 512, fp);
		fread(&const_ff, 1, 1, fp);
	}


	non_decode_waypoints(sector_data, 0x0028, 9941);
	//non_decode_proc(sector_data, 0x00FD, 5003);


	/*

	uint8_t v4 = *(uint8_t *)(sector_data + 1);
	*(uint8_t *)(sector_data + 1) = v4 | 0x80;
	uint8_t v6 = v4;
	nononemeg_crc_funtion(sector_data, 240, &result_crc);
	nononemeg_crc_funtion(sector_data + 240, 272, &result_crc);
	*(uint8_t *)(sector_data + 1) = v6;
	cnt = 1;

	
	for(int i=1; i< sector_num; i++)
		nononemeg_check_crc_(&cnt, (uint8_t *)(sector_data + i * 512), 512, &result_crc);
	printf("result_crc: %08x\r\n", result_crc);
	*/



	/**
	*(uint8_t *)(sector_data + 1) = (*(uint8_t *)(sector_data + 1)) | 0x80;

	sector_data[240] = 0; sector_data[241] = 0; sector_data[242] = 0; sector_data[243] = 0;
	int result_crc1 = 0;
	nononemeg_crc_funtion(sector_data, 240, &result_crc1);
	int result_crc2 = 0;
	for (int i = sector_num-1; i > 0; i--)
		nononemeg_crc_funtion_reverse((uint8_t *)(sector_data + i * 512), 512, &result_crc2);
	


	nononemeg_crc_funtion_reverse((uint8_t *)(sector_data + 240), 272, &result_crc2);
	int calculated_crc = result_crc1 ^ result_crc2;
	printf("%08x, %08x, %08x\r\n", result_crc1, result_crc2, result_crc1 ^ result_crc2);
	


	//int calculated_crc = 0x1db00080;
	//printf("%02x, %02x, %02x, %02x\r\n", sector_data[240], sector_data[241], sector_data[242], sector_data[243]);


	int result_crc3;

	result_crc3 = 0;
	sector_data[240] = (calculated_crc >> 24) & 0xFF; sector_data[241] = (calculated_crc >> 16) & 0xFF; sector_data[242] = (calculated_crc >> 8) & 0xFF; sector_data[243] = (calculated_crc) & 0xFF;
	//printf("%02x, %02x, %02x, %02x\r\n", sector_data[240], sector_data[241], sector_data[242], sector_data[243]);
	for (int i = 0; i < sector_num; i++)
		nononemeg_crc_funtion((uint8_t *)(sector_data + i * 512), 512, &result_crc3);
	printf("result_crc3 = %08x\r\n", result_crc3);

	result_crc3 = 0;
	sector_data[240] = (calculated_crc >> 16) & 0xFF; sector_data[241] = (calculated_crc >> 24) & 0xFF; sector_data[242] = (calculated_crc) & 0xFF; sector_data[243] = (calculated_crc >> 8) & 0xFF;
	//printf("%02x, %02x, %02x, %02x\r\n", sector_data[240], sector_data[241], sector_data[242], sector_data[243]);
	for (int i = 0; i < sector_num; i++)
		nononemeg_crc_funtion((uint8_t *)(sector_data + i * 512), 512, &result_crc3);
	printf("result_crc3 = %08x\r\n", result_crc3);


	result_crc3 = 0;
	sector_data[240] = (calculated_crc >> 8) & 0xFF; sector_data[241] = (calculated_crc) & 0xFF; sector_data[242] = (calculated_crc >> 24) & 0xFF; sector_data[243] = (calculated_crc >> 16) & 0xFF;
	//printf("%02x, %02x, %02x, %02x\r\n", sector_data[240], sector_data[241], sector_data[242], sector_data[243]);
	for (int i = 0; i < sector_num; i++)
		nononemeg_crc_funtion((uint8_t *)(sector_data + i * 512), 512, &result_crc3);
	printf("result_crc3 = %08x\r\n", result_crc3);

	result_crc3 = 0;
	sector_data[240] = (calculated_crc) & 0xFF; sector_data[241] = (calculated_crc >> 8) & 0xFF; sector_data[242] = (calculated_crc >> 16) & 0xFF; sector_data[243] = (calculated_crc >> 24) & 0xFF;
	//printf("%02x, %02x, %02x, %02x\r\n", sector_data[240], sector_data[241], sector_data[242], sector_data[243]);
	for (int i = 0; i < sector_num; i++)
		nononemeg_crc_funtion((uint8_t *)(sector_data + i * 512), 512, &result_crc3);
	printf("result_crc3 = %08x\r\n", result_crc3);
	*/













/*
	uint16_t hope = 0;
	for (int i = 0; i < 512; i += 2)
	{
		uint16_t d = *(uint16_t *)(sector_data + 512 + i);
		printf("d: %04x\r\n", d);
		hope = d ^ hope;
	}
	printf("hope: %04x\r\n", hope);
*/
	



	






/*
	int i = 0;
	while (fread(sector, 1u, 520u, fp) == 520)
	{
		memcpy(data + i * SECTOR_SIZE, sector + 8, 512);
		i++;
	}

	//parse_airports_table(data, 0x05e5, 0x183a, 24);
	parse_procedures_table(data, 0, 0x0617, 0);
*/
	return 0;

}


int encode(const char * csv_filename, std::string prev_start, std::string prev_end, const char *tag, const char * res_filename)
{
	char file_hdr[SECTOR_SIZE];
	char file_hdr_sector_i[SECTOR_id_SIZE];
	//FILE * fp;
	int ret;

	hon_time ps, pe;

	ps.day = std::stoi(prev_start.substr(0, 2));
	ps.mon = month_to_int(prev_start.substr(3, 3));
	ps.year = std::stoi(prev_start.substr(7, 4));

	pe.day = std::stoi(prev_end.substr(0, 2));
	pe.mon = month_to_int(prev_end.substr(3, 3));
	pe.year = std::stoi(prev_end.substr(7, 4));



	if (strlen(tag) != 8)
	{
		cerr << "tag name must be 8 character" << endl;
		return -1;
	}


	csv_parser *csvp = new csv_parser(csv_filename, ps, pe, std::string(tag), std::string(res_filename), false);
	return 0;

}



//01-DEC-2018
int non_encode(const char * csv_filename, std::string prev_start, std::string prev_end,  const char *serial, const char * res_filename)
{
	/*
	if (strlen(serial) != 12)
	{
		cerr << "serial must be 12 character" << endl;
		throw (401);
	}*/
	hon_time ps, pe;

	ps.day = std::stoi(prev_start.substr(0, 2));
	ps.mon = month_to_int(prev_start.substr(3, 3));
	ps.year = std::stoi(prev_start.substr(7, 4)) - 85 - 1900;

	pe.day = std::stoi(prev_end.substr(0, 2));
	pe.mon = month_to_int(prev_end.substr(3, 3));
	pe.year = std::stoi(prev_end.substr(7, 4)) - 85 - 1900;

	csv_parser *csvp = new csv_parser(csv_filename, ps, pe, std::string(serial), std::string(res_filename), true);
	return 0;

}


int main(int argc, char *argv[])
{





	try {
#ifdef __linux__ 
		if(argc < 3){
			printf("wrong input, %d\r\n", argc);
			return -1;
		}

		int is_non = atoi(argv[2]);

		if(is_non == 1)
			non_encode(argv[1], argv[3], argv[4], "", "/tmp/res.bin");
		else if(is_non == 0)
			encode(argv[1], argv[3], argv[4], "161216#V", "/tmp/res.bin");
		else
		{
			cerr << "wrong argument" << endl;
			throw(401);
		}
		cout << "operation success\r\n";
#else

		//encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\12309001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");

		
		

		//decode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\11701001.001");
		//encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\11701001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");

	
		//encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\12111001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		//encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\MPN-2405-001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		//encode("C:\\Users\\amin\\Downloads\\new_test\\1\\MPN-2405-001.csv", "161216#V", "C:\\Users\\amin\\Downloads\\new_test\\1\\working_res.bin");
		//encode("C:\\Users\\amin\\Downloads\\new_test\\2\\11912001.csv", "161216#V", "C:\\Users\\amin\\Downloads\\new_test\\2\\working_res.bin");
		//encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\12309001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		//encode("C:\\Users\\amin\\Downloads\\new_test\\4\\BS2-2111-001.csv", "161216#V", "C:\\Users\\amin\\Downloads\\new_test\\4\\working_res.bin");
		//encode("C:\\Users\\amin\\Downloads\\new_test\\5\\KN1-1701-001.csv", "161216#V", "C:\\Users\\amin\\Downloads\\new_test\\5\\working_res.bin");
		//encode("C:\\Users\\amin\\Downloads\\new_test\\6\\12308001.csv", "161216#V", "C:\\Users\\amin\\Downloads\\new_test\\6\\working_res.bin");


		//non_encode("C:\\Users\\amin\\Downloads\\Avir1\\1403_07_07\\14\\ABC2501001.csv", "161216#V", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");




		//non_encode(  "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A320-FMS1-1701\\11701001.csv", "", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		non_encode("C:\\Users\\NEW\\Desktop\\11702002.csv", "08-DEC-2016", "08-DEC-2015", "", "C:\\Users\\NEW\\Desktop\\working_res.bin");
		//non_encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A320-FMS1-1703\\11703001.csv", "", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		//non_encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A306-310-1701\\11701001.csv", "", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		//non_encode("C:\\Users\\amin\\Downloads\\11\\MPN2411001.csv", "08-DEC-2016", "08-DEC-2015", " ", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");


		//non_encode("C:\\Users\\amin\\Downloads\\8\\11701001.csv", "", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");

		//non_encode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A306-310-1701\\11701001.txt", "", "C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");

		//non_decode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A320-FMS1-1701\\media1\\11701001.001");
		//non_decode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\Pair\\3\\A320-FMS1-1701\\media1\\11701001.001");
		//non_decode("C:\\Users\\amin\\Desktop\\code_mapna\\binaries\\working_res.bin");
		cout << "operation success\r\n";
#endif
	}
	catch (...) {
		cout << "operation fail!!!!\r\n";
	}
	//
	return 0;
}






