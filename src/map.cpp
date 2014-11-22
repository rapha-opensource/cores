#include "map.h"

Map::Map( map_fn* map, delete_map_t* delete_map, Write* write_)
	:
	m_map(map),
	m_delete_map(delete_map),
	m_write(write_)
{
	m_state_functions.push_back( [&](event_base* evt) { processing_new_chunk_action(evt); } );
	m_state_functions.push_back( [&](event_base* evt) { end_of_map_action(evt); } );
	m_state_transitions = {
		{ processing_new_chunk, end_of_map },
		{ cannot_happen, cannot_happen } };
}

Map::~Map()
{
	m_delete_map(m_map);
}

void Map::processing_new_chunk_action(const event_base* _evt)
{
	chunk out;
	const new_chunk_event* evt = (const new_chunk_event*)(_evt);
	//printf("Processing new chunk in mapper\n"); fflush(stdout);
	(*m_map)( evt->data, out );
	//printf("%s", out.second.c_str() );
	m_write->send( new Write::new_chunk_event(out) );
	delete _evt;
}

void Map::end_of_map_action(const event_base* evt)
{
	//printf("Received no_more_data event, terminating this mapper\n"); fflush(stdout);
	delete evt;
	m_write->send( new Write::no_more_data_event() ); //useless, we should re-use evt instead...
	send_internal( new termination_event() );
}
