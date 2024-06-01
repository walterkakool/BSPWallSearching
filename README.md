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

# Implemented functionality of BSPMaze.exe (Ver 0.9)
Copy-paste BSPMaze.exe and maze.txt into the same dir; then run it.\
`F2`----------------------Current dir has maze.pnm ? prints the contents : "No input file.";\
`F4`----------------------Update BSP (DEV debugging tool; will be removed soon);\
`F5`----------------------Refresh;\
`F8`----------------------Glancing/Normal mode switch;\
`Delete`-----------------Holding for info;\
`TAB`---------------------BSP/Naive switch;\
`↑↓→←`--------------------Move purple location\
`Alt+↑↓→←`---------------Scrolls screen (only under glancing mode)
