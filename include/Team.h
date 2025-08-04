#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <vector>

class Team {
private:
    std::optional<int> id_;
    std::string name_;
    std::string description_;
    int owner_user_id_;
    std::chrono::system_clock::time_point created_at_;

public:
    Team() = default;
    Team(const std::string& name, const std::string& description, int owner_user_id);
    Team(int id, const std::string& name, const std::string& description, 
         int owner_user_id, const std::chrono::system_clock::time_point& created_at);

    // Getters
    std::optional<int> getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    int getOwnerUserId() const { return owner_user_id_; }
    const std::chrono::system_clock::time_point& getCreatedAt() const { return created_at_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setOwnerUserId(int owner_user_id) { owner_user_id_ = owner_user_id; }
    void setCreatedAt(const std::chrono::system_clock::time_point& created_at) { created_at_ = created_at; }

    // Utility methods
    std::string toString() const;
    std::string getCreatedAtString() const;
    
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
};