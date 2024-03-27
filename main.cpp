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
    // Returns the node with a given id, or creates a new node if it doesn't exist
    Node *getOrAdd(char id) {
        if (nodes.find(id) == nodes.end()) {
            Node *node = new Node();
            node->id = id;
            nodes[id] = node;
            return node;
        }
        return nodes[id];
    }
    
    // Sets the distance between two nodes
    void setDistance(char from, char to, int distance) {
        Node *fromNode = getOrAdd(from);
        Node *toNode = getOrAdd(to);
        fromNode->neighbors[toNode] = distance;
        toNode->neighbors[fromNode] = distance;
    }
    
    // Prints the graph
    void print() {
        for (auto &pair: nodes) {
            std::cout << "Node " << pair.first << " is "
                      << (pair.second->isChargingStation ? "a charging station" : "not a charging station")
                      << '\n';
            for (auto &pair2: pair.second->neighbors) {
                std::cout << "  -> " << pair2.first->id << " with distance " << pair2.second << '\n';
            }
        }
    }
    
    // Public interface to initiate Dijkstra's algorithm
    void dijkstra(char start) {
        // Check if the node exists. If not, print an error message
        if (nodes.find(start) == nodes.end()) {
            std::cerr << "Node " << start << " does not exist\n";
            return;
        }
        dijkstra(nodes[start]);
    }
    
private:
    //Stores all nodes in the graph
    std::map<char, Node *> nodes;
    // Dijkstra's algorithm
    void dijkstra(Node *start) {
        // Check if the node has neighbors. If not, print an error message
        if (nodes[start->id]->neighbors.empty()) {
            std::cerr << "Node " << start->id << " has no neighbors\n";
            return;
        }
        
        std::cout << "Finding closest charging station from node " << start->id << "...\n";

        const int INF = 2147483647;
        std::map<Node *, int> distances;
        std::map<Node *, Node *> previous;
        std::set<std::pair<int, Node *>> priorityQueue;
        
        // Initialize distances and previous nodes
        for (auto &pair: nodes) {
            Node *node = pair.second;
            distances[node] = (node == start ? 0 : INF); // 0 for the start node, INF for all others
            previous[node] = nullptr;
            priorityQueue.insert({distances[node], node});
        }
        
        while (!priorityQueue.empty()) {
            // Get the node with the smallest distance and remove it from the priority queue (marking it as visited)
            Node *u = priorityQueue.begin()->second;
            priorityQueue.erase(priorityQueue.begin());
            
            // For each neighbor of the current node, update the distance if a shorter path is found
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

        // Find all charging stations and sort them by distance
        std::vector<std::pair<int, Node *>> chargingStations;
        for (auto &pair: nodes) {
            Node *node = pair.second;
            if (node->isChargingStation) {
                chargingStations.emplace_back(distances[node], node);
            }
        }
        
        std::sort(chargingStations.begin(), chargingStations.end());

        
        // Print the results
        for (auto &pair: chargingStations) {
            Node *node = pair.second;
            std::cout << "Distance to charging station " << node->id << ": " << pair.first << '\n';
            std::cout << "Path: ";
            std::string path;
            for (Node *v = node; v != nullptr; v = previous[v]) {
                path += v->id; // This will result in the path being backwards (from the charging station to the start node)
            }
            // Reverse the path to get the correct order
            std::reverse(path.begin(), path.end());
            std::cout << path << '\n';
            std::cout << '\n';
        }
    }
} graph;

class Application {
public:
    // Reads the data from the file and initializes the graph
    static void initialize() {
        std::ifstream file(filepath);
        std::string line;
        
        // Check if the file exists. If not, create a new file to guide the user on where to put the data
        if (!file.is_open()) {
            std::cerr << "File not found\n";
            std::ofstream newFile("PUT data.csv HERE.txt");
            exit(1);
        }
        
        // Note: Each line should be in the following format: ID,IsChargingStation,Neighbor1Distance,Neighbor2Distance,Neighbor3Distance,...
        // Example: A,Y,B1,C2
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens; // Store the tokens (entry in .csv) in a vector
            std::string token;
            
            // Split the line by commas and insert them in the vector
            while (std::getline(iss, token, ',')) {
                tokens.emplace_back(token);
            }
            
            Node *from = graph.getOrAdd(tokens[0][0]); // [0] is the ID as a string, [0][0] is the ID as a char
            from->isChargingStation = tokens[1][0] == 'Y';
            for (int i = 2; i < tokens.size(); i++) { // Start from 2 to skip the ID and IsChargingStation
                char to = tokens[i][0]; // Grabs the ID as a char
                int distance = std::stoi(tokens[i].substr(1)); // Grabs the distance as an int
                graph.setDistance(from->id, to, distance);
            }
        }
        
        file.close();
    }
    
    // Main loop
    static void start() {
        std::cout << "Welcome to the charging station finder!\n"
                     "Please enter the node you are at (A-W)\n"
                     "Type 'exit' to quit, or 'print' to view the map:\n";
        std::string input;
        // This loop will keep running until the user types 'exit'
        while (std::getline(std::cin, input)) {
            if (input == "exit") {
                break;
            } else if (input == "print") {
                graph.print();
            } else if (input.size() == 1) {
                graph.dijkstra(toupper(input[0])); // Make sure the input gets converted to uppercase
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