#include <gtest/gtest.h>
#include <boolean/signed_int.h>

using namespace boolean;

// Test signed_int construction and basic properties
TEST(SignedIntTest, Construction) {
    // Create with specified width and base variable
    signed_int a(4, 0);  // 4-bit value using variables starting from 0
    
    EXPECT_EQ(a.bits.size(), size_t(4));
    
    // Check that each bit contains the appropriate variable
    for (size_t i = 0; i < a.bits.size(); i++) {
        EXPECT_FALSE(a.bits[i].is_null());  // Each bit should be a non-null cover with a variable
    }
    
    // Create with constant value (positive)
    signed_int b(5L);  // Value 5
    // For a small positive value, only the necessary bits should be created
    EXPECT_GE(b.bits.size(), size_t(3));  // At least 3 bits for value 5 (binary 101)
    
    // Check sign extension behavior with negative values
    signed_int c(-5L);  // Value -5
    EXPECT_GE(c.bits.size(), size_t(3));  // At least 3 bits for the magnitude
    
    // Create with specific width and offset
    signed_int d(8, 10);  // 8-bit representation using variables starting from 10
    EXPECT_EQ(d.bits.size(), size_t(8));
    
    // Check that each bit contains the appropriate variable
    for (size_t i = 0; i < d.bits.size(); i++) {
        EXPECT_FALSE(d.bits[i].is_null());  // Each bit should be a non-null cover with a variable
    }
}

// Test unary negation operation
TEST(SignedIntTest, Negation) {
    // Create a symbolic signed integer
    signed_int a(4, 0);  // 4-bit value using variables starting from 0
    
    // Negate it
    signed_int negated = -a;
    
    // The result should have at least the same number of bits
    EXPECT_GE(negated.bits.size(), a.bits.size());
    
    // Each bit should have a boolean expression
    for (size_t i = 0; i < negated.bits.size(); i++) {
        EXPECT_FALSE(negated.bits[i].is_null());  // Each bit should have a boolean expression
    }
}

// Test addition operation
TEST(SignedIntTest, Addition) {
    // Test with symbolic values
    signed_int a(8, 5);   // 8-bit representation using variables starting from 5
    signed_int b(8, 13);  // 8-bit representation using variables starting from 13
    
    signed_int sum = a + b;
    // Result should include space for carry without changing the bit width for a signed number
    EXPECT_GE(sum.bits.size(), a.bits.size());
    
    // The result should be a complex boolean expression for each bit
    for (size_t i = 0; i < sum.bits.size(); i++) {
        EXPECT_FALSE(sum.bits[i].is_null());  // Each bit should have a boolean expression
    }
    
    // Test with different widths
    signed_int c(4, 21);  // 4-bit representation using variables starting from 21
    signed_int d(6, 25);  // 6-bit representation using variables starting from 25
    
    signed_int mixed_sum = c + d;
    // Result should have enough bits to represent a signed sum
    EXPECT_GE(mixed_sum.bits.size(), size_t(6));
    
    // The result should have expressions for each bit
    for (size_t i = 0; i < mixed_sum.bits.size(); i++) {
        EXPECT_FALSE(mixed_sum.bits[i].is_null());  // Each bit should have a boolean expression
    }
}

// Test subtraction operation
TEST(SignedIntTest, Subtraction) {
    // Test with symbolic values
    signed_int a(8, 5);   // 8-bit representation using variables starting from 5
    signed_int b(8, 13);  // 8-bit representation using variables starting from 13
    
    signed_int diff = a - b;
    // Result width should be sufficient for a signed difference
    EXPECT_GE(diff.bits.size(), a.bits.size());
    
    // The result should be a complex boolean expression for each bit
    for (size_t i = 0; i < diff.bits.size(); i++) {
        EXPECT_FALSE(diff.bits[i].is_null());  // Each bit should have a boolean expression
    }
    
    // Test with different widths
    signed_int c(4, 21);  // 4-bit representation using variables starting from 21
    signed_int d(6, 25);  // 6-bit representation using variables starting from 25
    
    signed_int mixed_diff = c - d;
    // Result should have enough bits to represent a signed difference
    EXPECT_GE(mixed_diff.bits.size(), size_t(6));
    
    // The result should have expressions for each bit
    for (size_t i = 0; i < mixed_diff.bits.size(); i++) {
        EXPECT_FALSE(mixed_diff.bits[i].is_null());  // Each bit should have a boolean expression
    }
}

// Test multiplication operation
TEST(SignedIntTest, Multiplication) {
    // Test with symbolic values
    signed_int a(4, 5);   // 4-bit representation using variables starting from 5
    signed_int b(4, 9);   // 4-bit representation using variables starting from 9
    
    signed_int product = a * b;
    
    // The result should be a complex boolean expression for each bit
    EXPECT_FALSE(product.is_constant());  // Should not be a constant
    
    // Test with different widths
    signed_int c(2, 13);  // 2-bit representation using variables starting from 13
    signed_int d(3, 15);  // 3-bit representation using variables starting from 15
    
    signed_int product2 = c * d;
    EXPECT_FALSE(product2.is_constant());  // Should not be a constant
    
    // Test with expressions that might include negative values
    EXPECT_GE(product.bits.size(), size_t(4));  // Result should have enough bits for sign extension
}

// Test division operation
/*TEST(SignedIntTest, Division) {
    // Test with symbolic values
    signed_int a(8, 5);   // 8-bit representation using variables starting from 5
    signed_int b(4, 13);  // 4-bit representation using variables starting from 13
    
    signed_int quotient = a / b;
    EXPECT_GE(quotient.bits.size(), size_t(8));  // Result should have sufficient bits
    
    // The result should be a complex boolean expression for each bit
    EXPECT_FALSE(quotient.is_constant());  // Should not be a constant
    
    // Test with different widths
    signed_int c(6, 17);  // 6-bit representation using variables starting from 17
    signed_int d(3, 23);  // 3-bit representation using variables starting from 23
    
    signed_int quotient2 = c / d;
    EXPECT_GE(quotient2.bits.size(), size_t(6));  // Result should have sufficient bits
    EXPECT_FALSE(quotient2.is_constant());  // Should not be a constant
}*/

// Test comparison operations
TEST(SignedIntTest, Comparison) {
    // Create symbolic values
    signed_int a(4, 5);  // 4-bit representation using variables starting from 5
    signed_int b(4, 9);  // 4-bit representation using variables starting from 9
    signed_int c(4, 5);  // 4-bit representation using variables starting from 5 (same as a)
    
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

// Test signed arithmetic with symbolic inputs
TEST(SignedIntTest, SymbolicArithmetic) {
    // Create symbolic signed integers
    signed_int a(3, 0);  // 3-bit value using variables 0-2 (including sign bit)
    signed_int b(3, 3);  // 3-bit value using variables 3-5 (including sign bit)
    
    // Test addition
    signed_int sum = a + b;
    EXPECT_GE(sum.bits.size(), size_t(3));  // Should have enough bits
    
    // Each bit of the result should encode the symbolic addition logic
    for (size_t i = 0; i < sum.bits.size(); i++) {
        EXPECT_FALSE(sum.bits[i].is_null());  // Each bit should have an expression
    }
    
    // Test subtraction
    signed_int diff = a - b;
    EXPECT_GE(diff.bits.size(), size_t(3));  // Should have enough bits
    
    // Each bit should encode the symbolic subtraction logic
    for (size_t i = 0; i < diff.bits.size(); i++) {
        EXPECT_FALSE(diff.bits[i].is_null());  // Each bit should have an expression
    }
    
    // Test multiplication
    signed_int product = a * b;
    
    // Each bit should encode the symbolic multiplication logic
    for (size_t i = 0; i < product.bits.size(); i++) {
        EXPECT_FALSE(product.bits[i].is_null());  // Each bit should have an expression
    }
    
    // Test comparison operations for signed values
    cover lt = (a < b);
    EXPECT_FALSE(lt.is_null());       // Should have an expression
    EXPECT_FALSE(lt.is_tautology());  // Not tautologically true
    
    cover eq = (a == b);
    EXPECT_FALSE(eq.is_null());       // Should have an expression
    EXPECT_FALSE(eq.is_tautology());  // Not tautologically true
}

// Test sign extension behavior
TEST(SignedIntTest, SignExtension) {
    // Create a signed_int with some symbolic variables
    signed_int a(4, 0);  // 4-bit value with variables 0-3
    
    // Test the extend method which should produce the sign extension cover
    cover sign_ext = a.extend();
    EXPECT_FALSE(sign_ext.is_null());  // Should not be null
    
    // Using bitset operations that would trigger sign extension
    signed_int b(6, 4);  // 6-bit value with variables 4-9
    
    // Operations that should cause sign extension
    signed_int result = a + b;  // Addition with different widths
    
    // Verify the MSB (sign bit) propagation behavior
    EXPECT_GE(result.bits.size(), size_t(6));  // Result width should be at least the larger operand
    
    // Check each bit has a valid expression
    for (size_t i = 0; i < result.bits.size(); i++) {
        EXPECT_FALSE(result.bits[i].is_null());
    }
} 
