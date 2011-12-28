#ifndef G_HELP_H
# define G_HELP_H

#include "g_all.h"
#include "../sig/signal.h"
#include "../gui/gui_select.h"

void convertCoords(const vector<SelectPlane::Coord>& in, vector<SigCoord>& out);

SoundComp* SCFactory(SCType id, int x, int y, BitmapFont* f, GroundMixer& ground);

#endif
