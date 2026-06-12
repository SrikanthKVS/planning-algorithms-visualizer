//
// Created by Srikanth.Sudarshan on 6/4/2026.
//
#include "planners/bfs.h"

#include <algorithm>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::optional<std::vector<Cell> > bfs(const Grid &grid, Cell start, Cell goal) {
    std::queue<Cell> frontier;
    std::unordered_set<Cell> visited;
    std::unordered_map<Cell, Cell> parentMap;

    // Directions list for cell traversal
    const std::vector<Cell> directions = {
        {-1,  0},   // up
        { 1,  0},   // down
        { 0, -1},   // left
        { 0,  1}    // right
    };

    // Setup
    int rows = grid.size(); // number of rows
    int cols = grid[0].size(); // number of columns
    frontier.push(start);
    visited.insert(start);

    // Main Loop
    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();

        if (current == goal) {
            std::vector<Cell> path;
            Cell step = goal;
            while (step != start) {
                path.push_back(step);
                step = parentMap[step];
            }
            path.push_back(start);  // add start explicitly
            std::reverse(path.begin(), path.end());
            return path;
        } else {
            for (const auto &dir: directions) {
                Cell neighbour = {current.row + dir.row, current.col + dir.col};
                if (neighbour.row >= 0 && neighbour.row < rows &&
                    neighbour.col >= 0 && neighbour.col < cols &&
                    grid[neighbour.row][neighbour.col] != 1 &&
                    visited.count(neighbour) == 0) {
                    visited.insert(neighbour);
                    frontier.push(neighbour);
                    parentMap[neighbour] = current;
                }
            }
        }
    }
    return std::nullopt;
}
