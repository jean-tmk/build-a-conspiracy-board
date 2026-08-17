#pragma once
#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace conspiracy {

enum class EvidenceType { Person, Place, Document, Event, Object, Signal, Unknown };

struct Node {
  int id{};
  std::string title;
  std::string detail;
  EvidenceType type{EvidenceType::Unknown};
  double x{};
  double y{};
  double confidence{0.5};
  std::set<std::string> tags;
};

struct Edge {
  int source{};
  int target{};
  std::string label;
  double weight{1.0};
  bool verified{false};
};

struct Analysis {
  int node_count{};
  int edge_count{};
  int component_count{};
  int central_node{-1};
  int bridge_node{-1};
  double density{};
  double suspicion_score{};
  std::vector<int> isolated_nodes;
  std::vector<std::vector<int>> clusters;
  std::map<int, double> centrality;
  std::string narrative;
};

class GraphEngine {
 public:
  GraphEngine() = default;
  int add_node(const std::string& title, const std::string& detail,
               const std::string& type, double x, double y,
               double confidence = 0.5);
  bool remove_node(int id);
  bool add_edge(int source, int target, const std::string& label,
                double weight = 1.0, bool verified = false);
  bool remove_edge(int source, int target);
  bool update_position(int id, double x, double y);
  bool update_confidence(int id, double confidence);
  bool add_tag(int id, const std::string& tag);
  bool contains(int id) const;
  std::size_t node_count() const;
  std::size_t edge_count() const;
  std::optional<Node> node(int id) const;
  std::vector<Node> nodes() const;
  std::vector<Edge> edges() const;
  std::vector<int> neighbors(int id) const;
  std::vector<int> shortest_path(int start, int goal) const;
  std::vector<std::vector<int>> connected_components() const;
  std::map<int, double> degree_centrality() const;
  std::map<int, double> betweenness_centrality() const;
  Analysis analyze() const;
  std::string analyze_json() const;
  std::string board_json() const;
  std::string path_json(int start, int goal) const;
  void clear();
  void load_demo_case();

 private:
  int next_id_{1};
  std::unordered_map<int, Node> nodes_;
  std::vector<Edge> edges_;
  static EvidenceType parse_type(const std::string& value);
  static std::string type_name(EvidenceType type);
  static std::string escape_json(const std::string& value);
  bool has_edge(int source, int target) const;
  std::vector<std::pair<int, double>> weighted_neighbors(int id) const;
};

}  // namespace conspiracy
