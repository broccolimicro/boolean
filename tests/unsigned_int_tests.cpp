#include <gtest/gtest.h>
#include <boolean/unsigned_int.h>

using namespace boolean;

// Test unsigned_int construction and basic properties
TEST(UnsignedIntTest, Construction) {
    // Create with specified width and base variable
    unsigned_int a(4, 0);  // 4-bit value using variables starting from 0
    
    EXPECT_EQ(a.bits.size(), size_t(4));
    
    // Check that each bit contains the appropriate variable
    for (size_t i = 0; i < a.bits.size(); i++) {
        EXPECT_FALSE(a.bits[i].is_null());  // Each bit should be a non-null cover with a variable
    }
    
    // Create with constant value
    unsigned_int b(5UL);  // Value 5
    EXPECT_EQ(b.bits.size(), size_t(3));  // For value 5 (binary 101), we expect 3 bits
    
    // Check that bits are set correctly for value 5 (binary 101)
    EXPECT_FALSE(b.bits[0].is_null());  // LSB should be 1
    EXPECT_TRUE(b.bits[1].is_null());   // Next bit should be 0
    EXPECT_FALSE(b.bits[2].is_null());  // Next bit should be 1
    
    // Create with specific width and constant value
    unsigned_int c(8, 10);  // 8-bit representation using variables starting from 10
    EXPECT_EQ(c.bits.size(), size_t(8));
    
    // Check that each bit contains the appropriate variable
    for (size_t i = 0; i < c.bits.size(); i++) {
        EXPECT_FALSE(c.bits[i].is_null());  // Each bit should be a non-null cover with a variable
    }
}

// Test addition operation
TEST(UnsignedIntTest, Addition) {
    // Test with constant values
    unsigned_int a(8, 5);   // 8-bit representation using variables starting from 5
    unsigned_int b(8, 13);  // 8-bit representation using variables starting from 13
    
    unsigned_int sum = a + b;
    EXPECT_EQ(sum.bits.size(), size_t(9));  // Result will have 9 bits (8 + carry)
    
    // The result should be a complex boolean expression for each bit
    // Each bit should have a non-null expression
    for (size_t i = 0; i < sum.bits.size(); i++) {
        EXPECT_FALSE(sum.bits[i].is_null());  // Each bit should have a boolean expression
    }
    
    // Test with different widths
    unsigned_int c(4, 21);  // 4-bit representation using variables starting from 21
    unsigned_int d(4, 25);  // 4-bit representation using variables starting from 25
    
    unsigned_int overflow_sum = c + d;
    EXPECT_EQ(overflow_sum.bits.size(), size_t(5));  // 4 bits + 1 carry bit
    
    // The result should have expressions for each bit
    for (size_t i = 0; i < overflow_sum.bits.size(); i++) {
        EXPECT_FALSE(overflow_sum.bits[i].is_null());  // Each bit should have a boolean expression
    }
}

// Test subtraction operation
TEST(UnsignedIntTest, Subtraction) {
    // Test with symbolic values
    unsigned_int a(8, 5);   // 8-bit representation using variables starting from 5
    unsigned_int b(8, 13);  // 8-bit representation using variables starting from 13
    
    unsigned_int diff = a - b;
    EXPECT_EQ(diff.bits.size(), size_t(9));  // Result will have 8 bits + 1 borrow bit
    
    // The result should be a complex boolean expression for each bit
    for (size_t i = 0; i < diff.bits.size(); i++) {
        EXPECT_FALSE(diff.bits[i].is_null());  // Each bit should have a boolean expression
    }
    
    // Test with different widths
    unsigned_int c(4, 21);  // 4-bit representation using variables starting from 21
    unsigned_int d(4, 25);  // 4-bit representation using variables starting from 25
    
    unsigned_int underflow_diff = c - d;
    EXPECT_EQ(underflow_diff.bits.size(), size_t(5));  // 4 bits + 1 borrow bit
    
    // The result should have expressions for each bit
    for (size_t i = 0; i < underflow_diff.bits.size(); i++) {
        EXPECT_FALSE(underflow_diff.bits[i].is_null());  // Each bit should have a boolean expression
    }
}

// Test multiplication operation
TEST(UnsignedIntTest, Multiplication) {
    // Test with symbolic values
    unsigned_int a(4, 5);   // 4-bit representation using variables starting from 5
    unsigned_int b(4, 9);   // 4-bit representation using variables starting from 9
    
    unsigned_int product = a * b;
    
    // The result should be a complex boolean expression for each bit
    EXPECT_FALSE(product.is_constant());  // Should not be a constant
    
    // Test with different widths
    unsigned_int c(2, 13);  // 2-bit representation using variables starting from 13
    unsigned_int d(3, 15);  // 3-bit representation using variables starting from 15
    
    unsigned_int product2 = c * d;
    EXPECT_FALSE(product2.is_constant());  // Should not be a constant
}

// Test division operation
TEST(UnsignedIntTest, Division) {
    // Test with symbolic values
    unsigned_int a(8, 5);   // 8-bit representation using variables starting from 5
    unsigned_int b(4, 13);  // 4-bit representation using variables starting from 13
    
    unsigned_int quotient = a / b;
    EXPECT_EQ(quotient.bits.size(), size_t(8));  // Result will have same size as dividend
    
    // The result should be a complex boolean expression for each bit
    EXPECT_FALSE(quotient.is_constant());  // Should not be a constant
    
    // Test with different widths
    unsigned_int c(6, 17);  // 6-bit representation using variables starting from 17
    unsigned_int d(3, 23);  // 3-bit representation using variables starting from 23
    
    unsigned_int quotient2 = c / d;
    EXPECT_EQ(quotient2.bits.size(), size_t(6));  // Result should have 6 bits
    EXPECT_FALSE(quotient2.is_constant());  // Should not be a constant
}

// Test comparison operations
TEST(UnsignedIntTest, Comparison) {
    // Create symbolic values
    unsigned_int a(4, 5);  // 4-bit representation using variables starting from 5
    unsigned_int b(4, 9);  // 4-bit representation using variables starting from 9
    unsigned_int c(4, 5);  // 4-bit representation using variables starting from 5 (same as a)
    
    // Test equality
    cover eq_ab = (a == b);
    EXPECT_FALSE(eq_ab.is_tautology());  // a and b are not tautologically equal
    EXPECT_FALSE(eq_ab.is_null());       // But they can be equal in some cases
    
    // Test less than
    cover lt_ab = (a < b);
    EXPECT_FALSE(lt_ab.is_tautology());  // Not tautologically less than
    EXPECT_FALSE(lt_ab.is_null());       // Can be less than in some cases
    
    // Test greater than
    cover gt_ab = (a > b);
    EXPECT_FALSE(gt_ab.is_tautology());  // Not tautologically greater than
    EXPECT_FALSE(gt_ab.is_null());       // Can be greater than in some cases
    
    // Test less than or equal
    cover le_ab = (a <= b);
    EXPECT_FALSE(le_ab.is_tautology());  // Not tautologically less than or equal
    EXPECT_FALSE(le_ab.is_null());       // Can be less than or equal in some cases
    
    // Test greater than or equal
    cover ge_ab = (a >= b);
    EXPECT_FALSE(ge_ab.is_tautology());  // Not tautologically greater than or equal
    EXPECT_FALSE(ge_ab.is_null());       // Can be greater than or equal in some cases
}

// Test bitwise operations
TEST(UnsignedIntTest, BitwiseOperations) {
    // Create symbolic values
    unsigned_int a(4, 5);  // 4-bit representation using variables starting from 5
    unsigned_int b(4, 9);  // 4-bit representation using variables starting from 9
    
    // Test bitwise AND
    unsigned_int and_result = a & b;
    EXPECT_EQ(and_result.bits.size(), size_t(4));
    EXPECT_FALSE(and_result.is_constant());  // Should not be a constant
    
    // Test bitwise OR
    unsigned_int or_result = a | b;
    EXPECT_EQ(or_result.bits.size(), size_t(4));
    EXPECT_FALSE(or_result.is_constant());  // Should not be a constant
    
    // Test bitwise XOR
    unsigned_int xor_result = a ^ b;
    EXPECT_EQ(xor_result.bits.size(), size_t(4));
    EXPECT_FALSE(xor_result.is_constant());  // Should not be a constant
}

// Test arithmetic with symbolic inputs
TEST(UnsignedIntTest, SymbolicArithmetic) {
    // Create symbolic unsigned integers
    unsigned_int a(2, 0);  // 2-bit value using variables 0-1
    unsigned_int b(2, 2);  // 2-bit value using variables 2-3
    
    // Test addition
    unsigned_int sum = a + b;
    EXPECT_EQ(sum.bits.size(), size_t(3));  // Should include carry bit
    
    // Each bit of the result should encode the symbolic addition logic
    for (size_t i = 0; i < sum.bits.size(); i++) {
        EXPECT_FALSE(sum.bits[i].is_null());  // Each bit should have an expression
    }
    
    // Test multiplication
    unsigned_int product = a * b;
    
    // Each bit should encode the symbolic multiplication logic
    for (size_t i = 0; i < product.bits.size(); i++) {
        EXPECT_FALSE(product.bits[i].is_null());  // Each bit should have an expression
    }
    
    // Test comparison operations
    cover lt = (a < b);
    EXPECT_FALSE(lt.is_null());       // Should have an expression
    EXPECT_FALSE(lt.is_tautology());  // Not tautologically true
    
    cover eq = (a == b);
    EXPECT_FALSE(eq.is_null());       // Should have an expression
    EXPECT_FALSE(eq.is_tautology());  // Not tautologically true
} 