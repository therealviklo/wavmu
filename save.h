#pragma once
#include <map>
#include "track.h"

void saveSong(Tracks& tracks, const char* filename);
void loadSong(Tracks& tracks, const char* filename);