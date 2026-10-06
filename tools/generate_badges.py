#!/usr/bin/env python3
"""Writes one SVG per streak badge into assets/badges/.

The app loads these files straight into a QPixmap, so no drawing code lives in
the C++ side. Re-run after changing a colour, a tier or an icon.
"""

from pathlib import Path

# Bronze is a circle, every rank above it is a hexagon.
RANKS = [
    ("bronze",  "#F2E3D3", "#A9683C", "#7A4A2A"),
    ("silver",  "#E7EBED", "#7F8D97", "#49565E"),
    ("gold",    "#F7E9C4", "#B8942F", "#73590F"),
    ("emerald", "#CFE8E1", "#007C68", "#064F42"),
    ("diamond", "#DDE4F6", "#5468A0", "#2E3B6B"),
]

TIERS = [3, 7, 15, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 365]

ICONS = {
    "water": [
        "M12 3.5s-5.8 6.2-5.8 10.1a5.8 5.8 0 0 0 11.6 0C17.8 9.7 12 3.5 12 3.5z",
    ],
    "meals": [
        "M12 9c-1.9-2.1-5.4-1.7-6.6 1.3-1.1 2.9.3 7 2.4 9.1 1 1 2 .9 2.6.4"
        ".6-.5 1.6-.5 2.2 0 .6.5 1.6.6 2.6-.4 2.1-2.1 3.5-6.2 2.4-9.1"
        "C18.4 7.3 14.9 6.9 13 9z",
        "M12 9V6.4",
        "M12.2 6.6c.9-2.1 3.1-2.3 3.1-2.3s.2 2.1-1.3 2.9c-.9.5-1.8-.6-1.8-.6z",
    ],
    "exercise": [
        "M3.5 10v4", "M20.5 10v4", "M7 7.5v9", "M17 7.5v9", "M7 12h10",
    ],
}

HEXAGON = "35,2 63,17 63,47 35,62 7,47 7,17"


def icon_group(habit, colour):
    """Scale and style one habit icon for insertion into a badge SVG."""
    # The icons are drawn on a 24x24 grid; 1.15 scales them to fill the shape.
    scale = 1.15
    offset = 35 - (24 * scale) / 2
    paths = "".join(
        f'<path d="{d}"/>' for d in ICONS[habit]
    )
    return (
        f'<g transform="translate({offset:.2f},{30 - (24 * scale) / 2:.2f}) '
        f'scale({scale})" fill="none" stroke="{colour}" stroke-width="1.6" '
        f'stroke-linecap="round" stroke-linejoin="round">{paths}</g>'
    )


def badge_svg(habit, days, rank):
    """Build the SVG markup for one habit's streak tier."""
    _, fill, border, ink = rank
    shape = (
        f'<circle cx="35" cy="32" r="28" fill="{fill}" '
        f'stroke="{border}" stroke-width="4"/>'
        if rank[0] == "bronze"
        else f'<polygon points="{HEXAGON}" fill="{fill}" '
             f'stroke="{border}" stroke-width="4" stroke-linejoin="round"/>'
    )
    # Three digits need a smaller size to stay inside the ribbon.
    font_size = 13 if days < 100 else 11
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 70 86" '
        'width="70" height="86">'
        f"{shape}"
        f"{icon_group(habit, ink)}"
        f'<rect x="6" y="58" width="58" height="23" rx="6" fill="{ink}"/>'
        f'<text x="35" y="74.5" text-anchor="middle" font-family="Arial" '
        f'font-size="{font_size}" font-weight="bold" fill="#FFFFFF">{days}</text>'
        "</svg>"
    )


def locked_svg():
    """Build the artwork shown when a habit has no unlocked tier."""
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 70 86" '
        'width="70" height="86">'
        '<circle cx="35" cy="32" r="28" fill="#F4F7F5" stroke="#D5E2DD" '
        'stroke-width="4" stroke-dasharray="5 4"/>'
        '<g transform="translate(23,20) scale(1)" fill="none" stroke="#A9B6B1" '
        'stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">'
        '<rect x="5" y="11" width="14" height="9" rx="2"/>'
        '<path d="M8 11V8a4 4 0 0 1 8 0v3"/>'
        "</g>"
        '<rect x="6" y="58" width="58" height="23" rx="6" fill="#D5E2DD"/>'
        '<text x="35" y="74.5" text-anchor="middle" font-family="Arial" '
        'font-size="11" font-weight="bold" fill="#8A9B95">LOCKED</text>'
        "</svg>"
    )


def main():
    """Generate the SVG file for every habit and configured streak tier."""
    out = Path(__file__).resolve().parent.parent / "assets" / "badges"
    out.mkdir(parents=True, exist_ok=True)

    written = 0
    for habit in ICONS:
        for index, days in enumerate(TIERS):
            rank = RANKS[index // 3]
            (out / f"{habit}-{days:03d}.svg").write_text(badge_svg(habit, days, rank))
            written += 1

    (out / "locked.svg").write_text(locked_svg())
    written += 1
    print(f"{written} badges written to {out}")


if __name__ == "__main__":
    main()
