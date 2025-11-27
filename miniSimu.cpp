

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

const string DEV_KEY = "1038";
const int INT_MAXI = 2147483647;


//defining structs;
struct Coord {
	int _xCoord;
	int _yCoord;
};
struct Terrain {
	string _name;
	string _displayName;
	int _value;
};
struct Cell{
	Coord _coord;
	Terrain _terrainInfo;
	
	int _horiVal;
	int _miniDis;
	Coord _fatherCoord;
};
struct Dummy {
	Coord _coord;
	
	//Players: 1 == still, 2 == attacking, 0 == dead;
	//Foe: 1 == moving left constantly, 2 == attacking, 3 == falnking, 4 == idle (waiting to either flank, attack, or become marching if their target it dead), 0 == dead;
	int _status;
	
	string _name;
	string _displayName;
	
	//max hpx
	int _mhp;
	int _hpx;
	int _def;
	int _atk;
	
	//only used for foe when path finding 
	vector<Coord> _pathList;
	//only used for foe when path findig
	Coord _targetCoord;
	//only used when player/foe is in attacking mode
	string _attackingName;
};


//public vars;
vector<vector<Cell> > _mainMap;
int _mapColumnNum = 0;
int _mapRowNum = 0;
//the general type of terrain the map is (special terrain can be added)
string _mapType = "";
//unordered map since sorting in normal map is not needed (and accessing unordered is o(1)
unordered_map<string, Terrain> _terrainMap;
unordered_map<string, Dummy> _defaultDummMap;

unordered_map<string, Dummy> _playerMap;
unordered_map<string, Dummy> _foeMap;


//input: two coords
//output: true->coords are same; false->coord are different
bool EqualCoord (Coord one, Coord two) {
	return one._xCoord == two._xCoord && one._yCoord == two._yCoord;
}


//display functions;
void DisplayStrList (vector<string> theList) {
	for (const auto& curStr : theList) {
		cout << curStr << ",";
	}
	cout << endl;
}
void DisplayCoord (Coord theCoord) {
	cout << "(" << theCoord._xCoord << "," << theCoord._yCoord << ")";
}
void DisplayCoordList (vector<Coord> theList) {
	for (int i = 0; i < theList.size(); i++) {
		DisplayCoord(theList[i]); 
		cout << ",";
	}
	cout << endl;
}
void DisplayTerrain (Terrain theTerrain) {
	cout << theTerrain._name << "," ;
	cout << theTerrain._displayName << "$";
	cout << theTerrain._value;
}
void DisplayTerrainMap () {
	for (unordered_map<string, Terrain>::iterator it = _terrainMap.begin(); it != _terrainMap.end(); it++) {
		DisplayTerrain(it->second);
		cout << endl;
	}
	cout << endl;
}
void DisplayCell (Cell theCell) {
	DisplayTerrain(theCell._terrainInfo); 
	cout << ",hori" << theCell._horiVal;
	cout << ",miniDis" << theCell._miniDis;
	cout << "@";
	DisplayCoord(theCell._coord);
	cout << ",father:";
	DisplayCoord(theCell._fatherCoord);
	cout << endl;
}
void DisplayCellList (vector<Cell> theList) {
	for (int i = 0; i < theList.size(); i++) {
		DisplayCell(theList[i]);
	}
	cout << endl;
}
void DisplayCellListList (vector<vector<Cell> > theMap) {
	for (int i = 0; i < theMap.size(); i++) {
		DisplayCellList(theMap[i]);
		cout << endl; 
	}
	cout << endl;
}
void DisplayDummy (Dummy theDummy) {
	cout << theDummy._name << "," << theDummy._displayName;
	cout << "{" << theDummy._status << "}";
	cout << "hp(" << theDummy._hpx << "/" << theDummy._mhp << "),";
	cout << "atk" << theDummy._atk << ",";
	cout << "def" << theDummy._def;
	cout << "@";
	DisplayCoord(theDummy._coord); 
	
	//display _pathList
	cout << "[";
	for (int i = 0; i < theDummy._pathList.size(); i++) {
		DisplayCoord(theDummy._pathList[i]);
		//visual purposes
		if (i < theDummy._pathList.size() - 1) {
			cout << "->";
		} 
	}
	
	cout << "]" << "->";
	DisplayCoord(theDummy._targetCoord);
	cout << "@" << theDummy._attackingName << endl;
}
void DisplayDummyMap (unordered_map<string, Dummy> theMap) {
	for (unordered_map<string, Dummy>::iterator it = theMap.begin(); it != theMap.end(); it++) {
		DisplayDummy(it->second);
	}
	cout << endl;
}



//MAIN / VISUAL Displays

//(used in MainMapDisplay) 
//input: a cell to be checked
//output: checks if cell is occupied by player/foe, then returns string value that should be displayed
string CheckOccupied (Cell theCell) {
	for (const auto& mapIt : _playerMap) {
		//if it is dead, no need to display
		if (mapIt.second._status == 0) {
			continue;
		}
		
		if (EqualCoord(theCell._coord, mapIt.second._coord)) {
			return mapIt.second._displayName;
		}
	}
	for (const auto& mapIt : _foeMap) {
		//if it is dead, no need to display
		if (mapIt.second._status == 0) {
			continue;
		}
		
		if (EqualCoord(theCell._coord, mapIt.second._coord)) {
			return mapIt.second._displayName;
		}
	}
	
	return theCell._terrainInfo._displayName;
}
//(used in Display())
void MainMapDisplay () {
	Cell curCell;
	vector<Cell> curList;
	
	char spacer = ' ';
	string bigSpace = "  ";
	
	string curDisplayStr = "";
	
	
	//display first line of numbers;
	cout << bigSpace << spacer;
	for (int i = 0; i < _mapColumnNum; i++) {
		//adds extra zero for one digit numbers
		if (i / 10 < 1) {
			cout << 0;
		}
		
		cout << i << spacer;
	}
	cout << endl;
	
	
	for (int i = 0; i < _mainMap.size(); i++) {
		curList = _mainMap[i];
		
		//adds extra zero for one digit numbers
		if (i / 10 < 1) {
			cout << 0;
		}
		
		cout << i << spacer;
		
		
		for (int m = 0; m < curList.size(); m++) {
			curCell = curList[m];
			curDisplayStr = CheckOccupied(curCell); 
			
			//for one character display  names, adds a space
			if (curDisplayStr.size() == 1) {
				cout << spacer;
			} 			
			
			cout << curDisplayStr << spacer;
		}
		
		cout << endl;
	}	
}

//(used in ConvertMapToSortedList())
//sorts vector of dummy according to ._name from smallest->biggest
bool CompDummyUp (Dummy one, Dummy two) {
	return one._name < two._name;
}
//(used in MainFoeMapDisplay(), MainPlayerMapDisplay())
//intput: a map of dummies
//outpout: a sorted vector of dummmies according to _name from smallest->biggest
vector<Dummy> ConvertMapToSortedList (unordered_map<string, Dummy> theMap) {
	vector<Dummy> resList;
	
	for (const auto& curDummy : theMap) {
		resList.push_back(curDummy.second);
	}
	
	sort(resList.begin(), resList.end(), CompDummyUp);
	return resList;
}

//(used in Display())
void MainFoeMapDisplay () {
	vector<Dummy> sortedDummy = ConvertMapToSortedList(_foeMap); 
	
	cout << "FOE(s):" << endl;
	
	for (const auto& curDummy : sortedDummy) {
		
		cout << "[" << curDummy._displayName << "]";
		cout << curDummy._name;
		
		if (curDummy._status == 0) {
			cout << ":  DEAD" << endl;
			continue;
		}
		
		
		cout << " {";
		cout << "hp" << "(" << curDummy._hpx << "/" << curDummy._mhp << "),";
		cout << "atk" << curDummy._atk << ",";
		cout << "def" << curDummy._def;
		cout << "}:  ";
		

		switch (curDummy._status) {
			case 1:
				cout << "MRCH ";
				break;
			case 2:
				cout << "ATCK->" << curDummy._attackingName << " ";
				break;
			case 3:
				cout << "FLNK->" << curDummy._attackingName << " ";
				cout << " [headed to ";
				DisplayCoord(curDummy._targetCoord); 
				cout << "] ";
				break;
			case 4:
				cout << "WAIT->" << curDummy._attackingName << " ";
				break;
		}
		
		
		cout << endl;
	}
}
//(used in Display())
void MainPlayerMapDisplay () {
	vector<Dummy> sortedDummy = ConvertMapToSortedList(_playerMap); 

	cout << "PLAYER(s):" << endl;
	
	for (const auto& curDummy : sortedDummy) {
		
		cout << "[" << curDummy._displayName << "]";
		cout << curDummy._name;
		
		if (curDummy._status == 0) {
			cout << ":  DEAD" << endl;
			continue;
		}
		
		
		cout << " {";
		cout << "hp" << "(" << curDummy._hpx << "/" << curDummy._mhp << "),";
		cout << "atk" << curDummy._atk << ",";
		cout << "def" << curDummy._def;
		cout << "}:  ";
		

		switch (curDummy._status) {
			case 1:
				cout << "IDLE ";
				break;
			case 2:
				cout << "ATCK->" << curDummy._attackingName << " ";
				break;
		}
		
		cout << endl;
	}
}

void Display() {
	MainPlayerMapDisplay();
	cout << endl;
	MainFoeMapDisplay();
	cout << endl;
	MainMapDisplay();
	cout << endl << endl;
}



//uses: _mapColumnNum & _mapRowNum & _mapType to create a map
//generates _mainMap of _mapType (used in ParseMainMap() before adding special terrain) 
void GenMainMap () {
	Terrain mainTerrain;
	Coord curCoord;
	Cell curCell;
	vector<Cell> curList;
	
	mainTerrain = _terrainMap.find(_mapType)->second;
	for (int i = 0; i < _mapRowNum; i++) {
		for (int m = 0; m < _mapColumnNum; m++) {
			curCoord._xCoord = i;
			curCoord._yCoord = m;
			curCell._coord = curCoord;
			curCell._terrainInfo = mainTerrain;
			
			curList.push_back(curCell);
		}
		_mainMap.push_back(curList);
		curList.clear();
	}
}

//input: coord where theTerrain information should be inserted
//updates MainMap @theCoord location w/ theTerrain info
void UpdateMainMap (Coord theCoord, Terrain theTerrain) {
	_mainMap[theCoord._xCoord][theCoord._yCoord]._terrainInfo = theTerrain;	
}



//; == commented out
void ParseTerrainTypes () {
	Terrain curTerrain;
	ifstream inFile("terrainTypes.in");
	
	while (inFile >> curTerrain._name) {
		
		//commented out? then skip line
		if (curTerrain._name[0] == ';') {
			getline(inFile, curTerrain._name);
			continue;
		}
		
		
		inFile >> curTerrain._value >> curTerrain._displayName;
		_terrainMap.insert(pair<string, Terrain>(curTerrain._name, curTerrain));
	}
	
	inFile.close();
}
void ParseDummyTypes () {
	Dummy curDummy;
	string curStr;
	int curInt = 0;
	
	string typeName = "";
	vector<int> statsList;
	
	ifstream inFile("dummyTypes.in");
	
	
	while (inFile >> curStr) {
		
		//commented out, then skip
		if (curStr[0] == ';') {
			for (int i = 0; i < 5; i++) {
				getline(inFile, curStr);
			}
			continue;
		}
		
		//inFile typeName, aka key to map _defaultDummyMap
		inFile >> typeName;
		curDummy._name = typeName;
		
		//inFile
		for (int i = 0; i < 4; i++) {
			inFile >> curStr >> curInt;
			statsList.push_back(curInt);
		}
		
		curDummy._mhp = statsList[0];
		curDummy._hpx = statsList[1];
		curDummy._atk = statsList[2];
		curDummy._def = statsList[3];
		
		_defaultDummMap.insert(pair<string, Dummy>(typeName, curDummy));	
	}
	
	
	inFile.close();
}
void ParseDummiesStats () {
	
	Dummy curDummy;
	string curStr = "";
	int curInt = 0;

	vector<int> dummyinfo;
	
	int cc = 0;
	
	
	//counting how many playesr are there first time
	ifstream firstTime("dummies.in");
	
	getline(firstTime, curStr);

	while (getline(firstTime, curStr)) {
		//if reached foe, then break;
		if (curStr == "Foe") {
			break;
		}
		cc++;

		//deleting extra lines
		getline(firstTime, curStr);

		firstTime >> curStr;
		
		//if using default dummy types, then little clean up
		if (curStr == "TYPE") {
			getline(firstTime, curStr);
			continue;
		}
		
		//extra clean up 4 lines
		for (int i = 0; i < 4; i++) {
			getline(firstTime, curStr);
		}
	}
	firstTime.close();
	

	//second time inFile for actual data
	ifstream secondTime("dummies.in");

	//store playerData
	//trash "Players" before input Data
	secondTime >> curStr;
	
	for (int i = 0; i < cc; i++) {
		//infile names
		secondTime >> curStr;
		
		//commented out = ';'
		if (curStr[0] == ';') {

			//deleting extra lines
			getline(secondTime, curStr);
			getline(secondTime, curStr);
			
			secondTime >> curStr;
			
			//if using default dummy types, then little clean up
			if (curStr == "TYPE") {
				getline(secondTime, curStr);
				continue;
			}
			
			//extra clean up 4 lines
			for (int i = 0; i < 4; i++) {
				getline(secondTime, curStr);
			}

			continue;
		}
		
		secondTime >> curStr;
		curDummy._name = curStr;		
		secondTime >> curStr >> curStr;
		curDummy._displayName = curStr;

		//inFile stats
		for (int m = 0; m < 4; m++) {
			secondTime >> curStr;
			
			//if usses default dummy types, then construct dummyINfo and break;
			if (curStr == "TYPE") {
				secondTime >> curStr;
				
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._mhp);
				
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._hpx);
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._atk);
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._def);
				break;
			}
			
			secondTime >> curInt;
			dummyinfo.push_back(curInt);
		}
		
		
		//insert stats into curDummy
		curDummy._mhp = dummyinfo[0];
		curDummy._hpx = dummyinfo[1];
		curDummy._atk = dummyinfo[2];
		curDummy._def = dummyinfo[3];
		curDummy._status = 1;
		//insert curDummy into playerMap
		_playerMap.insert(pair<string, Dummy>(curDummy._name, curDummy));
		dummyinfo.clear();
	}



	//store playerData
	//trash "Players" before input Data

	secondTime >> curStr;
	
	while(secondTime >> curStr) {
		//infile names
		
		//commented out = ';'
		if (curStr[0] == ';') {

			//deleting extra lines
			getline(secondTime, curStr);
			getline(secondTime, curStr);
			
			secondTime >> curStr;
			
			//if using default dummy types, then little clean up
			if (curStr == "TYPE") {
				getline(secondTime, curStr);
				continue;
			}
			
			//extra clean up 4 lines
			for (int i = 0; i < 4; i++) {
				getline(secondTime, curStr);
			}
			
			continue;
		}
		
		secondTime >> curStr;
		curDummy._name = curStr;		
		secondTime >> curStr >> curStr;
		curDummy._displayName = curStr;

		//inFile stats
		for (int m = 0; m < 4; m++) {
			secondTime >> curStr;
			
			
			//if usses default dummy types, then construct dummyINfo and break;
			if (curStr == "TYPE") {
				secondTime >> curStr;
				
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._mhp);
				
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._hpx);
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._atk);
				dummyinfo.push_back(_defaultDummMap.find(curStr)->second._def);
				break;
			}
			
			secondTime >> curInt;
			dummyinfo.push_back(curInt);
		}
		
		
		//insert stats into curDummy
		curDummy._mhp = dummyinfo[0];
		curDummy._hpx = dummyinfo[1];
		curDummy._atk = dummyinfo[2];
		curDummy._def = dummyinfo[3];
		curDummy._status = 1;
		//insert curDummy into playerMap
		_foeMap.insert(pair<string, Dummy>(curDummy._name, curDummy));
		dummyinfo.clear();
	}
	
	secondTime.close();
}
void ParseMainMap () {
	Cell curCell;
	Coord curCoord;
	int cc = 0;
	string curStr = "";
	unordered_map<string, Terrain>::iterator terrainIt;
	unordered_map<string, Dummy>::iterator dummyIt;
		
	ifstream inFile("mapInfo.in");
	
	//gen base map
	inFile >> _mapRowNum >> _mapColumnNum >> _mapType;
	GenMainMap();
	
	//inFile terrain coords until player/foe coords
	while (inFile >> curStr) {
		
		//commented out == ';'
		if (curStr[0] == ';') {
			getline(inFile, curStr);
			continue;
		}
		
		inFile >> curCoord._xCoord >> curCoord._yCoord;
		terrainIt = _terrainMap.find(curStr);
		
		//reached the player/foe coord inputs
		if (terrainIt == _terrainMap.end()) {
			break;
		}
		
		UpdateMainMap(curCoord, terrainIt->second);
	}
	
	
	//inFile player/foe coords
	//adding the already read dummy input
	if (_playerMap.find(curStr) != _playerMap.end()) {
		_playerMap.find(curStr)->second._coord = curCoord;
	}
	else {
		_foeMap.find(curStr)->second._coord = curCoord;
	}
	

	while (inFile >> curStr) {
		
		//commented out == ';'
		if (curStr[0] == ';') {
			getline(inFile, curStr);
			continue;
		}
		
		inFile >> curCoord._xCoord >> curCoord._yCoord;
		
		//determinig which list to modify
		if (_playerMap.find(curStr) != _playerMap.end()) {
			_playerMap.find(curStr)->second._coord = curCoord;
			continue;
		}
		_foeMap.find(curStr)->second._coord = curCoord;
	}
	

	
	inFile.close();
}
void ParseIn () {
	ParseTerrainTypes();
	ParseDummyTypes();
	ParseDummiesStats();
	ParseMainMap();
}


//input: two coords
//output: the horisitc value of two coords
int CalcHori (Coord one, Coord two) {
	int res = 0;
	res = max(one._xCoord, two._xCoord) - min(one._xCoord, two._xCoord);
	res += max(one._yCoord, two._yCoord) - min(one._yCoord, two._yCoord);
	return res;
}

//input: vector of maps of dummies which will be searched in to find if any are on theTarget Coord
//output: true->one or more dummies are at theTarget location; false->none are
bool CheckDummyListMapInLocation (vector< unordered_map<string, Dummy> > theList, Coord theTarget) {
	
	for (int i = 0; i < theList.size(); i++) {
		for (const auto& mapIt : theList[i]) {
			
			//if dummy is dead then skip
			if (mapIt.second._status == 0) {
				continue;
			}
			
			
			if (EqualCoord(mapIt.second._coord, theTarget) ) {
				return true;
			}


		}
	}
	
	return false;
}

//input: a coord
//output: a vector of coords of the coords surrounding theCoord (up, down, left, right), does not matter if they are occupied or not
vector<Coord> GenNeighborCoordList (Coord theCoord) {
	vector<Coord> resList;
	Coord curCoord;
	
	
	curCoord._yCoord = theCoord._yCoord;
	//up
	if (theCoord._xCoord > 0) {
		curCoord._xCoord = theCoord._xCoord - 1;
		resList.push_back(curCoord);
	}
	//down
	if (theCoord._xCoord < _mapRowNum - 1) {
		curCoord._xCoord = theCoord._xCoord + 1;
		resList.push_back(curCoord);
	}
	
	
	curCoord._xCoord = theCoord._xCoord;
	//left
	if (theCoord._yCoord > 0) {
		curCoord._yCoord = theCoord._yCoord - 1;
		resList.push_back(curCoord);
	}
	//right
	if (theCoord._yCoord < _mapColumnNum - 1) {
		curCoord._yCoord = theCoord._yCoord + 1;
		resList.push_back(curCoord);
	}
	
	
	return resList;
}
//input: a coord and vector of maps
//output: a vector of coords of the coords surrounding theCoord (up, down, left, right) whicih are NOT occupied by any dummies in the maps
vector<Coord> GenNeighborEmptyCoordList (vector< unordered_map<string, Dummy> > theList, Coord theCoord) {
	vector<Coord> allCoordList = GenNeighborCoordList(theCoord);
	Coord curCoord;
	
	vector<Coord> resList;
	
	for (int i = 0; i < allCoordList.size(); i++) {
		curCoord = allCoordList[i];
		
		if (!CheckDummyListMapInLocation(theList, curCoord) ) {
			resList.push_back(curCoord);
		}
	} 
	
	return resList;
}

//input: a coord and vector of maps
//output: a vector of coords of the coords surrounding theCoord (two at horizontal/veritcal, one at diagonal), does not care if occupied or not
vector<Coord> GenTwoTwoSurroundingCoordList (Coord theCoord) {
	
	Coord curCoord = theCoord;
	
	vector<Coord> resList;
	
	//in case theCoord actualy is invalid
	if (theCoord._xCoord < 0 || theCoord._xCoord > _mapRowNum - 1) {
		return resList;
	}
	if (theCoord._yCoord < 0 || theCoord._yCoord > _mapColumnNum - 1) {
		return resList;
	}
	
	
	//generates coords surrounding player all by one
	//up and down
	for (int i = -1; i < 2; i++) {
		curCoord._xCoord = theCoord._xCoord + i;
		
		//if too high, or too low, then skip
		if (curCoord._xCoord < 0) {
			continue;
		}
		if (curCoord._xCoord > _mapRowNum - 1) {
			continue;
		}
	
	
	
		//left and right
		for (int m = -1; m < 2; m++) {
			curCoord._yCoord = theCoord._yCoord + m;
			
			//if is the same as theCoord, not surronding, so skip
			if (EqualCoord(curCoord, theCoord)) {
				continue;
			}
			
			//if it is too left or right, then skip
			if (curCoord._yCoord < 0) {
				continue;
			}
			if (curCoord._yCoord > _mapColumnNum - 1) {
				continue;
			}
				
			resList.push_back(curCoord);
		}
		
	}
	
	//generates up, down, left, right of length two
	
	//2up
	curCoord = theCoord;
	curCoord._xCoord -= 2;
	if (curCoord._xCoord > 0) {
		resList.push_back(curCoord);
	}
	
	//2down
	curCoord = theCoord;
	curCoord._xCoord += 2;
	if (curCoord._xCoord < _mapRowNum - 1) {
		resList.push_back(curCoord);
	}
	
	
	//2left
	curCoord = theCoord;
	curCoord._yCoord -= 2;
	if (curCoord._yCoord > 0) {
		resList.push_back(curCoord);
	}
	
	//2right
	curCoord = theCoord;
	curCoord._yCoord += 2;
	if (curCoord._yCoord < _mapColumnNum - 1) {
		resList.push_back(curCoord);
	}
	
	return resList;
}
//input: a coord and vector of maps
//output: a vector of coords of the coords surrounding theCoord (2x2) whicih are NOT occupied by any dummies in the maps
vector<Coord> GenTwoTwoSurroundingEmptyCoordList (vector< unordered_map<string, Dummy> > theList, Coord theCoord) {
	vector<Coord> allCoordList = GenTwoTwoSurroundingCoordList(theCoord);
	Coord curCoord;
	
	vector<Coord> resList;
	
	for (int i = 0; i < allCoordList.size(); i++) {
		curCoord = allCoordList[i];
		
		if (!CheckDummyListMapInLocation(theList, curCoord) ) {
			resList.push_back(curCoord);
		}
	} 
	
	return resList;
}

//(used in FindAttackingName())
//input: vector of maps of dummies to check if they are besides theCoord (up down left right)
//output: true->one or more things in vector of maps are beside theCoord; false->none
bool CheckSurrounding (vector<unordered_map<string, Dummy> > theList, Coord theCoord) {
	Coord curCheckingCoord;
	
	//list of neighboring coords, up, down, left, right
	vector<Coord> coordList;
	coordList = GenNeighborCoordList(theCoord);
	
	
	for (int i = 0; i < coordList.size(); i++) {
		curCheckingCoord = coordList[i];
		
		if (CheckDummyListMapInLocation(theList, theCoord) ) {
			return true;
		}
	}
	
	return false;
}

//(used in GenPathFlankingFoe())
//intpu: Coord and vector of coords
//output: true->coord is in vector of coords, false->coord is not in vector of coords
bool CheckCoordInList (Coord theCoord, vector<Coord> theList) {
	for (int i = 0; i < theList.size(); i++) {
		if (EqualCoord(theCoord, theList[i]) ) {
			return true;
		}
	}
	return false;
}

//(used in CheckAllIfDead() )
//input: a coord
//output: whether it is inside of the bounds of the map ( ._xCoord = [0, _mapRowNum) && ._yCoord =  [0, _mapColumnNum) )
//        true -> in bounary; false -> not in bounds
bool CheckInCoordInBounds (Coord theCoord) {
	
	if (theCoord._xCoord < 0) {
		return false;
	}
	if (theCoord._xCoord > _mapRowNum - 1) {
		return false;
	}
	
	if (theCoord._yCoord < 0) {
		return false;
	}
	if (theCoord._yCoord > _mapColumnNum - 1) {
		return false;
	}
	
	return true;
}

//(used in GenFlankingFoeList(), CheckIdleIfFlanking())
//input: list of coords
//output: list of coords whihc are not already _targetcoords of any flanking foes
vector<Coord> RemoveFlankingTargetCoordList (vector<Coord> oriList) {
	Coord curCoord;
	vector<Coord> resList;
	
	bool notInMap = true;
	
	//goes through list of coords
	for (int i = 0; i < oriList.size(); i++) {
		curCoord = oriList[i];
		
		notInMap = true;
		
		//goes through foe map to find foes
		for (const auto& curFoe : _foeMap) {
			
			//if not flanking, then skip
			if (curFoe.second._status != 3) {
				continue;
			}
			
			//if targetcoord are the same, then in the list and break
			if (EqualCoord(curFoe.second._targetCoord, curCoord) ) {
				notInMap = false;
				break;
			} 
		}
		
		
		if (notInMap) {
			resList.push_back(curCoord);
		}	
		
	}
	
	
	return resList;
}




/////////ATTACKING

//(used in FindNewAttackingPlayer())
//input: coord and vector of maps of Dummies the coords could possibly attack
//output: name theCoord should be attacking, "" if should not be attacking aka there are no Dummies close to theCoord
string FindAttackingName (Coord theCoord, vector< unordered_map<string, Dummy> > theList) {
	
	//if no foes are surrounding, there is no reason to check the coordinates surrounding the Coord
	if (CheckSurrounding(theList, theCoord) ) {
		return "";
	}
	
	
	Coord curCheckingCoord;
	
	vector<Coord> coordList;
	coordList = GenNeighborCoordList(theCoord);
	
	int miniHealth = INT_MAX;
	string miniStr = "";
	
	
	//goes through each position
	for (int i = 0; i < coordList.size(); i++) {
		curCheckingCoord = coordList[i];
		
		//goes through the list of maps
		for (const auto& curMap : theList) {
			for (const auto& curDummy : curMap) {
				
				//if curDummy is dead, skip
				if (curDummy.second._status == 0) {
					continue;
				}
				
				//checks if curDummy is on curCoord
				if (EqualCoord(curCheckingCoord, curDummy.second._coord)) {
					
					//find lowest Dummy health & updates name to attack
					if (curDummy.second._hpx < miniHealth) {
						miniHealth = curDummy.second._hpx;
						miniStr = curDummy.second._name;
					}
				}
			}
		}
	}

	return miniStr;
}

//(used in CheckAllDummy())
//finds updates all players in _playerMap who are in marching but should be in attack mode
//a player should be in attack mode if foes are in any surroudning cells
//Changes: _status 1->2 & _attacking name
void FindNewAttackingPlayer () {
	Coord curCoord;
	string curStr = "";
	
	//creates list of foes to put into functions
	vector<unordered_map<string, Dummy> > targetList;
	targetList.push_back(_foeMap);
	
	
	for (const auto& curPlayer : _playerMap) {
		curCoord = curPlayer.second._coord;
		
		//if not marching, then skip
		if (curPlayer.second._status != 1) {
			continue;
		}
		
		//checks if should be attacking, then change status & attacking name;
		curStr = FindAttackingName(curCoord, targetList);
		
		if (curStr != "") {
			_playerMap.find(curPlayer.first)->second._status = 2;
			_playerMap.find(curPlayer.first)->second._attackingName = curStr;
		}
	}
}

//(Used in CheckAllDummy())
//finds updates all foes in _foeMap who are in marching/idle but should be in attack mode
//a foe should be in attack mode if players are in any surroudning cells
//._status ?->2 & _attacking name 
//output: a vector of players ._name foes should target (if any)
vector<string> FindNewAttackingFoe () {
	Coord curCoord;
	string curStr = "";
	
	set<string> resSet;
	
	//creates list of map to plug into functions later
	vector<unordered_map<string, Dummy> > theList;
	theList.push_back(_playerMap);
	
	
	for (const auto& curFoe : _foeMap) {
		curCoord = curFoe.second._coord;
		
		//if not marching or idle, skip
		if ( !(curFoe.second._status == 1 || curFoe.second._status == 4) ) {
			continue;
		}
		
		//checks if attacking, then change status & attacking name;
		curStr = FindAttackingName(curCoord, theList);
		
		if (curStr != "") {
			
			_foeMap.find(curFoe.first)->second._status = 2;
			_foeMap.find(curFoe.first)->second._attackingName = curStr;
			
			resSet.insert(curStr);
			continue;
		}
	}
	
	
	//converts set -> list
	vector<string> resList;
	for (const auto& curName : resSet) {
		resList.push_back(curName);
	}
	
	return resList;
}

//(usedin CheckAllDummy())
//goes over all foes and palyers, if _attackingName is dead, then return to marching mode;
void CheckAllAttackingIfDead () {
	
	unordered_map<string, Dummy>::iterator attackingIt;
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _foeMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to marching
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}	
	}
	

	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _playerMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to marching
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}	
	}	
}

//(usaed in CheckAllDummy())
//goes over all attacking dummies, it _attackingName is not in attack range, then ._status->1
void CheckAllAttackingPos () {
	
	unordered_map<string, Dummy>::iterator attackingIt;
	
	vector<Coord> surroundingCoordList;
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _foeMap.find(mapIt->second._attackingName);
		
		surroundingCoordList = GenNeighborCoordList(mapIt->second._coord); 
		
		//if not surrounding, then go back to marching mode
		if (!CheckCoordInList(attackingIt->second._coord, surroundingCoordList) ) {
			mapIt->second._status = 1;
		}
	}
	
	
	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _playerMap.find(mapIt->second._attackingName);
		
		surroundingCoordList = GenNeighborCoordList(mapIt->second._coord); 
		
		//if not surrounding, then go back to marhing mode
		if (!CheckCoordInList(attackingIt->second._coord, surroundingCoordList) ) {
			mapIt->second._status = 1;
		}
	}
}

//(usedin Update())
//goes over all attacking dummies, calcs attacked dummy's hpx
void CalcAllAttackingHpx () {
	unordered_map<string, Dummy>::iterator attackingIt;
	
	int curHealthLoss = 0;
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _foeMap.find(mapIt->second._attackingName);
		
		//health loss = person attacking strength - attacked person def
		curHealthLoss = mapIt->second._atk - attackingIt->second._def;
		
		//in case def is > atk
		if (curHealthLoss < 0) {
			curHealthLoss = 0;
		}
		
		attackingIt->second._hpx -= curHealthLoss;
	}
	
	
	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in attackign mode, skip
		if (mapIt->second._status != 2) {
			continue;
		}
		
		attackingIt = _playerMap.find(mapIt->second._attackingName);
		
		//health loss = person attacking strength - attacked person def
		curHealthLoss = mapIt->second._atk - attackingIt->second._def;
		
		//in case def is > atk
		if (curHealthLoss < 0) {
			curHealthLoss = 0;
		}
		
		attackingIt->second._hpx -= curHealthLoss;
	}
}




/////////FLANKING (FOE)

//(used in FindNewFlankingFoe())
//input: a player name who should be targeted
//output: a list of foes' ._name in marching mode who are closest to player and will path find to the player
vector<string> GenFlankingFoeList (string targetName) {
	
	//creates a list of things to avoid to put into other functions
	vector<unordered_map<string, Dummy> > possibleFoeList;
	possibleFoeList.push_back(_foeMap);
	possibleFoeList.push_back(_playerMap);
	
	//gets list of coordinates not filled by foes yet around the targetName
	vector<Coord> targetCoordList = GenTwoTwoSurroundingEmptyCoordList(possibleFoeList, _playerMap.find(targetName)->second._coord);
	
	targetCoordList = RemoveFlankingTargetCoordList(targetCoordList);
	
	
	int neededFoeNum = targetCoordList.size();
	int curInt = 0;
	pair<string, int> tempPair;
	pair<string, int> curPair;
	Coord targetCoord = _playerMap.find(targetName)->second._coord;
	
	//makes a list with neededFoeNum amount of INT_MAXI value ints paired with an empty string
	vector<pair<string,int> > resList;
	for (int i = 0; i < neededFoeNum; i++) {
		resList.push_back(pair<string, int>("", INT_MAXI));
	}
	
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//skips dummies who are not marching
		if (mapIt->second._status != 1) {
			continue;
		}		

		curInt = CalcHori(mapIt->second._coord, targetCoord);
		curPair.first = mapIt->first;
		curPair.second = curInt;
		
		
		//checks if cur foe is closer than foes in resList currently then saves the foe number & name
		for (int i = 0; i < resList.size(); i++) {
			
			if (curPair.second < resList[i].second) {
				
				//swap curPiar & resList[i]
				tempPair = resList[i];
				
				resList[i] = curPair;
				
				curPair = tempPair;
			}			
		} 				
	}
	
	
	

	//filters out the resList if there aren't enough foes to take all spots & makes a vector<string>
	vector<string> stringResList;
	for (int i = resList.size() - 1; i > -1; i--) {
		if (resList[i].first == "") {
			resList.resize(i);
			continue;
		}		
		stringResList.push_back(resList[i].first);
	}
	
	
	return stringResList;
}

//(used in FindNewFlankingFoe())
//input: vector of ._name of foes who's status need to be updated to flanking mode, and ._name of their target player
//Changes: ._status ?->3;  ._attackingName ?->targetName; _targetCoord ?->whatever coord is fitting
//targetcoords updated can be up to a 2x2 grid around targetName
void UpdateFlankingFoeListStatusBig (vector<string> foeNameList, string targetName) {
	string curStr = "";
	
	unordered_map<string, Dummy>::iterator foeIt;
	
	//creates vector of maps for other functions to plug in
	vector<unordered_map<string, Dummy> > theList;
	theList.push_back(_foeMap);
	theList.push_back(_playerMap);
	
	vector<Coord> avaCoordList = GenTwoTwoSurroundingEmptyCoordList(theList, _playerMap.find(targetName)->second._coord); 
	avaCoordList = RemoveFlankingTargetCoordList(avaCoordList); 
	Coord curCoord;
	
	
	int smallerListSize = min(foeNameList.size(), avaCoordList.size());
	
	for (int i = 0; i < smallerListSize; i++) {
		curStr = foeNameList[i];
		curCoord = avaCoordList[i];
		
		foeIt = _foeMap.find(curStr);
		
		foeIt->second._attackingName = targetName;
		foeIt->second._targetCoord = curCoord;
		foeIt->second._status = 3;		
	}
}

//(used in FindNewIdleToFlankingFoe())
//input: vector of ._name of foes who's status need to be updated to flanking mode, and ._name of their target player
//Changes: ._status ?->3;  ._attackingName ?->targetName; _targetCoord ?->whatever coord is fitting
//targetCoords consistent of only neighboring cells of the targetName (left, right, up, down)
void UpdateFlankingFoeListStatusSmall (vector<string> foeNameList, string targetName) {
	string curStr = "";
	
	unordered_map<string, Dummy>::iterator foeIt;
	
	//creates vector of maps for other functions to plug in
	vector<unordered_map<string, Dummy> > theList;
	theList.push_back(_foeMap);
	theList.push_back(_playerMap);
	
	vector<Coord> avaCoordList = GenNeighborEmptyCoordList(theList, _playerMap.find(targetName)->second._coord); 
	avaCoordList = RemoveFlankingTargetCoordList(avaCoordList); 
	Coord curCoord;
	
	
	int smallerListSize = min(foeNameList.size(), avaCoordList.size());
	
	for (int i = 0; i < smallerListSize; i++) {
		curStr = foeNameList[i];
		curCoord = avaCoordList[i];
		
		foeIt = _foeMap.find(curStr);
		
		foeIt->second._attackingName = targetName;
		foeIt->second._targetCoord = curCoord;
		foeIt->second._status = 3;		
	}
}

//(used in GenPathFlankingFoe())
//input: list of coords
//output: list of coords in reverse order
vector<Coord> RevCoordList (vector<Coord> oriList) {
	vector<Coord> resList;
	for (int i = oriList.size() - 1; i > -1; i--) {
		resList.push_back(oriList[i]);
	}
	return resList;
}
//(used in GenPathFlankingFoe())
//used for sorting priority queue from least -> greatest
class Compare {
public:
    bool operator()(pair<Coord, int>  one, pair<Coord, int>  two) {
        return one.second > two.second;
    }
};
//(used in GenPathFlankingFoeList())
//input: start coord, end coord, and the map it is tyring to traverse
//output: a list of coords to get there using A*, returns an empty list if it is impossible to reach currently 
vector<Coord> GenPathFoe (Coord startCoord, Coord endCoord, vector<vector<Cell> > mainMap) {
	vector<Coord> resList;
	
	Cell curCell = mainMap[startCoord._xCoord][startCoord._yCoord];
	
	vector<Coord> newCoordList;
	Cell newCell;

	//makes vector of map for plugging into functions	
	vector<unordered_map<string, Dummy> > avoidList;
	avoidList.push_back(_playerMap);
	
	vector<Coord> openList; //keeps track of what coords are open
	vector<Coord> closedList; //keeps track of what coords are closed
	
	
	//priority queue to easily get the next smallest vertext to visit
    priority_queue< pair<Coord, int> , vector<pair<Coord, int> >, Compare> openPriorQueue;	
	
	//prep starting coord
	mainMap[startCoord._xCoord][startCoord._yCoord]._miniDis = 0;
	mainMap[startCoord._xCoord][startCoord._yCoord]._horiVal = CalcHori(startCoord, endCoord);
	openPriorQueue.push(pair<Coord, int>(startCoord, CalcHori(startCoord, endCoord)));
	
	
	int curDis = 0;	
	int cc = 0;
	
	
	//a* algo
	while (!(EqualCoord(curCell._coord, endCoord) || openPriorQueue.empty())) {
		cc++;		
		
		//finds smallest next curCoord to visit
		curCell._coord = openPriorQueue.top().first;
		openPriorQueue.pop();
			//if already closed don't need to go througha gain 
		if (CheckCoordInList(curCell._coord, closedList)) {
			continue;
		}
			//if player occupies, don't go
		if (CheckDummyListMapInLocation(avoidList, curCell._coord)) {
			continue;
		}
		
		
		closedList.push_back(curCell._coord);
		curCell = mainMap[curCell._coord._xCoord][curCell._coord._yCoord];
		
		
		//reached goal, no need to calculate more
		if (EqualCoord(curCell._coord, endCoord) ) {
			break;
		}
		
		
		//generates surrounding blocks w/out playesr standing on
		newCoordList = GenNeighborEmptyCoordList(avoidList, curCell._coord);
		
		
		//updates surrounding blocks
		for (int i = 0; i < newCoordList.size(); i++) {
			newCell = mainMap[newCoordList[i]._xCoord][newCoordList[i]._yCoord];
			
			
			//if not yet explored then add to openList and openPriorQueue		
			if (!CheckCoordInList(newCell._coord, openList)) {
				openList.push_back(newCell._coord);
				
				//gens hori and minidis val
				mainMap[newCell._coord._xCoord][newCell._coord._yCoord]._horiVal = CalcHori(newCell._coord, endCoord);
				mainMap[newCell._coord._xCoord][newCell._coord._yCoord]._miniDis = newCell._terrainInfo._value + curCell._miniDis;
				mainMap[newCell._coord._xCoord][newCell._coord._yCoord]._fatherCoord = curCell._coord;
				
				newCell = mainMap[newCell._coord._xCoord][newCell._coord._yCoord];
				curDis = newCell._miniDis + newCell._horiVal;
				
				openPriorQueue.push(pair<Coord, int>(newCell._coord, curDis));
				continue;
			}
			
			
			//updates element in priority queue if better solution
			curDis = newCell._terrainInfo._value + curCell._miniDis;
			
			if (curDis < newCell._miniDis) {
				mainMap[newCell._coord._xCoord][newCell._coord._yCoord]._miniDis = curDis;
				mainMap[newCell._coord._xCoord][newCell._coord._yCoord]._fatherCoord = curCell._coord;
				
				newCell = mainMap[newCell._coord._xCoord][newCell._coord._yCoord];
				curDis = newCell._miniDis + newCell._horiVal;
				
				openPriorQueue.push(pair<Coord, int>(newCell._coord, curDis));
			}	
			
		}	
		
	}
	
	
	Coord curCoord = curCell._coord;
	
	//if there is no path, then return empty list
	if (!EqualCoord(curCoord, endCoord) ) {
		return resList;		
	}
	
	//find path using recursive
	resList.push_back(curCoord);
	while (!EqualCoord(curCoord, startCoord) ) {
		resList.push_back(mainMap[curCoord._xCoord][curCoord._yCoord]._fatherCoord);
		curCoord = mainMap[curCoord._xCoord][curCoord._yCoord]._fatherCoord;
	}
	
	//gets rid of repeating coord at the end
	resList.resize(resList.size() - 1);
	
	return RevCoordList(resList);	
}

//(used in FindNewFlankingFoe(), FindNewIdleToFlankingFoe())
//input: list of names of foes whose paths need to generated
//saves the generated paths in _foeMap, if impossible to reach then becomes idle
void GenPathFlankingFoeList (vector<string> theList) {
	vector<Coord> curPath;
	
	unordered_map<string, Dummy>::iterator foeIt;
	
	for (int i = 0; i < theList.size(); i++) {
		foeIt = _foeMap.find(theList[i]);
		
		curPath = GenPathFoe(foeIt->second._coord, foeIt->second._targetCoord, _mainMap);
		
		//means there isn't a valid path, then just go to idle
		if (curPath.size() < 1) {
			foeIt->second._status = 4;
			continue;
		}
		
		foeIt->second._pathList = curPath;
	}
}

//(used in FindNewFlankingFoe())
//input: ._name of player
//checks if player is the target of any other falnking or idle foes lready
//output: true->target has been taken; false->targetSTR has NOT been taken
bool CheckTargetTaken (string targetStr) {
	for (const auto& foeIt : _foeMap) {
		//if not flanking or idle, then skip
		if (foeIt.second._status != 3 || foeIt.second._status != 4) {
			continue;
		}
		
		if (targetStr == foeIt.second._attackingName) {
			return true;
		}
	}
	
	return false;
}

//(used in CheckAllDummy())
//input -> a list of ._name of _players for foes to target
//finds & updates foes who should path find to players in targetList
//a foes should path find to players in targetList if they are currently in marching mode and are one of the closest to the player
//Changes: _status 1->3, gens _attackingName, gens _pathList
void FindNewFlankingFoe (vector<string> targetList) {
	string curStr = "";
	
	vector<string> curList;
	
	for (int i = 0; i < targetList.size(); i++) {
		curStr = targetList[i];
		
		//if others are already targeting, no need to target
		if (CheckTargetTaken(curStr)) {
			continue;
		}
		
		curList = GenFlankingFoeList(curStr);
		
		UpdateFlankingFoeListStatusBig(curList, curStr);
		GenPathFlankingFoeList(curList);
	}
	
}

//(usedin CheckAllDummy())
//(similar to CheckAllAttackingPos())
//goes over flanking foes, if reached _targetCoord, then become marching again
void CheckFlankingFoePos () {
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in flanking mode, skip
		if (mapIt->second._status != 3) {
			continue;
		}
		
		
		//if arrived at _targetCoord, then become idle
		if (EqualCoord(mapIt->second._coord, mapIt->second._targetCoord) ) {
			mapIt->second._status = 4;
			continue;
		}	
		
		
		//if it runs out of path, then become idle
		if (mapIt->second._pathList.size() < 1) {
			mapIt->second._status = 4;
		}	
		
	}
	
}

//(usedin CheckAllDummy())
//(similar to CheckAllAttackingIfDead())
//goes over all flanking foes and palyers, if _attackingName is dead, then return to marching mode;
void CheckAllFlankingIfDead () {
	
	unordered_map<string, Dummy>::iterator attackingIt;
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if not in falkning mode, skip
		if (mapIt->second._status != 3) {
			continue;
		}
		
		attackingIt = _foeMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to idle
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}	
		
	}
	
	
	
	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in flanking mode, skip
		if (mapIt->second._status != 3) {
			continue;
		}
		
		attackingIt = _playerMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to marching
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}		
		
	}
	
}

//(used in CheckAllDummy())
//gens or updates all flanking foe paths
void UpdateFlankingFoePath() {
	Dummy curDummy;
	vector<Coord> curList;
	
	//goes through foe list
	for (unordered_map<string, Dummy>::iterator foeIt = _foeMap.begin(); foeIt != _foeMap.end(); foeIt++) {
		
		//if not flanking, then skip
		if (foeIt->second._status != 3) {
			continue;
		}
		
		curDummy = foeIt->second;
		
		curList = GenPathFoe(curDummy._coord, curDummy._targetCoord, _mainMap);
		
		foeIt->second._pathList = curList;
	}
}



/////IDLE (FOE)

//(used in CheckAllDummy())
//       checks ._attacingName of all idle players
//output: list of ._names of players who has idle foes who should flank and attack
vector<string> FindIdleToFlankingTarget () {
	set<string> resSet;
	vector<string> resList;
	
	set<string> calcedSet;
	
	Dummy attackingDummy;
	
	vector<Coord> avaCoordList;
	
	vector<unordered_map<string, Dummy> > avoidList;
	avoidList.push_back(_playerMap);
	avoidList.push_back(_foeMap);
	
	
    for (const auto& curFoe : _foeMap) {
		
		//if not idle, then skip
		if (curFoe.second._status != 4) {
			continue;
		}
		
		//if already calclated the attacking name, no need to calculator again
		if (calcedSet.find(curFoe.second._attackingName) != calcedSet.end()) {
			continue;
		}
		
		attackingDummy = _playerMap.find(curFoe.second._attackingName)->second;
		
		avaCoordList = GenNeighborEmptyCoordList(avoidList, attackingDummy._coord);
		avaCoordList = RemoveFlankingTargetCoordList(avaCoordList);
		
		//if there are valid spots around ._attackingName not occupied by playesr/foes, nor target of another already flanking foe, then idle foes of this player are needed
		if (avaCoordList.size() > 0) {
			resSet.insert(attackingDummy._name);
		} 
		
		//update and save as calculated already to avoid re calcing
		calcedSet.insert(attackingDummy._name);
    }
    
    
    //converts resSet -> resList
    
    for (const auto& curName : resSet) {
    	resList.push_back(curName);
	}
	
	return resList;
}

//(used in FindNewIdleToFlankingFoe())
//input: a player name who should be targeted
//output: a list of foes' ._name in idle mode who are targetting the player and who are closest to player and will path find to the player
vector<string> GenNewIdleToFlankingFoeList (string targetName) {
	
	//creates a list of things to avoid to put into other functions
	vector<unordered_map<string, Dummy> > avoidList;
	avoidList.push_back(_foeMap);
	avoidList.push_back(_playerMap);
	
	//gets list of coordinates not filled by foes yet around the targetName
	vector<Coord> targetCoordList = GenNeighborEmptyCoordList(avoidList, _playerMap.find(targetName)->second._coord);
	targetCoordList = RemoveFlankingTargetCoordList(targetCoordList);
	
	
	int neededFoeNum = targetCoordList.size();
	int curInt = 0;
	
	Coord targetCoord = _playerMap.find(targetName)->second._coord;
	pair<string, int> middlePair;
	pair<string, int> curPair;
	
	//makes a list with neededFoeNum amount of INT_MAXI value ints paired with an empty string
	vector<pair<string,int> > resList;
	for (int i = 0; i < neededFoeNum; i++) {
		resList.push_back(pair<string, int>("", INT_MAXI));
	}
	
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//skips dummies who are not waiting
		if (mapIt->second._status != 4) {
			continue;
		}		
		//if idle dummy's target isn't targetName,then skip
		if (mapIt->second._attackingName != targetName) {
			continue;
		}

		
		curInt = CalcHori(mapIt->second._coord, targetCoord);
		curPair.first = mapIt->first;
		curPair.second = curInt;
		
		
		//checks if cur foe is closer than foes in resList currently then saves the foe number & name
		for (int i = 0; i < resList.size(); i++) {
			if (curPair.second < resList[i].second) {
				//swaps curPiar & resLIst[i]
				middlePair = resList[i];
				
				resList[i] = curPair;
				
				curPair = middlePair;
			}
		} 
		
	}
	
	
	//filters out the resList if there aren't enough foes to take all spots & makes a vector<string>
	vector<string> stringResList;
	for (int i = resList.size() - 1; i > -1; i--) {
		if (resList[i].first == "") {
			resList.resize(i);
			continue;
		}
		
		stringResList.push_back(resList[i].first);
	}
	
	
	return stringResList;
}

//(used in ())
//input: list of ._names of _players who has idle foes who should be flanking
//       finds closest idle foes to tell to move
//       updates those idle foes, _status 4->3, update _targetCoord, gen _pathList
void FindNewIdleToFlankingFoe (vector<string> targetNameList) {
	string curName = "";
	vector<string> newFlankingFoeList;
	
	for (int i = 0; i < targetNameList.size(); i++) {
		curName = targetNameList[i];
		
		
		newFlankingFoeList = GenNewIdleToFlankingFoeList(curName); 
		
		UpdateFlankingFoeListStatusSmall(newFlankingFoeList, curName);
		GenPathFlankingFoeList(newFlankingFoeList);
	}
}


//(usedin CheckAllDummy())
//(similar to CheckAllAttackingIfDead(), CheckAllFlankingIfDead())
//goes over all idle foes and palyers, if _attackingName is dead, then return to marching mode;
void CheckAllIdleIfDead () {
	
	unordered_map<string, Dummy>::iterator attackingIt;
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if not in idle mode, skip
		if (mapIt->second._status != 4) {
			continue;
		}
		
		attackingIt = _foeMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to idle
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}	
		
	}
	
	
	
	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if not in idle mode, skip
		if (mapIt->second._status != 4) {
			continue;
		}
		
		attackingIt = _playerMap.find(mapIt->second._attackingName);
		
		//if attacking person is dead, then return to marching
		if (attackingIt->second._status == 0) {
			mapIt->second._status = 1;
		}	
	}	
}



////////MARCHING (FOE)

//(used in CheckAllDummy())
//gens or updates all marching foe paths to (_xCoord, 0)
void UpdateMarchingFoePath() {
	Dummy curDummy;
	Coord targetCoord;
	vector<Coord> curList;
	
	//goes through foe list
	for (unordered_map<string, Dummy>::iterator foeIt = _foeMap.begin(); foeIt != _foeMap.end(); foeIt++) {
		
		//if not marching, then skip
		if (foeIt->second._status != 1) {
			continue;
		}
		
		curDummy = foeIt->second;
		
		//end coord is straight to left @ ._yCoord == 0
		targetCoord = curDummy._coord;
		targetCoord._yCoord = 0;
		
		curList = GenPathFoe(curDummy._coord, targetCoord, _mainMap);
		
		foeIt->second._pathList = curList;
	}
}


///////MOVEMENT

//(used in MoveFlankingFoe(), MoveMarchingFoe())
//input: vector of coords;
//output: vector of coords w/out first coord
vector<Coord> EraseFirstCoord (vector<Coord> theList) {
	vector<Coord> resList;
	for (int i = 1; i < theList.size(); i++) {
		resList.push_back(theList[i]);
	}
	return resList;
}

//(used in Update())
//moves all foes in _foeList who are in marching once down their path
void MoveMarchingFoe () {
	vector<Coord> curPath;
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {

		//if not marching, the skip
		if (mapIt->second._status != 1) {
			continue;
		}
		
		curPath = mapIt->second._pathList;
		
		//if no path exsists, then skip
		if (curPath.size() < 1) {
			continue;
		}
		
		
		mapIt->second._coord = curPath[0];
		mapIt->second._pathList = EraseFirstCoord(curPath);
	}	

}

//(used in Update())
//moves falnking foes once down path
void MoveFlankingFoe () {
	vector<Coord> curPath;
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {

		//if not flanking, the skip
		if (mapIt->second._status != 3) {
			continue;
		}
		
		curPath = mapIt->second._pathList;
		mapIt->second._coord = curPath[0];
		
		mapIt->second._pathList = EraseFirstCoord(curPath);
		
		//if reaches end of path finding, reverts foe back to marching mode
		if (mapIt->second._pathList.size() < 1) {
			mapIt->second._status = 4;
		}
	}
	
}


//(used in MoveMarchingFoeLeft())
//input: string of ._name of foe to be moved left once
//moves foe left one cell
void MoveFoeLeft (string name) {
	unordered_map<string, Dummy>::iterator mapIt = _foeMap.find(name);
	
	mapIt->second._coord._yCoord--;
	
	//if moves outside of bounds, then dead
	if (mapIt->second._coord._yCoord < 0) {
		mapIt->second._status = 0;
	}
}

//(used in Update())
//moves all marching foes w/out any paths left
void MoveMarchingFoeLeft () {
	vector<Coord> curPath;
	
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {

		//if not marching, the skip
		if (mapIt->second._status != 1) {
			continue;
		}
		
		curPath = mapIt->second._pathList;
		
		//if do have a path, then skip
		if (curPath.size() > 0) {
			continue;
		}
		
		MoveFoeLeft(mapIt->first);		
	}	
}


//(used in CheckAllDummy())
//goes over all foes and players, if hpx <= 0 or are out of frame, then changes status to 0 (aka dead)
void CheckAllIfDead () {
	
	//checking player map
	for (unordered_map<string, Dummy>::iterator mapIt = _playerMap.begin(); mapIt != _playerMap.end(); mapIt++) {
		
		//if already dead, no need to check
		if (mapIt->second._status == 0) {
			continue;
		}
		
		//checks hpx
		if (mapIt->second._hpx < 1) {
			mapIt->second._status = 0;
			continue;
		}
		
		//check if in boundary
		if (!CheckInCoordInBounds(mapIt->second._coord)) {
			mapIt->second._status = 0;
		}
		
	}
	
	//checking foe map
	for (unordered_map<string, Dummy>::iterator mapIt = _foeMap.begin(); mapIt != _foeMap.end(); mapIt++) {
		
		//if already dead, no need to check
		if (mapIt->second._status == 0) {
			continue;
		}
		
		//checks hpx
		if (mapIt->second._hpx < 1) {
			mapIt->second._status = 0;
			continue;
		}
		
		//check if in boundary
		if (!CheckInCoordInBounds(mapIt->second._coord)) {
			mapIt->second._status = 0;
		}
	}
}


//(used in Update(), Core())
//checks all dummmies
void CheckAllDummy () {
	//list of names of players that should be targetted by flanking foes
	vector<string> targetList;
	

	//updates all dead, then checks if _targetName is dead for statuses, checks _attackingName if still valid to attack, checks _pathList if reached end of path
	CheckAllIfDead();
	
	CheckAllAttackingIfDead();
	CheckAllAttackingPos();
	
	CheckAllFlankingIfDead();
	CheckFlankingFoePos();
	
	CheckAllIdleIfDead();
	
	
	//find new attacking player / foes
	FindNewAttackingPlayer();
	targetList = FindNewAttackingFoe(); 
	
	//update/gen new flanking foes
	if (targetList.size() > 0) {
		FindNewFlankingFoe(targetList);
	}


	//find new idle->flanking playesr
	targetList = FindIdleToFlankingTarget();

	//update/gen new idle->flanking foes
	if (targetList.size() > 0) {
		FindNewIdleToFlankingFoe(targetList);
	}
	

	//update / regen paths for flanking & marching foes	
	UpdateMarchingFoePath();
	UpdateFlankingFoePath();	
}

//(used in Core())
//Updates ALL dummies & ALL info to next position
void Update () {
	
	//all move
	MoveMarchingFoeLeft();
	MoveMarchingFoe();
	MoveFlankingFoe();
	
	//all attack
	CalcAllAttackingHpx();
	
	CheckAllDummy();
}


//(used in CheckIfWin())
//input: map of dummies
//output: true -> all dummies in map are dead, false -> not all dummies in map are dead
bool CheckAllMapDead(unordered_map<string, Dummy> theMap) {
	for (const auto& curIt : theMap) {
		if (curIt.second._status != 0) {
			return false;
		}
	}
	return true;
}
//(used in Core())
//checks if one, or both sides are dead
bool CheckIfWin () {
	if (CheckAllMapDead(_playerMap)) {
		return true;
	}
	if (CheckAllMapDead(_foeMap)) {
		return true;
	}
	
	return false;
}



//ENDINGS & OPENINGS
//(used in Core(), main())
//displays when player forces ending  before simulation ends
int FastEnding () {
	cout << endl;
	cout << "Ah, I see you're a busy person! Well, see you next time!" << endl;
	return 0;
}

//prints opening words
//output: true -> proceed to battlie simulation, false -> end simluation
bool OpeningWord () {
	string input = "";
	
	cout << "Welcome to the BATTLE SIMULATION \n";
	cout << "Customize your own battle! Root for the players... or the foes! Simply relax and watch the action unfold.";
	cout << "You are overlooking a battlefield, soon to be full of blood shed. \n \n";
	cout << "Type 'c' to learn about the competition:" << endl;
	cin >> input;
	
	if (input == DEV_KEY) {
		return true;
	}
	
	if (input != "c") {
		FastEnding();
		return false;
	}
	
	cout << endl << endl;
	cout << "CHARACTERS: \n";
	cout << "-There are two types of fighters on today's battlefield, nicknamed 'players' and 'foes'. \n";
	cout << "-Players:\n";
	cout << "	-Players are quite lazy. Players stay where they are till their last breath.\n";
	cout << "	-If a Foe reaches their immediate vicinity, Players attack.\n";
	cout << "	-Players like to go the easy route, so they always attack the foe with the least health.\n";
	cout << "	-Once a Player finds a weak Foe to attack, they only attack that particular Foe until one of them, eventually, dies. \n";
	cout << "	-Once a Foe is dead, Players continue lazying about, like they always do.\n";
	cout << "	-Players aren't exactly friendly, but they're not hostile to one another.\n";
	cout << "-Foes:\n";
	cout << "	-Foes are the adventurous bunch.\n";
	cout << "	-A tight knit community, Foes continually move to the West... until one Foe encounters a Player\n";
	cout << "	-Then, that Foe calls upon fellow Foes who are closest to help flank and attack the Player\n";
	cout << "	-Foes called to action try to quickly traverse the terrain to help their fello Foe. Sometimes, they're so focused on their destination that they run directly passed Players without realizing and don't attack! Players with their keen eyes however always spot Foes passing by and attack. Some Foes get by with a scratch and continue on their journey, but some have not been so lucky.\n";
	cout << " 	-On their style of attacking, Foes and very similar to Players. Foes won't move on to attacking another Player until their previous target Player is struck down, or they are. \n";
	cout << "	-Unfortunately, Foes' communication systems aren't quite advanced. Only when ALL previous Foes notified for attack arrive at the scene (or perish during the arduous journey), can a Foe call for another attack on that same Player\n";
	cout << "	-Once Foes have successfully taken down a Player, they continue their absent-minded trek West, until they eventually walk off the map, find a Player, or receive another call. \n";
	cout << "	-Foes value collaboration over everything, and would never harm each other. \n";
	cout << "\n\n\nNow that you've learned about our Characters, type 'i' for the Instructions!" << endl;
	cin >> input;
	if (input != "i") {
		FastEnding();
		return false;
	}
	
	cout << endl << endl;
	cout << "INSTRUCTIONS:\n";
	cout << "-To end the simulation, type 'stop' and the simulation will end. \n";
	cout << "-To continue to watch the simulation play out, type anything else you want (WITHOUT spaces) and press 'Enter' key. The next frame of the Simulation will run. \n";
	cout << "-If one party is completely wiped out by another, then the surviving group wins.\n";
	cout << "-If both parties perish at the same time, then it's a tie. \n";
	cout << "\n\n\nReady to see how YOUR battle plan plays out? Type 'start' to get your simulation running, or anything else to cancel. See you on the another side comrade! :)" << endl;
	cin >> input;
	if (input != "start") {
		FastEnding();
		return false;
	} 

	cout << endl << endl;
	
	return true;
}
 
//(used in Core())
//displays when at least one side is deadd
void WinningEnding () {
	cout << "GAME OVER!!!" << endl;
	cout << "It was a fierce battle, but ";
	
	//tie
	if (CheckAllMapDead(_playerMap) && CheckAllMapDead(_foeMap)) {
		cout << "both sized have ceased to exsist." << endl;
		cout << "Countless bodies are strewn across the field, enemy with enemy, was there really a difference? Only you remain standing, the sole witness. Maybe such a futile fight could've been avoided. As a new dawn rises and casts its, oddly, cool rays upon this deathly silent battlefield, remember that a new day means new beginnings. A new day means new possibilities. " << endl;
		cout << "You have Unlocked 'New Beginnings'" << endl;
		return;
	}
	
	//player wins
	if (CheckAllMapDead(_foeMap)) {
		cout << "the Players have won the fight!" << endl;
		cout << "Seems that fending for yourself was the best tactic for this battle. Surely all the Players believe so too. So, what's next?" << endl;
		cout << "You have Unlocked 'solidarity'" << endl;
		return;
	}
	
	//foe wins
	cout << "the Foes have won the fight!" << endl;
	cout << "As the maxim goes: 'teamwork makes the dream work'. Banding together the Foes are strong in number! But... how about individuality? Nevermind that, the next battle is approaching..." << endl;
	cout << "You have Unlocked 'Comrades'" << endl;
}



void Core () {
	string input = "start";
	
	
	//sorts maps for asthetic and display reasons
	
	
	bool firstCycle = true;
	
	while (input != "stop") {
		cout << endl << endl;
		
		//first time needs slightly differnet procedures
		if (firstCycle) {
			firstCycle = false;
			CheckAllDummy();
			Display();
			
			//check if game over!
			if (CheckIfWin()) {
				break;
			}
			
			cin >> input;
			continue;
		}
		
		
		//check if game over!
		if (CheckIfWin()) {
			break;
		}
		
		Update();
		Display();
		
		
		cin >> input;
	}
	
	
	
	//player forced ending
	if (input == "stop") {
		FastEnding();
		return;
	}
	
	WinningEnding();
}


int main () {
	ParseIn();
	
	//if the player quits early, then stop simulation
	if (!OpeningWord()) {
		return 0;
	}
	
	Core();
	
	return 0;
}
