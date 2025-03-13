// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

#include "../../include/vebtree/vebtree.hpp"
// #include "../../include/vebtree/baseline.hpp"
// #include "../../include/vebtree/geek.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <set>
#include <vector>
#include <chrono>
#include <cassert>
// #include <cstdio>
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
		std::cout << "Specify the number of items for the test." << std::endl;
		exit(1);
	}
	uint32_t N = atoi(argv[1]); 	// number of items
	bool test = argv[2];

	// Generate N numbers to insert
	uint32_t *in_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!in_numbers) {
		std::cout << "Malloc in_numbers failed." << std::endl;
		exit(0);
	}
	safe_rand_bytes((unsigned char *)in_numbers, sizeof(*in_numbers) * N);

	// Generate N numbers different from in_numbers for succ queries
	uint32_t *out_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!out_numbers) {
		std::cout << "Malloc out_numbers failed." << std::endl;
		exit(0);
	}
	safe_rand_bytes((unsigned char *)out_numbers, sizeof(*out_numbers) * N);

	if (test) {
		std::ifstream fin;
		fin.open("in_numbers.txt");
		if (fin.is_open()) {
			for (uint32_t i = 0; i < N; ++i) {
				fin >> in_numbers[i];
			}
			fin.close();
		}
		fin.open("out_numbers.txt");
		if (fin.is_open()) {
			for (uint32_t i = 0; i < N; ++i) {
				fin >> out_numbers[i];
			}
			fin.close();
		}
	}

	// Store successor ground truth
	std::vector<uint32_t> bst_succ(N);
	std::vector<uint32_t> bst_succ_after_del(N);

/***********************************************************************
 * Test Red-Black Tree (BST) std::set
 ***********************************************************************/
{    
	std::cout << "Testing Binary Search Tree (BST) by std::set..." << std::endl;
    // Create a bst using std::set
	std::set<uint32_t> bst;
	high_resolution_clock::time_point t1, t2;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bst.insert(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.find(in_numbers[i]);
		if (ret == bst.end()) {
			std::cout << "Find in BST failed. Item: " + std::to_string(in_numbers[i]) << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.lower_bound(out_numbers[i]);
		// auto ret = bst.upper_bound(out_numbers[i]);
		if (ret != bst.end() && *ret < out_numbers[i]) {
			std::cout << "successor query in BST failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) << std::endl;
			exit(0);
		}
		bst_succ[i] = (ret == bst.end() ? 0 : *ret);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N / 2 deletions from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N / 2; ++i) {
		bst.erase(in_numbers[i]);
		// assert(bst.find(in_numbers[i]) == bst.end());
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to delete " + std::to_string(N / 2) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl << std::endl;

	bst.clear();
}

/***********************************************************************
 * Test sequential model std::vector (lower bound log(n))
 ***********************************************************************/
{
    std::cout << "Testing sequencial model by std::vector..." << std::endl;
    std::vector<uint32_t> vec;
	high_resolution_clock::time_point t1, t2;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		vec.push_back(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

    std::sort(vec.begin(), vec.end());

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		// auto ret = std::find(vec.begin(), vec.end(), in_numbers[i]);
		auto ret = std::lower_bound(vec.begin(), vec.end(), in_numbers[i]);
		if (ret == vec.end()) {
			std::cout << "Find in VEC failed. Item: " + std::to_string(in_numbers[i]) << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = std::lower_bound(vec.begin(), vec.end(), out_numbers[i]);
		// auto ret = std::upper_bound(vec.begin(), vec.end(), out_numbers[i]);
		if (ret != vec.end() && (*ret < out_numbers[i] || *ret != bst_succ[i])) {
		// if ((ret == vec.end() && bst_succ[i] != 0) || (ret != vec.end() && (*ret < out_numbers[i] || *ret != bst_succ[i]))) {
			std::cout << "successor query in VEC failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl << std::endl;
	
	vec.clear();
}

/***********************************************************************
 * Test van Emde Boas (vEB) Tree Baseline
 ***********************************************************************/
/*
{
    std::cout << "Testing van Emde Boas Tree Baseline..." << std::endl;
	high_resolution_clock::time_point t1, t2;
	
	t1 = high_resolution_clock::now();
    geek::Van_Emde_Boas* tree = new geek::Van_Emde_Boas(N);
	t2 = high_resolution_clock::now();
	std::cout << "Time to construction: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		geek::insert(tree, in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bool found = geek::isMember(tree, in_numbers[i]);
		if (!found) {
			std::cout << "Find in vEB-tree failed. Item: " + std::to_string(in_numbers[i]) << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		uint32_t res = geek::VEB_successor(tree, out_numbers[i]);
		if (bst_succ[i] != 0 && res != bst_succ[i]) {
			std::cout << "successor query in vEB-tree failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(res) << std::endl;
			std::cout << "Successor should be: " + std::to_string(bst_succ[i]) << ", res: " << res << std::endl;
			exit(1);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl << std::endl;

	// N / 2 deletion from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N / 2; ++i) {
		geek::VEB_delete(tree, in_numbers[i]);
		if (geek::isMember(tree, in_numbers[i])) {
			std::cout << "Item " << i << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to delete " + std::to_string(N / 2) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;
	exit(0);
}
*/

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
	std::cout << "Time to construction: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		vebtree.insert(in_numbers[i]);
		// auto succ = vebtree.succ(in_numbers[i]);
		// if (succ == -1 && succ != in_numbers[i]) {
		// 	std::cerr << "Insert: " << in_numbers[i] << std::endl;
		// 	std::cerr << "Succ: " << succ << std::endl;
		// 	exit(0);
		// }
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bool found = vebtree.find(in_numbers[i]);
		if (!found) {
			std::cout << "Find in vEB-tree failed. Item: " + std::to_string(in_numbers[i]) << std::endl;
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		uint32_t res = vebtree.successor(out_numbers[i] - 1);
		if ((res != (uint32_t)-1 && res != bst_succ[i]) || (res == (uint32_t)-1 && bst_succ[i] != 0)) {
			std::cout << "successor query in vEB-tree failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(res) << std::endl;
			std::cout << "Successor should be: " + std::to_string(bst_succ[i]) << ", res: " << res << std::endl;
			// std::cerr << (vebtree.find(res)) << " & " << (bst.find(res) != bst.end()) << std::endl;
			std::ofstream fout;
			fout.open("in_numbers.txt");
			if (fout.is_open()) {
				for (uint32_t i = 0; i < N; ++i) {
					fout << in_numbers[i] << std::endl;
				}
				fout.close();
			}
			fout.open("out_numbers.txt");
			if (fout.is_open()) {
				for (uint32_t i = 0; i < N; ++i) {
					fout << out_numbers[i] << std::endl;
				}
				fout.close();
			}
			// exit(1);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;

	// N / 2 deletion from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N / 2; ++i) {
		vebtree.remove(in_numbers[i]);
		// vebtree.remove(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	
	std::cout << "Time to delete " + std::to_string(N / 2) + " items: " + std::to_string(elapsed(t1, t2)) + " secs" << std::endl;
	
	// for (uint32_t i = 0; i < N / 2; ++i) {
	// 	if (vebtree.find(in_numbers[i])) {
	// 		std::cout << "#Item " << i << ">> " << in_numbers[i] << std::endl;
	// 	}
	// }
	exit(0);
}
	return 0;
}
