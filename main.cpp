#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

static std::string filepath = "data.csv";

struct Node {
    char id{};
    bool isChargingStation{};
    std::map<Node *, int> neighbors;
};

class Graph {
public:
    Node *getOrAdd(char id) {
        if (nodes.find(id) == nodes.end()) {
            Node *node = new Node();
            node->id = id;
            nodes[id] = node;
            return node;
        }
        return nodes[id];
    }
    void setDistance(char from, char to, int distance) {
        Node *fromNode = getOrAdd(from);
        Node *toNode = getOrAdd(to);
        fromNode->neighbors[toNode] = distance;
        toNode->neighbors[fromNode] = distance;
    }
    void print() {
        for (auto &pair: nodes) {
            std::cout << "Node " << pair.second->id << " is "
                      << (pair.second->isChargingStation ? "a charging station" : "not a charging station")
                      << '\n';
            for (auto &pair2: pair.second->neighbors) {
                std::cout << "  -> " << pair2.first->id << " with distance " << pair2.second << '\n';
            }
        }
    }
    void dijkstra(char start) {
        dijkstra(getOrAdd(start));
    }
private:
    std::map<char, Node *> nodes;
    void dijkstra(Node *start) {
        if (nodes[start->id]->neighbors.empty()) {
            std::cerr << "Node " << start->id << " has no neighbors\n";
            return;
        }

        std::cout << "Finding closest charging station from node " << start->id << "...\n";

        const int INF = 2147483647;
        std::map<Node *, int> distances;
        std::map<Node *, Node *> previous;
        std::set<std::pair<int, Node *>> priorityQueue;

        for (auto &pair: nodes) {
            Node *node = pair.second;
            distances[node] = (node == start ? 0 : INF);
            previous[node] = nullptr;
            priorityQueue.insert({distances[node], node});
        }

        while (!priorityQueue.empty()) {
            Node *u = priorityQueue.begin()->second;
            priorityQueue.erase(priorityQueue.begin());

            for (auto &pair: u->neighbors) {
                Node *v = pair.first;
                int alt = distances[u] + pair.second;
                if (alt < distances[v]) {
                    priorityQueue.erase({distances[v], v});
                    distances[v] = alt;
                    previous[v] = u;
                    priorityQueue.insert({distances[v], v});
                }
            }
        }

        std::vector<std::pair<int, Node *>> chargingStations;
        for (auto &pair: nodes) {
            Node *node = pair.second;
            if (node->isChargingStation) {
                chargingStations.emplace_back(distances[node], node);
            }
        }
        std::sort(chargingStations.begin(), chargingStations.end());

        for (auto &pair: chargingStations) {
            Node *node = pair.second;
            std::cout << "Distance to charging station " << node->id << ": " << pair.first << '\n';
            std::cout << "Path: ";
            std::string path;
            for (Node *v = node; v != nullptr; v = previous[v]) {
                path += v->id;
            }
            std::reverse(path.begin(), path.end());
            std::cout << path << '\n';
            std::cout << '\n';
        }
    }
} graph;

class Application {
public:
    static void initialize() {
        std::ifstream file(filepath);
        std::string line;

        if (!file.is_open()) {
            std::cerr << "File not found\n";
            return;
        }

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(iss, token, ',')) {
                tokens.emplace_back(token);
            }
            Node *from = graph.getOrAdd(tokens[0][0]);
            from->isChargingStation = tokens[1][0] == 'Y';
            for (int i = 2; i < tokens.size(); i++) {
                char to = tokens[i][0];
                int distance = std::stoi(tokens[i].substr(1));
                graph.setDistance(from->id, to, distance);
            }
        }
        
        file.close();
    }
    static void start() {
        std::cout << "Welcome to the charging station finder!\n"
                     "Please enter the node you are at (A-W)\n"
                     "Type 'exit' to quit, or 'print' to view the map:\n";
        std::string input;
        while (std::getline(std::cin, input)) {
            if (input == "exit") {
                break;
            } else if (input == "print") {
                graph.print();
            } else if (input.size() == 1 && toupper(input[0]) >= 'A' && toupper(input[0]) <= 'W') {
                graph.dijkstra(toupper(input[0]));
            } else {
                std::cerr << "Invalid input\n";
            }
        }
    }
};

int main() {
    Application::initialize();
    Application::start();
    return 0;
}