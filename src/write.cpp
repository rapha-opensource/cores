#include "write.h" 

Write:: Write( std::ostream& out, reduce_fn& reducer, unsigned int num_mappers)
	:m_output_stream(out), m_reducer(reducer), m_number_of_mappers(num_mappers)
{
	m_state_functions.push_back( [&](event_base* evt) { reduce_chunk_action(evt); } );
	m_state_functions.push_back( [&](event_base* evt) { count_exhausted_pipes_action(evt); } );

	m_state_transitions = {
		{ reduce_chunk, count_exhausted_pipes },
		{ reduce_chunk, count_exhausted_pipes } };
}

void Write::reduce_chunk_action(const event_base* evt)
{
	//printf("in reduce_chunk_action of Write\n"); fflush(stdout);
	chunk output_chunk;
	if( m_output_stream.good() )
	{
		new_chunk_event* wc = (new_chunk_event*)(evt);
		m_reducer(wc->data, m_output_stream);
		m_output_stream.flush();
		//printf("just reduced new chunk in Write\n"); fflush(stdout);
	}
	delete evt;
}

void Write::count_exhausted_pipes_action(const event_base* evt)
{
	m_number_of_mappers--;
	//printf("Only %d mappers left\n", m_number_of_mappers); fflush(stdout);
	delete evt;
	if(m_number_of_mappers == 0)
	{
		send_internal(new termination_event());
		//printf("Write thread is terminating\n"); fflush(stdout);
	}
}
