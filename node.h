#ifndef NODE_H_
#define NODE_H_


#include <cstdint>
#include <memory>
#include <vector>

/**
* The basic structure of a node. Basically this is a contiguous memory block
*
*/
struct Node {

	typedef uint64_t node_id_t;

	// Global Node ID
	node_id_t nodeID;

	// Number of Outlinks
	uint32_t numOutLinks;
	
	// Number of Inlinks
	uint32_t numInLinks;
	
	// Other features


	// Pointer to the out links, which are in memory directly
	// behind the node
	inline node_id_t* outLinks() {
		return reinterpret_cast<uint64_t*>(this + 1);
	}

	// Pointer to the inlinnks, which are in memory after the 
	// out links
	inline node_id_t* inLinks() {
		return outLinks() + numOutLinks;
	}

	static inline Node* build(node_id_t id, const std::vector<node_id_t>& in, const std::vector<node_id_t>& out) {

		Node* data = reinterpret_cast<Node*>(malloc( sizeof(Node) + (in.size() + out.size()) * sizeof(node_id_t) ));
		
		data->nodeID = id;
		data->numOutLinks = out.size();
		data->numInLinks = in.size();

		auto ptr = data->inLinks();
		for(size_t i=0; i < in.size(); ++i ) {
			ptr[i] = in[i];
		}

		ptr = data->outLinks();
		for(size_t i=0; i < out.size(); ++i ) {
			ptr[i] = out[i];
		}
		return data;
	}

};


#endif // NODE_H_