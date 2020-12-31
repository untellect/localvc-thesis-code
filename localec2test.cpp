#include "pch.h"

// LOCALEC_FUNC is a macro used in localectest.h
// It is both the test suite name and the function used for local edge connectivity.
// This way the *exact* same tests will be run for all versions.

#ifndef LOCALEC_FUNC

#define LOCALEC_FUNC local_edge_connectivity_v2
#define LOCALEC_TESTNAME local_edge_connectivity_v2_undo
#include "localec2test.h"

#define LOCALEC_FUNC local_edge_connectivity_degreecount
#define LOCALEC_TESTNAME local_edge_connectivity_degreecount_undo
#include "localec2test.h"

#undef LOCALEC_FUNC
#endif