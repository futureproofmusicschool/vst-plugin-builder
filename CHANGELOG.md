# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `CHANGELOG.md`, `THIRD_PARTY_LICENSES.md`.
- GitHub issue and pull request templates.
- Dependabot config for GitHub Actions and the JUCE Docs MCP server's npm dependencies.
- CodeQL workflow scanning the JUCE Docs MCP server (JavaScript/TypeScript).
- README badges and a third-party-licensing note covering JUCE.

### Changed
- Example plugins now use a generic `ExampleCo` / `com.example.*` publisher identity instead of a specific brand, so they read clearly as templates.
- `build-installer.yml` and `codeql.yml` workflows: third-party Actions pinned to commit SHAs (Dependabot keeps them current).
- LICENSE copyright line now reads `Futureproof Music School and contributors`.

### Removed
- Example plugin `RoomCorrection` (was untested and not ready for distribution).

## [0.1.0] - 2026-05-05

### Added
- Initial public release of the conversational plugin-building workflow.
- Five-phase prompts: `dream`, `plan`, `implement`, `deploy`, `installer`, plus the `debug` helper.
- JUCE Docs MCP server at `.mcp-servers/juce-docs/`.
- Example plugins: `underwater`, `ReverbATron`, `RoomCorrection`.
- CI workflow `build-installer.yml` producing Mac `.pkg` and Windows `.exe` installers, with `pluginval --strictness-level 5` and optional `ctest`.

[Unreleased]: https://github.com/futureproofmusicschool/vst-plugin-builder/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/futureproofmusicschool/vst-plugin-builder/releases/tag/v0.1.0
