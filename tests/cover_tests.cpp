#include <gtest/gtest.h>
#include <boolean/cover.h>
#include <boolean/cube.h>

using namespace boolean;

// Test cover construction and basic properties
TEST(CoverTest, Construction) {
    // Create an empty cover
    cover empty_cover;
    EXPECT_TRUE(empty_cover.is_null());
    EXPECT_EQ(empty_cover.size(), 0);
    
    // Create a cover from a cube
    cube a(0, 1);  // Variable 0 is true
    cover c1;
    c1.push_back(a);
    EXPECT_FALSE(c1.is_null());
    EXPECT_EQ(c1.size(), 1);
    
    // Create a cover with multiple cubes
    cube b(1, 0);  // Variable 1 is false
    cover c2;
    c2.push_back(a);
    c2.push_back(b);
    EXPECT_EQ(c2.size(), 2);
}

// Test cover operations (union, intersection)
TEST(CoverTest, CoverOperations) {
    // Create test cubes
    cube a(0, 1);  // x
    cube b(1, 0);  // ~y
    cube c(2, 1);  // z
    
    // Create test covers
    cover f1, f2;
    f1.push_back(a & b);  // x & ~y
    f2.push_back(b & c);  // ~y & z
    
    // Test union operation
    cover result = f1 | f2;
    EXPECT_EQ(result.size(), 2);  // Should contain both cubes
    
    // Test intersection
    cube common(1, 0);  // ~y is common to both
    cover f3;
    f3.push_back(common);
    
    cover intersection = f1 & f3;
    EXPECT_FALSE(intersection.is_null());  // Should contain ~y
}

// Test cover operations with complementation
TEST(CoverTest, Complementation) {
    // Create a cover representing x
    cube a(0, 1);
    cover f1;
    f1.push_back(a);
    
    // Complement should be ~x
    cover complement = ~f1;
    EXPECT_FALSE(complement.is_null());
    
    // Double complementation should get us back to original
    cover double_complement = ~complement;
    EXPECT_EQ(double_complement.size(), f1.size());
    
    // Create a tautology cover
    cover tautology;
    cube taut_cube;  // Empty cube is a tautology
    tautology.push_back(taut_cube);
    
    // Complementing a tautology should give an empty cover (representing false)
    cover compl_taut = ~tautology;
    EXPECT_TRUE(compl_taut.is_null());
}

// Test cover minimization
TEST(CoverTest, Minimization) {
    // Create a cover with redundant cubes
    // For example: (x & y) + (x & ~y) = x
    
    // Create x & y (first set x=1, then y=1)
    cube x_and_y(0, 1);
    x_and_y.set(1, 1);
    
    // Create x & ~y (first set x=1, then y=0)
    cube x_and_not_y(0, 1);
    x_and_not_y.set(1, 0);
    
    cover redundant;
    redundant.push_back(x_and_y);
    redundant.push_back(x_and_not_y);
    
    // Minimize the cover
    redundant.minimize();
    
    // Should result in just x
    EXPECT_LE(redundant.size(), 1);  // Should be reduced to at most 1 cube
    
    if (redundant.size() == 1) {
        cube result = redundant[0];
        EXPECT_EQ(result.get(0), 1);  // x should be 1
        EXPECT_EQ(result.get(1), 2);  // y should be don't care
    }
}

// Test irredundant operation
TEST(CoverTest, Irredundant) {
    // Create a cover with a redundant cube
    cube a(0, 1);  // x
    
    // Create x & ~y (first set x=1, then y=0)
    cube b(0, 1);
    b.set(1, 0);
    
    cover redundant;
    redundant.push_back(a);
    redundant.push_back(b);
    
    // b is redundant because a covers it
    EXPECT_EQ(redundant.size(), 2);
    
    // We can achieve irredundancy with minimize
    redundant.minimize();
    
    // Should remove b because it's implied by a
    EXPECT_EQ(redundant.size(), 1);
}

// Test espresso algorithm
TEST(CoverTest, Espresso) {
    // Create on-set, off-set, and don't care set
    cover F;  // On-set
    cover D;  // Don't care set (empty for this test)
    
    // Create a function to minimize: F = x & y + x & ~y + ~x & y
    
    // Create x & y (first set x=1, then y=1)
    cube xy(0, 1);
    xy.set(1, 1);
    
    // Create x & ~y (first set x=1, then y=0)
    cube x_not_y(0, 1);
    x_not_y.set(1, 0);
    
    // Create ~x & y (first set x=0, then y=1)
    cube not_x_y(0, 0);
    not_x_y.set(1, 1);
    
    F.push_back(xy);
    F.push_back(x_not_y);
    F.push_back(not_x_y);
    
    // This should simplify to x + y
    cover R = ~F;  // Off-set is complement of on-set when no don't cares
    
    F.espresso();
    
    // Check result - should be 2 cubes: x and y
    EXPECT_EQ(F.size(), 2);
    
    // Verify that the cover is correct
    bool has_x = false;
    bool has_y = false;
    
    for (int i = 0; i < F.size(); i++) {
        if (F[i].get(0) == 1 && F[i].get(1) == 2) {
            has_x = true;  // This is the cube representing just x
        }
        if (F[i].get(0) == 2 && F[i].get(1) == 1) {
            has_y = true;  // This is the cube representing just y
        }
    }
    
    EXPECT_TRUE(has_x || has_y);  // At least one of these should be true
}

// Test tautology and contradiction checking
TEST(CoverTest, TautologyContradiction) {
    // Create an empty cover (represents contradiction)
    cover contradiction;
    EXPECT_TRUE(contradiction.is_null());
    EXPECT_FALSE(contradiction.is_tautology());
    
    // Create a tautology cover
    cover tautology;
    cube taut_cube;  // Empty cube is a tautology
    tautology.push_back(taut_cube);
    
    EXPECT_TRUE(tautology.is_tautology());
    
    // Create a cover that's neither tautology nor contradiction
    cube a(0, 1);  // x
    cover normal;
    normal.push_back(a);
    
    EXPECT_FALSE(normal.is_null());
    EXPECT_FALSE(normal.is_tautology());
} 