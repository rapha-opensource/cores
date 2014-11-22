#ifndef __write_h__
#define __write_h__

#include "state_model.h"
#include "plugin.h" 

class Write : public StateModel
{
private:
	enum
	{
		reduce_chunk = 0,
		count_exhausted_pipes,
		cannot_happen
	} states;

	enum
	{
		new_chunk = 0,
		no_more_data
	} event_id;

	std::ostream& m_output_stream;
	reduce_fn& m_reducer;
	unsigned int m_number_of_mappers;

public:
	Write( std::ostream&, reduce_fn&, unsigned int);
	void reduce_chunk_action( const event_base* );
	void count_exhausted_pipes_action( const event_base* );

	typedef event<new_chunk, chunk> new_chunk_event;
	SEND(new_chunk);
	EVENT(no_more_data);
	EVENT(termination);
};

#endif

