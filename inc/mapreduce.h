#ifndef __mapreduce_h__
#define __mapreduce_h__

#include "plugin.h" 

// runs the tuple (splitter, map, reduce), potentially spawning threads to parallelize the workload.
void execute(
		std::istream& in,
		std::ostream& out,
		splitter_fn* splitter,
		new_map_t* new_map,
		delete_map_t* delete_map,
		reduce_fn& reduce,
		unsigned int force_num_cores = 0,
		bool single_thread_mode = false 
		);

#endif
