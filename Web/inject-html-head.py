#!/usr/bin/env python3

import argparse
from pathlib import Path


APP_SCRIPT = "<script async src=Siv3DApp.js></script>"
TAGGED_APP_SCRIPT = (
    '<script defer id="siv3d-app-script" src=Siv3DApp.js></script>'
)
GENERIC_BOOTSTRAP_SELECTOR = 'document.querySelector("script[async]")'
APP_BOOTSTRAP_SELECTOR = 'document.getElementById("siv3d-app-script")'


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Inject an HTML fragment immediately after the opening <head> tag."
    )
    parser.add_argument("html", type=Path)
    parser.add_argument("template", type=Path)
    args = parser.parse_args()

    html = args.html.read_text(encoding="utf-8")
    template = args.template.read_text(encoding="utf-8").strip()

    if template not in html:
        head = "<head>"
        if head not in html:
            raise SystemExit(f"{args.html}: opening <head> tag was not found")
        html = html.replace(head, f"{head}\n{template}\n", 1)

    # The Siv3D template originally watches the first async script in the
    # document. GTM is also async and is injected into <head>, so that generic
    # selector creates a race and can leave the canvas hidden forever. Defer
    # also guarantees that the following bootstrap code registers its listener
    # before the application script executes, including on a warm cache.
    if APP_SCRIPT in html:
        html = html.replace(APP_SCRIPT, TAGGED_APP_SCRIPT, 1)
    elif TAGGED_APP_SCRIPT not in html:
        raise SystemExit(f"{args.html}: Siv3D application script was not found")

    if GENERIC_BOOTSTRAP_SELECTOR in html:
        html = html.replace(
            GENERIC_BOOTSTRAP_SELECTOR, APP_BOOTSTRAP_SELECTOR, 1
        )
    elif APP_BOOTSTRAP_SELECTOR not in html:
        raise SystemExit(f"{args.html}: Siv3D bootstrap selector was not found")

    args.html.write_text(html, encoding="utf-8")


if __name__ == "__main__":
    main()
