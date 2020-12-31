#include "pch.h"

#include "Localec-templated.h"

// LOCALEC_FUNC is a macro used in localectest.h
// It is both the test suite name and the function used for local edge connectivity.
// This way the *exact* same tests will be run for all versions.

#ifndef LOCALEC_FUNC

#define LOCALEC_FUNC local_edge_connectivity_v1
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_v2
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_v3
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_v4
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_v5
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_v6
#define LOCALEC_TESTNAME LOCALEC_FUNC
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated<page10, false>
#define LOCALEC_TESTNAME local_edge_connectivity_templated_10_false_false
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated<page10, true>
#define LOCALEC_TESTNAME local_edge_connectivity_templated_10_true_false
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated<page10, true, true>
#define LOCALEC_TESTNAME local_edge_connectivity_templated_10_true_true
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated<page35, false>
#define LOCALEC_TESTNAME local_edge_connectivity_templated_35_false_false
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated<page35, true, true>
#define LOCALEC_TESTNAME local_edge_connectivity_templated_35_true_true
#include "localectest.h"

#define LOCALEC_FUNC local_edge_connectivity_templated
#define LOCALEC_TESTNAME local_edge_connectivity_templated_default
#include "localectest.h"

#undef LOCALEC_FUNC
#endif