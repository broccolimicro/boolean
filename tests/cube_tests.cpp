#include <gtest/gtest.h>
#include <boolean/cube.h>
#include <boolean/cover.h>

using namespace boolean;

// Test basic cube construction
TEST(CubeTest, Construction) {
    cube a(0, 1);  // Variable 0 is true (positive)
    cube b(1, 0);  // Variable 1 is false (negative)
    cube c;
    c.set(2, 2);   // Variable 2 is don't care
    
    EXPECT_EQ(a.get(0), 1);  // True
    EXPECT_EQ(b.get(1), 0);  // False
    EXPECT_EQ(c.get(2), 2);  // Don't care
    
    // Default values for unspecified variables
    EXPECT_EQ(a.get(1), 2);  // Unspecified vars should be don't care
    EXPECT_EQ(a.get(2), 2);  // Unspecified vars should be don't care
}

// Test logical operations
TEST(CubeTest, LogicalOperations) {
    cube a(0, 1);  // Variable 0 is true
    cube b(1, 0);  // Variable 1 is false
    
    cube result = a & b;
    EXPECT_EQ(result.get(0), 1);  // Should remain true
    EXPECT_EQ(result.get(1), 0);  // Should remain false
    
    // Test OR operation if applicable
    // cover or_result = a | b;
    // Check expected values
}

// Test containment operations
TEST(CubeTest, Containment) {
    cube a(0, 1);  // Variable 0 is true
    
    // Create a supercube of a
    cube super = a;
    super.set(2, 2);  // Don't care for variable 2
    
    EXPECT_TRUE(a.is_subset_of(super));
    EXPECT_TRUE(super.is_subset_of(a));  // This is true because super only has one more don't care compared to a
    
    // Create a conflicting cube
    cube b(0, 0);  // Variable 0 is false
    EXPECT_FALSE(a.is_subset_of(b));
    EXPECT_FALSE(b.is_subset_of(a));
}

// Test intersection detection
TEST(CubeTest, Intersection) {
    cube a(0, 1);  // Variable 0 is true
    cube b(0, 0);  // Variable 0 is false (conflicts with a)
    cube c(1, 1);  // Variable 1 is true (compatible with a)
    
    EXPECT_TRUE(are_mutex(a, b));  // Should be mutex due to conflict
    EXPECT_FALSE(are_mutex(a, c));  // Should not be mutex as no conflicts
}

// Test distance calculation
TEST(CubeTest, Distance) {
    cube a(0, 1);  // Variable 0 is true
    cube b(0, 0);  // Variable 0 is false
    
    // Distance calculation is not directly available, but we can check mutex relationship
    EXPECT_TRUE(are_mutex(a, b));  // Should be mutex due to conflict
    
    cube c(0, 1);  // Variable 0 is true
    c.set(1, 1);   // Variable 1 is true
    
    cube d(0, 1);  // Variable 0 is true
    d.set(1, 0);   // Variable 1 is false
    
    EXPECT_TRUE(are_mutex(c, d));  // Should be mutex due to conflict
}

// Test cube cofactoring
TEST(CubeTest, Cofactoring) {
    cube a(0, 1);  // Variable 0 is true
    a.set(1, 1);   // Variable 1 is true
    a.set(2, 2);   // Variable 2 is don't care
    
    // Cofactor with respect to variable 0 being true
    cube result = a;
    result.cofactor(0, 1);
    
    // Variable 0 should be eliminated (set to don't care) in the result
    EXPECT_EQ(result.get(0), 2);
    EXPECT_EQ(result.get(1), 1);
    
    // Cofactor with respect to variable 0 being false
    result = a;
    result.cofactor(0, 0);
    
    // Should result in null cube since variable 0 is true in a
    EXPECT_TRUE(result.is_null());
}

// Test handling of empty and tautology cubes
TEST(CubeTest, SpecialCases) {
    // Create a null cube (empty)
    cube empty(0);
    EXPECT_TRUE(empty.is_null());
    
    // Create a tautology cube (all don't cares)
    cube tautology;
    EXPECT_TRUE(tautology.is_tautology());
    
    // Test that empty cube contains nothing
    cube a(0, 1);
    EXPECT_FALSE(a.is_subset_of(empty));
    
    // Test that tautology contains everything
    EXPECT_TRUE(a.is_subset_of(tautology));
} 