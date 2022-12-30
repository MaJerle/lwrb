#ifdef TEST

#include "unity.h"

#include "lwrb.h"

#define TEST_ARRAY_SIZE 10

static uint8_t lwrb_data[TEST_ARRAY_SIZE] = { 0x00 };
static uint8_t lwrb_result[TEST_ARRAY_SIZE];
static lwrb_t lwrb_test;


void setUp(void)
{
    memset(lwrb_data, 0x00, TEST_ARRAY_SIZE);
    lwrb_init(&lwrb_test, lwrb_data, TEST_ARRAY_SIZE);
}

void tearDown(void)
{
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
