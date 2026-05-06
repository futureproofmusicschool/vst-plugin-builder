# Security Policy

## Reporting a vulnerability

If you discover a security issue in this repository — including in the
JUCE Docs MCP server, the build/installer scripts, or example plugins —
please report it privately rather than opening a public issue.

Use GitHub's [private vulnerability reporting](https://docs.github.com/en/code-security/security-advisories/guidance-on-reporting-and-writing-information-about-vulnerabilities/privately-reporting-a-security-vulnerability)
on this repository, or open a draft security advisory.

We aim to acknowledge reports within 7 days.

## Scope

In scope:
- Code execution risks in `.mcp-servers/juce-docs/`
- Build script issues in `scripts/`
- GitHub Actions workflow issues in `.github/workflows/`
- Example plugin code under `plugins/`

Out of scope:
- Vulnerabilities in JUCE itself — report those upstream at
  https://github.com/juce-framework/JUCE
- Vulnerabilities in third-party npm dependencies — report to the
  upstream maintainer; we will update the pin when a fix lands.

## What this repo executes on your machine

Opening this repo in Claude Code auto-launches a local Node.js subprocess
for the JUCE Docs MCP server (see `.mcp.json`). The server fetches HTML
from public documentation URLs and parses it. Review
`.mcp-servers/juce-docs/juce-docs-mcp-server/src/` before opening the
repo if you do not trust the source you cloned from.
