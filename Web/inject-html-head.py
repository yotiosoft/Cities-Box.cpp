#!/usr/bin/env python3

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Inject an HTML fragment immediately after the opening <head> tag."
    )
    parser.add_argument("html", type=Path)
    parser.add_argument("template", type=Path)
    args = parser.parse_args()

    html = args.html.read_text(encoding="utf-8")
    template = args.template.read_text(encoding="utf-8").strip()

    if template in html:
        return

    head = "<head>"
    if head not in html:
        raise SystemExit(f"{args.html}: opening <head> tag was not found")

    html = html.replace(head, f"{head}\n{template}\n", 1)
    args.html.write_text(html, encoding="utf-8")


if __name__ == "__main__":
    main()
