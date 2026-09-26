"""Project layout shared by the helper scripts.

Keep this in sync with CMakePresets.json: presets are named ``web-<config>`` and
build into ``build/<preset>``; the deployable site ends up in ``<build>/dist``.
"""

from __future__ import annotations

from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BUILD_ROOT = PROJECT_ROOT / "build"

CONFIGS = ("debug", "release")
DEFAULT_CONFIG = "debug"


def preset_name(config: str) -> str:
    return f"web-{config}"


def build_dir(config: str) -> Path:
    return BUILD_ROOT / preset_name(config)


def dist_dir(config: str) -> Path:
    return build_dir(config) / "dist"
