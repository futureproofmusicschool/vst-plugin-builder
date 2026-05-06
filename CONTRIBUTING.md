# Contributing

Thanks for your interest in improving **vst-plugin-builder**. This repo is a conversational workshop for building JUCE plugins with an AI agent — contributions are welcome across prompts, rules, the JUCE Docs MCP server, example plugins, and CI.

## Ground rules

- Be respectful. See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
- Security issues go through GitHub's private vulnerability reporting, not public issues. See [SECURITY.md](SECURITY.md).
- By submitting a contribution you agree it is licensed under the project's [MIT LICENSE](LICENSE).

## Setup

```bash
git clone --recurse-submodules https://github.com/futureproofmusicschool/vst-plugin-builder
cd vst-plugin-builder
```

Prerequisites (see [CLAUDE.md](CLAUDE.md) for the full table):

- CMake 3.22+
- Xcode Command Line Tools (macOS) or MSVC (Windows)
- Node.js (for the JUCE Docs MCP server)
- Optional: `pluginval` (`brew install pluginval`)

Verify the toolchain by building one of the bundled example plugins:

```bash
cd plugins/underwater
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## What to contribute

| Area | Files |
|------|-------|
| Workflow phases | `prompts/*.md` |
| JUCE rules / known issues | `rules/*.md`, `rules/known-issues.yaml` |
| MCP server | `.mcp-servers/juce-docs/` |
| Example plugins | `plugins/<Name>/` |
| CI / installers | `.github/workflows/`, `scripts/`, `templates/` |
| Docs | `README.md`, `WORKFLOW.md`, `AGENTS.md`, `CLAUDE.md` |

### Adding a new example plugin

1. Copy `templates/CMakeLists.txt.template` into `plugins/<Name>/`.
2. Set `COMPANY_NAME`, `PLUGIN_MANUFACTURER_CODE` (4 chars), and `BUNDLE_ID` per [rules/plugin-conventions.md](rules/plugin-conventions.md).
3. Verify it builds clean and (if applicable) passes `pluginval --strictness-level 5`.
4. Add a short blurb to the README's examples list.

## Pull requests

- Branch from `main`. Keep PRs focused — one logical change per PR.
- Include a short description of *why*, not just *what*.
- Confirm the build passes locally for any plugin you touched.
- For DSP changes, run `ctest` in the plugin's build directory if it has a `tests/` folder.
- Don't commit `build/`, `installer/`, signing identities, Apple Team IDs, or `.env` files (the `.gitignore` covers most of this — double-check).
- The `build-installer.yml` CI workflow runs on tagged releases and manual dispatch; PRs should at minimum compile cleanly.

## Releasing

Tag-driven release flow (handled by `.github/workflows/build-installer.yml`):

```bash
git tag v1.0.0-PluginName
git push origin v1.0.0-PluginName
```

CI builds Mac `.pkg` and Windows `.exe` installers and attaches them to a GitHub Release. The tag format `v<semver>-<PluginDirName>` is required.

Update [CHANGELOG.md](CHANGELOG.md) under `## [Unreleased]` for any user-facing change before tagging.

## Code style

- C++: follow JUCE house style (4-space indent, brace on new line for functions). Defer to the surrounding file when in doubt.
- Markdown: wrap at a comfortable width; tables and fenced code blocks are encouraged.
- Shell scripts: `set -euo pipefail` at the top.

## Questions

Open a [Discussion](../../discussions) for design questions, or a regular issue for bugs and feature requests.
