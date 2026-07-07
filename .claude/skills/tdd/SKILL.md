---
name: tdd
description: Test-driven development workflow for implementing a plan
---

# Test-Driven Development

Implement the given plan using strict Red-Green-Refactor TDD.

## Input

$ARGUMENTS

## Workflow

For each piece of behavior in the plan:

### 1. RED — Write a failing test first
- Write a minimal test that captures the next desired behavior.
- Build and run it. **Confirm it fails** (compile error or assertion failure).
- If it already passes, skip to the next behavior.

### 2. GREEN — Minimum implementation to pass
- Write only enough code to make the failing test pass.
- No extras, no abstractions, no "nice to haves".
- Build and run. **Confirm it passes.**

### 3. REFACTOR — Clean up while green
- Improve naming, remove duplication, improve structure.
- Run tests after each change. Nothing should break.

### 4. REPEAT
- Go back to step 1 for the next behavior.
- Continue until the plan is fully implemented.

## Rules

- **Never write implementation before a failing test.**
- **Format** - test_* for test name - NO test classes
- **Small steps** — each cycle should be a focused increment.
- **Build frequently** — catch errors early.
- **Run tests frequently** — validate after every change.
- **If stuck**, write a simpler test for a smaller piece of behavior.

## Build & Test

```bash
uv sync --all-extras -v
uv run pytest
```
