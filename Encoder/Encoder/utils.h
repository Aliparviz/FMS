#pragma once
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cmath>
#include <limits>


unsigned int crc_func3(unsigned char *data_in, int len, int *out);
void set_uint16(unsigned char * row, int loc, int start_bit, int bit_len, uint16_t data);
void set_uint(unsigned char * row, int loc, int start_bit, int bit_len, uint32_t data);
int get_char_int(char in);
void set_ident(unsigned char * row, int loc, int start_bit, int bit_len, std::string data);
void set_double(unsigned char * row, int loc, int start_bit, int bit_len,  double coeff,  double data, bool print = false);
int32_t double_to_int(long double coeff,  double data);
void set_f_int(unsigned char * row, int loc, int start_bit, int bit_len, double coeff, int32_t data);