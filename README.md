# Build a Conspiracy Board

> A tactile evidence wall backed by a C++ graph-analysis engine.

**Live exhibit:** https://jean-tmk.github.io/build-a-conspiracy-board/

## What it is

The joke is that every coincidence can look official if it has enough red thread. Underneath that joke is a real graph problem: clues are nodes, claims are weighted edges, and “suspicion” can be measured with connectivity, centrality, paths, and clusters.

## What a visitor can do

1. Drag evidence cards around the board.
2. Add your own clue with a type and confidence score.
3. Enter connection mode and select two clues to tie together.
4. Open Findings to run the graph analysis and inspect hubs, clusters, and the suspicion score.

## How it works

- C++ owns the graph, evidence model, weighted edges, connected components, shortest paths, degree metrics, centrality, and JSON export.
- Emscripten/Embind exposes that engine to the browser as WebAssembly.
- JavaScript is an adapter: it maps pointer interactions to engine calls and renders cards and SVG thread.
- SQL describes a normalized persistent version of cases, evidence, and connections; Python creates deterministic sample cases.

## Repository map

| Path | What it does |
|---|---|
| `.github/workflows/pages.yml` | GitHub Actions workflow that validates, builds, and/or deploys the exhibit. |
| `.gitignore` | Project configuration or supporting source used by the build/deployment. |
| `CMakeLists.txt` | Native and WebAssembly build graph. |
| `db/schema.sql` | Relational schema, query, trigger, view, seed, or validation source. |
| `scripts/generate_case.py` | Domain, engine, tooling, or specification source in the repository’s polyglot architecture. |
| `src/bindings.cpp` | Domain, engine, tooling, or specification source in the repository’s polyglot architecture. |
| `src/graph_engine.cpp` | Domain, engine, tooling, or specification source in the repository’s polyglot architecture. |
| `src/graph_engine.hpp` | Domain, engine, tooling, or specification source in the repository’s polyglot architecture. |
| `tests/engine_test.cpp` | Domain, engine, tooling, or specification source in the repository’s polyglot architecture. |
| `web/board.js` | Browser/application source for the behavior named by this file. |
| `web/case-file.css` | A focused style layer for this named area of the experience. |
| `web/compact.css` | A focused style layer for this named area of the experience. |
| `web/index.html` | The deployable HTML shell: metadata, accessible structure, controls, and script/style entry points. |
| `web/styles.css` | The primary responsive visual system. |

## Languages and why they are here

Percentages below are calculated from the byte counts currently returned by GitHub Linguist. Tiny language-atlas modules are intentionally isolated from the production frontend.

| Language | GitHub | Role |
|---|---:|---|
| C++ | 48.9% | the authoritative graph engine and majority implementation |
| CSS | 35.4% | evidence-wall layout and responsive styling |
| HTML | 11.3% | semantic application shell |
| Python | 2.2% | deterministic case generation |
| CMake | 2.2% | native and WebAssembly build orchestration |

### About the language atlas

Where present, `polyglot/language-atlas.json` is the machine-readable index of the languages assigned to this repository. `polyglot/languages/` contains one small, independent signature module per assignment, and `polyglot/majority/` contains the larger registry that preserves the intended majority language. These files are documentation and comparative code specimens: the live site does not download or execute them.

## Local development

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Then open `http://localhost:8000` unless the framework development server prints a different local address.

## Privacy and access

- No sign-in is required.
- No API key is required for the live exhibit.
- No visitor text is sent to an AI service.
- Any saved progress stays in local browser storage unless the README explicitly describes an optional external architecture.
- Sound begins only after a user gesture where browser autoplay rules require it.

## Deployment

The public version is a static GitHub Pages deployment. The workflow in `.github/workflows/` is the source of truth for its exact build and publish steps. The favicon is stored with the deployed app so browser tabs and bookmarks use the project’s own mark.
