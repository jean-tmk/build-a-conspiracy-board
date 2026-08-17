#include "graph_engine.hpp"
#include <emscripten/bind.h>
using conspiracy::GraphEngine;
EMSCRIPTEN_BINDINGS(conspiracy_engine) {
  emscripten::class_<GraphEngine>("GraphEngine")
    .constructor<>()
    .function("addNode", &GraphEngine::add_node)
    .function("removeNode", &GraphEngine::remove_node)
    .function("addEdge", &GraphEngine::add_edge)
    .function("removeEdge", &GraphEngine::remove_edge)
    .function("updatePosition", &GraphEngine::update_position)
    .function("updateConfidence", &GraphEngine::update_confidence)
    .function("addTag", &GraphEngine::add_tag)
    .function("contains", &GraphEngine::contains)
    .function("nodeCount", &GraphEngine::node_count)
    .function("edgeCount", &GraphEngine::edge_count)
    .function("analyzeJSON", &GraphEngine::analyze_json)
    .function("boardJSON", &GraphEngine::board_json)
    .function("pathJSON", &GraphEngine::path_json)
    .function("clear", &GraphEngine::clear)
    .function("loadDemoCase", &GraphEngine::load_demo_case);
}
