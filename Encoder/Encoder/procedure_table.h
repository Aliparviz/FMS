#pragma once
#include <string>
#include <vector>
#define  SECTOR_SIZE  512


class procedure_table {
	const int max_size = 8192 * 1024;
public:
	int cur_pos;
	int row_num;
	unsigned char * data;
	int sector_num;
	int sector_loc;
	std::vector<int> row_lens;
	std::vector<int> row_locs;

	procedure_table() {
		this->row_num = 0;
		this->cur_pos = 0;
		this->data = new unsigned char[max_size];
		memset(this->data, 0, max_size);
	}
	~procedure_table() {

	}
	void print_row(int row) {
		unsigned char * row_ptr = &data[row_locs[row]];
		for (int i = 0; i < row_lens[row]; i++) {
			printf("%02x, ", row_ptr[i]);
			if (((i + 1) % 32) == 0 && (i != (row_lens[row] - 1)))
				printf("\r\n");
		}
		printf("\r\n");
	}


	void add_row(unsigned char * row_data, int row_len) {
		row_locs.push_back(this->cur_pos);
		row_lens.push_back(row_len);
		memcpy(data + this->cur_pos, row_data, row_len);
		this->cur_pos += row_len;
		row_num++;
	}
	int get_padded_len() {
		int len = this->cur_pos;
		int rem = len % SECTOR_SIZE;
		return (len + (SECTOR_SIZE - rem));
	}
};
