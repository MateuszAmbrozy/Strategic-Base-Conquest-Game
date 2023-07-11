#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <future>
#include <chrono>
#include <thread>
//CLASSES AND STRUCTURES DECLARATIONS
struct actionLine
{
    int unitID = 0;
    char action = '0';
    int x = 0, y = 0;
    int unitIDToAttack = 0;
    char unitToBuild = '0';
};
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

//Stopper for base - counting time to create units
class Stopper;

//GLOBAL VARIABLE
long gold = 0;
double player1_remaining_time = 0;
double player2_remaining_time = 0;

//FUNCTION DECLARATION

//ACCESSORS
std::vector<int> getAllID(std::vector<Unit> units);
std::vector<std::pair<int, int>> getAllEnemyPositionsUnits(std::vector<Unit> units);
int getDistanceOfEntinty(const int id, std::vector<Unit>& units);
int getResilienceOfEntity(const int id, std::vector<Unit> units);
char getTypeOfEntity(const int id, std::vector<Unit> units);
std::pair<int, int> getPositionOfStructOnMap(char num, std::vector<std::vector<char>> map);
int getMaxUnitID(const std::vector<Unit> units);
int GetNumberWorkersInMine(std::vector<Unit> units, std::vector<std::vector<char>> map);
int getAttackRangeOfEntity(const int id, std::vector<Unit> units);
std::pair<int, int> getPositionOfUnitOfID(const int id, std::vector<Unit> units);

//ORDERS FUNCTIONS
void getAction(std::vector<std::vector<char>> map, std::vector<Unit>& units, std::vector<actionLine>& actions);

bool isEntityFriendly(std::vector<Unit> units, const int id);
bool isIDExistent(std::vector<Unit> units, const int id);
bool isAttackCorrect(std::vector<Unit>& units, const int enemyId, const int friendlyId);
bool isMoveCorrect(std::vector<std::vector<char>> map, std::vector<Unit> units, const int newPosX, const int newPosY, const int id);
bool isCreatingCorrect(std::vector<Unit>& units, const char type, const int id);

//REGULAR FUNCTIONS
Unit specifyUnit(std::vector<Unit>& units, char type, char affilation = 'P');
void showMap(const std::vector<std::vector<char>>& map);

//LOADING FUNCTIONS
std::vector<std::vector<char>> loadMapFile(const std::string& filename);
void loadStatusFile(const std::string& filename, std::vector<Unit>& units, std::vector<std::vector<char>> map);
double loadAuxiliaryFile(const std::string& filename);

//SAVE FUNCTIONS
void saveStatusFile(const std::string filename, const std::vector<Unit> units);
void saveActionsFile(const std::string& filename, const std::vector<actionLine> actions);
void saveAuciliaryFile(const std::string& filename, double value);

class Stopper 
{
private:
    std::chrono::steady_clock::time_point start;
    std::chrono::duration<double> remaining_time;
    std::chrono::duration<double> elapsed;
    bool running;
    char type;
    const int baseID;
public:
    Stopper(const int baseID) : running(false), baseID(baseID) {}

    void startTimer(double time) 
    {
        start = std::chrono::steady_clock::now();
        remaining_time = std::chrono::duration<double>(time);
        running = true;
    }

    bool isRunning() 
    {
        return running;
    }

    void update(std::vector<Unit>& units, std::vector<std::vector<char>> map) 
    {
        if (running) 
        {
            std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
            elapsed = currentTime - start;

            if (remaining_time.count() <= elapsed.count())  //end of the building time (stop timer, create unit, set building to false, set unitToBuild to 0)
            {
                stopTimer();
                Unit unit = specifyUnit(units, units[baseID].unitToBuild, units[baseID].affiliation);
                unit.x = units[baseID].x;
                unit.y = units[baseID].y;

                units.push_back(unit);
                units[baseID].building = false;
                units[baseID].unitToBuild = '0';
                if(baseID == 0)
                {                
                    player1_remaining_time = 0;
                }
                else if(baseID == 1)
                {                
                    player2_remaining_time = 0;
                }
                    
            }
        }
    }

    void stopTimer() 
    {
        running = false;
        remaining_time = std::chrono::duration<double>(0.0);
    }

    double getRemainingTime() 
    {
        return elapsed.count();
    }
};

//MAIN FUNCITON
int main(int argc, char* argv[])
{

    std::string mapFile = argv[1];
    std::string statusFile = argv[2];
    std::string ordersFile = argv[3];
    std::string stringTime = argv[4];
    int time;

     try
    {
        time = std::stoi(stringTime);
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Invalid :  " << stringTime << "\n";
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "Coordinates out of range: " << stringTime << "\n";
    }

    std::vector<std::vector<char>> map = loadMapFile(mapFile);
    showMap(map);
    std::vector<Unit> units;
    std::vector<actionLine> actions;
    loadStatusFile(statusFile, units, map);
    player1_remaining_time = loadAuxiliaryFile("Player.txt");
    player2_remaining_time = loadAuxiliaryFile("Enemy.txt");

    Stopper playerStopper(0);
    Stopper enemyStopper(1);

    //start timer if base has unit to build
    if(units[0].unitToBuild != '0')
        playerStopper.startTimer(player1_remaining_time);
    if(units[1].unitToBuild != '0')
        enemyStopper.startTimer(player2_remaining_time);

    std::chrono::steady_clock::time_point program_start = std::chrono::steady_clock::now();

    // start a thread that will terminate the program after 5 seconds
    std::future<void> timer_future = std::async(std::launch::async, [&]()
        {
            std::this_thread::sleep_for(std::chrono::seconds(time));
            if (playerStopper.isRunning())
                player1_remaining_time = playerStopper.getRemainingTime();
            if (enemyStopper.isRunning())
                player2_remaining_time = enemyStopper.getRemainingTime();

            gold += GetNumberWorkersInMine(units, map) * 60;
            saveStatusFile("status.txt", units);
            saveActionsFile("rozkazy.txt", actions);
            saveAuciliaryFile("Player.txt", player1_remaining_time);
            saveAuciliaryFile("Enemy.txt", player2_remaining_time);

            std::exit(0);
        });
        
        std::future<void> build_future = std::async(std::launch::async, [&]()
        {
            for(;;)
            {         // Aktualizacja stopera
                if(units[0].unitToBuild != '0')
                    playerStopper.update(units, map);
                if(units[1].unitToBuild != '0')
                   enemyStopper.update(units, map);
            }
        });





    //executing the command fetching loop after 5 seconds the program will terminate itself by the thread
    while (true) 
    {
        getAction(map, units, actions);
    }
    return 0;
}

//ACCESSORS

std::vector<int> getAllID(std::vector<Unit> units)
{
    /*
    RETURNS VECTOR IDS OF ALL ENTITIES
    */
    std::vector<int> ids;

    for (auto& unit : units) 
    {
        ids.push_back(unit.id);
    }
    return ids;
}
std::vector<std::pair<int, int>> getAllEnemyPositionsUnits(std::vector<Unit> units)
{
    std::vector<std::pair<int, int>> enemyPosition;
    for (auto& unit : units)
    {
        if (unit.affiliation == 'E')
            enemyPosition.push_back(std::make_pair(unit.x, unit.y));
    }
    return enemyPosition;

}
int getDistanceOfEntinty(const int id, std::vector<Unit>& units)
{
    for (const auto& unit : units)
    {
        if (unit.id == id)
        {
            return unit.distance;
        }
    }
    return -1;
}
int getResilienceOfEntity(const int id, std::vector<Unit> units)
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
char getTypeOfEntity(const int id, std::vector<Unit> units)
{
    for (const auto& unit : units)
    {
        if (unit.id == id)
        {
            return unit.type;
        }
    }
    return '0';
}
std::pair<int, int> getPositionOfStructOnMap(const char num, std::vector<std::vector<char>> map)
{
    /*
    RETURN POSITION OF ENTERED NUM
    */
    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map[i].size(); j++)
        {
            if (map[i][j] == num)
            {
                return std::make_pair(i, j);
            }
        }
    }

    // IF THERE IS NOT STRUCT OF ENTERED ID RETURN (-1, -1)
    return std::make_pair(-1, -1);
}
int getMaxUnitID(const std::vector<Unit> units) 
{
    int maxID = -1;

    for (const Unit& unit : units) {
        if (unit.id > maxID) {
            maxID = unit.id;
        }
    }

    return maxID + 1;
}
int GetNumberWorkersInMine(std::vector<Unit> units, std::vector<std::vector<char>> map)
{
    /*
    GET ALL FRIENDLY WORKERS IN MINER, WHO HAVE POSITIVE RESILIENCE 
    */
    int workersInMine = 0;
    for (const auto& unit : units)
    {
        if (
            unit.affiliation == 'P' && getTypeOfEntity(unit.id, units) == 'W' &&
            unit.x == getPositionOfStructOnMap('6', map).first && unit.y == getPositionOfStructOnMap('6', map).second &&
            getResilienceOfEntity(unit.id, units) > 0
            )
        {
            ++workersInMine;
        }
    }
    return workersInMine;
}
int getAttackRangeOfEntity(const int id, std::vector<Unit> units)
{
    for (const auto& unit : units)
    {
        if (unit.id == id)
        {
            return unit.attackRange;
        }
    }
    return 0;
}
std::pair<int, int> getPositionOfUnitOfID(const int id, std::vector<Unit> units)
{
    for (const auto& unit : units)
    {
        if (unit.id == id)
        {
            return std::make_pair(unit.x, unit.y);
        }
    }
    return std::make_pair(-1, -1);
}

//ORDERS FUNCTIONS
void getAction(std::vector<std::vector<char>> map, std::vector<Unit>& units,std::vector<actionLine>& actions)
{
    /*
    MAIN FUNCTION TO GET ACTION AND CHECK IF ACTION IS CORRECT
    */
    std::string line;
    bool error = false;
    std::cout << "Pass comand: \n";

    std::getline(std::cin, line);
    
    std::istringstream iss(line);
    actionLine actionLine;
    std::string stringUnitID;
    iss >> stringUnitID;
    try
    {
        actionLine.unitID = std::stoi(stringUnitID);
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Invalid :  " << stringUnitID << "\n";
        error = true;
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "Coordinates out of range: " << stringUnitID << "\n";
        error = true;
    }
    if (!isIDExistent(units, actionLine.unitID)) //checks if there is entity of entered id
    {
        error = true;
        std::cout << "ERROR::COULD NOT FIND ENTITY OF ID: " << actionLine.unitID << std::endl;
    }

    if (isEntityFriendly(units, actionLine.unitID))
    {
        iss >> actionLine.action;
        
            //MOVE
            if (actionLine.action == 'M')
            {
                std::string xString, yString;
                iss >> xString >> yString;
                try
                {
                    actionLine.x = std::stoi(xString);
                    actionLine.y = std::stoi(yString);
                }
                catch (const std::invalid_argument& e)
                {
                    std::cout << "Invalid coordinates: " << xString << ", " << yString << "\n";
                    error = true;
                }
                catch (const std::out_of_range& e)
                {
                    std::cout << "Coordinates out of range: " << xString << ", " << yString << "\n";
                    error = true;
                }
                if (!isMoveCorrect(map, units, actionLine.x, actionLine.y, actionLine.unitID))
                {
                    error = true;
                    std::cout << "ERROR::COULD NOT MOVE TO THIS POSITION" << std::endl;
                }
            }
            //ATTACK
            else if (actionLine.action == 'A')
            {
                std::string idString;
                iss >> idString;
                try
                {
                    actionLine.unitIDToAttack = std::stoi(idString);
                }
                catch (const std::invalid_argument& e)
                {
                    std::cout << "Invalid id: " << idString << "\n";
                    error = true;
                }
                catch (const std::out_of_range& e)
                {
                    std::cout << "Id out of range: " << idString << "\n";
                    error = true;
                }

                if (!isIDExistent(units, actionLine.unitIDToAttack) || (!isAttackCorrect(units, actionLine.unitID, actionLine.unitIDToAttack)))
                {
                    error = true;
                    std::cout << "ERROR::COULD NOT ATTACK ENTITY OF ID: " << actionLine.unitIDToAttack << std::endl;
                }


            }
            //BUILD
            else if (actionLine.action == 'B')
            {

                iss >> actionLine.unitToBuild;
                if (!isCreatingCorrect(units, actionLine.unitToBuild, actionLine.unitID))
                {
                    error = true;
                    std::cout << "ERROR::COULD NOT BUILD\n";
                }
            }
            else
            {
                std::cout << "Incorrect order\n";
                error = true;
            }
        }
        if (!error)
            actions.push_back(actionLine);
}

bool isEntityFriendly(std::vector<Unit> units, const int id)
{
    /*
    CHECK IF ENTITY OF ENTERED ID IS FRIENDLY. RETURN TRUE IF IT IS AND IN THE OPPOSITE CASE RETURN FALSE
    */
    for (const auto& unit : units)
    {
        if (unit.id == id && unit.affiliation == 'P')
        {
            return true;
        }
    }
    return false;
}
bool isIDExistent(std::vector<Unit> units, const int id)
{
    /*
    CHECKS IF ENTITY OF GIVEN ID 
    */
    for (auto& statusID : getAllID(units))
    {
        if (id == statusID)
            return true;
    }
    return false;
}
bool isMoveCorrect(std::vector<std::vector<char>> map, std::vector<Unit> units, const int newPosX, const int newPosY, const int id)
{
    if (newPosX < 0 || newPosX >= map[0].size() ||     //warunek sprawdzaj�cy czy nowa pozycja jednostki nie wychodzi poza map�
        newPosY < 0 || newPosY >= map.size() ||
        map[newPosX][newPosY] == '9' || //player2 = baza drugiego programu gracza = 2. Warunek sprawdza czy nowa pozycja jednostki jest dozwolona
        map[newPosX][newPosY] == '2' ||
        std::abs(getPositionOfUnitOfID(id, units).first - newPosX) + std::abs(getPositionOfUnitOfID(id, units).second - newPosY) > getDistanceOfEntinty(id, units) || 
        getResilienceOfEntity(id, units) < 0 ||
        getTypeOfEntity(id, units) == 'B')
    {
        return false;
    }

    for (int i = 0; i < getAllEnemyPositionsUnits(units).size(); i++)
        if(newPosX == getAllEnemyPositionsUnits(units)[i].first && //warunek sprawdza czy nowa pozycja jednostki nie jest zaj�ta przez jednost� przeciwnika
            newPosY == getAllEnemyPositionsUnits(units)[i].second)
        {
            return false;
        }

    //update distance after move
    for (auto& unit : units)
    {
        if (unit.id == id)
        {
            unit.distance -= std::abs(getPositionOfUnitOfID(id, units).first - newPosX) + std::abs(getPositionOfUnitOfID(id, units).second - newPosY);
            return true;
        }
    }
    return true;
    
}
bool isAttackCorrect(std::vector<Unit>& units, const int friendlyId, const int enemyId)
{
    /*
    THIS FUNCTION CHECKS MAKE A DECISSION IF ATTACK IS CORRECT
    - entity cannot friendly entities, 
    - entity's attack range is bigger than distance
    - entity cannot attack 2 times in one tour
    - entity cannot attack died entity

    UPDATE OF SPEED AFTER ATTACK 
    */
    if (std::abs(getPositionOfUnitOfID(enemyId, units).first - getPositionOfUnitOfID(friendlyId, units).first) + std::abs(getPositionOfUnitOfID(enemyId, units).second - getPositionOfUnitOfID(friendlyId, units).second) > getAttackRangeOfEntity(friendlyId, units) ||
        getResilienceOfEntity(enemyId, units) <= 0 || 
        getTypeOfEntity(friendlyId, units) == 'B')
        return false;

    //check if unit attacked in this tour
    for (auto& unit : units)
    {
        if (unit.id == friendlyId && unit.attacked == true)
        {
            return false;
        }
    }

    //update Distance and attacked variable(1 attack per tour)
    for (auto& unit : units)
    {
        if (unit.id == friendlyId)
        {
            unit.distance -= 1;
            unit.attacked = true;
        }
    }

    return true;
}
bool isCreatingCorrect(std::vector<Unit>& units, const char type, const int id)
{
    /*FINAL FUNCTIONS CHECKS IF BUILDING IS CORRECT
    */
        
    if (units[0].building == true || gold < specifyUnit(units, type).cost || getTypeOfEntity(id, units) != 'B' || type == 'B')
    {
        return false;
    }
    else
    {
        gold -= specifyUnit(units, type).cost;
        units[0].building = true;
        units[0].unitToBuild = type;
        player1_remaining_time = specifyUnit(units, type).builindTime;
        return true;
    }
    return true;
}

//REGULAR FUNCTIONS
Unit specifyUnit(std::vector<Unit>& units, const char type, const char affilation)
{
    if ((type == 'K' || type == 'S' || type == 'A' || type == 'P' || type == 'R' || type == 'C' || type == 'W') && 
        (affilation == 'P' || affilation == 'E'))
    {
        Unit unit;
        unit.id = getMaxUnitID(units);
        unit.affiliation = affilation;
        unit.type = type;
        if (type == 'K')
        {
            unit.resilience = 70;
            unit.distance = 5;
            unit.cost = 400;
            unit.attackRange = 1;
            unit.builindTime = 5;
        }
        else if (type == 'S')
        {
            unit.resilience = 60;
            unit.distance = 2;
            unit.cost = 250;
            unit.attackRange = 1;
            unit.builindTime = 3;
        }
        else if (type == 'A')
        {
            unit.resilience = 40;
            unit.distance = 2;
            unit.cost = 250;
            unit.attackRange = 5;
            unit.builindTime = 3;
        }
        else if (type == 'P')
        {
            unit.resilience = 50;
            unit.distance = 2;
            unit.cost = 200;
            unit.attackRange = 2;
            unit.builindTime = 3;
        }
        else if (type == 'R')
        {
            unit.resilience = 90;
            unit.distance = 2;
            unit.cost = 500;
            unit.attackRange = 1;
            unit.builindTime = 4;
        }
        else  if (type == 'C')
        {
            unit.resilience = 50;
            unit.distance = 2;
            unit.cost = 800;
            unit.attackRange = 7;
            unit.builindTime = 6;
        }
        else if (type == 'W')
        {
            unit.resilience = 20;
            unit.distance = 2;
            unit.cost = 100;
            unit.attackRange = 1;
            unit.builindTime = 2;
        }
        return unit;
    }
    return Unit{-1, 0, 0, 0, 0, 0, 0, 0};
}
void showMap(const std::vector<std::vector<char>>& map)
{
    /*
    PRINT ALL MAP (WITHOUT UNITS)
    */
    std::cout<<"\tMAPA\n";
    for (const auto& row : map)
    {
        for (const auto& element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

//LOAD FUNCTIONS
std::vector<std::vector<char>> loadMapFile(const std::string& filename)
{
    /*
    LOAD MAP FILE TO VECTOR2D OF CHARS, IF MAP FILE NOT EXISTS MAKE DEFAULT FILE
    */
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
    }
    return map;
}
void loadStatusFile(const std::string& filename, std::vector<Unit>& units, std::vector<std::vector<char>> map)
{
    /*
    LOAD STATUS OF ALL UNITS TO VECTOR, THIS VECTOR HELPS TO MANAGE OF UNITS
    */
    std::ifstream in_file(filename);

    if (in_file.is_open())
    {
        Unit unit;
        in_file >> gold;
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
    else
    {
        Unit unit;
        std::cerr << "ERROR::COULD NOT OPEN FILE " << filename << ". MAKING DEFAULT STATUS FILE... " << std::endl;
        gold = 2000;
        unit.affiliation = 'P';
        unit.type = 'B';
        unit.id = 0;
        unit.x = getPositionOfStructOnMap('1', map).first;
        unit.y = getPositionOfStructOnMap('1', map).second;
        unit.resilience = 200;
        unit.unitToBuild = '0';
        units.push_back(unit);
        unit.id = 1;
        unit.affiliation = 'E';
        unit.x = getPositionOfStructOnMap('2', map).first;
        unit.y = getPositionOfStructOnMap('2', map).second;
        units.push_back(unit);
    }

    
}
double loadAuxiliaryFile(const std::string& filename)
{
    std::ifstream file(filename);
    double value = 0.0;

    if (file.is_open())
    {
        file >> value;
        file.close();
    }
    else
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
    }

    return value;
}
//SAVE FUNCTIONS
void saveStatusFile(std::string filename, const std::vector<Unit> units)
{
    /*
    Save status file and save the entire units to text - file
    Format:
        Basic:
             player playerGold TIME TO BUILD PLAYER'S UNIT, TIME TO BUILD ENEMY UNIT
             affiliation type id x y resilience if its base save also unit to build
    */
    std::ofstream out_file;

    out_file.open(filename);
    if (out_file.is_open())
    {
        out_file << gold << std::endl;
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
        std::cout << "ERROR::SAVESTATUS::Could not save to file::FILENAME: " << filename << std::endl;
    out_file.close();
}
void saveActionsFile(const std::string& filename, const std::vector<actionLine> actions)
{
    /*
    Save action file and save the all correct actions to text - file
    Format:
        Basic:
            unitID action
            Move - x, y
            Attack - unit ID To Attack
            Build - char of unit To Build
    */
    std::ofstream out_file;

    out_file.open(filename);
    if (out_file.is_open())
    {
        for (auto& action : actions)
        {
            out_file << action.unitID << " " << action.action << " ";
            if (action.action == 'M')
            {
                std::cout << action.x << " " << action.y << std::endl;
                out_file << action.x << " " << action.y << " ";
            }
            else if (action.action == 'A')
            {
                std::cout << action.unitIDToAttack << " ";
                out_file << action.unitIDToAttack << " ";
            }
            else if (action.action == 'B')
            {
                std::cout << action.unitToBuild << " ";
                out_file << action.unitToBuild << " ";
            }
            else
                std::cout << "ERROR::COLD NOT SAVE ACTIONS\n";
        }
    }
    else
        std::cout << "ERROR::SAVEACTIONS::Could not save to file::FILENAME: " << filename << std::endl;
    out_file.close();
}
void saveAuciliaryFile(const std::string& filename, double value)
{
    std::ofstream file(filename);

    if (file.is_open())
    {
        file << value;
        file.close();
    }
    else
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
    }
}