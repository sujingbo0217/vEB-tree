// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

#include "../../include/vebtree/vebtree.hpp"

#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <chrono>
#include <openssl/rand.h>

using namespace std::chrono;

double elapsed(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2) {
	return (duration_cast<duration<double>>(t2 - t1)).count();
}

void safe_rand_bytes(unsigned char *v, uint32_t n) {
	while (n > 0) {
		size_t round_size = n >= INT_MAX ? INT_MAX - 1 : n;
		RAND_bytes(v, round_size);
		v += round_size;
		n -= round_size;
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Specify the number of items for the test.\n";
		exit(1);
	}
	uint32_t N = atoi(argv[1]); 	// number of items

	// Generate N numbers to insert
	uint32_t *in_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!in_numbers) {
		std::cerr << "Malloc in_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char *)in_numbers, sizeof(*in_numbers) * N);

	// Generate N numbers different from in_numbers for succ queries
	uint32_t *out_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!out_numbers) {
		std::cerr << "Malloc out_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char *)out_numbers, sizeof(*out_numbers) * N);

	// Store successor ground truth
	std::vector<uint32_t> bst_succ(N);

	// auto new_in = in_numbers;
	// new_in[0] = 26112;
	// new_in[1] = 26113;
	// new_in[2] = 26111;
	// new_in[3] = 26114;
	// new_in[4] = 26115;
	// new_in[5] = 26110;
	// new_in[6] = 26116;

/***********************************************************************
 * Test Binary Search Tree (BST) std::set
 ***********************************************************************/
{    
	std::cout << "Testing Binary Search Tree (BST) by std::set..." << std::endl;
    // Create a bst using std::set
	std::set<uint32_t> bst;
	high_resolution_clock::time_point t1, t2;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "BST Insert Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		bst.insert(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "BST Find Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		auto ret = bst.find(in_numbers[i]);
		if (ret == bst.end()) {
			std::cerr << "Find in BST failed. Item: " + std::to_string(in_numbers[i]) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "BST Find Successor Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		auto ret = bst.lower_bound(out_numbers[i]);
		if (ret != bst.end() && *ret < out_numbers[i]) {
			std::cerr << "successor query in BST failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) + "\n";
			exit(0);
		}
		bst_succ[i] = (ret == bst.end() ? 0 : *ret);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n\n";

	bst.clear();
}

/***********************************************************************
 * Test sequential model std::vector
 ***********************************************************************/
{
    std::cout << "Testing sequencial model by std::vector..." << std::endl;
    std::vector<uint32_t> vec;
	high_resolution_clock::time_point t1, t2;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "VEC Insert Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		vec.push_back(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
    std::sort(vec.begin(), vec.end());
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "VEC Find Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		// auto ret = std::find(vec.begin(), vec.end(), in_numbers[i]);
		auto ret = std::lower_bound(vec.begin(), vec.end(), in_numbers[i]);
		if (ret == vec.end()) {
			std::cerr << "Find in VEC failed. Item: " + std::to_string(in_numbers[i]) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		if (i > 0 && (i + 1) % (N / 10) == 0) {
			std::cerr << std::setprecision(2) << "VEC Find Successor Process: " << ((i + 1) * 100 / N) << "%...\n";
		}
		auto ret = lower_bound(vec.begin(), vec.end(), out_numbers[i]);
		if (ret != vec.end() && (*ret < out_numbers[i] || *ret != bst_succ[i])) {
			std::cerr << "successor query in VEC failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n\n";
	
	vec.clear();
}

/***********************************************************************
 * Test van Emde Boas (vEB) Tree
 ***********************************************************************/
{
    std::cout << "Testing van Emde Boas Tree..." << std::endl;
	high_resolution_clock::time_point t1, t2;
	
	t1 = high_resolution_clock::now();
    // vebtree::vEBTree<uint16_t> vebtree;
    vebtree::vEBTree<uint32_t> vebtree;
	t2 = high_resolution_clock::now();
	std::cout << "Time to construction: " + std::to_string(elapsed(t1, t2)) + " secs\n";
	fflush(stdout);

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		// if (i > 0 && (i + 1) % (N / 10) == 0) {
		// 	std::cerr << std::setprecision(2) << "vEB-tree Insert Process: " << ((i + 1) * 100 / N) << "%...\n";
		// }
        if (!vebtree.insert(in_numbers[i])) {
			std::cout << "Insert to vEB-tree failed. Item: " << i + 1 << ", Val: " << in_numbers[i] << std::endl;
			exit(0);
		}
		// auto succ = vebtree.succ(in_numbers[i]);
		// if (succ == -1 && succ != in_numbers[i]) {
		// 	std::cerr << "Insert: " << in_numbers[i] << std::endl;
		// 	std::cerr << "Succ: " << succ << std::endl;
		// 	exit(0);
		// }
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";
	fflush(stdout);

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		// if (i > 0 && (i + 1) % (N / 10) == 0) {
		// 	std::cerr << std::setprecision(2) << "vEB-tree Find Process: " << ((i + 1) * 100 / N) << "%...\n";
		// }
		auto res = vebtree.find(in_numbers[i]);
		if (res != in_numbers[i]) {
			std::cout << "Find in vEB-tree failed. Item: " + std::to_string(in_numbers[i]) << ", found: " << res << "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";
	fflush(stdout);

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		// if (i > 0 && (i + 1) % (N / 10) == 0) {
		// 	std::cerr << std::setprecision(2) << "vEB-tree Find Successor Process: " << ((i + 1) * 100 / N) << "%...\n";
		// }
		uint32_t res = vebtree.succ(out_numbers[i]);
		bool ok = (res == bst_succ[i] || (bst_succ[i] == 0 && res == ((uint32_t)-1)));
		if (!ok) {
			std::cout << "successor query in vEB-tree failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(res) + "\n";
			std::cout << "Successor should be: " + std::to_string(bst_succ[i]) << ", res: " << res << std::endl;
			// std::cerr << (vebtree.find(res)) << " & " << (bst.find(res) != bst.end()) << std::endl;
			// exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";
	fflush(stdout);
	exit(0);
}
	return 0;
}
