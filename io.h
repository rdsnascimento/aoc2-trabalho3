#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

struct result_io {
	char * requests;
	uint32_t busy;
	uint32_t idle; 
	uint32_t cpu_usage;
};

struct param {
	uint32_t read_time;
	uint32_t write_time;
	uint32_t throughput; 
};


struct result_io * sim_io(struct param * devices, char * filename, int debug);


#endif /* _IO_H_ */
