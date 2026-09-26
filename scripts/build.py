#!/usr/bin/env python3
"""Configure and build the project for the web with Emscripten.

Examples:
    python scripts/build.py                          # debug build of everything
    python scripts/build.py -c release               # optimised build
    python scripts/build.py -t hello_triangle        # build a single target
    python scripts/build.py --fresh                  # wipe the build directory first
    python scripts/build.py -c release -D PXL_ENABLE_CLOSURE=OFF
    python scripts/build.py --clean                  # delete the build directory and exit

This is a thin, cross-platform wrapper around the CMake presets; everything it
does can also be done by hand:
    cmake --preset web-debug && cmake --build --preset web-debug
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

import _project

TOOLCHAIN_RELATIVE_PATH = Path("upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")


class BuildError(Exception):
    """A problem the user can fix; reported without a stack trace."""


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "-c", "--config", choices=_project.CONFIGS, default=_project.DEFAULT_CONFIG,
        help="build configuration (default: %(default)s)",
    )
    parser.add_argument(
        "-t", "--target", action="append", default=[], metavar="NAME",
        help="build only this target; may be repeated (default: all targets)",
    )
    parser.add_argument(
        "-D", dest="definitions", action="append", default=[], metavar="NAME=VALUE",
        help="CMake cache entry forwarded to the configure step; may be repeated",
    )
    parser.add_argument(
        "-j", "--jobs", type=int, metavar="N",
        help="number of parallel build jobs (default: decided by Ninja)",
    )
    parser.add_argument(
        "--fresh", action="store_true",
        help="delete the build directory before building (full rebuild)",
    )
    parser.add_argument(
        "--clean", action="store_true",
        help="delete the build directory and exit",
    )
    parser.add_argument(
        "--emsdk", type=Path, metavar="PATH",
        help="emsdk root directory (default: the EMSDK environment variable, "
             "then the emcc found on PATH)",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="show the full compiler commands")
    return parser.parse_args()


def is_emsdk_root(path: Path) -> bool:
    return (path / TOOLCHAIN_RELATIVE_PATH).is_file()


def find_emsdk(explicit_root: Path | None) -> Path:
    """Locates the emsdk root that provides the CMake toolchain file."""
    if explicit_root is not None:
        if not is_emsdk_root(explicit_root):
            raise BuildError(f"--emsdk: '{explicit_root}' does not look like an emsdk directory "
                             f"(missing {TOOLCHAIN_RELATIVE_PATH.as_posix()}).")
        return explicit_root.resolve()

    candidates: list[Path] = []
    if os.environ.get("EMSDK"):
        candidates.append(Path(os.environ["EMSDK"]))
    emcc = shutil.which("emcc")
    if emcc:
        # <emsdk>/upstream/emscripten/emcc
        candidates.append(Path(emcc).resolve().parents[2])

    for candidate in candidates:
        if is_emsdk_root(candidate):
            return candidate.resolve()

    raise BuildError(
        "Could not find the Emscripten SDK.\n"
        "  1. Install it: https://emscripten.org/docs/getting_started/downloads.html\n"
        "       git clone https://github.com/emscripten-core/emsdk && cd emsdk\n"
        "       ./emsdk install latest && ./emsdk activate latest\n"
        "  2. Then either set the EMSDK environment variable (emsdk_env does this),\n"
        "     or pass --emsdk <path-to-emsdk>."
    )


def require_tool(name: str, install_hint: str) -> None:
    if shutil.which(name) is None:
        raise BuildError(f"'{name}' was not found on PATH. {install_hint}")


def run(command: list[str], environment: dict[str, str]) -> None:
    print(f"\n> {' '.join(command)}", flush=True)
    result = subprocess.run(command, cwd=_project.PROJECT_ROOT, env=environment, check=False)
    if result.returncode != 0:
        raise BuildError(f"Command failed with exit code {result.returncode}.")


def remove_build_dir(build_dir: Path) -> None:
    if build_dir.exists():
        print(f"Removing {build_dir}")
        shutil.rmtree(build_dir)


def main() -> int:
    arguments = parse_arguments()
    preset = _project.preset_name(arguments.config)
    build_dir = _project.build_dir(arguments.config)

    if arguments.clean:
        remove_build_dir(build_dir)
        return 0

    require_tool("cmake", "Install CMake 3.25+ from https://cmake.org/download/.")
    require_tool("ninja", "Install Ninja from https://ninja-build.org/ (or your package manager).")

    # The presets locate the toolchain file through $env{EMSDK}.
    environment = os.environ.copy()
    environment["EMSDK"] = find_emsdk(arguments.emsdk).as_posix()
    print(f"Using emsdk at {environment['EMSDK']}")

    if arguments.fresh:
        remove_build_dir(build_dir)

    # Once configured, Ninja re-runs CMake by itself whenever a CMake file changes,
    # so an explicit configure is only needed the first time or to change options.
    is_configured = (build_dir / "build.ninja").is_file()
    if not is_configured or arguments.definitions:
        definitions = [f"-D{definition}" for definition in arguments.definitions]
        run(["cmake", "--preset", preset, *definitions], environment)

    build_command = ["cmake", "--build", "--preset", preset]
    for target in arguments.target:
        build_command += ["--target", target]
    if arguments.jobs:
        build_command += ["--parallel", str(arguments.jobs)]
    if arguments.verbose:
        build_command.append("--verbose")
    run(build_command, environment)

    print(f"\nBuild succeeded. Site: {_project.dist_dir(arguments.config)}")
    print(f"Preview it with: python scripts/serve.py -c {arguments.config}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BuildError as error:
        print(f"\nerror: {error}", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(130)
