# Explanation Video
https://www.youtube.com/watch?v=4f6F0BoJN-c
# Introduction
This project is developed with Win32 API in C++. In many commercial games, such as CS:GO and Assassin’s Creed, when a location has been given, it does not render everything because the obstructed objects are unseen. Since rendering unseen objects are useless, and scanning the whole input maze is unrealistic when it is big, this app needs an efficient way to abruptly locate the visible objects to current location, based on binary spatial partitions (BSP) in a binary tree.  Such that, BSPWallSearching uses a BSP tree to manage the divided segments. A pre-built BSP tree escalates the process of detecting unblocked objects around the purple location. Meanwhile, this app demonstrates a comparison between BSP and Naïve for the sake of visualizing the efficiency of BSP. 

# Five Colorful Objects
Under BSP mode, when a black wall has turned to red, blue, brown, or green, this is the closest visible wall successfully blocking all the further walls behind in the north, south, west, or east direction respectively. Such that, the walls behind a red wall can never be seen when the purple stays still; the same rule applies to the other walls behind other colors. The app assigns a height value to a black wall at the beginning arbitrarily. On the other hand, under Naive mode, this app only cares about the closest walls regardless of the heights. The details regarding to the heights, expose during my interviews.  

# Long Loading for Building BSP Tree
A successful process of binary spatial partition is always time-consuming. Along with the location, which is purple or light purple depending on BSP or Naïve respectively, the first step has to identify the boundaries for every object. A black object is a wall meaning the purple cannot step on it or pass through it. However, the purple can see through when a black object is short in height.  A white object has zero in height, allowing a purple to step on it. The process of division accords to the identified boundaries; when an input maze is sparse, the number of boundaries is up to millions. Instead of scanning the whole maze every single time, a binary tree has some “pure nodes” and “impure nodes”.  If a segment has either only walls (unavailable to tread) or only available points for treading inside, this is a pure node; otherwise, this is an impure node. Such that, a pure node must be a child node; an impure node always has child nodes. Once the loading process has been done, the program completely relies on the tree without re-building any trees before the exit in BSP mode, in which the program prunes the unnecessary impure nodes away for the sake of efficient searching.

# Closest Walls versus Closest Visible Walls (Windows Version)
Because the height value of a black object is assigned arbitrarily during the loading process, some locations may have some closest visible walls that are not the closest walls in two-dimension terms (x-y plane). The algorithm always justifies the result in a trigonometric calculation at the end. For example, when there are some walls between the purple and the red, it means the walls in the middle are so short that the purple can see through them by simply tilting the viewing angle.    

# Efficiency of BSP
BSP is more efficient because it solely relies on the pre-built tree. When the wanted walls are still missing, the algorithm traverses from the current node to other nodes. Under BSP mode, it only checks the boundaries of the current node rather than scanning the current segment, which requires much more redundant iterations. 


# Linux Version Hotkeys
Note: Please runs this under full-screen.\
`Esc`  Exits;\
`Tab`  Switches Method;\
`Arrow keys`  Movements;

# Linux Version Running Guide
`git clone https://github.com/walterkakool/BSPWallSearching.git` \
`cd ./BSPWallSearching/Linux`\
`chmod u+x compiles demo`\
`./compiles`\
`./WalterKakool/bin/demo`

# BSPMaze.exe Guide (Ver. 1.00)
**DUE TO PHYSICAL SCREENS ARE DIFFERENT, MAKE SURE TO ADJUST THE FIVE COLORFUL BOXES PROPERLY AT FIRST. THAT'S WHY THE START IS ALWAYS (1,1). YOU MIGHT NEED TO ADJUST IT AGAIN IF YOU CHANGE THE PAGE.**\
• Download EXEPack.zip.\
• Copy-paste BSPMaze.exe and maze.txt into the same dir; then run it.\
• Press F2 to input maze.txt. Wait until the purple is present.\
• Once you've pressed F4, wait until the colorful boxes are present.\
• The purple one always starts at (1,1). If that doesn't align, use the hotkeys to align the five colorful boxes properly.\
• You might need to re-align the purple outside the first page.\
`F2`  Current dir has maze.txt ? prints the contents : nothing;\
`F4`  Initializes BSP tree; showing five colorful boxes means the process is done;\
`F5`  Refreshes;\
`F8`  Glancing/Normal mode switch;\
`Delete`  Holding for info;\
`TAB`  BSP/Naive switch;\
`↑↓←→`  Moves purple location\
`Alt + ↑↓←→`  Scrolls screen (only under glancing mode)\
`Alt + ,/.`  Adjusts the alignments for the purple location along x-axis.\
`ALT + -/+`  Adjusts the alignments for the purple location along y-axis.\
`,/.`  Adjusts the alignments for the four-walls location along x-axis.\
`-/+`  Adjusts the alignments for the four-walls location along y-axis.

# MazeRandomizer.exe Guide
Runs it then a new maze.txt will be in the same dir.

# How to modify maze.txt
• The height in the title line must be followed by a space. For example: `P1 1024 1024 `;\
• The width must be exactly 1024; the height must be at least 1024;\
• (1,1) which is on the bottom-left corner, must be 0;\
• The top, bottom, right, and left boundaries must be all 1's.\
• Every displaying sub-page of the maze must be big enough to fully fill out the whole window. If it doesn't, the sub-page will be abandoned.  
