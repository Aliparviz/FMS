#include "pch.h"
#include "utils.h"
#include <iostream>
unsigned int crc_func3(unsigned char *data_in, int len, int *out)
{
	unsigned int result; // eax
	int i; // esi
	int v7; // edx
	unsigned char a1;
	int j; // ecx

	if (len > 0)
	{
		for (i = 0; i != len; ++i)
		{
			a1 = *(data_in + i);
			v7 = *out;
			for (j = 0; j != 8; ++j)
			{
				while (1)
				{
					result = (a1 & 0x80) ? 1 : 0;
					a1 = a1 << 1;
					if (v7 < 0)
						break;
					v7 = result + 2 * v7;
					if (++j == 8)
						goto LABEL_7;
				}
				v7 = (result + 2 * v7) ^ 0x400007;
			}
		LABEL_7:
			*out = v7;
		}
	}
	return len;
}

void set_uint16(unsigned char * row, int loc, int start_bit, int bit_len, uint16_t data)
{
	uint16_t * row16 = (uint16_t *)row;
	uint16_t mask = 0xFFFF;
	uint16_t one = 0x1;
	if (bit_len <= 16)
	{
		uint16_t data16 = (uint16_t)data;
		if (bit_len == 16)
			mask = 0xFFFF;
		else
			mask = (((one << bit_len) - 1) << (start_bit - bit_len + 1));

		row16[loc] &= ~mask;
		row16[loc] |= (mask & (data16 << (start_bit - bit_len + 1)));
		return;

	}
}

void set_uint(unsigned char * row, int loc, int start_bit, int bit_len, uint32_t data)
{
	uint16_t one = 0x1;
	if (bit_len <= 16)
	{

		if (start_bit >= (bit_len - 1))
			set_uint16(row, loc, start_bit, bit_len, data);
		else {
			int bit_len1 = start_bit + 1;
			int start_bit1 = start_bit;
			int bit_len2 = bit_len - bit_len1;
			int start_bit2 = 15;
			set_uint16(row, loc, start_bit1, bit_len1, data >> bit_len2);
			set_uint16(row, loc + 1, start_bit2, bit_len2, data & ((one << bit_len2) - 1));
		}
		return;

	}

	if (bit_len <= 32) {
		int bit_len1 = 16 - (16 - start_bit - 1);
		int start_bit1 = start_bit;
		int bit_len2 = bit_len - bit_len1;
		int start_bit2 = 15;
		set_uint16(row, loc, start_bit1, bit_len1, data >> bit_len2);
		set_uint16(row, loc + 1, start_bit2, bit_len2, data & ((one << bit_len2) - 1));
		return;

	}

}

int get_char_int(char in)
{
	if (in >= 65 && in <= 90)
		return in - 64;
	if (in >= 48 && in <= 57)
		return in - 19;
	if (in == '-')
		return 27;
	if (in == '+')
		return 28;
	return 0;
}

void set_ident(unsigned char * row, int loc, int start_bit, int bit_len, std::string data)
{
	const char * c = data.c_str();
	uint16_t data16 = 0;
	if (bit_len == 16)
	{
		int char_num = data.length();

		if (char_num > 3)
		{
			std::cerr << "unsupported string converssion " << std::endl;
			throw (1001);
		}

		if (char_num == 3)
			data16 = get_char_int(c[0]) * 1600 + get_char_int(c[1]) * 40 + get_char_int(c[2]);
		if (char_num == 2)
			data16 = get_char_int(c[0]) * 40 + get_char_int(c[1]);
		if (char_num == 1)
			data16 = get_char_int(c[0]);

		return set_uint(row, loc, start_bit, bit_len, data16);
	}

	if (bit_len == 11)
	{
		int char_num = data.length();

		if (char_num > 2)
		{
			std::cerr << "unsupported string converssion " << std::endl;
			throw (1001);
		}

		if (char_num == 2)
			data16 = get_char_int(c[0]) * 40 + get_char_int(c[1]);
		if (char_num == 1)
			data16 = get_char_int(c[0]);

		return set_uint(row, loc, start_bit, bit_len, data16);
	}

	if (bit_len == 6)
	{
		int char_num = data.length();

		if (char_num > 1)
		{
			std::cerr << "unsupported string converssion " << std::endl;
			throw (1001);
		}

		if (char_num == 1)
			data16 = get_char_int(c[0]);

		return set_uint(row, loc, start_bit, bit_len, data16);
	}


}

void set_double(unsigned char * row, int loc, int start_bit, int bit_len,  double coeff,  double data, bool print)
{
	 double d = data / coeff;


	bool neg = false;
	if (d < 0) {
		neg = true;
		d = -d;
	}

	int32_t d_int = (int32_t)d;





	if ((d - floor(d)) > 0.5)
		d_int += 1;
	/*
	if ((ceil(d) - d) < coeff && (ceil(d) - d) != 0)
		d_int += 1;

	if ((d - floor(d)) < coeff && (d - floor(d)) != 0)
		d_int -= 1;
	*/


	//printf("%08x\r\n", d_int);
	if (neg)
		d_int = -d_int;

	return set_uint(row, loc, start_bit, bit_len, d_int);


	return;

}


int32_t double_to_int(long double coeff,  double data)
{
	long double d = data / coeff;
	bool neg = false;
	if (d < 0) {
		neg = true;
		d = -d;
	}
	int32_t d_int = (int32_t)d;


	if ((d - floor(d)) > 0.5)
		d_int += 1;

	/*
	if ((ceil(d) - d) < coeff && (ceil(d) - d) != 0)
		d_int += 1;

	if ((d - floor(d)) < coeff && (d - floor(d)) != 0)
		d_int -= 1;
	*/

	if (neg)
		d_int = -d_int;
	return d_int;

}

void set_f_int(unsigned char * row, int loc, int start_bit, int bit_len, double coeff, int32_t data)
{
	int32_t d = round((double)data / coeff);
	return set_uint(row, loc, start_bit, bit_len, d);


	return;

}