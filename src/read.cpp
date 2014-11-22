#include "read.h" 

#include <stdexcept> 

Read::Read( std::istream& in, splitter_fn* splitter, std::vector<Map*>& mappers)
: m_input_stream(in),
	m_splitter(splitter),
	m_mappers(mappers)
{
	// 0 : extracting_chunks
	m_state_functions.push_back( [&](event_base* evt) { extract_chunks(evt); } );
	// 1 : end_of_stream
	m_state_functions.push_back( [&](event_base* evt) { broadcast_end_of_stream(evt); } );
	// 2 : cannot_happen
	m_state_functions.push_back( [&](event_base* evt) { throw std::runtime_error("FUCK"); } );
	m_state_transitions =	{	{ extracting_chunks, end_of_stream },
					{ cannot_happen, cannot_happen } };
	m_write_chunk = [&](const chunk& c)
	{
		if(m_mappers.size())
		{
			size_t selected_mapper = c.first % m_mappers.size();
			m_mappers[selected_mapper]->send( new Map::new_chunk_event(c) );
		}
	};
}

void Read::extract_chunks(const event_base* event)
{
	if( m_input_stream.good() )
	{
		//printf("In Read!\n");
		fflush(stdout);

		(*m_splitter)(m_input_stream, m_write_chunk);

		if( m_input_stream.eof() )
		{
			//printf("reached end of input stream\n"); fflush(stdout);
			delete event;
			send_internal( new eof_event() );
		}
		else
		{
			send_internal( (read_chunk_event*)event );
		}
	}
}

void Read::broadcast_end_of_stream(const event_base* evt)
{
	//printf("in broadcast_end_of_stream\n"); fflush(stdout);
	for( Map* mapper : m_mappers )
		mapper->send( new Map::no_more_data_event() );

	delete evt;
	send_internal( new termination_event() );
}
