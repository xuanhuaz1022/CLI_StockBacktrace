# 🤖 Elite Agentic Development Team Protocol

## 1. Role Definition

You are working in a team You have the following team members:

- **Architect Agent**: @Backend Architect
- **Coder Agent**: @SOLOCODER.
- **Reviewer Agent**: @Backend Architect.
- **QA Agent**: Debug Bot.
- **DevOps Agent**: @Git 仓库专家

## 2. Core Memory Principle

> **CRITICAL**: The file system is the ONLY memory.

- **Single Source of Truth**: Rely on `PROGRESS.md` and `backend_architecture.md`, NOT chat history.
- **State Persistence**: Every action must update `PROGRESS.md`.
- **Context Reset**: Assume every new task starts in a FRESH window.

---

## 3. Global Workflow Rules

### Phase 1: Pre-Run (Planning)

1. **Load State**: Read `backend_architecture.md` AND `PROGRESS.md`.
2. **Architecture Check**: Stop if request deviates from architecture.
3. **Decomposition**: Break request into atomic sub-tasks.
4. **Wait**: Present list and WAIT for user selection.

### Phase 2: Execution (Quality Loop)

For each selected task:

1. **Implement**: Write code (Strict Typing, Modular).
2. **Peer-Review**: Let backend architect check for leaks, security, and edge cases. Let debug Bot check for errors and runtime bugs. **Fix immediately**.
3. **Test**: Generate & Run tests. **Self-heal** until GREEN.
4. **Document**: Update `PROGRESS.md` (See Format Below).

### Phase 3: Post-Run (Handover)

1. **Git**: Stage changes & Commit (Semantic Message).
2. **Stop**: Output "🛑 CODING PAUSED. Waiting for Human Review."
3. Let "git仓库专家" check the commit message and push to remote repository.
4. **Reset Instruction**: Tell user to **CLOSE this window** and open a NEW one.

---

## 4. PROGRESS.md Update Format

When updating `PROGRESS.md`, strictly follow this structure:

```markdown
## Current Status: [WAITING_REVIEW]

## Last Completed Task: [Task ID]

## Next Immediate Action: [Next Task Description]

### Task Log

- [x] [Task ID]: [Name]
  - Files: [List files]
  - Notes: [Key decisions]
- [ ] [Next Task ID]: [Name]
```

## 🛡️ C++ Specific Protocol (CRITICAL)

- **Trigger**: Whenever the task involves C++ files (`.cpp`, `.h`, `.hpp`, `.cc`).
- **Mandatory Pre-Run**: You MUST read `@cpp_rule.md` BEFORE writing any C++ code.
- **Compliance Check**:
  - Verify that memory management follows the rules in `cpp_rule.md` (e.g., Smart Pointers vs. Raw Pointers).
  - Ensure modern C++ standards (C++17/20) are used as defined.
  - Check naming conventions and error handling strategies against `cpp_rule.md`.
- **Violation Action**: If your planned implementation conflicts with `cpp_rule.md`, STOP and propose a compliant alternative.
- **Self-Review Update**: In the Reviewer Agent phase, add a specific check: "Does this code strictly adhere to `cpp_rule.md`?"
