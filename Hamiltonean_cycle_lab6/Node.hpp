#pragma once
#include <string>
#include <vector>

class Node
{
private:
	int information;
	std::vector<Node*> goToNodes;
	bool visited;

public:
	Node(int info);

	int getInfo();
	void setInfo(int newInfo);
	void setVisited(bool visited);
	bool getVisited();
	std::vector<Node*> getGoToNodes();

	void addGoToNode(Node* node);
	friend std::ostream& operator<< (std::ostream& out, const Node& node);
	

};

