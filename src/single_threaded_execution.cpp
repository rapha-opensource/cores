#include "single_threaded_execution.h"
#include <queue> 

typedef std::queue<chunk> queue;

void single_threaded_execute( std::istream& in, std::ostream& out, splitter_fn& splitter, map_fn& map, reduce_fn& reduce)
{
	chunk input_chunk, output_chunk;
	queue input_q;
	write_chunk_fn write_input_chunk = [&input_q](const chunk& c) {
		input_q.push(c);
	};

	while( in.good() )
	{
		splitter(in, write_input_chunk);
		while( !input_q.empty() )
		{
			map(input_q.front(), output_chunk);
			reduce(output_chunk, out);
			input_q.pop();
		}

		if( in.eof() )
			break;
	}
}


