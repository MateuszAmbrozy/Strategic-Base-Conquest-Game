#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include <unordered_map>
//STRUCTURES
struct Unit
{
    int id = 0;
    char affiliation = '0';
    char type = '0';
    int resilience = 0;
    int distance = 0;
    int x = 0, y = 0;
    int cost = 0;
    int attackRange = 0;
    int builindTime = 0;
    bool attacked = false;
    bool building = false;
    char unitToBuild = '0';
};
struct actionLine
{
    int unitID = 0;
    char action = '0';
    int x = 0, y = 0;
    int unitIDToAttack = 0;
    char unitToBuild = '0';
};

//ATTACK TABLE
std::unordered_map<char, std::unordered_map<char, int>> attackTable = 
{
    {'K', {{'K', 35}, {'S', 35}, {'A', 35}, {'P', 35}, {'C', 35}, {'R', 50}, {'W', 35}, {'B', 35}}},
    {'S', {{'K', 30}, {'S', 30}, {'A', 30}, {'P', 20}, {'C', 20}, {'R', 30}, {'W', 30}, {'B', 30}}},
    {'A', {{'K', 15}, {'S', 15}, {'A', 15}, {'P', 15}, {'C', 10}, {'R', 10}, {'W', 15}, {'B', 15}}},
    {'P', {{'K', 35}, {'S', 15}, {'A', 15}, {'P', 15}, {'C', 15}, {'R', 10}, {'W', 15}, {'B', 10}}},
    {'C', {{'K', 40}, {'S', 40}, {'A', 40}, {'P', 40}, {'C', 40}, {'R', 40}, {'W', 40}, {'B', 50}}},
    {'R', {{'K', 10}, {'S', 10}, {'A', 10}, {'P', 10}, {'C', 10}, {'R', 10}, {'W', 10}, {'B', 50}}},
    {'W', {{'K', 5}, {'S', 5}, {'A', 5}, {'P', 5}, {'C', 5}, {'R', 5}, {'W', 5}, {'B', 1}}},
};

//GLOBAL VARIABLES
long enemyGold;
long playerGold;
double player_remaining_time = 0;
double enemy_remaining_time = 0;

//ACCESSORS
char getType(std::vector<Unit>& units, int id);
void getNumberOfUnitsByAffiliation(std::vector<Unit>& units, int& playerUnits, int& enemyUnits);
int getMaxUnitID(const std::vector<Unit> units);
int getResilienceOfEntity(int id, std::vector<Unit> units);
int getAttackValue(const char attacker, const char defender);

//LOAD FUNCTIONS
void loadStatusFile(const std::string& filename, std::vector<Unit>& units, std::vector<std::vector<char>>& map);
void loadActions(const std::string filename, std::vector<actionLine>& actions);
int loadEndGameInfo(std::string filename);
std::vector<std::vector<char>> loadMapFile(const std::string& filename);

//SAVE FUNCTIONS
void saveStatusFile(std::string filename, const std::vector<Unit> units);
void saveEndGameInfo(std::string filename, int tour);

//REGULAR FUNCTIONS
void runPlayerProgram(const std::string& player, const std::string& mapFile, const std::string& statusFile, const std::string& ordersFile, const std::string& timeLimit);
void followOrders(std::vector<actionLine> actions, std::vector<Unit>& units);

//MAIN
int main(int argc, char* argv[]) 
{
    std::string timeLimit;
    std::vector<std::vector<char>> map;
    std::vector<Unit> units;
    std::vector<actionLine> actions;
    if (argc < 5 || argc > 6) 
    {
        std::cerr << "Uzycie: " << argv[0] << " <program_gracza> <plik_mapy> <plik_statusu> <plik_zamowien> [<limit_czasu>]" << std::endl;
        return 1;
    }
    else 
        timeLimit = (argc > 5) ? argv[5] : "5";


    std::string player = argv[1];
    std::string mapFile = argv[2];
    std::string statusFile = argv[3];
    std::string ordersFile = argv[4];

    int tour = loadEndGameInfo("endGame.txt"); //endGame.txt contains number of tours
    map = loadMapFile(mapFile);

    if(tour < 2000)
    {    
        runPlayerProgram(player, mapFile, statusFile, ordersFile, timeLimit);
        loadActions(ordersFile, actions);
        loadStatusFile(statusFile, units, map);

         //CHECK IF PLAYER BASE IS DESTROYED
        if(units[0].resilience <= 0)
        {
            std::cout<<"Enemy Wins\n";
            return 0;
        }
        //CHECK IF ENEMY BASE IS DESTROYED
        else if(units[1].resilience <= 0)
        {   
            std::cout<<"Player Wins!\n";
            return 0;
        }
       
        followOrders(actions, units);
        saveStatusFile(statusFile, units);
        //INCREASE TOUR AND SAVE TO FILE
        tour++;
        remove("endGame.txt");
        saveEndGameInfo("endGame.txt", tour);
    }
    else
    {
        std::cout << "END GAME::GAME RUNS TO LONG\n";
        int playerUnits = 0;
        int enemyUnits = 0;
        loadStatusFile(statusFile, units, map);
        getNumberOfUnitsByAffiliation(units, playerUnits, enemyUnits);
        if(playerUnits > enemyUnits)
        {
    
            std::cout<<"Player Wins!\n" << "Player has " << playerUnits <<" units\n" << "Enemy has " <<  enemyUnits << " units\n" ;
        }
        else if(enemyUnits > playerUnits)
            {
                std::cout<<"Enemy Wins!\n" << "Player has " << playerUnits <<" units\n" << "Enemy has " <<  enemyUnits << " units\n" ;
            }
        else
            std::cout<<"Remis\n";
    }

    return 0;
}

//ACCESSORS
char getType(std::vector<Unit>& units, int id)
{
    for(auto& unit: units)
    {
         if(unit.id == id)
         {
            return unit.type;
         }
    }
    return '0';
}
void getNumberOfUnitsByAffiliation(std::vector<Unit>& units, int& playerUnits, int& enemyUnits)
{
    for(auto& unit: units)
    {
        if(getResilienceOfEntity(unit.id, units) > 0)
        if(unit.affiliation == 'P')
        {
            ++playerUnits;
        }
        else if(unit.affiliation == 'E')
        {
            ++enemyUnits;
        }
    }
}
int getResilienceOfEntity(int id, std::vector<Unit> units)
{
    for (const auto& unit : units)
    {
        if (unit.id == id)
        {
            return unit.resilience;
        }
    }
    return 0;
}
int getMaxUnitID(const std::vector<Unit> units) 
{
    //RETURN NEXT FREE ID FOR UNIT TO CREATE
    int maxID = -1;

    for (const Unit& unit : units) {
        if (unit.id > maxID) {
            maxID = unit.id;
        }
    }

    return maxID + 1;
}
int getAttackValue(const char attacker, const char defender) 
{
    /*
    return the unit's attack based on the attack table
    */
    if (attackTable.count(attacker) && attackTable[attacker].count(defender)) 
    {
        return attackTable[attacker][defender];
    } else 
    {
        return 0;
    }
}
//LOAD FUNCTIONS
void loadStatusFile(const std::string& filename, std::vector<Unit>& units, std::vector<std::vector<char>>& map)
{
    /*
    LOAD STATUS OF ALL UNITS TO VECTOR, THIS VECTOR HELPS TO MANAGE OF UNITS
    */
    std::ifstream in_file(filename);
    if (in_file.is_open())
    {
        Unit unit;
        in_file >> playerGold >> enemyGold >> player_remaining_time >> enemy_remaining_time;
        while (in_file >> unit.affiliation >> unit.type >> unit.id >> unit.x >> unit.y >> unit.resilience)
        {
            if (unit.type == 'K')
            {
                unit.distance = 5;
                unit.cost = 400;
                unit.attackRange = 1;
                unit.builindTime = 5;
            }
            else if (unit.type == 'S')
            {
                unit.distance = 2;
                unit.cost = 250;
                unit.attackRange = 1;
                unit.builindTime = 3;
            }
            else  if (unit.type == 'A')
            {
                unit.distance = 2;
                unit.cost = 250;
                unit.attackRange = 5;
                unit.builindTime = 3;
            }
            else if (unit.type == 'P')
            {
                unit.distance = 2;
                unit.cost = 200;
                unit.attackRange = 2;
                unit.builindTime = 3;
            }
            else if (unit.type == 'R')
            {
                unit.distance = 2;
                unit.cost = 500;
                unit.attackRange = 1;
                unit.builindTime = 4;
            }
            else if (unit.type == 'C')
            {
                unit.distance = 2;
                unit.cost = 800;
                unit.attackRange = 7;
                unit.builindTime = 6;
            }
            else if (unit.type == 'W')
            {
                unit.distance = 2;
                unit.cost = 100;
                unit.attackRange = 1;
                unit.builindTime = 2;
            }
            else if (unit.type == 'B')
            {

                in_file >> unit.unitToBuild;
                unit.distance = 0;
            }
            units.push_back(unit);
        }
        in_file.close();
    }

    
}
void loadActions(const std::string filename, std::vector<actionLine>& actions)
{
    std::ifstream file(filename);
    if(file.is_open())
    {
        actionLine action;
        while (file>> action.unitID >> action.action) 
        {
            if (action.action == 'M')
            {
                file >> action.x >> action.y;
            }
            else if (action.action == 'A')
            {
                file >> action.unitIDToAttack;
                std::cout<<"UnitToAttack: " << action.unitIDToAttack << std::endl;
            }
            else if (action.action == 'B')
            {
                file >> action.unitToBuild;
            }
            actions.push_back(action);
        }
    }
}
int loadEndGameInfo(std::string filename)
{
    std::ifstream file(filename);
    int tour = 0;
    if(file.is_open())
    {
        file >> tour;
    }
    return tour;
}
std::vector<std::vector<char>> loadMapFile(const std::string& filename)
{
    std::vector<std::vector<char>> map;

    std::ifstream in_file(filename);
    if (in_file.is_open())
    {
        std::string line;
        while (std::getline(in_file, line))
        {
            std::vector<char> row;
            std::istringstream iss(line);
            char c;
            while (iss >> c)
            {
                row.push_back(c);
            }
            map.push_back(row);
        }
        in_file.close();
        std::cerr << "FILE " << filename << " LOADED." << std::endl;
    }
    else
    {
        std::cerr << "ERROR::COULD NOT OPEN " << filename << ". MAKING DEFAULT MAP... " << std::endl;
        map = {
            {'1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'6', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '6', '0', '9', '9', '9', '9', '9', '0', '0', '6', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '9', '9', '9', '9', '9', '0', '0', '0', '0', '0', '6'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2'}
        };

        std::ofstream out_file(filename);
        if (out_file.is_open())
        {
            for (const auto& row : map)
            {
                for (const auto& c : row)
                {
                    out_file << c << " ";
                }
                out_file << '\n';
            }
            out_file.close();
            std::cerr << "FILE " << filename << " CREATED AND LOADED WITH DEFAULT MAP." << std::endl;
        }
        else
        {
            std::cerr << "ERROR::COULD NOT CREATE " << filename << ". DEFAULT MAP NOT SAVED." << std::endl;
        }
    }

    return map;
}

//SAVE FUNCTIONS
void saveStatusFile(std::string filename, const std::vector<Unit> units)
{
    /*
    Save status file and save the entire units to text - file
    Format:
        Basic:
             player playerGold
             affiliation type id x y resilience if its base save also unit to build
    */
    std::ofstream out_file;

    out_file.open(filename);
    if (out_file.is_open())
    {
        out_file << playerGold << " " << enemyGold << " " << player_remaining_time << " " << enemy_remaining_time <<  std::endl;
        for (auto& unit : units)
        {
            out_file << unit.affiliation << " " << unit.type << " " << unit.id << " " << unit.x << " " << unit.y << " " << unit.resilience;
            if (unit.type == 'B')
                {
                    out_file << " " <<  unit.unitToBuild;
                }
                out_file << std::endl;
        }
    }
    else
        std::cout << "ERROR::SAVESTATUSFILE::Could not save to file::FILENAME: " << filename << std::endl;
    out_file.close();
}
void saveEndGameInfo(std::string filename, int tour)
{
    std::ofstream out_file;
    out_file.open(filename);
    if (out_file.is_open())
    {
        out_file << tour;
    }
    out_file.close();
}

//REGULAR FUNCTIONS
void runPlayerProgram(const std::string& player, const std::string& mapFile, const std::string& statusFile, const std::string& ordersFile, const std::string& timeLimit)
{
    std::string command = player + " " + mapFile + " " + statusFile + " " + ordersFile + " " + timeLimit;

    std::thread programThread([&]() {
        int result = std::system(command.c_str());
if (WIFEXITED(result)) {
    int exitCode = WEXITSTATUS(result);
    if (exitCode != 0) {
        std::cerr << "Błąd: Wywołanie programu gracza zakończyło się niepowodzeniem. Kod wyjścia: " << exitCode << std::endl;
        // Dodaj obsługę błędu, jeśli jest to wymagane
    }
} else {
    std::cerr << "Błąd: Wywołanie programu gracza zakończyło się niepowodzeniem." << std::endl;
    // Dodaj obsługę błędu, jeśli jest to wymagane
}
    });

    programThread.join();
}
void followOrders(std::vector<actionLine> actions, std::vector<Unit>& units)
{
    for(auto& action : actions)
    {
        //FINAL MOVE UNIT TO DESTINATION
        if (action.action == 'M')
        {
            for (auto& unit : units)
            {
                if (unit.id == action.unitID)
                {
                    unit.x = action.x;
                    unit.y = action.y;
                    break;
                }
            }
        }
        else if (action.action == 'A')
        {
            for (auto& unit : units)
            {
                if (unit.id == action.unitIDToAttack)
                {
                    unit.resilience -= getAttackValue(getType(units, action.unitID), getType(units, action.unitIDToAttack));
                    std::cout<< getType(units, action.unitID) << " attacked " << getType(units, action.unitIDToAttack)<< " damage: " << getAttackValue(getType(units, action.unitID), getType(units, action.unitIDToAttack)) << "\n";
                    break;
                }
            }
        }

        //build orders are executed in the player's program as the unit build timer counts down as the game progresses
    }
}
