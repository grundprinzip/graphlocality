#include "parser.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>
#include <sstream>

Parser::Parser(uint64_t n, Parser::options o) : _numNodes(n), _nodes(n), _options(o) {

}

void Parser::parse(uint64_t egoNode, std::string filename) {


	std::ifstream file(filename);
	if (file) {

		// Read the complete data
		std::string line;
		while(std::getline(file, line)) {

			// Split the line according to the definition, attention
			// splits only by whitespaces
			std::istringstream iss(line);
			std::vector<std::string> tokens;
			std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter< decltype(tokens) >(tokens));

			// Now create the edges, first the ego node
			if (_options.hasEgo) {
				_nodes[egoNode].insert(std::stoull(tokens.front()));
				_nodes[egoNode].insert(std::stoull(tokens.back()));
				_nodes[std::stoull(tokens.front())].insert(egoNode);
				_nodes[std::stoull(tokens.back())].insert(egoNode);
			}

			_nodes[std::stoull(tokens.front())].insert(std::stoull(tokens.back()));
			_nodes[std::stoull(tokens.back())].insert(std::stoull(tokens.front()));
			
		}

		// Close the file after processing
		file.close();
	}

}


std::string Parser::stats() {
	std::stringstream result;
	result << "Nodes: " << _nodes.size() << std::endl;
	result << "Nodes with Edges " << 
	    std::count_if(_nodes.begin(), _nodes.end(), [](edge_list_t i) {return i.size() > 0;}) << std::endl;
    result << "Total Edges"	<<
        std::accumulate(_nodes.begin(), _nodes.end(), 0ull, []( decltype(0ull) m, edge_list_t i) { return m + i.size();}) << std::endl;
	return result.str();
}