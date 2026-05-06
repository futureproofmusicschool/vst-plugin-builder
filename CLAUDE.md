# VST Plugin Builder - Audio Plugin Workshop

A creative workspace for designing and building VST/AU audio plugins using the JUCE framework.

## How This Works

The workflow is **agent-agnostic** — the source of truth for each phase is a markdown prompt file at the repo root. Claude Code wraps these as slash commands via thin shims in `.claude/skills/`. Other agents (Cursor, Cline, Aider, etc.) can read the prompts directly. See [AGENTS.md](AGENTS.md).

Five phases guide the workflow, with `/debug` available as a horizontal helper at any point:

| Phase | Claude Code | Source of truth | What it does |
|-------|-------------|-----------------|--------------|
| 1 | `/dream` | `prompts/dream.md` | Concept exploration → creative brief, parameter spec, UI concept |
| 2 | `/plan` | `prompts/plan.md` | DSP architecture, JUCE module pick, CMakeLists.txt, source stubs that compile |
| 3 | `/implement` | `prompts/implement.md` | Three verified stages: foundation → DSP → UI |
| 4 | `/deploy` | `prompts/deploy.md` | Clean Release build, install to system plugin folders |
| 5 | `/installer` | `prompts/installer.md` | Signed, notarized macOS `.pkg` for distribution |
| — | `/debug` | `prompts/debug.md` | Diagnose build/load/audio/state/crash/automation failures; consults `rules/known-issues.yaml` |

The prompts instruct the agent to **teach as it builds** — narrate decisions, explain JUCE rules, walk through DSP choices in plain language. The user finishes with a working plugin and a clearer mental model of how plugins are made.

Each plugin also has a `status.json` at its root tracking lifecycle state (current phase, formats built, last build/validation result). See `rules/status-schema.md` for the schema.

## Plugin Conventions

| Setting | Value |
|---------|-------|
| Company | Set per plugin (no default) |
| Manufacturer Code | Set per plugin (4 chars) |
| Bundle ID pattern | `com.[yourcompany].[pluginname]` |
| Formats | AU, VST3, Standalone, CLAP |
| Build system | CMake (not Projucer) |

Each plugin lives in `plugins/[PluginName]/` with its own `CMakeLists.txt`, `Source/`, and `.ideas/` directories.

## Build Commands

```bash
# Build a plugin
cd plugins/[PluginName]
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Clean build
rm -rf plugins/[PluginName]/build
```

## JUCE

JUCE is included as a git submodule at `./JUCE`. Each plugin's CMakeLists.txt references it via `add_subdirectory(../../JUCE JUCE)`.

After cloning this repo, run:
```bash
git submodule update --init --recursive
```

## MCP Server

The **JUCE Docs MCP Server** is configured in `.mcp.json`. It provides access to JUCE C++ class documentation so the agent can look up real APIs instead of guessing. The server lives at `.mcp-servers/juce-docs/` and auto-loads when you open the repo in any MCP-aware agent (Claude Code, Cursor, Cline, Continue, Windsurf, etc.).

## Project Structure

```
vst-plugin-builder/
├── README.md                     <- Public-facing intro
├── AGENTS.md                     <- How to use this with any coding agent
├── CLAUDE.md                     <- You are here (Claude Code-specific notes)
├── WORKFLOW.md                   <- Detailed walkthrough of the five phases
├── SECURITY.md                   <- Security policy
├── LICENSE                       <- MIT
├── .mcp.json                     <- JUCE Docs MCP server config
├── prompts/                      <- Agent-agnostic source of truth for each phase
│   ├── dream.md
│   ├── plan.md
│   ├── implement.md
│   ├── deploy.md
│   ├── installer.md
│   └── debug.md
├── rules/                        <- JUCE conventions referenced by prompts
│   ├── juce-patterns.md
│   ├── plugin-conventions.md
│   ├── brand.md
│   ├── status-schema.md          <- Per-plugin status.json shape
│   └── known-issues.yaml         <- Recurring failures + fixes (consulted by /deploy and /debug)
├── JUCE/                         <- JUCE framework (git submodule)
├── .mcp-servers/juce-docs/       <- JUCE Docs MCP server
├── plugins/                      <- Each plugin gets its own directory
│   └── underwater/               <- Example
│       ├── .ideas/               <- Creative brief, specs, UI mockups
│       ├── Source/               <- C++ source files
│       ├── CMakeLists.txt        <- Build config
│       ├── build/                <- Build output (gitignored)
│       └── installer/            <- .pkg installer output (gitignored)
├── templates/                    <- CMake and project templates
├── resources/fonts/              <- Optional bundled fonts (Panchang, Satoshi)
├── scripts/                      <- Build/installer helper scripts
├── .github/workflows/            <- CI for cross-platform installers
└── .claude/skills/               <- Thin shims that delegate to prompts/
    ├── dream/SKILL.md
    ├── plan/SKILL.md
    ├── implement/SKILL.md
    ├── deploy/SKILL.md
    ├── installer/SKILL.md
    └── debug/SKILL.md
```

## Prerequisites

| Tool | Status | Notes |
|------|--------|-------|
| CMake 3.22+ | Required | `brew install cmake` |
| Xcode Command Line Tools | Required | `xcode-select --install` |
| Xcode (full app) | Recommended | Required for AU plugin signing. CLT alone works for VST3. |
| pluginval | Optional | Automated plugin validation. `brew install pluginval` |
| Node.js | Required | For JUCE Docs MCP server |

## CI / Installers

A GitHub Actions workflow at `.github/workflows/build-installer.yml` builds plugins on both Mac and Windows and produces downloadable installers:

- **Manual trigger**: Actions → "Build Plugin Installers" → Run workflow, enter the plugin name
- **Tag trigger**: Push a tag like `v1.0.0-ReverbATron` to auto-build and create a GitHub Release with both installers attached

Every CI build also runs:
- **pluginval** at strictness 5 against the produced VST3 — fails the build on validation errors
- **ctest** for plugins that have a `tests/` directory — fails the build on test regressions

| Platform | Installer | Plugin Location |
|----------|-----------|-----------------|
| Mac | `.pkg` | `/Library/Audio/Plug-Ins/VST3/` and `.../Components/` |
| Windows | `.exe` (Inno Setup) | `C:\Program Files\Common Files\VST3\` |

## Reference Files

| File | Contents |
|------|----------|
| `prompts/*.md` | Agent-agnostic instructions for each phase |
| `rules/juce-patterns.md` | CMake workflow, module usage, common gotchas |
| `rules/plugin-conventions.md` | Naming, structure, format standards |
| `rules/brand.md` | Visual branding notes (no default brand; user-supplied) |
| `rules/status-schema.md` | Per-plugin `status.json` schema and phase values |
| `rules/known-issues.yaml` | Recurring failure modes with detection patterns and fixes |
| `templates/tests/` | Catch2 + CTest scaffold for opt-in DSP unit tests |
| `AGENTS.md` | How to use this repo with non-Claude-Code agents |
