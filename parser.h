#ifndef PARSER_H_
#define PARSER_H_

#include <cstdint>
#include <iostream>
#include <set>
#include <vector>


/**
* This parser assumes unidirectional graph structure.
* 
* If a is friend of b, b is friend of a. Not like Twitter or G+.
*/
class Parser {

public:

	struct options {
		bool hasEgo;

		options(): hasEgo(false){}
	};

private:

	typedef std::set<uint64_t> edge_list_t;
	typedef std::vector<edge_list_t> node_list_t;

	// Global number of nodes
	uint64_t _numNodes;

	// Basic pattern for all nodes
	node_list_t _nodes;

	// Options
	options _options;

public:


	Parser(uint64_t numNodes, Parser::options o=options());

	/**
	* Parse a file according to the file format described in
	* http://snap.stanford.edu/data/egonets-Facebook.html
	*/
	void parse(uint64_t egoNode, std::string filename);

	std::string stats();

	inline node_list_t get() { return std::move(_nodes); }

};



#endif // PARSER_H_
