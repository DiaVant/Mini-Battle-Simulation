# A* Pathfinding Mini Project

## Introduction

A mini project involving A* algorithms.  
Change the `.in` input files to create a custom simulation. The simulation will appear in the terminal using `cout`.

---

**DEV_KEY:**
1038 (use to skip preamble)

## Inupt files:

> Note: ';' in the files == '//' in coding

### dummyTypes.in:
- Folder for default types of dummies
- Used in dummies.in for convenience

### dummies.in:
- Folder for all dummy information (except position)
- Attributes:
  - Name (name)
  - Display name (display name)
  - Dummy Type (TYPE - dummy inherts preset stats from 'dummyTypes.in')
  - Starting HPX (hpx)
  - Max HPX (mhpx)
  - Defense (def)
  - Attack (atk)
 
### `terrainTypes.in`
- Folder for all different types of terrain information (position specified in 'mapInfo.in')
- Attributes:
  - Name
  - Display name
  - Value / cost to traverse
    
### `mapInfo.in`
> Note: first number denotes y axis, second number denotes x axis
1. Size of the 2D rectangular map  
   - Indexes will be in the range `[0, n)`
   - Defines map type
2. List of terrain types and their corresponding locations on the map
3. List of dummy names and their corresponding locations on the map

---
## Limitations of Code and Mechanics
- Side lengths of map must be smaller than 100
- Display name of all terrain and dummies at most can be 2 characters
- Terrain names & dummys names must be unique and consistently the same (they are used as keys)
- Although foes summoned to flank a player is evaluated by their distnace to the player, the actually destianation coordinates assigned to a foe is "random", thus, the target destiation of a foe in "flanking" mode may not be the most efficient or most logical distribution. 
- After a foe has been summoned to flank a player, it will not change paths / lock onto a different player, not even if they meet. Nor will the foe attack the player in their vicinity during flanking mode, although the player might attack the foe. 
- After a player/foe has found someone to attack, they will not stop attacking that person UNTIL they themselves are dead or their adversary is dead, not even if a dummy with even lower health arrives later
- Once a dummy is dead, its information is still stored in the map(s)
- The code isn't the 100% optimized
