# Third-Party Licenses & Acknowledgements

This repository is MIT-licensed (see [LICENSE](LICENSE)), but it builds on, bundles, or downloads several third-party components with their own license terms. **Anyone distributing plugins built with this repo must comply with these terms** — most importantly JUCE's.

## JUCE (git submodule at `JUCE/`)

- Upstream: https://github.com/juce-framework/JUCE
- License: **GPLv3** by default, with a **commercial license** available from JUCE Ltd.
- What this means for you:
  - If you ship a plugin built against JUCE under the GPL, your plugin must also be GPL-licensed and you must provide source.
  - To ship a closed-source / proprietary plugin, purchase a JUCE commercial license: https://juce.com/get-juce
  - Educational, hobby, and open-source projects can freely use the GPL path.
- Full text: see `JUCE/LICENSE.md` after running `git submodule update --init --recursive`.

## JUCE Docs MCP server (`.mcp-servers/juce-docs/`)

- A local Node.js subprocess that fetches and parses public JUCE 8 documentation HTML. Source code under this directory is MIT-licensed as part of this repo.
- npm dependencies: see `.mcp-servers/juce-docs/juce-docs-mcp-server/package.json`. Run `npm ls --all` in that directory for a full transitive list. Each dependency is governed by its own license (predominantly MIT / Apache-2.0 / ISC).

## Bundled fonts (`resources/fonts/`)

- **Satoshi** (Regular, Bold) — Indian Type Foundry. Free for personal and commercial use under the [ITF Free Font License](https://www.fontshare.com/licenses/itf-ffl).
- **Panchang** (Medium) — Indian Type Foundry. Free for personal and commercial use under the [ITF Free Font License](https://www.fontshare.com/licenses/itf-ffl).
- These fonts are bundled for convenience; verify the current ITF license terms before redistribution.

## Example plugins (`plugins/`)

The DSP and UI code under `plugins/` is original to this project and MIT-licensed alongside the rest of the repo. No third-party impulse responses, sample data, or presets are bundled.

## Tools invoked at build / CI time

These are not redistributed by this repo but are referenced by scripts and CI:

- **CMake** — BSD 3-Clause
- **pluginval** — GPLv3 (https://github.com/Tracktion/pluginval)
- **Inno Setup** — Free, with its own license (https://jrsoftware.org/isinfo.php)
- **GitHub Actions** used in `.github/workflows/build-installer.yml` — see each action's repo for its license.

---

If you spot a missing attribution or believe this file is inaccurate, please open an issue or PR.
