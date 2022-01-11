#include "../inc/node.hpp"


Node::Node(int id): id(id), colorId(1), visited(false) {
    this->adjacentNodes = std::vector<Node*>();
}

int Node::getId(){
    return id;
}

int Node::getColorId(){
    return colorId;
}

std::vector<Node*> Node::getAdjacentNodes() {
    return adjacentNodes;
}

bool Node::getVisited() {
    return visited;
}

void Node::setColorId(int colorId){
    colorMutex.lock();
    this->colorId = colorId;
    colorMutex.unlock();
}

void Node::setVisited(bool visited){
    visitedMutex.lock();
    this->visited = visited;
    visitedMutex.unlock();
}

void Node::addEdge(Node* node){
    this->adjacentNodes.push_back(node);
}