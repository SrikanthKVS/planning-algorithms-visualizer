#ifndef PLANNING_ALGS_BFS_H
#define PLANNING_ALGS_BFS_H

#include "grid_types.h"
#include <optional>
#include <vector>

// ── BFS function declaration ──────────────────────────────────
// Returns the path as a vector of cells, or nullopt if no path exists
std::optional<std::vector<Cell>> bfs(const Grid& grid, Cell start, Cell goal);

#endif //PLANNING_ALGS_BFS_H