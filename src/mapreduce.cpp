#include "mapreduce.h" 
#include "state_model.h" 
#include "single_threaded_execution.h" 
#include "read.h" 

#include <stdexcept> 

void execute(
		std::istream& in,
		std::ostream& out,
		splitter_fn* splitter,
		new_map_t* new_map,
		delete_map_t* delete_map,
		reduce_fn& reduce,
		unsigned int force_num_cores,
		bool single_threaded_mode)
{
	unsigned int num_cores = boost::thread::hardware_concurrency();
	printf("Max # of cores on this processor: %u\n", num_cores);
	if(force_num_cores)
	{
		printf("Forcing apparent number of cores to be: %d\n", force_num_cores);
		num_cores = force_num_cores;
	}
	if(single_threaded_mode)
	{
		printf("Running using only one thread\n");
		map_fn* map = new_map();
		single_threaded_execute(in, out, *splitter, *map, reduce);
		delete_map(map);
	}
	else
	{ 	// multithreaded execution:
		//
		// thread 0 :	splitter <= input stream
		// thread 1 :	map
		// thread 2 :	map
		//   ...	map
		// thread n :	map
		// thread n+1 :	reduce => output stream
		// 
		// where n is the number of cores.

		boost::thread_group all_threads;

		// Create and launch the Write thread
		Write write( out, reduce, num_cores);
		all_threads.create_thread( std::ref(write) );

		// Create and launch the Map threads
		std::vector< Map* > mappers;
		for( unsigned int i=0; i<num_cores; i++)
		{
			Map* map_ptr = new Map( new_map(), delete_map, &write );
			mappers.push_back(map_ptr);
			all_threads.create_thread( std::ref(*map_ptr) );
		}

		// Create and launch the Read thread
		Read read( in, splitter, mappers );
		read.send( new Read::read_chunk_event() );
		all_threads.create_thread( std::ref(read) );

		// Wait for all threads to terminate or fail
		all_threads.join_all();

		for( auto mapper : mappers )
			delete mapper;
	}
}

void order_preserving_reducer(const chunk& output_chunk, std::ostream& out)
{
	out << output_chunk.second;
}
