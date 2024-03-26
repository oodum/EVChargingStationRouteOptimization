#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

struct Node {
    char id{};
    bool isChargingStation{};
    std::map<Node *, int> neighbors;
};

class Graph {
public:
    Node *GetOrAdd(char id) {
        if (nodes.find(id) == nodes.end()) {
            Node *node = new Node();
            node->id = id;
            nodes[id] = node;
            return node;
        }
        return nodes[id];
    }

    void setEdge(char from, char to, int weight) {
        Node *fromNode = GetOrAdd(from);
        Node *toNode = GetOrAdd(to);
        fromNode->neighbors[toNode] = weight;
        toNode->neighbors[fromNode] = weight;
    }

    void print() {
        for (auto &pair: nodes) {
            std::cout << "Node " << pair.second->id << " is "
                      << (pair.second->isChargingStation ? "a charging station" : "not a charging station")
                      << std::endl;
            for (auto &pair2: pair.second->neighbors) {
                std::cout << "  -> " << pair2.first->id << " with weight " << pair2.second << std::endl;
            }
        }
    }

private:
    std::map<char, Node *> nodes;
} graph;

class Application {
public:
    static void Initialize() {
        std::ifstream file("data.csv");
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(iss, token, ',')) {
                tokens.push_back(token);
            }
            Node *from = graph.GetOrAdd(tokens[0][0]);
            from->isChargingStation = tokens[1][0] == 'Y';
            for (int i = 2; i < tokens.size(); i++) {
                char to = tokens[i][0];
                int weight = std::stoi(tokens[i].substr(1));
                graph.setEdge(from->id, to, weight);
            }
        }
    }
} app;

int main() {
    Application::Initialize();
    return 0;
}