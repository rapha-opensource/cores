#ifndef __read_h__
#define __read_h__

#include "plugin.h" 
#include "state_model.h" 
#include "map.h" 

class Read : public StateModel
{
private:
	std::istream& m_input_stream;
	splitter_fn* m_splitter;
	write_chunk_fn m_write_chunk;
	std::vector< Map* >& m_mappers;

	enum
	{
		extracting_chunks = 0,
		end_of_stream,
		cannot_happen,
		number_of_states
	} states;

	enum
	{
		read_chunk = 0,
		eof,
		number_of_events
	} event_id;

public:
	Read( std::istream& in, splitter_fn* splitter, std::vector<Map*>&);

	void extract_chunks(const event_base* evt);
	void broadcast_end_of_stream(const event_base* evt);

	EVENT(read_chunk);
	EVENT(eof);
	EVENT(termination);
};

#endif
