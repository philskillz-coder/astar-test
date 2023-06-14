#include <Windows.h>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include "astar.h"

struct Node {
    Point point;
    int g;  // cost from start node to current node
    int h;  // heuristic cost from current node to goal node
    int f;  // total cost

    Node(Point point, int g, int h) : point(point), g(g), h(h), f(g + h) {}
};

struct NodeCompare {
    bool operator()(const Node& lhs, const Node& rhs) const {
        return lhs.f > rhs.f;  // Min-heap based on f value
    }
};

bool AStar::isValid(const std::vector<std::vector<int>>& grid, int x, int y) {
    int rows = grid.size();
    int cols = grid[0].size();
    // != 1: is not wall
    return (x >= 0 && x < rows && y >= 0 && y < cols && grid[x][y] != 1);
}

int AStar::calculateHeuristic(const Point& current, const Point& finish) {
    return abs(current.x - finish.x) + abs(current.y - finish.y);
}

std::vector<Point> AStar::getNeighbors(const std::vector<std::vector<int>>& grid, const Point& node) {
    std::vector<Point> neighbors;
    int rows = grid.size();
    int cols = grid[0].size();

    std::vector<Point> offsets = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };  // right, left, down, up

    for (const Point& offset : offsets) {
        int newX = node.x + offset.x;
        int newY = node.y + offset.y;

        if (isValid(grid, newX, newY)) {
            neighbors.emplace_back(newX, newY);
        }
    }

    return neighbors;
}

std::vector<Point> AStar::reconstructPath(const std::unordered_map<Point, Point>& cameFrom, const Point& current) {
    std::vector<Point> path;
    Point curr = current;

    while (cameFrom.find(curr) != cameFrom.end()) {
        path.push_back(curr);
        curr = cameFrom.at(curr);
    }

    path.push_back(curr);
    std::reverse(path.begin(), path.end());

    return path;
}

std::vector<Point> AStar::findPath(const std::vector<std::vector<int>>& grid, const Point& start, const Point& finish) {
    int rows = grid.size();
    int cols = grid[0].size();

    std::priority_queue<Node, std::vector<Node>, NodeCompare> openSet;
    std::unordered_set<Point> closedSet;
    std::unordered_map<Point, Point> cameFrom;

    openSet.push(Node(start, 0, calculateHeuristic(start, finish)));

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.point.x == finish.x && current.point.y == finish.y) {
            return reconstructPath(cameFrom, current.point);
        }

        closedSet.insert(current.point);

        std::vector<Point> neighbors = getNeighbors(grid, current.point);
        for (const Point& neighbor : neighbors) {
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue;
            }

            int neighborG = current.g + 1;  // assuming each step has a cost of 1
            int neighborH = calculateHeuristic(neighbor, finish);
            int neighborF = neighborG + neighborH;

            Node neighborNode(neighbor, neighborG, neighborH);

            bool found = false;
            std::priority_queue<Node, std::vector<Node>, NodeCompare> openSetCopy = openSet;
            while (!openSetCopy.empty()) {
                const Node& node = openSetCopy.top();
                if ((node.point.x == neighbor.x && node.point.y == neighbor.y) && neighborF >= node.f) {
                    found = true;
                    break;
                }
                openSetCopy.pop();
            }

            if (!found) {
                cameFrom[neighbor] = current.point;
                openSet.push(neighborNode);
            }
        }
    }

    return {};  // no path found
}
