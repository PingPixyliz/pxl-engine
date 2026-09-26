#!/usr/bin/env python3
"""Serve a built site locally for development.

WebAssembly and WebGPU do not work from file:// URLs; the page has to come from
a web server. http://localhost counts as a secure context, so no TLS is needed.

Examples:
    python scripts/serve.py                  # serves build/web-debug/dist
    python scripts/serve.py -c release --open
    python scripts/serve.py --port 9000

Development only: there is no caching, compression or hardening. For production,
upload the dist directory to any static file host.
"""

from __future__ import annotations

import argparse
import sys
import webbrowser
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

import _project


class DevelopmentRequestHandler(SimpleHTTPRequestHandler):
    """Static file handler tuned for iterating on WebAssembly builds."""

    # Browsers only stream-compile .wasm served with the right MIME type, and
    # not every platform's MIME database knows about it.
    extensions_map = {
        **SimpleHTTPRequestHandler.extensions_map,
        ".wasm": "application/wasm",
        ".js": "text/javascript",
    }

    cross_origin_isolated = False

    def end_headers(self) -> None:
        # Always serve the latest build instead of a cached one.
        self.send_header("Cache-Control", "no-store")
        if self.cross_origin_isolated:
            # Required for SharedArrayBuffer, i.e. for Emscripten pthreads builds.
            self.send_header("Cross-Origin-Opener-Policy", "same-origin")
            self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "-c", "--config", choices=_project.CONFIGS, default=_project.DEFAULT_CONFIG,
        help="which build to serve (default: %(default)s)",
    )
    parser.add_argument(
        "-d", "--directory", type=Path,
        help="serve this directory instead of the build's dist directory",
    )
    parser.add_argument("-p", "--port", type=int, default=8080, help="default: %(default)s")
    parser.add_argument(
        "--bind", default="127.0.0.1", metavar="ADDRESS",
        help="address to listen on; use 0.0.0.0 to test from other devices (default: %(default)s)",
    )
    parser.add_argument("--open", action="store_true", help="open the site in the default browser")
    parser.add_argument(
        "--cross-origin-isolation", action="store_true",
        help="send COOP/COEP headers (needed once you enable threads)",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()

    directory = arguments.directory or _project.dist_dir(arguments.config)
    if not (directory / "index.html").is_file():
        print(f"error: nothing to serve in {directory}\n"
              f"Build first: python scripts/build.py -c {arguments.config}", file=sys.stderr)
        return 1

    DevelopmentRequestHandler.cross_origin_isolated = arguments.cross_origin_isolation
    handler = partial(DevelopmentRequestHandler, directory=str(directory))

    with ThreadingHTTPServer((arguments.bind, arguments.port), handler) as server:
        host = "localhost" if arguments.bind in ("127.0.0.1", "0.0.0.0") else arguments.bind
        url = f"http://{host}:{arguments.port}/"
        print(f"Serving {directory}\n     at {url}   (Ctrl+C to stop)", flush=True)
        if arguments.open:
            webbrowser.open(url)
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nStopped.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
