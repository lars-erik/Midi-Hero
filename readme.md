## Midi Hero

### Overview

A work in progress... 

Logs midi on messages and times them against the grid.  
Scores each note as:  

| Milliseconds | Score |
| --- | --- |
| < 10 | Perfect |
| < 20 | Great |
| < 40 | Good |
| < 80 | Off |
| >= 80 | Bad |

Shows statistics after play stops.

Works with MIDI controller input.  
Should work for non-midi instruments with effects like "Audio to MIDI".

### Demo

[![Midi Hero Demo](https://img.youtube.com/vi/pKvXAbV4N4w/0.jpg)](https://www.youtube.com/watch?v=pKvXAbV4N4w)

### How to install

Drop the .vst3 file in your common VST3 folder.

### How to use (Ableton)

- Add plugin to individual chain in instrument rack, or route midi to an instrument-less track.  
- Open the plugin editor.  
- Start the playhead.  
- Watch scored notes in realtime
- Stop the playhead
- View your statistics

The plugin-in automatically switches between "Hero" and "Statistics" as the playhead starts and stops.

### Roadmap

This is a hobby project and may never get another release.  
However, these are the planned features:

- GUI
  - Settings tab
    - Setting to disable auto-switch between "Hero" and "Statistics"
    - Settings for millisecond levels
    - Settings for score percentages
    - Setting for "slack" (i.e. scale the millisecond levels)
    - Division level setting
  - More stats to show
  - Even Prettier UI (flexbox, fonts, colors, general layout)
- Features
  - Using a midi file as "target" instead of the "grid"
