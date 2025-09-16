#include <gtest/gtest.h>
#include <boolean/cube.h>
#include <boolean/cover.h>
#include <boolean/bitset.h>
#include <boolean/unsigned_int.h>
#include <boolean/signed_int.h>

using namespace boolean;
// Explicitly use boolean::bitset, not std::bitset

// Test integration between cube and cover classes
TEST(IntegrationTest, CubeCoverInteraction) {
	// Create cubes
	cube a(0, 1);  // Variable 0 is true
	cube b(1, 0);  // Variable 1 is false

	// Create a cover from cubes
	cover c;
	c.push_back(a);
	c.push_back(b);

	// Verify cube properties within the cover
	EXPECT_EQ(c.size(), 2);
	EXPECT_EQ(c[0].get(0), 1);  // First cube's variable 0 is true
	EXPECT_EQ(c[1].get(1), 0);  // Second cube's variable 1 is false

	// Test interactions between cube and cover operations
	cube d(2, 1);  // Variable 2 is true
	cover c_and_d = c & d;

	// The resulting cover should have each original cube AND'd with d
	EXPECT_EQ(c_and_d.size(), 2);
	EXPECT_EQ(c_and_d[0].get(0), 1);  // First cube keeps variable 0 true
	EXPECT_EQ(c_and_d[0].get(2), 1);  // First cube now has variable 2 true
	EXPECT_EQ(c_and_d[1].get(1), 0);  // Second cube keeps variable 1 false
	EXPECT_EQ(c_and_d[1].get(2), 1);  // Second cube now has variable 2 true
}

// Test integration between cover and bitset classes
TEST(IntegrationTest, CoverBitsetInteraction) {
	// Create a cover
	cover c;
	c.push_back(cube(0, 1));  // Variable 0 is true
	c.push_back(cube(1, 0));  // Variable 1 is false

	// Create a bitset with the cover
	boolean::bitset b(c);
	EXPECT_EQ(b.bits.size(), size_t(1));  // Should have one bit
	EXPECT_EQ(b.bits[0].size(), 2);  // The bit should have two cubes

	// Create another bitset directly
	boolean::bitset b2(2, 2);  // 2 bits with variables starting from 2

	// Perform logical operation between bitset and cover
	boolean::bitset b_and_c = b & c;

	// The result should have the same structure as b but with modified expressions
	EXPECT_EQ(b_and_c.bits.size(), size_t(1));
	EXPECT_FALSE(b_and_c.bits[0].is_null());
}

// Test integration between bitset and unsigned_int
TEST(IntegrationTest, BitsetUnsignedIntInteraction) {
	// Create a bitset
	boolean::bitset b(4, 0);  // 4 bits with variables 0-3

	// Create an unsigned_int from the bitset
	unsigned_int ui(b);
	EXPECT_EQ(ui.bits.size(), b.bits.size());

	// Perform arithmetic on the unsigned_int
	unsigned_int ui2(4, 4);  // 4 bits with variables 4-7
	unsigned_int sum = ui + ui2;

	// The result should be a valid bitset as well
	boolean::bitset b_sum(sum);
	EXPECT_EQ(b_sum.bits.size(), sum.bits.size());

	// Verify bitwise operations work after conversion
	boolean::bitset b_and = b & sum;
	EXPECT_EQ(b_and.bits.size(), size_t(4));  // Should match the smaller operand size
}

// Test integration between unsigned_int and signed_int
TEST(IntegrationTest, UnsignedSignedIntInteraction) {
	// Create an unsigned_int and a signed_int
	unsigned_int ui(4, 0);  // 4 bits with variables 0-3
	signed_int si(4, 4);    // 4 bits with variables 4-7

	// Convert unsigned to signed
	signed_int si_from_ui(ui);
	EXPECT_EQ(si_from_ui.bits.size(), ui.bits.size());

	// Convert signed to unsigned (with potential information loss)
	unsigned_int ui_from_si(si);
	EXPECT_EQ(ui_from_si.bits.size(), si.bits.size());

	// Perform operations between different types
	// For example, a cover from comparison
	cover comparison = (ui_from_si == ui_from_si);
	EXPECT_FALSE(comparison.is_null());  // Should have some expression
}

// Test complex operations involving multiple classes
TEST(IntegrationTest, ComplexMultiClassOperations) {
	// Create cubes and a cover
	cube c1(0, 1);  // Variable 0 is true
	cube c2(1, 0);  // Variable 1 is false
	cover cov;
	cov.push_back(c1);
	cov.push_back(c2);

	// Create unsigned and signed integers
	unsigned_int ui(4, 2);  // 4 bits with variables 2-5
	signed_int si(4, 6);    // 4 bits with variables 6-9

	// Create a mapping
	Mapping<int> nets(-1, false);
	nets.set({{0, 5}, {1, 3}, {2, 1}, {3, 7}});

	// Perform a sequence of operations
	// 1. Create a comparison using same types to avoid ambiguity
	unsigned_int ui_copy = ui;
	cover comparison = (ui == ui_copy);

	// 2. Combine with the original cover
	cover combined = cov | comparison;

	// 3. Apply the mapping
	combined.apply(nets);

	// 4. Use the result in a bitset
	boolean::bitset b(combined);

	// 5. Convert to unsigned_int and perform arithmetic
	unsigned_int final_ui(b);
	unsigned_int result = final_ui + ui;

	// Verify the result has valid expressions
	EXPECT_FALSE(result.is_constant());

	// Check each bit has a non-null expression
	for (size_t i = 0; i < result.bits.size(); i++) {
		EXPECT_FALSE(result.bits[i].is_null());
	}
} 
