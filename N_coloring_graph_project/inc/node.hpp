#pragma once
#include <vector>
#include <mutex>

class Node {
    private:
        int id;
        int colorId;
        std::vector<Node*> adjacentNodes;
        bool visited;
        std::mutex colorMutex;
        std::mutex visitedMutex;

    public:
        Node(int id);
        int getId();
        int getColorId();
        std::vector<Node*> getAdjacentNodes();
        bool getVisited();
        void setColorId(int colorId);
        void setVisited(bool visited);
        void addEdge(Node* node);
};