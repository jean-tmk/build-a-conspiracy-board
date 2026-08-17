# Build a Conspiracy Board

A tactile browser evidence wall powered by a real **C++20 graph-analysis engine** compiled to WebAssembly. Drag clues, connect them with red thread, add evidence, and ask the engine to identify hubs, bridges, clusters, isolated clues, shortest paths, density, and a calculated suspicion score.

## Architecture

- **C++20 (majority of the codebase):** graph model, weighted edges, Dijkstra pathfinding, connected components, degree and betweenness centrality, JSON serialization, case analysis
- **WebAssembly / Embind:** exposes the C++ engine safely to the browser
- **CMake:** native test and Emscripten build orchestration
- **SQL:** normalized case, evidence, and connection schema
- **Python:** deterministic case-file generator
- **JavaScript + CSS:** tactile drag-and-drop corkboard interface and SVG thread renderer
- **GitHub Actions:** compiles C++ to WASM, tests the native engine, and deploys GitHub Pages

## Build natively

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Build for the browser

```bash
emcmake cmake -S . -B build-wasm
cmake --build build-wasm
cp build-wasm/web/engine.* web/
```

The production deployment is compiled automatically by GitHub Actions.
