#ifndef __single_threaded_execution_h__
#define __single_threaded_execution_h__

#include "plugin.h"

void single_threaded_execute( std::istream& in, std::ostream& out, splitter_fn& splitter, map_fn& map, reduce_fn& reduce);

#endif
