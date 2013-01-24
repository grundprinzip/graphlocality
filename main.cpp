#include <cstdint>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "node.h"
#include "parser.h"

#define EXPAND_NAMES(n) {n, "facebook/" #n ".edges"}

std::vector<Node*> allNodes;
bool ego = false;

std::vector<std::pair<uint64_t, std::string>> data = {
	//{0, "facebook/nodes_clustered.txt"}
	//{0, "nodes_t2_cmp.txt"}
	{0, "nodes.txt"}
	//{0, "nodes_clus.txt"}
	// EXPAND_NAMES(0),
	// EXPAND_NAMES(107),
	// EXPAND_NAMES(0), EXPAND_NAMES(1684), EXPAND_NAMES(1912),
	// EXPAND_NAMES(3437), EXPAND_NAMES(348), EXPAND_NAMES(3980),
	// EXPAND_NAMES(414), EXPAND_NAMES(686), EXPAND_NAMES(698)
};


uint64_t dfs(Node* node, size_t level) {
	uint32_t outs = node->numOutLinks;
	uint64_t count = 0;
	if (level == 0)
		return 1;

	// Check depth-first
	for(size_t i=0; i < outs; ++i ) {
		count += dfs(allNodes[node->outLinks()[i]], level-1);
	}

	// Return the number of visited nodes
	return count;
}

uint64_t dfsWithColoring(Node* node, size_t level, std::vector<bool>& colors) {
	uint32_t outs = node->numOutLinks;
	uint64_t count = 1;

	if (level == 0 || outs == 0)
		return count;

	// Check depth-first
	for(size_t i=0; i < outs; ++i ) {
		auto link = node->outLinks()[i];
		if (!(colors)[link]) {
			count += dfsWithColoring(allNodes[link], level-1, colors);
			(colors)[link] = true;
		}
	}
	// Return the number of visited nodes
	return count;
}


void test1(size_t level) {
	// Now perform a three-hop search for all nodes in the graph
	auto begin = std::chrono::high_resolution_clock::now();
	auto numNodes = allNodes.size();
	uint64_t count = 0;
	for(size_t i=0; i < numNodes; ++i) {
		count += dfs(allNodes[i], level);
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	std::cerr << "Count " << count << std::endl;
	std::cerr << "Duration: " << duration << "ms" << std::endl;
	std::cerr << "Nodes per ms " << count / duration << std::endl;

	std::cerr << "-----------------------------------------------" << std::endl;
}


void test2(size_t level) {
	// Now perform a three-hop search for all nodes in the graph
	auto begin = std::chrono::high_resolution_clock::now();
	auto numNodes = allNodes.size();
	uint64_t count = 0;
	for(size_t i=0; i < numNodes; ++i) {
		std::vector<bool> colors(numNodes, false);
		auto tmp = dfsWithColoring(allNodes[i], level, colors);
		count += tmp;
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	std::cerr << "Count " << count << std::endl;
	std::cerr << "Duration: " << duration << "ms" << std::endl;
	std::cerr << "Nodes per ms " << count / duration << std::endl;

	std::cerr << "-----------------------------------------------" << std::endl;
}

void test3(size_t level) {
	// Now perform a three-hop search for all nodes in the graph
	auto begin = std::chrono::high_resolution_clock::now();
	auto numNodes = allNodes.size();
	uint64_t count = 0;

	// Define future and lambda
	std::vector<std::future<uint64_t>> futures;
	auto lambda = [numNodes] (Node* n, size_t level) -> uint64_t {
		std::vector<bool> colors(numNodes, false);
		return dfsWithColoring(n, level, colors);	
	};
	
	for(size_t i=0; i < numNodes; ++i) {
		auto fut = std::async(lambda, allNodes[i], level);
		futures.push_back(std::move(fut));
	}

	for(size_t i=0; i < futures.size(); ++i) {
		futures[i].wait();
		count += futures[i].get();
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	std::cerr << "Count " << count << std::endl;
	std::cerr << "Duration: " << duration << "ms" << std::endl;
	std::cerr << "Nodes per ms " << count / duration << std::endl;

	std::cerr << "-----------------------------------------------" << std::endl;
}


void dump(std::string filename) {
	std::ofstream f(filename);
	for(const auto& n : allNodes) {
		for(size_t i =0; i < n->numOutLinks; ++i)
			f << n->nodeID << " " << n->outLinks()[i] << std::endl;
	}
	f.close();
}


int main(int argc, char* argv[]) {

	if (argc < 2) {
		std::cout << "Usage: main [level]" << std::endl;
		return 1;
	}

	// Load with options
	Parser::options o;
	o.hasEgo = ego;

	// Open the parser
	Parser p(4039, o);

	for( auto t : data) {
		p.parse(t.first, t.second);
	}

	std::cerr << "Done Parsing..." << std::endl;
	std::cerr << p.stats() << std::endl;

	// Build the nodes
	auto nodes = p.get();
	for(size_t i=0; i < nodes.size(); ++i) {
		std::vector<Node::node_id_t> edges(nodes[i].begin(), nodes[i].end());
		allNodes.push_back(Node::build(i, edges, edges));
	}

	// Dump the data
	if (argc == 3) {
		std::string fn(argv[2]);
		dump(fn);
		return 0;
	}

	// Start the testing..............
	uint32_t level = std::stoul(argv[1]);
	//test1(level);
	test2(level);
	//test3(level);

	return 0;
}