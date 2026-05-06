# VST Plugin Builder

[![Build Plugin Installers](https://github.com/futureproofmusicschool/vst-plugin-builder/actions/workflows/build-installer.yml/badge.svg)](https://github.com/futureproofmusicschool/vst-plugin-builder/actions/workflows/build-installer.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![JUCE](https://img.shields.io/badge/JUCE-8-blue.svg)](https://juce.com)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

Design and build professional audio plugins by talking to an AI. This repo is a conversational workshop for creating VST3, AU, and standalone plugins on top of the [JUCE](https://juce.com) framework, no C++ wizardry required to get started.

You describe what you want the plugin to sound like and how the UI should feel. Your AI agent handles the architecture, DSP scaffolding, JUCE plumbing, and build pipeline. You stay in the creative seat.

## What makes this different

**Real JUCE API grounding via MCP.** Most AI plugin scaffolds let the model improvise C++ from training data — which is how you end up with calls to `juce::Doesnt::Exist()` that compile-fail or, worse, silently produce wrong audio. This repo ships a [Model Context Protocol](https://modelcontextprotocol.io) server (`.mcp-servers/juce-docs/`) that gives the agent live access to real JUCE class signatures, so it looks up APIs instead of guessing. Your agent works from the actual JUCE 8 docs in real time. This is the single hardest-to-replicate feature here.

**It teaches as it builds.** Every phase narrates what it's doing in plain language — why it picked a particular DSP block, what `processBlock` is and what rules govern it, what code signing is for. By the end of building a plugin, you'll have a clearer mental model of how plugins actually work, not just a binary.

**Stage-gated verification.** The implementation phase runs in three verified stages (foundation → DSP → UI). The build is checked after each stage; if three fix attempts on the same error fail, the workflow stops and asks you rather than patching forever.

**Working examples, not just templates.** Two example plugins ship in the repo and build out of the box: **underwater** (modulation effect) and **ReverbATron** (a harmonically-tracking reverb).

## Who this is for

- **Producers and sound designers** who have plugin ideas but don't want to spend a year learning C++ before hearing them.
- **Students** working through audio programming, DSP, and creative tool building.
- **Developers** who want a structured, conversational workflow on top of JUCE 8 with sensible defaults already wired up.

## The five-phase workflow

The instructions live in plain markdown at the repo root, in `prompts/`:

1. **`prompts/dream.md`** — Describe the plugin in plain language. Generates a creative brief, parameter spec, and UI concept.
2. **`prompts/plan.md`** — Designs the DSP architecture, picks JUCE modules, and produces a `CMakeLists.txt` that compiles as an empty shell.
3. **`prompts/implement.md`** — Builds the plugin in three verified stages. Validates the build at each one.
4. **`prompts/deploy.md`** — Clean Release build, installs to your DAW's plugin folders.
5. **`prompts/installer.md`** — Packages a macOS `.pkg` (and Windows `.exe` via CI) for distribution.

Each plugin lives in its own folder under `plugins/` with its own CMake config, source, and design notes.

## Quick start (Claude Code)

```bash
git clone --recurse-submodules https://github.com/futureproofmusicschool/vst-plugin-builder
cd vst-plugin-builder
claude
```

Then:

```
/dream "shimmer reverb with pitch-shifted tails"
```

Follow the phases. Full walkthrough in [WORKFLOW.md](WORKFLOW.md).

## Use with other agents

The `prompts/` and `rules/` directories are agent-agnostic. Cursor, Cline, Aider, Continue, Windsurf, and any other coding agent that can read files and run shell commands can drive the workflow. The MCP server is also standard, so any MCP-aware agent can use it.

See [AGENTS.md](AGENTS.md) for setup instructions per agent. Claude Code is the only first-class supported agent — others are tested less, but the architecture is open.

## Prerequisites

| Tool | Required | Install |
|------|----------|---------|
| CMake 3.22+ | Yes | `brew install cmake` |
| Xcode Command Line Tools | Yes | `xcode-select --install` |
| Xcode (full app) | For AU plugins | Mac App Store |
| Node.js | Yes (for JUCE Docs MCP) | `brew install node` |
| pluginval | Optional | `brew install pluginval` |
| A coding agent | Yes | e.g. [Claude Code](https://claude.com/claude-code) |

## Build from the command line

If you want to build a plugin without an AI:

```bash
cd plugins/YourPlugin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## CI and distribution

A GitHub Actions workflow (`.github/workflows/build-installer.yml`) builds Mac `.pkg` and Windows `.exe` installers. Trigger it manually from the Actions tab, or push a tag like `v1.0.0-YourPlugin` to auto-build and attach installers to a GitHub Release.

> **Heads up — local code execution:** opening this repo in an MCP-aware agent starts a local Node.js subprocess (the MCP server defined in `.mcp.json`). The server fetches HTML from `docs.juce.com` and parses it with `cheerio`. If you cloned this repo from a fork or untrusted source, review `.mcp-servers/juce-docs/juce-docs-mcp-server/src/` before opening it. See [SECURITY.md](SECURITY.md).

## License and contributions

This repo is released under the [MIT License](LICENSE).

JUCE is included as a submodule and is licensed separately under GPLv3 / commercial dual-license. Review [JUCE's licensing](https://juce.com/legal/juce-8-licence/) before distributing any plugin built with this repo, especially commercially. The bundled fonts in `resources/fonts/` are distributed by Indian Type Foundry under their own terms — verify before redistributing.

Pull requests, bug reports, and showcases welcome — see [CONTRIBUTING.md](CONTRIBUTING.md) for setup, conventions, and the release flow, and [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community expectations. A full list of bundled third-party components is in [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md). For security issues, see [SECURITY.md](SECURITY.md).

Happy plugin building.
