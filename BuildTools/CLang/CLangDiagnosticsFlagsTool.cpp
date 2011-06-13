/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <set>
#include <vector>
#include <cassert>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>

// -----------------------------------------------------------------------------
// Include diagnostics data from CLang
// -----------------------------------------------------------------------------

#define DIAG(name, a, b, c, d, e, f, g) name,
	
namespace diag {
	enum LexKinds {
#include <clang/Basic/DiagnosticLexKinds.inc>
#include <clang/Basic/DiagnosticParseKinds.inc>
#include <clang/Basic/DiagnosticCommonKinds.inc>
#include <clang/Basic/DiagnosticDriverKinds.inc>
#include <clang/Basic/DiagnosticFrontendKinds.inc>
#include <clang/Basic/DiagnosticSemaKinds.inc>
	};
}

#define GET_DIAG_ARRAYS
#include <clang/Basic/DiagnosticGroups.inc>
#undef GET_DIAG_ARRAYS

struct DiagTableEntry {
	const char* name;
	const short* array;
	const short* group;
};

static const DiagTableEntry diagnostics[] = {
#define GET_DIAG_TABLE
#include <clang/Basic/DiagnosticGroups.inc>
#undef GET_DIAG_TABLE
};
static const size_t diagnostics_count = sizeof(diagnostics) / sizeof(diagnostics[0]);

// -----------------------------------------------------------------------------

using namespace boost;

struct Properties {
	Properties() : have(false), implicitHave(false), dontWant(false), implicitDontWant(false), ignored(false), available(false), missing(false), redundant(false), alreadyCovered(false) {
	}

	std::string name;
	bool have;
	bool implicitHave;
	bool dontWant;
	bool implicitDontWant;
	bool ignored;
	bool available;
	bool missing;
	bool redundant;
	bool alreadyCovered;
};

class GraphVizLabelWriter {
	public:
		GraphVizLabelWriter(const std::vector<Properties>& properties) : properties(properties) {
		}	
		
		template <class VertexOrEdge>
		void operator()(std::ostream& out, const VertexOrEdge& v) const {
			std::string color;
			if (properties[v].missing) {
				color = "orange";
			}
			else if (properties[v].redundant) {
				color = "lightblue";
			}
			else if (properties[v].have) {
				color = "darkgreen";
			}
			else if (properties[v].implicitHave) {
				color = "green";
			}
			else if (properties[v].dontWant) {
				color = "red";
			}
			else if (properties[v].implicitDontWant) {
				color = "pink";
			}
			else if (properties[v].ignored) {
				color = "white";
			}
			else if (properties[v].available) {
				color = "yellow";
			}
			else {
				assert(false);
			}
			out << "[label=" << escape_dot_string(properties[v].name) << " fillcolor=\"" << color << "\" style=filled]";
		}

	private:
		const std::vector<Properties> properties;
};

int main(int argc, char* argv[]) {
	// Parse command-line arguments
	std::set<std::string> have;
	std::set<std::string> dontWant;
	std::string outputDir;
	for (int i = 1; i < argc; ++i) {
		std::string arg(argv[i]);
		if (starts_with(arg, "-W")) {
			have.insert(arg.substr(2, arg.npos));
		}
		else if (starts_with(arg, "-w")) {
			dontWant.insert(arg.substr(2, arg.npos));
		}
		else if (starts_with(arg, "-O")) {
			outputDir = arg.substr(2, arg.npos) + "/";
		}
	}

	// Build the graph and initialize properties
	typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
	typedef graph_traits<Graph>::vertex_descriptor Vertex;
	Graph g(diagnostics_count);
	std::vector<Properties> properties(num_vertices(g));
	for (size_t i = 0; i < diagnostics_count; ++i) {
		std::string name(diagnostics[i].name);
		properties[i].name = name;
		properties[i].implicitHave = properties[i].have = have.find(name) != have.end();
		properties[i].implicitDontWant = properties[i].dontWant = dontWant.find(name) != dontWant.end();
		properties[i].ignored = diagnostics[i].group == 0 && diagnostics[i].array == 0;
		properties[i].alreadyCovered = false;
		properties[i].available = true;
		for (const short* j = diagnostics[i].group; j && *j != -1; ++j) {
			add_edge(i, *j, g);
		}
	}

	// Sort the diagnostics
	std::list<Vertex> sortedDiagnostics;
	boost::topological_sort(g, std::front_inserter(sortedDiagnostics));

	// Propagate dontWant and have properties down
	for(std::list<Vertex>::const_iterator i = sortedDiagnostics.begin(); i != sortedDiagnostics.end(); ++i) {
		graph_traits<Graph>::adjacency_iterator adjacentIt, adjacentEnd;
		for (tie(adjacentIt, adjacentEnd) = adjacent_vertices(*i, g); adjacentIt != adjacentEnd; ++adjacentIt) {
			properties[*adjacentIt].implicitDontWant = properties[*i].implicitDontWant || properties[*adjacentIt].implicitDontWant;
			properties[*adjacentIt].implicitHave = properties[*i].implicitHave || properties[*adjacentIt].implicitHave;
		}
	}

	// Propagate 'available' property upwards
	for(std::list<Vertex>::const_reverse_iterator i = sortedDiagnostics.rbegin(); i != sortedDiagnostics.rend(); ++i) {
		properties[*i].available = properties[*i].available && !properties[*i].implicitDontWant;
		graph_traits<Graph>::in_edge_iterator edgesIt, edgesEnd;
		graph_traits<Graph>::edge_descriptor edge;
		for (tie(edgesIt, edgesEnd) = in_edges(*i, g); edgesIt != edgesEnd; ++edgesIt) {
			properties[source(*edgesIt, g)].available = properties[source(*edgesIt, g)].available && properties[*i].available;
		}
	}

	// Collect missing & redundant flags
	std::set<std::string> missing;
	std::set<std::string> redundant;
	for(std::list<Vertex>::const_iterator i = sortedDiagnostics.begin(); i != sortedDiagnostics.end(); ++i) {
		bool markChildrenCovered = true;
		if (properties[*i].alreadyCovered) {
			if (properties[*i].have) {
				properties[*i].redundant = true;
				redundant.insert(properties[*i].name);
			}
		}
		else {
			if (properties[*i].available) {
				if (!properties[*i].implicitHave && !properties[*i].ignored) {
					properties[*i].missing = true;
					missing.insert(properties[*i].name);
				}
			}
			else {
				markChildrenCovered = false;
			}
		}
		if (markChildrenCovered) {
			graph_traits<Graph>::adjacency_iterator adjacentIt, adjacentEnd;
			for (tie(adjacentIt, adjacentEnd) = adjacent_vertices(*i, g); adjacentIt != adjacentEnd; ++adjacentIt) {
				properties[*adjacentIt].alreadyCovered = true;
			}
		}
	}

	// Write information
	if (!missing.empty()) {
		std::cout << "Missing diagnostic flags: ";
		for(std::set<std::string>::const_iterator i = missing.begin(); i != missing.end(); ++i) {
			std::cout << "-W" << *i << " ";
		}
		std::cout<< std::endl;
	}

	if (!redundant.empty()) {
		std::cout << "Redundant diagnostic flags: ";
		for(std::set<std::string>::const_iterator i = redundant.begin(); i != redundant.end(); ++i) {
			std::cout << "-W" << *i << " ";
		}
		std::cout<< std::endl;
	}

	// Write graphviz file
	if (!outputDir.empty()) {
		std::ofstream f((outputDir + "clang-diagnostics-overview.dot").c_str());
		write_graphviz(f, g, GraphVizLabelWriter(properties));
		f.close();
	}

	return 0;
}
