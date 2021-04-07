#pragma once

#include <cstring>

#define ID_MAP(map,type,max_count)  type map[max_count]
#define ID_MAP_COUNT(map)   (sizeof(map)/sizeof(map[0]))
#define ID_MAP_CLEAR(map)   memset(map,0,sizeof(map))
#define ID_MAP_FIND(map,id) (id>=ID_MAP_COUNT(map)?nullptr:map[id])
#define ID_MAP_END(map)     (nullptr)
#define ID_MAP_FOR_EACH(map,type,item) \
    type item = ID_MAP_FIND(map,0); \
    for ( size_t id = 0; id < ID_MAP_COUNT(map); id++ ) \
    if ( (item = ID_MAP_FIND(map,id)) != ID_MAP_END(map) )
#define ID_MAP_INSERT(map,id,item)  (id>=ID_MAP_COUNT(map)?nullptr:map[id]=item)
