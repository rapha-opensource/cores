#ifndef __map_h__
#define __map_h__

#include "plugin.h" 
#include "state_model.h" 
#include "write.h" 

class Map : public StateModel
{
private:
	enum
	{
		processing_new_chunk = 0,
		end_of_map,
		cannot_happen,
		number_of_states
	} states;

	enum
	{
		new_chunk = 0,
		no_more_data,
		number_of_events
	} event_id;

	map_fn* m_map;
	delete_map_t* m_delete_map;
	Write* m_write;

	void processing_new_chunk_action(const event_base* evt);
	void end_of_map_action(const event_base* evt);

	EVENT(termination);

	void init( map_fn*, delete_map_t*, Write*);

public:
	Map( map_fn*, delete_map_t*, Write*);
	~Map();

	typedef event<new_chunk, chunk> new_chunk_event;
	SEND(new_chunk);
	EVENT(no_more_data);
};

#endif

