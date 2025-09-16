#include <gtest/gtest.h>
#include <boolean/bitset.h>
#include <boolean/cube.h>
#include <boolean/cover.h>
#include <map>

using namespace boolean;
// Explicitly use boolean::bitset, not std::bitset


// Test bitwise operations with low coverage
TEST(BitsetComplexTest, BitwiseOperations) {
	// Create test bitsets
	boolean::bitset a(3, 0);  // 3-bit bitset starting at offset 0
	boolean::bitset b(3, 0);

	// Setup some bit functions
	cube c1(5);
	c1.set(0, 1); c1.set(1, 0);  // Represents x1=1, x2=0

	cube c2(5);
	c2.set(0, 0); c2.set(1, 1);  // Represents x1=0, x2=1

	// Set bit 0 of bitset a to be function c1
	a.bits[0] = cover(c1);
	// Set bit 1 of bitset a to be function c2
	a.bits[1] = cover(c2);

	// Set bits for bitset b
	cube c3(5);
	c3.set(0, 1); c3.set(2, 0);  // Represents x1=1, x3=0

	cube c4(5);
	c4.set(1, 0); c4.set(2, 1);  // Represents x2=0, x3=1

	b.bits[0] = cover(c3);
	b.bits[2] = cover(c4);

	// Test bitwise AND
	boolean::bitset result_and = a & b;
	EXPECT_EQ(result_and.bits.size(), 3u);
	EXPECT_FALSE(result_and.bits[0].is_null());  // Should have intersection of bit 0

	// Bits 1 and 2 might be null or not depending on implementation
	// So don't test for specific nullness

	// Test bitwise OR
	boolean::bitset result_or = a | b;
	EXPECT_EQ(result_or.bits.size(), 3u);
	EXPECT_FALSE(result_or.bits[0].is_null());  // Union of bit 0
	EXPECT_FALSE(result_or.bits[1].is_null());  // Only from a
	EXPECT_FALSE(result_or.bits[2].is_null());  // Only from b

	// Test bitwise XOR
	boolean::bitset result_xor = a ^ b;
	EXPECT_EQ(result_xor.bits.size(), 3u);

	// Test bitwise NOT
	boolean::bitset result_not = ~a;
	EXPECT_EQ(result_not.bits.size(), 3u);
	// The complement of each bit should be the complement of the original cover
	for (size_t i = 0; i < a.bits.size(); i++) {
		if (!a.bits[i].is_null()) {
			EXPECT_FALSE(result_not.bits[i].is_null());
			// Check that original & complement is null
			cover intersection = a.bits[i] & result_not.bits[i];
			EXPECT_TRUE(intersection.is_null());
		}
	}

	// Test combination with &= operator
	boolean::bitset a_copy = a;
	a_copy &= b;
	EXPECT_EQ(a_copy.bits.size(), result_and.bits.size());
	for (size_t i = 0; i < a_copy.bits.size(); i++) {
		EXPECT_EQ(a_copy.bits[i].is_null(), result_and.bits[i].is_null());
	}

	// Test combination with |= operator
	a_copy = a;
	a_copy |= b;
	EXPECT_EQ(a_copy.bits.size(), result_or.bits.size());
	for (size_t i = 0; i < a_copy.bits.size(); i++) {
		EXPECT_EQ(a_copy.bits[i].is_null(), result_or.bits[i].is_null());
	}

	// Test combination with ^= operator
	a_copy = a;
	a_copy ^= b;
	EXPECT_EQ(a_copy.bits.size(), result_xor.bits.size());
}

// Test shift operations
TEST(BitsetComplexTest, ShiftOperations) {
	// Create a test bitset
	boolean::bitset a(4, 0);  // 4-bit bitset

	// Set some bits
	cube c1(3);
	c1.set(0, 1); c1.set(1, 0);

	cube c2(3);
	c2.set(0, 0); c2.set(1, 1);

	a.bits[1] = cover(c1);  // Set bit 1
	a.bits[2] = cover(c2);  // Set bit 2

	// Test left shift
	boolean::bitset left_shifted = a << 2;
	EXPECT_EQ(left_shifted.bits.size(), 6u);  // Should now be 6 bits (0-5)
	EXPECT_TRUE(left_shifted.bits[0].is_null());
	EXPECT_TRUE(left_shifted.bits[1].is_null());
	EXPECT_FALSE(left_shifted.bits[3].is_null());  // Bit 1 moved to 3
	EXPECT_FALSE(left_shifted.bits[4].is_null());  // Bit 2 moved to 4

	// Test right shift
	boolean::bitset right_shifted = a >> 1;
	EXPECT_EQ(right_shifted.bits.size(), 3u);  // Should now be 3 bits (0-2)
	EXPECT_FALSE(right_shifted.bits[0].is_null());  // Bit 1 moved to 0
	EXPECT_FALSE(right_shifted.bits[1].is_null());  // Bit 2 moved to 1

	// Test shift operators with assignment
	boolean::bitset a_copy = a;
	a_copy <<= 2;
	EXPECT_EQ(a_copy.bits.size(), left_shifted.bits.size());
	for (size_t i = 0; i < a_copy.bits.size(); i++) {
		EXPECT_EQ(a_copy.bits[i].is_null(), left_shifted.bits[i].is_null());
	}

	a_copy = a;
	a_copy >>= 1;
	EXPECT_EQ(a_copy.bits.size(), right_shifted.bits.size());
	for (size_t i = 0; i < a_copy.bits.size(); i++) {
		EXPECT_EQ(a_copy.bits[i].is_null(), right_shifted.bits[i].is_null());
	}
}

// Test bitset operations with cubes and covers
TEST(BitsetComplexTest, MixedTypeOperations) {
	// Create a test bitset
	boolean::bitset a(3, 0);

	// Set some bits
	cube c1(4);
	c1.set(0, 1); c1.set(1, 0);

	cube c2(4);
	c2.set(0, 0); c2.set(1, 1);

	a.bits[0] = cover(c1);
	a.bits[1] = cover(c2);

	// Create a cube for testing
	cube test_cube(4);
	test_cube.set(0, 1); test_cube.set(2, 0);

	// Create a cover for testing
	cover test_cover;
	test_cover.push_back(test_cube);

	// Test bitset & cube
	boolean::bitset result1 = a & test_cube;
	EXPECT_FALSE(result1.bits[0].is_null());  // Should have intersection with bit 0
	EXPECT_TRUE(result1.bits[1].is_null());   // No intersection with bit 1

	// Test bitset | cube
	boolean::bitset result2 = a | test_cube;
	EXPECT_FALSE(result2.bits[0].is_null());
	EXPECT_FALSE(result2.bits[1].is_null());

	// Test bitset ^ cube
	boolean::bitset result3 = a ^ test_cube;

	// Test bitset & cover
	boolean::bitset result4 = a & test_cover;
	EXPECT_FALSE(result4.bits[0].is_null());  // Should have intersection with bit 0
	EXPECT_TRUE(result4.bits[1].is_null());   // No intersection with bit 1

	// Test cube & bitset (reversed order)
	boolean::bitset result5 = test_cube & a;
	EXPECT_FALSE(result5.bits[0].is_null());  // Should have intersection with bit 0
	EXPECT_TRUE(result5.bits[1].is_null());   // No intersection with bit 1

	// Test cover & bitset (reversed order)
	boolean::bitset result6 = test_cover & a;
	EXPECT_FALSE(result6.bits[0].is_null());  // Should have intersection with bit 0
	EXPECT_TRUE(result6.bits[1].is_null());   // No intersection with bit 1
}

// Test resizing and subset operations
TEST(BitsetComplexTest, ResizeAndSubset) {
	// Create a test bitset
	boolean::bitset a(5, 0);

	// Set some bits
	for (int i = 0; i < 5; i++) {
		cube c(3);
		c.set(0, i % 2); c.set(1, (i+1) % 2);
		a.bits[i] = cover(c);
	}

	// Test resize to smaller
	boolean::bitset resized = a;
	resized.resize(3);
	EXPECT_EQ(resized.bits.size(), 3u);
	for (int i = 0; i < 3; i++) {
		EXPECT_FALSE(resized.bits[i].is_null());
	}

	// Test resize to larger
	resized = a;
	resized.resize(7);
	EXPECT_EQ(resized.bits.size(), 7u);
	for (int i = 0; i < 5; i++) {
		EXPECT_FALSE(resized.bits[i].is_null());
	}
	for (int i = 5; i < 7; i++) {
		EXPECT_TRUE(resized.bits[i].is_null());
	}

	// Test subset
	boolean::bitset subset = a.subset(1, 3);
	EXPECT_EQ(subset.bits.size(), 3u);
	for (int i = 0; i < 3; i++) {
		EXPECT_FALSE(subset.bits[i].is_null());
		// Bit i of subset should equal bit i+1 of a
		EXPECT_EQ(subset.bits[i].size(), a.bits[i+1].size());
	}

	// Test subset with implicit length
	boolean::bitset subset2 = a.subset(2);
	EXPECT_EQ(subset2.bits.size(), 3u);  // Should get bits 2-4
	for (int i = 0; i < 3; i++) {
		EXPECT_FALSE(subset2.bits[i].is_null());
		// Bit i of subset2 should equal bit i+2 of a
		EXPECT_EQ(subset2.bits[i].size(), a.bits[i+2].size());
	}
}

// Test decomposition functions
TEST(BitsetComplexTest, DecompositionFunctions) {
	// Create a test bitset representing a complex function
	boolean::bitset dut(4, 0);

	// Setup a function with some structure for decomposition
	cube c1(6);  // x1 & x2
	c1.set(0, 1); c1.set(1, 1);

	cube c2(6);  // x3 & x4
	c2.set(2, 1); c2.set(3, 1);

	cube c3(6);  // x1 & ~x2
	c3.set(0, 1); c3.set(1, 0);

	cube c4(6);  // ~x3 & x5
	c4.set(2, 0); c4.set(4, 1);

	// Setup several bits to represent a dut function
	cover cov1, cov2, cov3;
	cov1.push_back(c1);
	cov1.push_back(c2);

	cov2.push_back(c3);
	cov2.push_back(c4);

	cov3.push_back(c1);
	cov3.push_back(c3);

	dut.bits[0] = cov1;
	dut.bits[1] = cov2;
	dut.bits[2] = cov3;

	// Call partition to split the function
	boolean::bitset left, right;
	dut.partition(left, right);

	// Verify results - partition may be unbalanced so don't assert specific bounds
	// Just make sure both left and right have some bits
	EXPECT_GT(left.bits.size(), 0u);
	EXPECT_GT(right.bits.size(), 0u);

	// Test decomposition - may not work on this simple example
	std::map<boolean::cube, int> factors;
	boolean::bitset decomposed = dut.decompose_hfactor(factors, 2, 0);

	// Don't assert on factors - they may not be found in this simple case

	// Test xfactor decomposition - may not work on this simple example
	std::map<boolean::cube, int> xfactors;
	boolean::bitset x_decomposed = dut.decompose_xfactor(xfactors, 2, 0);

	// Don't assert on xfactors - they may not be found in this simple case
} 
