#pragma once
#include <map>
#include "track.h"
#include "createinstrument.h"

void saveSong(Tracks& tracks, const char* filename);
void loadSong(Tracks& tracks, const char* filename);