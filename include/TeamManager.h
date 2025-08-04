#pragma once

#include "Team.h"
#include "User.h"
#include <sqlite3.h>
#include <vector>
#include <memory>
#include <string>
#include <optional>

class TeamManager {
private:
    sqlite3* db_;
    std::string db_path_;

    bool createTeamsTable();
    bool createTeamMembershipsTable();
    bool executeQuery(const std::string& query);

public:
    explicit TeamManager(const std::string& db_path = "teams.db");
    ~TeamManager();

    bool initialize();
    void close();

    // Team operations
    bool createTeam(const std::string& name, const std::string& description, int owner_user_id);
    bool addTeam(const Team& team);
    std::optional<Team> getTeamById(int team_id);
    std::optional<Team> getTeamByName(const std::string& name);
    std::vector<Team> getAllTeams();
    std::vector<Team> getTeamsOwnedByUser(int user_id);
    std::vector<Team> getTeamsForUser(int user_id);
    bool updateTeam(const Team& team);
    bool removeTeam(int team_id);

    // Team membership operations
    bool addUserToTeam(int user_id, int team_id);
    bool removeUserFromTeam(int user_id, int team_id);
    std::vector<int> getTeamMembers(int team_id);
    std::vector<int> getUserTeams(int user_id);
    bool isUserInTeam(int user_id, int team_id);
    bool isUserTeamOwner(int user_id, int team_id);
    int getTeamMemberCount(int team_id);

    // Utility methods
    bool isConnected() const { return db_ != nullptr; }
    int getTeamCount();
    bool clearAllTeams();
    bool teamNameExists(const std::string& name);
    bool transferTeamOwnership(int team_id, int new_owner_user_id);

    static std::string getLastError(sqlite3* db);
};