#include "graph_engine.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using conspiracy::GraphEngine;
int main() {
  GraphEngine graph;
  const int a=graph.add_node("A","first","person",0,0,.8);
  const int b=graph.add_node("B","second","document",1,1,.7);
  const int c=graph.add_node("C","third","place",2,2,.6);
  assert(graph.node_count()==3);
  assert(graph.add_edge(a,b,"knows",1.0,true));
  assert(graph.add_edge(b,c,"visited",1.0,false));
  assert(!graph.add_edge(a,a,"impossible",1.0,false));
  assert(!graph.add_edge(a,b,"duplicate",1.0,false));
  const auto path=graph.shortest_path(a,c);
  assert(path.size()==3&&path[0]==a&&path[1]==b&&path[2]==c);
  const auto analysis=graph.analyze();
  assert(analysis.central_node==b);
  assert(analysis.component_count==1);
  assert(analysis.edge_count==2);
  assert(graph.update_position(a,12,14));
  assert(graph.update_confidence(a,5.0));
  assert(std::abs(graph.node(a)->confidence-1.0)<.001);
  assert(graph.remove_edge(a,b));
  assert(graph.connected_components().size()==2);
  assert(graph.remove_node(c));
  assert(graph.node_count()==2);
  graph.load_demo_case();
  assert(graph.node_count()==7);
  assert(graph.edge_count()==7);
  assert(graph.board_json().find("lighthouse")!=std::string::npos);
  assert(graph.analyze_json().find("suspicionScore")!=std::string::npos);
  std::cout<<"All conspiracy engine tests passed\n";
}
