#include <gtest/gtest.h>
#include <boolean/cube.h>
#include <boolean/cover.h>
#include <vector>
#include <chrono>

using namespace boolean;
using std::cout;
using std::endl;

// Test advanced distance metrics between cubes
TEST(CubeAdvancedTest, AdvancedDistanceMetrics) {
    // Create cubes with specific variable assignments
    cube a(10);  // 10-variable cube
    cube b(10);  // Another 10-variable cube
    
    // Set different variables
    a.set(0, 1); a.set(2, 0); a.set(5, 1);
    b.set(0, 1); b.set(2, 1); b.set(5, 1); b.set(7, 0);
    
    // Test mutual exclusivity (different values for same variable)
    EXPECT_TRUE(are_mutex(a, b));  // Should be mutex due to conflict on variable 2
    
    // Test with don't cares
    cube c(10);
    c.set(0, 1); c.set(5, 1);  // Note: var 2 is don't care in c
    
    // c should not be mutex with either a or b since don't care matches both 0 and 1
    EXPECT_FALSE(are_mutex(c, a));
    EXPECT_FALSE(are_mutex(c, b));
    
    // Test containment with don't cares
    EXPECT_TRUE(a.is_subset_of(c));  // a is more specific than c
    EXPECT_FALSE(c.is_subset_of(a));  // c is not more specific than a
    
    // Test transitive relations
    cube d(10);
    d.set(0, 1);  // Even more general than c
    
    EXPECT_TRUE(a.is_subset_of(c) && c.is_subset_of(d));  // Transitive containment
    EXPECT_TRUE(a.is_subset_of(d));  // Should follow from transitivity
}

// Test large cube handling and performance
TEST(CubeAdvancedTest, LargeCubeHandling) {
    // Create a large cube with 100 variables
    cube large(100);
    
    // Set values for some variables
    for (int i = 0; i < 100; i += 2) {
        large.set(i, 1);  // Set every even variable to true
    }
    for (int i = 1; i < 100; i += 2) {
        large.set(i, 0);  // Set every odd variable to false
    }
    
    // Verify correctness
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0) {
            EXPECT_EQ(large.get(i), 1);  // Even variables should be true
        } else {
            EXPECT_EQ(large.get(i), 0);  // Odd variables should be false
        }
    }
    
    // Create another large cube to test operations
    cube large2(100);
    
    // Set some overlapping values differently to create conflicts
    for (int i = 0; i < 50; i++) {
        large2.set(i, i % 2 == 0 ? 1 : 0);  // Same as large
        large2.set(i + 50, i % 2 == 0 ? 0 : 1);  // Opposite of large
    }
    
    // Test intersection - should have conflicts
    EXPECT_TRUE(are_mutex(large, large2));
    
    // Performance test - measure time for operations
    auto start = std::chrono::high_resolution_clock::now();
    cube result = large & large2;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		cout << "duration: " << duration << endl;
    
    // We're not asserting on the duration, just computing it to verify it's reasonable
    EXPECT_TRUE(result.is_null());  // The intersection should be null due to conflicts
}

// Test variable range handling
TEST(CubeAdvancedTest, VariableRangeHandling) {
    // Create a cube with non-contiguous variable assignments
    cube sparse(1000);  // Use a large space
    
    // Set scattered variables
    sparse.set(1, 1);
    sparse.set(10, 0);
    sparse.set(100, 1);
    sparse.set(999, 0);
    
    // Verify variables are set correctly
    EXPECT_EQ(sparse.get(1), 1);
    EXPECT_EQ(sparse.get(10), 0);
    EXPECT_EQ(sparse.get(100), 1);
    EXPECT_EQ(sparse.get(999), 0);
    
    // Variables in between should be don't care
    EXPECT_EQ(sparse.get(2), 2);
    EXPECT_EQ(sparse.get(50), 2);
    EXPECT_EQ(sparse.get(500), 2);
    
    // Test intersection with cube containing other scattered variables
    cube sparse2(1000);
    sparse2.set(5, 1);
    sparse2.set(10, 1);  // Conflict with sparse (which has 0)
    sparse2.set(200, 0);
    
    EXPECT_TRUE(are_mutex(sparse, sparse2));  // Should be mutex due to conflict on var 10
    
    // Create a cube with very high variable index
    cube high_var;
    high_var.set(10000, 1);  // Set a very high variable index
    
    EXPECT_EQ(high_var.get(10000), 1);  // Should handle high indices
    
    // Test intersection with sparse
    EXPECT_FALSE(are_mutex(sparse, high_var));  // No conflicts
}

// Test cube merging and consensus
TEST(CubeAdvancedTest, CubeMergingAndConsensus) {
    // Create two cubes that differ in one variable
    cube a(10);
    a.set(0, 1); a.set(1, 1); a.set(2, 0);
    
    cube b(10);
    b.set(0, 1); b.set(1, 0); b.set(2, 0);
    
    // These cubes have different values for variable 1
    // They can be merged to create a cube with don't care for variable 1
    
    // Create cover for the two cubes
    cover c;
    c.push_back(a);
    c.push_back(b);
    
    // Minimize to merge cubes that differ in one variable
    c.minimize();
    
    // Result should be one cube with don't care for variable 1
    EXPECT_EQ(c.size(), 1);
    EXPECT_EQ(c[0].get(0), 1);
    EXPECT_EQ(c[0].get(1), 2);  // Should be don't care
    EXPECT_EQ(c[0].get(2), 0);
    
    // Test consensus (finding common terms)
    // Create two cubes with opposite values for one variable
    cube p(5);
    p.set(0, 1); p.set(1, 0); p.set(2, 1);
    
    cube q(5);
    q.set(0, 1); q.set(1, 1); q.set(2, 1);
    
    // The consensus term should have variable 1 as don't care
    // but retain all other assignments
    cover consensus_cover;
    consensus_cover.push_back(p);
    consensus_cover.push_back(q);
    consensus_cover.minimize();
    
    // After minimization, we should get a cube with var 1 as don't care
    EXPECT_EQ(consensus_cover.size(), 1);
    if (consensus_cover.size() == 1) {
        cube consensus = consensus_cover[0];
        EXPECT_EQ(consensus.get(0), 1);  // Var 0 should still be 1
        EXPECT_EQ(consensus.get(1), 2);  // Var 1 should be don't care
        EXPECT_EQ(consensus.get(2), 1);  // Var 2 should still be 1
    }
}

// Test transformation operations on cubes
TEST(CubeAdvancedTest, TransformationOperations) {
    // Create a cube with specific variable assignments
    cube original(5);
    original.set(0, 1); original.set(1, 0); original.set(2, 1);
    
    // Test variable remapping
    // Create a mapping vector: old variable -> new variable
    // For example: 0->2, 1->0, 2->1
    std::vector<int> mapping = {2, 0, 1, 3, 4};
    
    // Apply remapping manually - properly this time
    cube remapped(5);
    remapped.set(mapping[0], original.get(0));  // 2 = original[0] = 1
    remapped.set(mapping[1], original.get(1));  // 0 = original[1] = 0 
    remapped.set(mapping[2], original.get(2));  // 1 = original[2] = 1
    
    // Verify remapping is correct
    EXPECT_EQ(remapped.get(2), 1);  // Old var 0 (value 1) moved to var 2
    EXPECT_EQ(remapped.get(0), 0);  // Old var 1 (value 0) moved to var 0
    EXPECT_EQ(remapped.get(1), 1);  // Old var 2 (value 1) moved to var 1
    
    // Verify logical equivalence under ordering
    // Create a cover for original and remapped
    cover c1, c2;
    c1.push_back(original);
    c2.push_back(remapped);
    
    // Create test points for specific cases we know should match
    // Test a point that should be contained by both
    cube matching_point(5);
    matching_point.set(0, 1);
    matching_point.set(1, 0);
    matching_point.set(2, 1);
    
    // Create corresponding remapped point
    cube remapped_matching(5);
    remapped_matching.set(mapping[0], matching_point.get(0));  // 2 = 1
    remapped_matching.set(mapping[1], matching_point.get(1));  // 0 = 0
    remapped_matching.set(mapping[2], matching_point.get(2));  // 1 = 1
    
    // Check that original contains original test point
    bool original_contains = !are_mutex(original, matching_point);
    // Check that remapped contains remapped test point
    bool remapped_contains = !are_mutex(remapped, remapped_matching);
    
    // Both should either contain or not contain their respective test points
    EXPECT_EQ(original_contains, remapped_contains);
    
    // Test a point that should not be contained by either
    cube non_matching_point(5);
    non_matching_point.set(0, 0);  // Conflicts with original
    non_matching_point.set(1, 0);
    non_matching_point.set(2, 1);
    
    // Create corresponding remapped point
    cube remapped_non_matching(5);
    remapped_non_matching.set(mapping[0], non_matching_point.get(0));  // 2 = 0
    remapped_non_matching.set(mapping[1], non_matching_point.get(1));  // 0 = 0
    remapped_non_matching.set(mapping[2], non_matching_point.get(2));  // 1 = 1
    
    // Check that original contains original test point
    original_contains = !are_mutex(original, non_matching_point);
    // Check that remapped contains remapped test point  
    remapped_contains = !are_mutex(remapped, remapped_non_matching);
    
    // Both should either contain or not contain their respective test points
    EXPECT_EQ(original_contains, remapped_contains);
} 
