#ifndef PLANNING_ALGS_GRID_TYPES_H
#define PLANNING_ALGS_GRID_TYPES_H

#include <vector>

// ── Cell ─────────────────────────────────────────────────────
struct Cell {
    int row = 0;
    int col = 0;

    bool operator==(const Cell& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Cell& other) const {
        return !(*this == other);
    }
};

// ── Hash specialization for Cell ─────────────────────────────
// Tells unordered_set/unordered_map how to hash a Cell
namespace std {
    template<>
    struct hash<Cell> {
        size_t operator()(const Cell& c) const {
            return hash<int>()(c.row) ^ (hash<int>()(c.col) << 1);
        }
    };
}

// ── Grid type alias ───────────────────────────────────────────
// 0 = free, 1 = obstacle, other values = terrain cost (for Dijkstra later)
using Grid = std::vector<std::vector<int>>;

#endif //PLANNING_ALGS_GRID_TYPES_H