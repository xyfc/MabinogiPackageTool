#pragma once

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_pack_entry {
	char name[MAX_PATH];
	unsigned long seed;				// 加密种子，只有在input里的entry才有此值，output中不需要设置
	unsigned long offset;			// 数据区偏移，只有在input里的entry才有此值，output中不需要设置
	unsigned long compress_size;	// 压缩后大小，只有在input里的entry才有此值，output中不需要设置
	unsigned long decompress_size;	// 解压后大小，只有在input里的entry才有此值，output中不需要设置
	unsigned long is_compressed;	// 是否进行了压缩，只有在input里的entry才有此值，output中不需要设置
	FILETIME ft[5];
} *PPACKENTRY;

typedef struct s_pack_input_stream {
	size_t _pos;	// 当前处理的entry index

	byte *_ptr;		// 当前读取的字节指针

	byte *_buffer;	// 当前entry解压出来的数据
	FILE *_file;
	s_pack_entry *_entries;
} *PPACKINPUT;

typedef struct s_pack_output_stram {
	size_t _pos;
	byte *_base;
	byte *_ptr;
	byte *_buffer;
} *PPACKOUTPUT;


PPACKINPUT pack_input(char *file_name);
PPACKOUTPUT pack_output(char *file_name);

void pack_input_close(PPACKINPUT input);
void pack_output_close(PPACKOUTPUT output);

void pack_inpu_reset(PPACKINPUT input);
PPACKENTRY pack_input_get_next_entry(PPACKINPUT input);
size_t pack_input_read(PPACKINPUT input, byte* buffer, size_t size);

void pack_output_put_next_entry(PPACKOUTPUT output, PPACKENTRY entry);
void pack_output_write(PPACKOUTPUT output, byte* buffer, size_t size);
void pack_output_close_entry(PPACKOUTPUT output);

#ifdef __cplusplus
}
#endif