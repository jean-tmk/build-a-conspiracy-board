#include "graph_engine.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numeric>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace conspiracy {
namespace {
constexpr double kEpsilon = 1e-9;
std::string lowercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return value;
}
double clamp01(double value) { return std::max(0.0, std::min(1.0, value)); }
}

EvidenceType GraphEngine::parse_type(const std::string& value) {
  const auto key = lowercase(value);
  if (key == "person") return EvidenceType::Person;
  if (key == "place") return EvidenceType::Place;
  if (key == "document") return EvidenceType::Document;
  if (key == "event") return EvidenceType::Event;
  if (key == "object") return EvidenceType::Object;
  if (key == "signal") return EvidenceType::Signal;
  return EvidenceType::Unknown;
}

std::string GraphEngine::type_name(EvidenceType type) {
  switch (type) {
    case EvidenceType::Person: return "person";
    case EvidenceType::Place: return "place";
    case EvidenceType::Document: return "document";
    case EvidenceType::Event: return "event";
    case EvidenceType::Object: return "object";
    case EvidenceType::Signal: return "signal";
    default: return "unknown";
  }
}

std::string GraphEngine::escape_json(const std::string& value) {
  std::ostringstream out;
  for (const unsigned char c : value) {
    switch (c) {
      case '"': out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default:
        if (c < 0x20) out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
        else out << c;
    }
  }
  return out.str();
}

int GraphEngine::add_node(const std::string& title, const std::string& detail,
                          const std::string& type, double x, double y,
                          double confidence) {
  if (title.empty()) throw std::invalid_argument("Evidence title cannot be empty");
  const int id = next_id_++;
  nodes_.emplace(id, Node{id, title, detail, parse_type(type), x, y,
                          clamp01(confidence), {}});
  return id;
}

bool GraphEngine::remove_node(int id) {
  if (nodes_.erase(id) == 0) return false;
  edges_.erase(std::remove_if(edges_.begin(), edges_.end(), [id](const Edge& edge) {
    return edge.source == id || edge.target == id;
  }), edges_.end());
  return true;
}

bool GraphEngine::has_edge(int source, int target) const {
  return std::any_of(edges_.begin(), edges_.end(), [source, target](const Edge& edge) {
    return (edge.source == source && edge.target == target) ||
           (edge.source == target && edge.target == source);
  });
}

bool GraphEngine::add_edge(int source, int target, const std::string& label,
                           double weight, bool verified) {
  if (source == target || !contains(source) || !contains(target) || has_edge(source, target)) return false;
  edges_.push_back({source, target, label.empty() ? "possibly connected" : label,
                    std::max(0.05, weight), verified});
  return true;
}

bool GraphEngine::remove_edge(int source, int target) {
  const auto old_size = edges_.size();
  edges_.erase(std::remove_if(edges_.begin(), edges_.end(), [source, target](const Edge& edge) {
    return (edge.source == source && edge.target == target) ||
           (edge.source == target && edge.target == source);
  }), edges_.end());
  return edges_.size() != old_size;
}

bool GraphEngine::update_position(int id, double x, double y) {
  auto found = nodes_.find(id);
  if (found == nodes_.end()) return false;
  found->second.x = x; found->second.y = y;
  return true;
}

bool GraphEngine::update_confidence(int id, double confidence) {
  auto found = nodes_.find(id);
  if (found == nodes_.end()) return false;
  found->second.confidence = clamp01(confidence);
  return true;
}

bool GraphEngine::add_tag(int id, const std::string& tag) {
  auto found = nodes_.find(id);
  if (found == nodes_.end() || tag.empty()) return false;
  found->second.tags.insert(lowercase(tag));
  return true;
}

bool GraphEngine::contains(int id) const { return nodes_.find(id) != nodes_.end(); }
std::size_t GraphEngine::node_count() const { return nodes_.size(); }
std::size_t GraphEngine::edge_count() const { return edges_.size(); }

std::optional<Node> GraphEngine::node(int id) const {
  const auto found = nodes_.find(id);
  if (found == nodes_.end()) return std::nullopt;
  return found->second;
}

std::vector<Node> GraphEngine::nodes() const {
  std::vector<Node> result;
  result.reserve(nodes_.size());
  for (const auto& [_, value] : nodes_) result.push_back(value);
  std::sort(result.begin(), result.end(), [](const Node& a, const Node& b) { return a.id < b.id; });
  return result;
}

std::vector<Edge> GraphEngine::edges() const { return edges_; }

std::vector<int> GraphEngine::neighbors(int id) const {
  std::vector<int> result;
  for (const auto& edge : edges_) {
    if (edge.source == id) result.push_back(edge.target);
    else if (edge.target == id) result.push_back(edge.source);
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<std::pair<int, double>> GraphEngine::weighted_neighbors(int id) const {
  std::vector<std::pair<int, double>> result;
  for (const auto& edge : edges_) {
    if (edge.source == id) result.emplace_back(edge.target, edge.weight);
    else if (edge.target == id) result.emplace_back(edge.source, edge.weight);
  }
  return result;
}

std::vector<int> GraphEngine::shortest_path(int start, int goal) const {
  if (!contains(start) || !contains(goal)) return {};
  std::unordered_map<int, double> distance;
  std::unordered_map<int, int> previous;
  for (const auto& [id, _] : nodes_) distance[id] = std::numeric_limits<double>::infinity();
  using State = std::pair<double, int>;
  std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
  distance[start] = 0.0; queue.emplace(0.0, start);
  while (!queue.empty()) {
    const auto [cost, current] = queue.top(); queue.pop();
    if (cost > distance[current] + kEpsilon) continue;
    if (current == goal) break;
    for (const auto& [next, weight] : weighted_neighbors(current)) {
      const double candidate = cost + weight;
      if (candidate + kEpsilon < distance[next]) {
        distance[next] = candidate; previous[next] = current;
        queue.emplace(candidate, next);
      }
    }
  }
  if (!std::isfinite(distance[goal])) return {};
  std::vector<int> path;
  for (int at = goal;; at = previous[at]) {
    path.push_back(at);
    if (at == start) break;
  }
  std::reverse(path.begin(), path.end());
  return path;
}

std::vector<std::vector<int>> GraphEngine::connected_components() const {
  std::set<int> unseen;
  for (const auto& [id, _] : nodes_) unseen.insert(id);
  std::vector<std::vector<int>> components;
  while (!unseen.empty()) {
    const int root = *unseen.begin(); unseen.erase(root);
    std::queue<int> queue; queue.push(root);
    std::vector<int> component;
    while (!queue.empty()) {
      const int current = queue.front(); queue.pop(); component.push_back(current);
      for (const int next : neighbors(current)) if (unseen.erase(next)) queue.push(next);
    }
    std::sort(component.begin(), component.end()); components.push_back(component);
  }
  std::sort(components.begin(), components.end(), [](const auto& a, const auto& b) { return a.size() > b.size(); });
  return components;
}

std::map<int, double> GraphEngine::degree_centrality() const {
  std::map<int, double> result;
  const double denominator = nodes_.size() > 1 ? static_cast<double>(nodes_.size() - 1) : 1.0;
  for (const auto& [id, _] : nodes_) result[id] = neighbors(id).size() / denominator;
  return result;
}

std::map<int, double> GraphEngine::betweenness_centrality() const {
  std::map<int, double> scores;
  for (const auto& [id, _] : nodes_) scores[id] = 0.0;
  const auto ordered = nodes();
  for (std::size_t i = 0; i < ordered.size(); ++i) {
    for (std::size_t j = i + 1; j < ordered.size(); ++j) {
      const auto path = shortest_path(ordered[i].id, ordered[j].id);
      if (path.size() < 3) continue;
      for (std::size_t k = 1; k + 1 < path.size(); ++k) scores[path[k]] += 1.0;
    }
  }
  const double normalizer = ordered.size() > 2 ? ((ordered.size() - 1.0) * (ordered.size() - 2.0)) / 2.0 : 1.0;
  for (auto& [_, score] : scores) score /= normalizer;
  return scores;
}

Analysis GraphEngine::analyze() const {
  Analysis result;
  result.node_count = static_cast<int>(nodes_.size()); result.edge_count = static_cast<int>(edges_.size());
  result.clusters = connected_components(); result.component_count = static_cast<int>(result.clusters.size());
  result.centrality = degree_centrality();
  const auto bridges = betweenness_centrality();
  double highest_degree = -1.0, highest_bridge = -1.0;
  for (const auto& [id, score] : result.centrality) {
    if (score > highest_degree) { highest_degree = score; result.central_node = id; }
    if (neighbors(id).empty()) result.isolated_nodes.push_back(id);
  }
  for (const auto& [id, score] : bridges) if (score > highest_bridge) { highest_bridge = score; result.bridge_node = id; }
  const double possible = nodes_.size() > 1 ? nodes_.size() * (nodes_.size() - 1.0) / 2.0 : 1.0;
  result.density = edges_.size() / possible;
  const double average_confidence = nodes_.empty() ? 0.0 : std::accumulate(nodes_.begin(), nodes_.end(), 0.0,
    [](double sum, const auto& pair) { return sum + pair.second.confidence; }) / nodes_.size();
  const double unverified = edges_.empty() ? 0.0 : std::count_if(edges_.begin(), edges_.end(), [](const Edge& edge) { return !edge.verified; }) / static_cast<double>(edges_.size());
  result.suspicion_score = clamp01(result.density * .35 + average_confidence * .35 + unverified * .3) * 100.0;
  std::ostringstream story;
  if (nodes_.empty()) story << "The board is empty. Suspiciously empty.";
  else {
    const auto center = node(result.central_node);
    story << (center ? center->title : "One clue") << " sits at the center of " << result.edge_count << " connections. ";
    if (result.component_count > 1) story << result.component_count << " separate clusters have not yet been reconciled. ";
    if (!result.isolated_nodes.empty()) story << result.isolated_nodes.size() << " clue(s) remain completely unconnected. ";
    story << "Calculated suspicion: " << std::fixed << std::setprecision(0) << result.suspicion_score << "%.";
  }
  result.narrative = story.str();
  return result;
}

std::string GraphEngine::analyze_json() const {
  const auto result = analyze();
  std::ostringstream out;
  out << "{\"nodeCount\":" << result.node_count << ",\"edgeCount\":" << result.edge_count
      << ",\"componentCount\":" << result.component_count << ",\"centralNode\":" << result.central_node
      << ",\"bridgeNode\":" << result.bridge_node << ",\"density\":" << result.density
      << ",\"suspicionScore\":" << result.suspicion_score << ",\"narrative\":\"" << escape_json(result.narrative) << "\",\"isolated\":[";
  for (std::size_t i = 0; i < result.isolated_nodes.size(); ++i) { if (i) out << ','; out << result.isolated_nodes[i]; }
  out << "],\"centrality\":{";
  bool first = true; for (const auto& [id, score] : result.centrality) { if (!first) out << ','; first = false; out << '\"' << id << "\":" << score; }
  out << "}}"; return out.str();
}

std::string GraphEngine::board_json() const {
  std::ostringstream out; out << "{\"nodes\":[";
  const auto ordered = nodes();
  for (std::size_t i = 0; i < ordered.size(); ++i) {
    const auto& n = ordered[i]; if (i) out << ',';
    out << "{\"id\":" << n.id << ",\"title\":\"" << escape_json(n.title) << "\",\"detail\":\"" << escape_json(n.detail)
        << "\",\"type\":\"" << type_name(n.type) << "\",\"x\":" << n.x << ",\"y\":" << n.y << ",\"confidence\":" << n.confidence << "}";
  }
  out << "],\"edges\":[";
  for (std::size_t i = 0; i < edges_.size(); ++i) {
    const auto& e = edges_[i]; if (i) out << ',';
    out << "{\"source\":" << e.source << ",\"target\":" << e.target << ",\"label\":\"" << escape_json(e.label)
        << "\",\"weight\":" << e.weight << ",\"verified\":" << (e.verified ? "true" : "false") << "}";
  }
  out << "]}"; return out.str();
}

std::string GraphEngine::path_json(int start, int goal) const {
  const auto path = shortest_path(start, goal); std::ostringstream out; out << '[';
  for (std::size_t i = 0; i < path.size(); ++i) { if (i) out << ','; out << path[i]; }
  out << ']'; return out.str();
}

void GraphEngine::clear() { nodes_.clear(); edges_.clear(); next_id_ = 1; }

void GraphEngine::load_demo_case() {
  clear();
  const int lighthouse = add_node("The lighthouse blinked twice", "Logbook says the lamp was dark all night.", "place", 18, 18, .82);
  const int receipt = add_node("Receipt / 02:17 AM", "Coffee, batteries, and one red umbrella.", "document", 55, 12, .91);
  const int caller = add_node("Unknown caller", "Seven seconds of breathing, then a ship bell.", "signal", 60, 31, .63);
  const int marina = add_node("Marina camera 04", "Missing exactly eleven minutes of footage.", "object", 36, 43, .88);
  const int gardener = add_node("The night gardener", "Claims the roses were louder than usual.", "person", 10, 62, .47);
  const int key = add_node("Brass key marked B", "Saltwater corrosion; no matching lock found.", "object", 62, 67, .76);
  const int tide = add_node("Impossible low tide", "Occurred forty-three minutes ahead of schedule.", "event", 68, 74, .71);
  add_edge(lighthouse, marina, "same power circuit", .7, true);
  add_edge(receipt, caller, "timestamp overlap", .8, false);
  add_edge(receipt, marina, "umbrella in frame", .55, false);
  add_edge(marina, key, "key found below camera", .65, true);
  add_edge(gardener, lighthouse, "heard generator", .9, false);
  add_edge(key, tide, "salt pattern", .75, false);
  add_edge(caller, tide, "bell matches buoy 9", .6, false);
}
}  // namespace conspiracy
