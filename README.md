INTRODUCTION:
	A mini project I worked on involving A* algos. 
	Change the ".in" input files to create a custom simulation, the simulation will appear in terminal (using "cout")

GUIDE:
DEV_KEY = 1038

".in" files:
	(';' in the files == '//' in coding)
	dummyTypes.in:
		-folder for default types of dummies, used in dummies.in for convenience
	dummies.in:
		-folder for all dummy information (except position)
			-eg: name, display name, hpx, max hpx, def, atk
	terrainTypes.in:
		-folder for all different types of terrain information (except position)
			-eg: name, display name, value/cost
	mapInfo.in:
		1. size of the 2D rectangular map (indexes will be [0, n) ) and map type
		2. list of what terrain type and @ what location on map
		3. list of dummy names and @ what location on map
		

Limitations of Code/Mechanics:
-Side lengths of map must be smaller than 100
-Display name of all terrain and dummies at most can be 2 characters
-Terrain names & dummys names must be unique and consistently the same (they are used as keys)
-Although foes summoned to flank a player is evaluated by their distnace to the player, the actually destianation coordinates assigned to a foe is "random", thus, the target destiation of a foe in "flanking" mode may not be the most efficient or most logical distribution. 
-After a foe has been summoned to flank a player, it will not change paths / lock onto a different player, not even if they meet. Nor will the foe attack the player in their vicinity during flanking mode, although the player might attack the foe. 
-After a player/foe has found someone to attack, they will not stop attacking that person UNTIL they themselves are dead or their adversary is dead, not even if a dummy with even lower health arrives later
-Once a dummy is dead, its information is still stored in the map(s)
-The code isn't the 100% optimized
