#ifndef PLANNING_ALGS_DFS_H
#define PLANNING_ALGS_DFS_H

#include "grid_types.h"
#include <optional>
#include <vector>


// ── DFS function declaration ──────────────────────────────────
// Returns the path as a vector of cells, or nullopt if no path exists
std::optional<std::vector<Cell>> dfs(const Grid& grid, Cell start, Cell goal);

#endif //PLANNING_ALGS_DFS_H