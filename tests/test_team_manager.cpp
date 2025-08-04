#include "test_framework.h"
#include "../include/TeamManager.h"
#include <filesystem>

void test_team_manager_creation() {
    TeamManager tm("test_teams.db");
    
    ASSERT_TRUE(tm.initialize());
    ASSERT_TRUE(tm.isConnected());
    
    tm.close();
}

void test_team_creation() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    // Clear any existing teams
    tm.clearAllTeams();
    
    ASSERT_TRUE(tm.createTeam("Dev Team", "Software development team", 1));
    ASSERT_EQ(1, tm.getTeamCount());
    
    // Should not be able to create team with same name
    ASSERT_FALSE(tm.createTeam("Dev Team", "Another team", 2));
    
    tm.close();
}

void test_team_retrieval() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    tm.createTeam("Marketing Team", "Marketing and promotion", 1);
    tm.createTeam("QA Team", "Quality Assurance", 2);
    
    // Test get by ID
    auto team1 = tm.getTeamByName("Marketing Team");
    ASSERT_TRUE(team1.has_value());
    ASSERT_STR_EQ("Marketing Team", team1.value().getName());
    ASSERT_EQ(1, team1.value().getOwnerUserId());
    
    // Test get by name
    auto team2 = tm.getTeamByName("QA Team");
    ASSERT_TRUE(team2.has_value());
    ASSERT_STR_EQ("QA Team", team2.value().getName());
    ASSERT_EQ(2, team2.value().getOwnerUserId());
    
    // Test get all teams
    auto all_teams = tm.getAllTeams();
    ASSERT_EQ(2, all_teams.size());
    
    // Test get teams owned by user
    auto user1_teams = tm.getTeamsOwnedByUser(1);
    ASSERT_EQ(1, user1_teams.size());
    ASSERT_STR_EQ("Marketing Team", user1_teams[0].getName());
    
    tm.close();
}

void test_team_membership() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    tm.createTeam("Test Team", "Test team description", 1);
    
    auto team = tm.getTeamByName("Test Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Owner should already be a member
    ASSERT_TRUE(tm.isUserInTeam(1, team_id));
    ASSERT_TRUE(tm.isUserTeamOwner(1, team_id));
    ASSERT_EQ(1, tm.getTeamMemberCount(team_id));
    
    // Add new member
    ASSERT_TRUE(tm.addUserToTeam(2, team_id));
    ASSERT_TRUE(tm.isUserInTeam(2, team_id));
    ASSERT_FALSE(tm.isUserTeamOwner(2, team_id));
    ASSERT_EQ(2, tm.getTeamMemberCount(team_id));
    
    // Add another member
    ASSERT_TRUE(tm.addUserToTeam(3, team_id));
    ASSERT_EQ(3, tm.getTeamMemberCount(team_id));
    
    // Get team members
    auto members = tm.getTeamMembers(team_id);
    ASSERT_EQ(3, members.size());
    
    // Get user teams
    auto user2_teams = tm.getUserTeams(2);
    ASSERT_EQ(1, user2_teams.size());
    ASSERT_EQ(team_id, user2_teams[0]);
    
    // Remove member
    ASSERT_TRUE(tm.removeUserFromTeam(2, team_id));
    ASSERT_FALSE(tm.isUserInTeam(2, team_id));
    ASSERT_EQ(2, tm.getTeamMemberCount(team_id));
    
    tm.close();
}

void test_team_update() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    tm.createTeam("Original Team", "Original description", 1);
    
    auto team = tm.getTeamByName("Original Team");
    ASSERT_TRUE(team.has_value());
    
    // Update team details
    team.value().setName("Updated Team");
    team.value().setDescription("Updated description");
    team.value().setOwnerUserId(2);
    
    ASSERT_TRUE(tm.updateTeam(team.value()));
    
    // Verify update
    auto updated_team = tm.getTeamById(team.value().getId().value());
    ASSERT_TRUE(updated_team.has_value());
    ASSERT_STR_EQ("Updated Team", updated_team.value().getName());
    ASSERT_STR_EQ("Updated description", updated_team.value().getDescription());
    ASSERT_EQ(2, updated_team.value().getOwnerUserId());
    
    tm.close();
}

void test_team_removal() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    tm.createTeam("To Delete", "Team to be deleted", 1);
    
    auto team = tm.getTeamByName("To Delete");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Add some members
    tm.addUserToTeam(2, team_id);
    tm.addUserToTeam(3, team_id);
    ASSERT_EQ(3, tm.getTeamMemberCount(team_id));
    
    // Remove team (should also remove memberships)
    ASSERT_TRUE(tm.removeTeam(team_id));
    
    // Verify team is gone
    auto deleted_team = tm.getTeamById(team_id);
    ASSERT_FALSE(deleted_team.has_value());
    
    // Verify memberships are gone
    ASSERT_EQ(0, tm.getTeamMemberCount(team_id));
    
    tm.close();
}

void test_team_ownership_transfer() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    tm.createTeam("Transfer Team", "Team for ownership transfer", 1);
    
    auto team = tm.getTeamByName("Transfer Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Add new member
    tm.addUserToTeam(2, team_id);
    
    // Transfer ownership to member
    ASSERT_TRUE(tm.transferTeamOwnership(team_id, 2));
    
    // Verify ownership transfer
    ASSERT_FALSE(tm.isUserTeamOwner(1, team_id));
    ASSERT_TRUE(tm.isUserTeamOwner(2, team_id));
    
    auto updated_team = tm.getTeamById(team_id);
    ASSERT_TRUE(updated_team.has_value());
    ASSERT_EQ(2, updated_team.value().getOwnerUserId());
    
    // Should fail to transfer to non-member
    ASSERT_FALSE(tm.transferTeamOwnership(team_id, 5));
    
    tm.close();
}

void test_teams_for_user() {
    TeamManager tm("test_teams.db");
    tm.initialize();
    
    tm.clearAllTeams();
    
    // Create teams with different owners
    tm.createTeam("Team 1", "First team", 1);
    tm.createTeam("Team 2", "Second team", 2);
    tm.createTeam("Team 3", "Third team", 3);
    
    // Get team IDs
    auto team1 = tm.getTeamByName("Team 1");
    auto team2 = tm.getTeamByName("Team 2");
    auto team3 = tm.getTeamByName("Team 3");
    
    ASSERT_TRUE(team1.has_value());
    ASSERT_TRUE(team2.has_value());
    ASSERT_TRUE(team3.has_value());
    
    int team1_id = team1.value().getId().value();
    int team2_id = team2.value().getId().value();
    int team3_id = team3.value().getId().value();
    
    // Add user 1 to team 2 and team 3
    tm.addUserToTeam(1, team2_id);
    tm.addUserToTeam(1, team3_id);
    
    // User 1 should be in 3 teams total (owns 1, member of 2)
    auto user1_teams = tm.getTeamsForUser(1);
    ASSERT_EQ(3, user1_teams.size());
    
    // User 2 should be in 1 team (owns 1)
    auto user2_teams = tm.getTeamsForUser(2);
    ASSERT_EQ(1, user2_teams.size());
    
    tm.close();
}

void run_team_manager_tests() {
    std::cout << "Running Team Manager tests..." << std::endl;
    
    // Clean up test database
    if (std::filesystem::exists("test_teams.db")) {
        std::filesystem::remove("test_teams.db");
    }
    
    test_team_manager_creation();
    test_team_creation();
    test_team_retrieval();
    test_team_membership();
    test_team_update();
    test_team_removal();
    test_team_ownership_transfer();
    test_teams_for_user();
    
    // Clean up
    if (std::filesystem::exists("test_teams.db")) {
        std::filesystem::remove("test_teams.db");
    }
    
    std::cout << "All Team Manager tests passed!" << std::endl;
}