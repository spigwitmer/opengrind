#pragma once

const char *Nepgear::FullName = "Nepgear Test Suite";
const char *Nepgear::UnixName = "nepgear";

#include <string>
#include <physfs.h>

void deinit_physfs()
{
	if (PHYSFS_isInit())
		PHYSFS_deinit();
}