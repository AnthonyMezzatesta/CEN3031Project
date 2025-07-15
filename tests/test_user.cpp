#include "test_framework.h"
#include "../include/User.h"
#include <chrono>

void test_user_creation() {
    User user("test@example.com", "hashed_password");
    
    ASSERT_STR_EQ("test@example.com", user.getEmail());
    ASSERT_STR_EQ("hashed_password", user.getPasswordHash());
    ASSERT_FALSE(user.getId().has_value());
}

void test_user_with_id() {
    auto now = std::chrono::system_clock::now();
    User user(1, "test@example.com", "hashed_password", now);
    
    ASSERT_TRUE(user.getId().has_value());
    ASSERT_EQ(1, user.getId().value());
    ASSERT_STR_EQ("test@example.com", user.getEmail());
}

void test_user_setters() {
    User user;
    auto now = std::chrono::system_clock::now();
    
    user.setId(42);
    user.setEmail("new@example.com");
    user.setPasswordHash("new_hash");
    user.setCreatedAt(now);
    
    ASSERT_EQ(42, user.getId().value());
    ASSERT_STR_EQ("new@example.com", user.getEmail());
    ASSERT_STR_EQ("new_hash", user.getPasswordHash());
}

void test_password_hashing() {
    std::string password = "testpassword123";
    std::string hash1 = User::hashPassword(password);
    std::string hash2 = User::hashPassword(password);
    
    // Hash should be consistent
    ASSERT_STR_EQ(hash1, hash2);
    
    // Password verification should work
    ASSERT_TRUE(User::verifyPassword(password, hash1));
    ASSERT_FALSE(User::verifyPassword("wrongpassword", hash1));
}

void test_user_to_string() {
    User user(1, "test@example.com", "hash", std::chrono::system_clock::now());
    std::string str = user.toString();
    
    // Should contain email in string representation
    ASSERT_TRUE(str.find("test@example.com") != std::string::npos);
}

void run_user_tests() {
    std::cout << "\n=== Running User Tests ===" << std::endl;
    
    RUN_TEST(test_user_creation);
    RUN_TEST(test_user_with_id);
    RUN_TEST(test_user_setters);
    RUN_TEST(test_password_hashing);
    RUN_TEST(test_user_to_string);
} 