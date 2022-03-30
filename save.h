#pragma once
#include <map>
#include "track.h"
#include "sininstrument.h"

void saveSong(Tracks& tracks, const char* filename);
void loadSong(Tracks& tracks, const char* filename);