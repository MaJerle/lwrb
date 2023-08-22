#ifdef TEST

#include "unity.h"

#include "lwrb.h"

#define TEST_ARRAY_SIZE 10

static uint8_t lwrb_data[TEST_ARRAY_SIZE]  = { 0x00 };
static uint8_t lwrb_data2[TEST_ARRAY_SIZE] = { 0x00 };
static uint8_t lwrb_result[TEST_ARRAY_SIZE];
static lwrb_t lwrb_test;
static lwrb_t lwrb_test2;


void setUp(void)
{
    memset(lwrb_data, 0x00, TEST_ARRAY_SIZE);
    memset(lwrb_data2, 0xFF, TEST_ARRAY_SIZE);
    lwrb_init(&lwrb_test, lwrb_data, TEST_ARRAY_SIZE);
    lwrb_init(&lwrb_test2, lwrb_data2, TEST_ARRAY_SIZE);
}

void tearDown(void)
{
}


void test_lwrb_ex_copy_1_linear_linear_simple(void)
{
    // Given
    uint8_t input[10]               = { 0x00 };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };
    size_t copied;

    // When
    //[AA][BB][xx][xx][xx][xx][xx][xx][xx][xx]
    // r        w
    //[00][00][00][00][00][00][00][00][00][00]
    // rw
    input[0] = 0xAA;
    input[1] = 0xBB;
    lwrb_write(&lwrb_test, input, 2);
    copied = lwrb_copy(&lwrb_test2, &lwrb_test);

    // Then
    //[AA][BB][xx][xx][xx][xx][xx][xx][xx][xx]
    //         rw
    //[AA][BB][00][00][00][00][00][00][00][00]
    // r        w
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_NOT_NULL(lwrb_test2.buff);
    TEST_ASSERT_EQUAL(copied, 2);
    TEST_ASSERT_EQUAL(lwrb_test.w, 2);
    TEST_ASSERT_EQUAL(lwrb_test.r, 2);
    TEST_ASSERT_EQUAL(lwrb_test2.w, 2);
    TEST_ASSERT_EQUAL(lwrb_test2.r, 0);

    TEST_ASSERT_EQUAL(lwrb_data[0], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[1], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data2[0], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data2[1], 0xBB);

    lwrb_read(&lwrb_test2, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
}


void test_lwrb_ex_copy_2_linear_linear_end(void)
{
    // Given
    uint8_t input[10]               = { 0x00 };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };
    size_t copied;

    // When
    //[xx][AA][BB][CC][DD][EE][FF][00][11][22]
    //  w  r
    //[xx][00][00][00][00][00][00][00][00][00]
    //     rw
    input[0]     = 0xAA;
    input[1]     = 0xBB;
    input[2]     = 0xCC;
    input[3]     = 0xDD;
    input[4]     = 0xEE;
    input[5]     = 0xFF;
    input[6]     = 0x00;
    input[7]     = 0x11;
    input[8]     = 0x22;
    lwrb_test.r  = 1;
    lwrb_test.w  = 1;
    lwrb_test2.r = 1;
    lwrb_test2.w = 1;
    lwrb_write(&lwrb_test, input, 9);
    copied = lwrb_copy(&lwrb_test2, &lwrb_test);

    // Then
    //[xx][AA][BB][CC][DD][EE][FF][00][11][22]
    // rw
    //[xx][AA][BB][CC][DD][EE][FF][00][11][22]
    //  w  r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_NOT_NULL(lwrb_test2.buff);
    TEST_ASSERT_EQUAL(copied, 9);
    TEST_ASSERT_EQUAL(lwrb_test.w, 0);
    TEST_ASSERT_EQUAL(lwrb_test.r, 0);
    TEST_ASSERT_EQUAL(lwrb_test2.w, 0);
    TEST_ASSERT_EQUAL(lwrb_test2.r, 1);

    TEST_ASSERT_EQUAL(lwrb_data[1], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[9], 0x22);
    TEST_ASSERT_EQUAL(lwrb_data2[1], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data2[9], 0x22);

    lwrb_read(&lwrb_test2, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[8], 0x22);
}


void test_lwrb_ex_copy_3_linear_wrap(void)
{
    // Given
    uint8_t input[10]               = { 0x00 };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };
    size_t copied;

    // When
    //[AA][BB][FF][FF][FF][FF][FF][FF][FF][FF]
    // r        w
    //[00][00][00][00][00][00][00][00][00][00]
    //                                     rw
    input[0]     = 0xAA;
    input[1]     = 0xBB;
    lwrb_test.r  = 0;
    lwrb_test.w  = 0;
    lwrb_test2.r = 9;
    lwrb_test2.w = 9;
    lwrb_write(&lwrb_test, input, 2);
    copied = lwrb_copy(&lwrb_test2, &lwrb_test);

    // Then
    //[AA][BB][FF][FF][FF][FF][FF][FF][FF][FF]
    //         rw
    //[BB][00][00][00][00][00][00][00][00][AA]
    //      w                              r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_NOT_NULL(lwrb_test2.buff);
    TEST_ASSERT_EQUAL(copied, 2);
    TEST_ASSERT_EQUAL(lwrb_test.w, 2);
    TEST_ASSERT_EQUAL(lwrb_test.r, 2);
    TEST_ASSERT_EQUAL(lwrb_test2.w, 1);
    TEST_ASSERT_EQUAL(lwrb_test2.r, 9);

    TEST_ASSERT_EQUAL(lwrb_data[0], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[1], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data2[9], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data2[0], 0xBB);

    lwrb_read(&lwrb_test2, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
}


void test_lwrb_ex_copy_4_wrap_wrap(void)
{
    // Given
    uint8_t input[10]               = { 0x00 };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };
    size_t copied;

    // When
    //[BB][00][00][00][00][00][00][00][00][AA]
    //      w                              r
    //[00][00][00][00][00][00][00][00][00][00]
    //                                     rw
    input[0]     = 0xAA;
    input[1]     = 0xBB;
    lwrb_test.r  = 9;
    lwrb_test.w  = 9;
    lwrb_test2.r = 9;
    lwrb_test2.w = 9;
    lwrb_write(&lwrb_test, input, 2);
    copied = lwrb_copy(&lwrb_test2, &lwrb_test);

    // Then
    //[BB][00][00][00][00][00][00][00][00][AA]
    //     rw
    //[BB][00][00][00][00][00][00][00][00][AA]
    //      w                              r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_NOT_NULL(lwrb_test2.buff);
    TEST_ASSERT_EQUAL(copied, 2);
    TEST_ASSERT_EQUAL(lwrb_test.w, 1);
    TEST_ASSERT_EQUAL(lwrb_test.r, 1);
    TEST_ASSERT_EQUAL(lwrb_test2.w, 1);
    TEST_ASSERT_EQUAL(lwrb_test2.r, 9);

    TEST_ASSERT_EQUAL(lwrb_data[9], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[0], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data2[9], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data2[0], 0xBB);

    lwrb_read(&lwrb_test2, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
}


void test_lwrb_ex_overwrite_1_linear_under_free(void)
{
    // Given
    uint8_t input[TEST_ARRAY_SIZE]  = { 0xAA, 0xBB };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    lwrb_overwrite(&lwrb_test, input, 2);

    // Then
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 2);
    TEST_ASSERT_EQUAL(lwrb_test.r, 0);

    TEST_ASSERT_EQUAL(lwrb_data[0], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[1], 0xBB);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
}

void test_lwrb_ex_overwrite_2_wrap_under_free(void)
{

    // Given
    uint8_t input[TEST_ARRAY_SIZE]  = { 0xAA, 0xBB };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    lwrb_test.r = 9;   // TODO: Try at 8
    lwrb_test.w = 9;
    lwrb_overwrite(&lwrb_test, input, 2);


    // Then
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 1);
    TEST_ASSERT_EQUAL(lwrb_test.r, 9);

    TEST_ASSERT_EQUAL(lwrb_data[9], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[0], 0xBB);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
}

void test_lwrb_ex_overwrite_3_linear_over_free(void)
{
    // Given
    uint8_t input[TEST_ARRAY_SIZE]  = { 0xAA, 0xBB };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    lwrb_test.r = 4;
    lwrb_test.w = 3;
    lwrb_overwrite(&lwrb_test, input, 2);

    // Then
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 5);
    TEST_ASSERT_EQUAL(lwrb_test.r, 6);

    TEST_ASSERT_EQUAL(lwrb_data[2], 0x00);
    TEST_ASSERT_EQUAL(lwrb_data[3], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[4], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data[5], 0x00);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[7], 0xAA);
    TEST_ASSERT_EQUAL(output[8], 0xBB);
}


void test_lwrb_ex_overwrite_4_linear_over_wrap(void)
{
    // Given
    //[00][00][00][00][00][00][00][00][00][00]
    // rw
    uint8_t input[TEST_ARRAY_SIZE]  = { 0xAA, 0xBB };
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    //[xx][xx][xx][xx][xx][xx][xx][xx][xx][00]
    // r                                    w
    lwrb_test.r = 0;
    lwrb_test.w = 9;
    lwrb_overwrite(&lwrb_test, input, 2);

    // Then
    //[BB][xx][xx][xx][xx][xx][xx][xx][xx][AA]
    //     w   r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 1);
    TEST_ASSERT_EQUAL(lwrb_test.r, 2);

    TEST_ASSERT_EQUAL(lwrb_data[8], 0x00);
    TEST_ASSERT_EQUAL(lwrb_data[9], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[0], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data[1], 0x00);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[7], 0xAA);
    TEST_ASSERT_EQUAL(output[8], 0xBB);
}


void test_lwrb_ex_overwrite_5_oversize_from_middle(void)
{
    // Given
    //[00][00][00][00][00][00][00][00][00][00]
    // rw
    uint8_t input[100]              = { 0x00 };
    input[91]                       = 0xAA;
    input[92]                       = 0xBB;
    input[98]                       = 0xEE;
    input[99]                       = 0xFF;
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    //[xx][xx][xx][xx][xx][xx][xx][xx][xx][00]
    //                 r   w
    lwrb_test.r = 5;
    lwrb_test.w = 6;
    lwrb_overwrite(&lwrb_test, input, 100);

    // Then
    //[xx][xx][xx][EE][FF][xx][AA][BB][xx][xx]
    //                      w  r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 5);
    TEST_ASSERT_EQUAL(lwrb_test.r, 6);

    TEST_ASSERT_EQUAL(lwrb_data[6], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[7], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data[3], 0xEE);
    TEST_ASSERT_EQUAL(lwrb_data[4], 0xFF);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
    TEST_ASSERT_EQUAL(output[7], 0xEE);
    TEST_ASSERT_EQUAL(output[8], 0xFF);
}


void test_lwrb_ex_overwrite_6_oversize_from_zero(void)
{
    // Given
    //[00][00][00][00][00][00][00][00][00][00]
    // rw
    uint8_t input[100]              = { 0x00 };
    input[91]                       = 0xAA;
    input[92]                       = 0xBB;
    input[98]                       = 0xEE;
    input[99]                       = 0xFF;
    uint8_t output[TEST_ARRAY_SIZE] = { 0x00 };

    // When
    //[xx][xx][xx][xx][xx][xx][xx][xx][xx][xx]
    // rw
    lwrb_test.r = 0;
    lwrb_test.w = 0;
    lwrb_overwrite(&lwrb_test, input, 100);

    // Then
    //[00][AA][BB][00][00][00][00][00][EE][FF]
    // w  r
    TEST_ASSERT_NOT_NULL(lwrb_test.buff);
    TEST_ASSERT_EQUAL(lwrb_test.w, 9);
    TEST_ASSERT_EQUAL(lwrb_test.r, 0);

    TEST_ASSERT_EQUAL(lwrb_data[0], 0xAA);
    TEST_ASSERT_EQUAL(lwrb_data[1], 0xBB);
    TEST_ASSERT_EQUAL(lwrb_data[7], 0xEE);
    TEST_ASSERT_EQUAL(lwrb_data[8], 0xFF);

    lwrb_read(&lwrb_test, output, TEST_ARRAY_SIZE);
    TEST_ASSERT_EQUAL(output[0], 0xAA);
    TEST_ASSERT_EQUAL(output[1], 0xBB);
    TEST_ASSERT_EQUAL(output[7], 0xEE);
    TEST_ASSERT_EQUAL(output[8], 0xFF);
}


#endif   // TEST
