#pragma once
#include <string>
#define  SECTOR_SIZE  512


class binary_table {
	std::string name;
	int cur_pos;
	const int max_size = 8192 * 1024;
public:
	int row_num;
	int row_len;
	unsigned char * data;
	int sector_num;
	int sector_loc;
	binary_table(std::string name, int row_len) {
		this->row_num = 0;
		this->row_len = row_len;
		this->cur_pos = 0;
		this->data = new unsigned char[max_size];
		memset(this->data, 0, max_size);
	}
	~binary_table() {

	}
	void print_row(int row_num) {
		unsigned char * row_ptr = &data[row_num*row_len];
		for (int i = 0; i < row_len; i++) {
			printf("%02x, ", row_ptr[i]);
			if (((i + 1) % 32) == 0 && (i != (row_len - 1)))
				printf("\r\n");
		}
		printf("\r\n");
	}

	unsigned char * get_row(int row_num) {
		return &data[row_num*row_len];
	}


	void add_row(unsigned char * row_data) {
		memcpy(data + cur_pos, row_data, row_len);
		cur_pos += row_len;
		row_num++;
	}


	int get_padded_len() {
		int len = row_num * row_len;
		int rem = len % SECTOR_SIZE;
		return (len + (SECTOR_SIZE - rem));
	}
};
