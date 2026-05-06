# Using VST Plugin Builder with Any Coding Agent

This repo's plugin-building workflow is **agent-agnostic**. The actual instructions live in plain markdown at the repo root:

- `prompts/dream.md` — concept exploration
- `prompts/plan.md` — DSP architecture and CMake setup
- `prompts/implement.md` — three-stage build with validation
- `prompts/deploy.md` — install to your DAW
- `prompts/installer.md` — package a signed, notarized `.pkg`
- `rules/juce-patterns.md`, `rules/plugin-conventions.md`, `rules/brand.md` — JUCE conventions and styling notes that the prompts reference

Any coding agent that can (a) read those files and (b) call shell commands and write files can drive the workflow.

The JUCE Docs MCP server in `.mcp-servers/juce-docs/` works with any [Model Context Protocol](https://modelcontextprotocol.io)-aware client — Claude Code, Cursor, Cline, Continue, Windsurf, etc. — and grounds the agent in real JUCE class signatures.

## Tested setups

| Agent | Status |
|-------|--------|
| **Claude Code** | First-class. Slash commands `/dream`, `/plan`, `/implement`, `/deploy`, `/installer` are wired up via `.claude/skills/` and the JUCE Docs MCP auto-loads via `.mcp.json`. |
| Cursor | Should work — see "Cursor" below. Not regularly tested. |
| Cline / Roo / Kilo / Continue | Should work — see "Generic agent" below. Not regularly tested. |
| Aider | Should work via `--read prompts/*.md`. Not regularly tested. |

PRs adding tested support for other agents are welcome.

## Setup by agent

### Claude Code (recommended)

Just open the repo:

```bash
git clone --recurse-submodules <repo-url>
cd vst-plugin-builder
claude
```

Then run `/dream "your plugin idea"`. The MCP server, skills, and rules auto-load. Nothing else to configure.

### Cursor

Cursor reads project rules from `.cursor/rules/`. The simplest setup:

1. Create `.cursor/rules/vst-plugin-builder.mdc` with this content:

   ```markdown
   ---
   description: VST plugin development workflow
   alwaysApply: true
   ---

   When the user asks to build, design, plan, implement, deploy, or package
   an audio plugin, follow the instructions in the relevant file under
   `prompts/`:

   - "dream" / "design" / "concept" → prompts/dream.md
   - "plan" / "architecture" → prompts/plan.md
   - "implement" / "build" / "code" → prompts/implement.md
   - "deploy" / "install" → prompts/deploy.md
   - "installer" / "package" / "ship" → prompts/installer.md

   Use the conventions in rules/juce-patterns.md and rules/plugin-conventions.md.
   ```

2. Configure the JUCE Docs MCP server in Cursor's MCP settings, pointing at:
   ```
   .mcp-servers/juce-docs/juce-docs-mcp-server/dist/index.js
   ```
   (Run `cd .mcp-servers/juce-docs/juce-docs-mcp-server && npm install && npm run build` first if `dist/` doesn't exist.)

3. In chat, just say "let's dream up a plugin" or "plan the underwater plugin" — Cursor will pick the right prompt file based on the rule.

### Aider

```bash
aider --read prompts/dream.md --read rules/juce-patterns.md --read rules/plugin-conventions.md
```

Then in the chat: "Follow the instructions in dream.md to build a shimmer reverb concept." Switch the `--read` files when you move between phases.

### Generic agent (Cline, Continue, Windsurf, etc.)

Drop the relevant `prompts/*.md` and `rules/*.md` files into the agent's context (most have a "files" or "context" mechanism), then ask the agent to follow them.

If the agent supports MCP, configure `juce-docs` from `.mcp.json` — the schema is standard.

## Workflow phases

The five phases are sequential but resumable. Each one writes its output to a known location so the next phase can pick up.

```
prompts/dream.md      → plugins/<Name>/.ideas/{creative-brief,parameter-spec,ui-mockup}.md
prompts/plan.md       → plugins/<Name>/{CMakeLists.txt, .ideas/architecture.md, Source/*}
prompts/implement.md  → fully working plugin in plugins/<Name>/build/
prompts/deploy.md     → installed plugin in ~/Library/Audio/Plug-Ins/
prompts/installer.md  → signed .pkg in plugins/<Name>/installer/
```

Resume from any phase by pointing the agent at the next prompt file with the plugin name as context.

## Why agent-agnostic

The differentiator that matters most — the JUCE Docs MCP server — is already standard, so any MCP-aware agent gets it for free. The slash-command UX in Claude Code is convenience, not a moat. Keeping the prompts at repo root means the workflow doesn't lock you into one tool.
