#include "pch.h"

#include <iostream>

#include "Mains.h"

/* Switch between various experiments here. */
void main() {
	std::ios_base::sync_with_stdio(false);

	//main1(); // old
	//main2(); // localec comparison
	//main3(); // old
	//main4(); // old

	main5(); // RFG with planted cut
	main6(); // RHG
	main7(); // RWG (computes k-cores before benchmarking)

	//main8(); // low memory k-core
	//main9(); // binary

	//main10(); // degree distribution

	//test0(); // parameter 'a' testing
}
