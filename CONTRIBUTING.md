# Contributing to ZUI

Thank you for your interest in contributing to ZUI.

## Getting Started

### Prerequisites

- GCC or Clang
- CMake 3.16+
- Wayland development libraries
- EGL and OpenGL development libraries
- xkbcommon

On Debian/Ubuntu:

```bash
sudo apt-get install build-essential cmake pkg-config \
    libwayland-dev libwayland-egl1 libegl1-mesa-dev \
    libgl1-mesa-dev libxkbcommon-dev
```

On Arch Linux:

```bash
sudo pacman -S base-devel cmake wayland mesa libxkbcommon
```

### Building

```bash
cmake -B build
cmake --build build
```

### Running the Example

```bash
./build/zui_example
```

## Development Workflow

1. Fork the repository.
2. Create a feature branch from `main`.
3. Make your changes.
4. Ensure the project builds without warnings.
5. Submit a pull request.

## Commit Messages

Use conventional commit format:

```
feat: add new widget type
fix: handle null pointer in event system
refactor: simplify renderer initialization
docs: update API documentation
test: add window lifecycle tests
```

Keep commits focused and logically separated.

## Pull Requests

- Keep PRs small and focused on a single change.
- Provide a clear description of what the PR does.
- Link related issues if applicable.
- Ensure CI passes before requesting review.

## Code Style

See [FORMATTING.md](FORMATTING.md) for detailed code formatting guidelines.

Key points:

- Use C11.
- Use `snake_case` for functions and variables.
- Use `PascalCase` for types (e.g., `ZuiWindow`).
- Prefer readability over cleverness.
- Keep functions small and focused.

## Architecture Guidelines

- Keep platform-specific code isolated in `src/platform/`.
- Do not leak Wayland details into the public API.
- Widgets describe **what** to render; the renderer decides **how**.
- Every allocated resource must have a clear owner and destruction path.

## Reporting Issues

When reporting bugs, include:

- Operating system and version.
- Wayland compositor (e.g., Sway, GNOME, KDE).
- Steps to reproduce.
- Expected vs actual behavior.
- Relevant error messages or logs.

## Questions

For questions about the codebase or contribution process, open a discussion or issue on GitHub.
