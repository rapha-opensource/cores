// 		Synopsis
//
//  istream => Read( splitter ) => input_q => Map( map ) => output_q => Write( reducer ) => ostream
//
// Read, Map, Write run either in parallel, via threads, if the # of cores is > 1, or serially.
//
// User provides (splitter, map, reducer) via library
//

#ifndef __plugin_h__
#define __plugin_h__

#include <ostream> 
#include <istream> 
#include <functional> 

typedef std::pair<size_t, std::string> chunk;

typedef std::function< bool (chunk&) > read_chunk_fn;
typedef std::function< void (const chunk&) > write_chunk_fn;

typedef std::function< void (std::istream&, write_chunk_fn&) > splitter_fn;
typedef std::function< void (const chunk&, chunk&) > map_fn;
typedef std::function< void (const chunk&, std::ostream&) > reduce_fn;

void order_preserving_reducer(const chunk&, std::ostream&);

// 
// factory functions: the real interface at the dll level
//

typedef splitter_fn* new_splitter_t();
typedef void delete_splitter_t( splitter_fn* );

typedef map_fn* new_map_t();
typedef void delete_map_t(map_fn*);

typedef reduce_fn* new_reduce_t();
typedef void delete_reduce_t(reduce_fn*);

#endif
