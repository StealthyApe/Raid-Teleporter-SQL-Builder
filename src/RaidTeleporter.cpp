#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

enum mode {
	normalOnly = 0,
	pre32_heroic_25man_norm = 1,



};

struct coord {
	int map = 0;
	float x = 0;
	float y = 0;
	float z = 0;
	float o = 0;
	mode mode = normalOnly;
};

void RaidTeleporterBroadcast(ofstream& output, vector<string>bossNames, int GossipIndexStart) {
	int boss = 0;
	output << "REPLACE INTO `broadcast_text` (`ID`, `LanguageID`, `MaleText`, `FemaleText`, `EmoteID1`, `EmoteID2`, `EmoteID3`, `EmoteDelay1`, `EmoteDelay2`, `EmoteDelay3`, `SoundEntriesId`, `EmotesID`, `Flags`, `VerifiedBuild`) VALUES";
	output << '\n';
	for (int i = GossipIndexStart; i < bossNames.size() + GossipIndexStart; i++) {
		output << '(' << i << ", 7, 'Teleport to " << bossNames[boss] << "', '', 0, 0, 0, 0, 0, 0, 0, 0, 1, 1)";
		if (i != bossNames.size() + GossipIndexStart - 1) {
			output << ",\n";
		}
		else {
			output << ";\n";
		}
		boss++;
	}
}
vector<string> BossNameList(ifstream& input, vector<int>& RaidSize) {
	string is;
	vector<string> bosses;
	int bossCount = 0;
	while (!input.eof()) {
		is.clear();
		getline(input, is);
		if (is.size() != 0) {
			bossCount++;
			bosses.push_back(is);
			is.clear();
		}
		else {
			RaidSize.push_back(bossCount);
			is.clear();
			bossCount = 0;
		}
	}
	RaidSize.push_back(bossCount);
	return bosses;
}

void GossipMenuOptions(ofstream& output, vector<string>bossNames, vector<int> bossNumber, int gossipMenuStart, int GossipIndexStart) {
	
	int bosslist = 0;
	int GossipIndex = GossipIndexStart;
	for (int i = gossipMenuStart; i < bossNumber.size() + gossipMenuStart; i++) {
		for (int i2 = 0; i2 < bossNumber[i - gossipMenuStart]; i2++) {
			output << "REPLACE INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`, `OptionType`, `OptionNpcFlag`, `ActionMenuID`, `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES ";
			output << "(" << i << ", "<< i2 <<", 0, 'Teleport to " << bossNames[bosslist] << "', " << GossipIndex << ", 1, 1, 0, 0, 0, 0, '', 0, 1);\n";
			bosslist++;
			GossipIndex++;
		}
	}
}

vector<string> GetInstanceNames(ifstream& is) {
	string stringIn;
	vector<string> instancelist;
	while (getline(is, stringIn)) {
		if (stringIn.size() > 0) {
			instancelist.push_back(stringIn);
		}
		stringIn.clear();
	}
	return instancelist;
}

void GameObjectTemplate(ofstream& output, vector<string>instanceNames, int gossipMenuStart, int ObjectStart) {
	output << "REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`,"
		<< "`Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES \n";
	for (int instanceNameIterator = 0; instanceNameIterator < instanceNames.size(); instanceNameIterator++) {
		output << "(" << ObjectStart << ", 10, 8632, '" << instanceNames[instanceNameIterator] << " Teleporter', '', '', '', 0.5, 0, 0, 0, 0, 0,"
			<< "0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, " << gossipMenuStart << ", 1, 0, 0, 0, '', 'RaidTeleporter', 0)";
		if (instanceNameIterator == instanceNames.size() - 1) {
			output << ';';
		}
		else {
			output << ',';
		}
		output << '\n';
		ObjectStart++;
		gossipMenuStart++;
	}

}

vector<coord*> GetCoordList(ifstream& is) {
	string input;
	stringstream ss;
	coord* line = new coord;
	vector<coord*> coordlist;
	bool ID = false;
	while (getline(is, input)) {
		ss << input;
		while (!ss.eof()) {
			line = new coord;
			ID = false;
			string temp;
			ss >> temp; // discards {
			if (temp[0] == '/') {
				ID = true;
				break;
			}
			ss >> temp;
			line->map = stoi(temp);
			ss >> temp; // discards ,
			ss >> temp;
			line->x = stof(temp);
			ss >> temp; // discards ,
			ss >> temp;;
			line->y = stof(temp);
			ss >> temp; // discards ,
			ss >> temp;
			line->z = stof(temp);
			ss >> temp; // discards ,
			ss >> temp;;
			line->o = stof(temp);
			while (ss >> temp); // emptys buffer
		}
		ss.clear();
		if (!ID) {
			coordlist.push_back(line);
		}
		input.clear();
	}
	return coordlist;

}

void GameObject(ofstream& output, vector<coord*> coordinateList, int ObjectStart,int GuidStart) {
	output << "REPLACE INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`,"
		"`position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`,"
		"`animprogress`, `state`, `ScriptName`, `VerifiedBuild`) VALUES";
	output << '\n';
	int storedMap = coordinateList[0]->map;
	for (int i = 0; i < coordinateList.size(); i++) {
		if (coordinateList[i]->map != storedMap) {
			ObjectStart++;
			storedMap = coordinateList[i]->map;
		}
		output << "(" << GuidStart << ", " << ObjectStart << ", " << coordinateList[i]->map
			<< ", 0, 0, 15, 1, " << coordinateList[i]->x << ", " << coordinateList[i]->y << ", "
			<< coordinateList[i]->z << ", " << coordinateList[i]->o << ", 0, 0, -0.819342, -0.573306, 300, 0, 1, '', NULL)";
		if (i == coordinateList.size() - 1) {
			output << ';';
		}
		else output << ',';
		GuidStart++;
		output << '\n';
	}

}

void ProduceNecesaryValues (int& GossipIndexStart, int& GossipMenuStart,int& ObjectIndexStart, int& GuidStart) {
	cout << "Enter a Value to start broadcast_text values\n";
	cout << "A good default is 92000\n";
	while (true) {
		cin >> GossipIndexStart;
		if (GossipIndexStart <= 0) {
			cout << "Value must be positive\n";
		}
		else {
			break;
		}
	}
	cout << '\n';
	cout << "Enter a Value to start gossip_menu_option values\n";
	cout << "A good default is 45000\n";
	while (true) {
		cin >> GossipMenuStart;
		if (GossipMenuStart <= 0) {
			cout << "Value must be positive\n";
		}
		else {
			break;
		}
	}
	cout << '\n';
	cout << "Enter a Value to start gameobject_template values\n";
	cout << "A good default is 902000\n";
	while (true) {
		cin >> ObjectIndexStart;
		if (ObjectIndexStart <= 0) {
			cout << "Value must be positive\n";
		}
		else {
			break;
		}
	}
	cout << '\n';
	cout << "Enter a Value to start gameobject values\n";
	cout << "A good default is 6000000\n";
	while (true) {
		cin >> GuidStart;
		if (GuidStart <= 0) {
			cout << "Value must be positive\n";
		}
		else {
			break;
		}
	}
	cout << '\n';
}

int main() {
	ofstream output;
	cout << "This is the sql Generator for RaidTeleporter a module designed for Azerothcore\n";
	cout << "Three Values are needed for these SQL files to be generated and three text files\n";
	cout << "The text files are provided and need to be placed in the same folder as the source code or the executable\n";
	cout << "bosslist.txt, instancenames.txt, and coordinatelist.txt are the two require files\n";
	cout << "Check the database for your server to find a large range of unused indexs for the following tables\n";
	cout << "gameobject_template -- Located in acore_world by default\n";
	cout << "gossip_menu_option -- Located in acore_world by default\n";
	cout << "braodcast_text -- Located in acore_world by default\n";
	ifstream bosses("bosslist.txt");
	if (!bosses.is_open()) {
		cout << "bosslist.txt not found!\n";
		cout << "Press any key to close the program\n";
		getchar(); // cross platform pause
		return 0;
	}
	ifstream instanceNames("instancenames.txt");
	if (!instanceNames.is_open()) {
		cout << "instancenames.txt not found!\n";
		cout << "Press any key to close the program\n";
		getchar(); // cross platform pause
		return 0;
	}
	ifstream coords("coordinatelist.txt");
	if (!coords.is_open()) {
		cout << "coordinatelist.txt not found!\n";
		cout << "Press any key to close the program\n";
		getchar();
		return 0;
	}
	int GossipIndexStart = 92000;
	int GossipMenuStart = 45000;
	int ObjectIndexStart = 902000;
	int GuidStart = 6000000;
	bool testing = false;
	if (!testing) {
		ProduceNecesaryValues(GossipIndexStart, GossipMenuStart, ObjectIndexStart, GuidStart);
	}
	
	vector<int> bossTotal;
	vector<string> bossNames = BossNameList(bosses, bossTotal);
	bosses.close();
	output.open("broadcast.sql");
	RaidTeleporterBroadcast(output, bossNames, GossipIndexStart);
	output.close();
	output.open("gossipmenuoptions.sql");
	GossipMenuOptions(output, bossNames, bossTotal, GossipMenuStart, GossipIndexStart);
	output.close();
	vector<string> instanceList = GetInstanceNames(instanceNames);
	output.open("gobjecttemplate.sql");
	GameObjectTemplate(output, instanceList, GossipMenuStart, ObjectIndexStart);
	output.close();
	vector<coord*> coordinateList = GetCoordList(coords);
	output.open("gameobject.sql");
	GameObject(output,coordinateList,ObjectIndexStart,GuidStart);
	output.close();
	
}