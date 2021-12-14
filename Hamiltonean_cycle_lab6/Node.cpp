#include "Node.hpp"
#include <iostream>

Node::Node(int info): information{info}, visited{false}
{
}

int Node::getInfo()
{
    return this->information;
}

void Node::setInfo(int newInfo)
{
    this->information = newInfo;
}

void Node::setVisited(bool visited)
{
    this->visited = visited;
}

bool Node::getVisited()
{
    return this->visited;
}

std::vector<Node*> Node::getGoToNodes()
{
    return this->goToNodes;
}

void Node::addGoToNode(Node* node)
{
    this->goToNodes.push_back(node);
}

bool Node::operator==(const Node node)
{
    return this->information == node.information;
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
    out << node.information << " -> ";
    return out;
}
