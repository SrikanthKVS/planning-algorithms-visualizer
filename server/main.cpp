// server/main.cpp
// ─────────────────────────────────────────────────────────────
// Step 1: Crow hello world
//   - Serves web/ files over HTTP on localhost:8080
//   - Echoes WebSocket messages back to the sender
//   - This is the foundation we'll build the planner on top of
// ─────────────────────────────────────────────────────────────
#include "planners/bfs.h"
#include "planners/dfs.h"
#include "crow_all.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// ── Helper: read a file into a string ────────────────────────
std::string read_file(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

Grid make_default_grid() {
    Grid grid(100, std::vector<int>(100, 0)); // 100x100, all free

    // Block 1 — left vertical wall
    for (int r = 20; r <= 50; r++)
        for (int c = 20; c <= 30; c++)
            grid[r][c] = 1;

    // Block 2 — top horizontal wall
    for (int r = 20; r <= 30; r++)
        for (int c = 20; c <= 60; c++)
            grid[r][c] = 1;

    // Block 3 — bottom right block
    for (int r = 60; r <= 80; r++)
        for (int c = 50; c <= 80; c++)
            grid[r][c] = 1;

    // Block 4 — right vertical wall
    for (int r = 40; r <= 70; r++)
        for (int c = 70; c <= 80; c++)
            grid[r][c] = 1;

    return grid;
}

int main() {
    crow::SimpleApp app;

    // ── HTTP route: serve index.html ─────────────────────────
    CROW_ROUTE(app, "/")([]() {
        auto html = read_file("web/index.html");
        if (html.empty()) {
            return crow::response(404, "index.html not found — "
                                  "make sure you run the server from the project root");
        }
        crow::response res(html);
        res.set_header("Content-Type", "text/html");
        return res;
    });

    // ── HTTP route: serve JS files ───────────────────────────
    CROW_ROUTE(app, "/js/<string>")([](const std::string &filename) {
        auto js = read_file("web/" + filename);
        if (js.empty()) {
            return crow::response(404, "file not found: " + filename);
        }
        crow::response res(js);
        res.set_header("Content-Type", "application/javascript");
        return res;
    });

    // ── WebSocket route ───────────────────────────────────────
    // Right now: just echo every message back
    // Later: parse JSON commands, run planner, stream events
    CROW_WEBSOCKET_ROUTE(app, "/ws")
            .onopen([](crow::websocket::connection &conn) {
                std::cout << "[ws] client connected\n";
                conn.send_text(R"({"type":"connected","message":"RRT server ready"})");

                // Build obstacle JSON from grid
                Grid grid = make_default_grid();
                std::string cells = "[";
                bool first = true;
                for (int r = 0; r < 100; r++) {
                    for (int c = 0; c < 100; c++) {
                        if (grid[r][c] == 1) {
                            if (!first) cells += ",";
                            cells += "{\"row\":" + std::to_string(r) +
                                    ",\"col\":" + std::to_string(c) + "}";
                            first = false;
                        }
                    }
                }
                cells += "]";
                conn.send_text(R"({"type":"set_obstacles","cells":)" + cells + "}");
            })
            .onclose([](crow::websocket::connection & /*conn*/, const std::string &reason) {
                std::cout << "[ws] client disconnected: " << reason << "\n";
            })
            .onmessage([](crow::websocket::connection &conn,
                          const std::string &data,
                          bool /*is_binary*/) {
                std::cout << "[ws] received: " << data << "\n";

                // Very simple JSON parsing — find start and goal row/col
                // We'll use proper JSON parsing later
                // For now use crow's built-in JSON parser
                auto json = crow::json::load(data);
                if (!json) {
                    conn.send_text(R"({"type":"error","message":"invalid json"})");
                    return;
                }

                if (json["type"].s() == std::string("plan")) {
                    Cell start = {
                        (int) json["start"]["row"].i(),
                        (int) json["start"]["col"].i()
                    };
                    Cell goal = {
                        (int) json["goal"]["row"].i(),
                        (int) json["goal"]["col"].i()
                    };

                    Grid grid = make_default_grid();
                    std::string algo = json["algorithm"].s();

                    std::optional<std::vector<Cell>> result;

                    if (algo == "dfs") {
                        result = dfs(grid, start, goal);
                    } else {
                        result = bfs(grid, start, goal);  // default to BFS
                    }

                    if (result.has_value()) {
                        std::string path_json = "[";
                        for (int i = 0; i < (int) result->size(); i++) {
                            path_json += "{\"row\":" + std::to_string((*result)[i].row) +
                                    ",\"col\":" + std::to_string((*result)[i].col) + "}";
                            if (i + 1 < (int) result->size()) path_json += ",";
                        }
                        path_json += "]";
                        conn.send_text(R"({"type":"path_update","path":)" + path_json + "}");
                    } else {
                        conn.send_text(R"({"type":"error","message":"no path found"})");
                    }
                }
            });

    std::cout << "──────────────────────────────────────\n";
    std::cout << "  Path Planning Visualizer\n";
    std::cout << "  http://localhost:8080\n";
    std::cout << "  WebSocket: ws://localhost:8080/ws\n";
    std::cout << "──────────────────────────────────────\n";
    std::cout << "  Open the URL in your Windows browser\n";
    std::cout << "  (WSL and Windows share localhost)\n";
    std::cout << "──────────────────────────────────────\n";

    app.port(8080)
            .multithreaded()
            .run();

    return 0;
}
