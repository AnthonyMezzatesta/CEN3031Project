#include "TeamManager.h"
#include <iostream>
#include <ctime>

// Constructor and destructor
TeamManager::TeamManager(const std::string& db_path) : db_(nullptr), db_path_(db_path) {}

TeamManager::~TeamManager() {
    close();
}

// Database operations
bool TeamManager::initialize() {
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open team database: " << sqlite3_errmsg(db_) << std::endl;
        close();
        return false;
    }
    
    return createTeamsTable() && createTeamMembershipsTable();
}

void TeamManager::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool TeamManager::createTeamsTable() {
    const std::string create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS teams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            description TEXT NOT NULL,
            owner_user_id INTEGER NOT NULL,
            created_at INTEGER NOT NULL
        );
    )";
    
    return executeQuery(create_table_sql);
}

bool TeamManager::createTeamMembershipsTable() {
    const std::string create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS team_memberships (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            team_id INTEGER NOT NULL,
            joined_at INTEGER NOT NULL,
            UNIQUE(user_id, team_id)
        );
    )";
    
    return executeQuery(create_table_sql);
}

bool TeamManager::executeQuery(const std::string& query) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    
    return true;
}

// Team operations
bool TeamManager::createTeam(const std::string& name, const std::string& description, int owner_user_id) {
    if (teamNameExists(name)) {
        std::cerr << "Team name already exists: " << name << std::endl;
        return false;
    }
    
    Team new_team(name, description, owner_user_id);
    bool team_added = addTeam(new_team);
    
    if (team_added) {
        // Get the team ID and add the owner as a member
        auto team = getTeamByName(name);
        if (team.has_value()) {
            addUserToTeam(owner_user_id, team.value().getId().value());
        }
    }
    
    return team_added;
}

bool TeamManager::addTeam(const Team& team) {
    const std::string sql = "INSERT INTO teams (name, description, owner_user_id, created_at) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, team.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, team.getDescription().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, team.getOwnerUserId());
    
    auto created_time_t = std::chrono::system_clock::to_time_t(team.getCreatedAt());
    sqlite3_bind_int64(stmt, 4, created_time_t);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::optional<Team> TeamManager::getTeamById(int team_id) {
    const std::string sql = "SELECT id, name, description, owner_user_id, created_at FROM teams WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int owner_user_id = sqlite3_column_int(stmt, 3);
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 4);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        sqlite3_finalize(stmt);
        return Team(id, name, description, owner_user_id, created_at);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<Team> TeamManager::getTeamByName(const std::string& name) {
    const std::string sql = "SELECT id, name, description, owner_user_id, created_at FROM teams WHERE name = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string team_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int owner_user_id = sqlite3_column_int(stmt, 3);
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 4);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        sqlite3_finalize(stmt);
        return Team(id, team_name, description, owner_user_id, created_at);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<Team> TeamManager::getAllTeams() {
    const std::string sql = "SELECT id, name, description, owner_user_id, created_at FROM teams ORDER BY created_at ASC";
    sqlite3_stmt* stmt;
    std::vector<Team> teams;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return teams;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int owner_user_id = sqlite3_column_int(stmt, 3);
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 4);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        teams.emplace_back(id, name, description, owner_user_id, created_at);
    }
    
    sqlite3_finalize(stmt);
    return teams;
}

std::vector<Team> TeamManager::getTeamsOwnedByUser(int user_id) {
    const std::string sql = "SELECT id, name, description, owner_user_id, created_at FROM teams WHERE owner_user_id = ? ORDER BY created_at ASC";
    sqlite3_stmt* stmt;
    std::vector<Team> teams;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return teams;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int owner_user_id = sqlite3_column_int(stmt, 3);
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 4);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        teams.emplace_back(id, name, description, owner_user_id, created_at);
    }
    
    sqlite3_finalize(stmt);
    return teams;
}

std::vector<Team> TeamManager::getTeamsForUser(int user_id) {
    const std::string sql = R"(
        SELECT t.id, t.name, t.description, t.owner_user_id, t.created_at 
        FROM teams t 
        INNER JOIN team_memberships tm ON t.id = tm.team_id 
        WHERE tm.user_id = ? 
        ORDER BY t.created_at ASC
    )";
    sqlite3_stmt* stmt;
    std::vector<Team> teams;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return teams;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int owner_user_id = sqlite3_column_int(stmt, 3);
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 4);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        teams.emplace_back(id, name, description, owner_user_id, created_at);
    }
    
    sqlite3_finalize(stmt);
    return teams;
}

bool TeamManager::updateTeam(const Team& team) {
    if (!team.getId().has_value()) {
        std::cerr << "Cannot update team without ID" << std::endl;
        return false;
    }
    
    const std::string sql = "UPDATE teams SET name = ?, description = ?, owner_user_id = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, team.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, team.getDescription().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, team.getOwnerUserId());
    sqlite3_bind_int(stmt, 4, team.getId().value());
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool TeamManager::removeTeam(int team_id) {
    // First remove all team memberships
    const std::string remove_memberships_sql = "DELETE FROM team_memberships WHERE team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, remove_memberships_sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to remove team memberships" << std::endl;
        return false;
    }
    
    // Then remove the team
    const std::string remove_team_sql = "DELETE FROM teams WHERE id = ?";
    
    rc = sqlite3_prepare_v2(db_, remove_team_sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// Team membership operations
bool TeamManager::addUserToTeam(int user_id, int team_id) {
    if (isUserInTeam(user_id, team_id)) {
        std::cout << "User " << user_id << " is already in team " << team_id << std::endl;
        return true; // Already in team, consider it successful
    }
    
    const std::string sql = "INSERT INTO team_memberships (user_id, team_id, joined_at) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    auto joined_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    sqlite3_bind_int64(stmt, 3, joined_time_t);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool TeamManager::removeUserFromTeam(int user_id, int team_id) {
    const std::string sql = "DELETE FROM team_memberships WHERE user_id = ? AND team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

std::vector<int> TeamManager::getTeamMembers(int team_id) {
    const std::string sql = "SELECT user_id FROM team_memberships WHERE team_id = ? ORDER BY joined_at ASC";
    sqlite3_stmt* stmt;
    std::vector<int> members;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return members;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int user_id = sqlite3_column_int(stmt, 0);
        members.push_back(user_id);
    }
    
    sqlite3_finalize(stmt);
    return members;
}

std::vector<int> TeamManager::getUserTeams(int user_id) {
    const std::string sql = "SELECT team_id FROM team_memberships WHERE user_id = ? ORDER BY joined_at ASC";
    sqlite3_stmt* stmt;
    std::vector<int> teams;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return teams;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int team_id = sqlite3_column_int(stmt, 0);
        teams.push_back(team_id);
    }
    
    sqlite3_finalize(stmt);
    return teams;
}

bool TeamManager::isUserInTeam(int user_id, int team_id) {
    const std::string sql = "SELECT COUNT(*) FROM team_memberships WHERE user_id = ? AND team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0;
}

bool TeamManager::isUserTeamOwner(int user_id, int team_id) {
    const std::string sql = "SELECT COUNT(*) FROM teams WHERE id = ? AND owner_user_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    sqlite3_bind_int(stmt, 2, user_id);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0;
}

int TeamManager::getTeamMemberCount(int team_id) {
    const std::string sql = "SELECT COUNT(*) FROM team_memberships WHERE team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, team_id);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

// Utility methods
int TeamManager::getTeamCount() {
    const std::string sql = "SELECT COUNT(*) FROM teams";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool TeamManager::clearAllTeams() {
    // Clear memberships first
    if (!executeQuery("DELETE FROM team_memberships")) {
        return false;
    }
    
    // Then clear teams
    return executeQuery("DELETE FROM teams");
}

bool TeamManager::teamNameExists(const std::string& name) {
    const std::string sql = "SELECT COUNT(*) FROM teams WHERE name = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0;
}

bool TeamManager::transferTeamOwnership(int team_id, int new_owner_user_id) {
    // First check if the team exists and if the new owner is a member
    if (!isUserInTeam(new_owner_user_id, team_id)) {
        std::cerr << "Cannot transfer ownership: new owner is not a team member" << std::endl;
        return false;
    }
    
    const std::string sql = "UPDATE teams SET owner_user_id = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, new_owner_user_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// Static error handling
std::string TeamManager::getLastError(sqlite3* db) {
    return std::string(sqlite3_errmsg(db));
}