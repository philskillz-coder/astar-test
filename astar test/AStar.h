#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <unordered_map>
#include <functional>

struct Point {
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template <>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            size_t h1 = std::hash<int>{}(p.x);
            size_t h2 = std::hash<int>{}(p.y);
            return h1 ^ (h2 << 1);
        }
    };
}

class AStar {
public:
    std::vector<Point> findPath(const std::vector<std::vector<int>>& grid, const Point& start, const Point& goal);

private:
    bool isValid(const std::vector<std::vector<int>>& grid, int x, int y);
    int calculateHeuristic(const Point& current, const Point& goal);
    std::vector<Point> getNeighbors(const std::vector<std::vector<int>>& grid, const Point& node);
    std::vector<Point> reconstructPath(const std::unordered_map<Point, Point>& cameFrom, const Point& current);
};

#endif  // ASTAR_H
