# Linux version goals
In the given maze, the four colors (red, green, blue, brown) indicate the four closest walls\
for the current location (purple). "time cost" indicates the calculation time.\
When it's dark purple, the current Method is BSP; otherwise, it scans the whole maze every single time.  
THIS VERSION HAS BEEN DONE.

# How to run the Linux version
Note: Please runs this under full-screen.
\
`git clone https://github.com/walterkakool/BSPWallSearching.git` \
`cd ./BSPWallSearching/Linux`\
`chmod u+x compiles demo`\
`./compiles`\
`./WalterKakool/bin/demo`

# Linux version key specs
`Esc`-----------exits;\
`Tab`-----------switches Method;\
`Arrow keys`----movements;

# BSPMaze.exe goals
This is the Windows version. Besides everything in the Linux version, it has\
the functionality of maze scrolling.

# Guides for running BSPMaze.exe (Ver. 1.00)
• Download EXEPack.zip.\
• Copy-paste BSPMaze.exe and maze.txt into the same dir; then run it.\
• Once you've pressed F2, wait until the colorful boxes are present.\
• The purple one always starts at (1,1). If that doesn't align, use the hotkeys to align the five colorful boxes properly.\
`F2`  Current dir has maze.txt ? prints the contents : nothing;\
~~`F4`----------------------Update BSP (DEV debugging tool; will be removed soon);~~\
`F5`  Refreshes;\
`F8`  Glancing/Normal mode switch;\
`Delete`  Holding for info;\
`TAB`  BSP/Naive switch;\
`↑↓←→`  Moves purple location\
`Alt + ↑↓←→`  Scrolls screen (only under glancing mode)\
`Alt + ,/.`  Adjusts the alignments for the purple location along x-axis.\
`ALT + -/+`  Adjusts the alignments for the purple location along y-axis.\
`,/.`  Adjusts the alignments for the four-walls location along x-axis.\
`-/+`  Adjusts the alignments for the four-walls location along y-axis.\

# Guides for modifying maze.txt
• The height in the title line must be followed by a space. For example: `P1 1024 1024 `;\
• The width must be exactly 1024; the height must be at least 1024;\
• (1,1) which is on the bottom-left corner, must be 0;\
• The top, bottom, right, and left boundaries must be all 1's.
