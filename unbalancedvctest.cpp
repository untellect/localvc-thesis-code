#include "pch.h"

#include "Localec-templated.h"

#ifndef LOCALEC_FUNC
#ifndef GLOBALVC_FUNC

	//#define LOCALEC_FUNC local_edge_connectivity_v2

	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_vertex_sampling
	//#define GLOBALVC_TEST_NAME GLOBALVC_FUNC
	//#include "unbalancedvctest.h"

	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_mixed_vertex_sampling
	//#define GLOBALVC_TEST_NAME GLOBALVC_FUNC
	//#include "unbalancedvctest.h"

	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_hybrid_sampling
	//#define GLOBALVC_TEST_NAME GLOBALVC_FUNC
	//#include "unbalancedvctest.h"

	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_mixed_hybrid_sampling
	//#define GLOBALVC_TEST_NAME GLOBALVC_FUNC
	//#include "unbalancedvctest.h"

	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_hybrid_lp
	//#define GLOBALVC_TEST_NAME GLOBALVC_FUNC
	//#include "unbalancedvctest.h"

	//#define LOCALEC_FUNC local_edge_connectivity_templated<page35,true,true>
	//#define GLOBALVC_FUNC vertex_connectivity_unbalanced_mixed_hybrid_sampling
	//#define GLOBALVC_TEST_NAME vertex_connectivity_unbalanced_mixed_hybrid_sampling_templated_localec
	//#include "unbalancedvctest.h"


	#undef LOCALEC_FUNC
	#undef GLOBALVC_FUNC

#endif
#endif