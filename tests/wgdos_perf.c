#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

#include <check.h>

#include "wgdosstuff.h"


#define LBEXT 19
#define BMDI 17


// libmo_unpack needs this symbol defined ... *rolls eyes*
void MO_syslog(int value, char *message, const function *const caller)
{
}


START_TEST(test_decompress)
{
    FILE *f;
    char *packed_data;

    int rc;
    int unpacked_len = 360 * 600;
    int *len_data_and_extra_data;
    int int_headers[45];
    float float_headers[19];
    int data_and_extra_data_len, data_len;
    float *unpacked_data;
    function *parent = NULL;

    size_t s;

    f = fopen("/home/richard/git/iris-test-data/test_data/PP/wgdos_packed/nae.20100104-06_0001.pp", "rb");
    if(!f) {
        perror("Open");
        exit(1);
    }

    // Skip leading header length word
    fseek(f, 4, SEEK_CUR);
    // Read headers
    if(fread(int_headers, sizeof(int), 45, f) <= 0) {
        perror("Read int headers");
        exit(1);
    }
    if(fread(float_headers, sizeof(int), 19, f) <= 0) {
        perror("Read float headers");
        exit(1);
    }
    // Skip trailing header length word
    fseek(f, 4, SEEK_CUR);
    // Read length of data-and-extra-data
    if(fread(&data_and_extra_data_len, sizeof(int), 1, f) <= 0) {
        perror("Read total data length");
        exit(1);
    }
    data_and_extra_data_len = ntohl(data_and_extra_data_len);
    printf("All-data len: %d\n", data_and_extra_data_len);

    data_len = data_and_extra_data_len - 4 * int_headers[LBEXT];
    printf("Data len: %d\n", data_len);

    packed_data = malloc(data_len);
    if(fread(packed_data, 1, data_len, f) != data_len) {
        perror("Read data");
        exit(1);
    }

    fclose(f);

    unpacked_data = malloc(360 * 600 * 4);

    struct timespec bts, ats;
    int i;
    printf("MDI: %f\n", float_headers[BMDI]);

    // Load CPU caches
    rc = wgdos_unpack(packed_data, unpacked_len, unpacked_data, float_headers[BMDI], parent);
    long total = 0, us;
    for(i=0; i<10; i++)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bts);
        rc = wgdos_unpack(packed_data, unpacked_len, unpacked_data, float_headers[BMDI], parent);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ats);
        us = 1000000 * (ats.tv_sec - bts.tv_sec) + (ats.tv_nsec - bts.tv_nsec) / 1000;
        printf("Duration: %ld us\n", us);
        total += us;
    }

    printf("Average: %ld us\n", total / 10);

    free(packed_data);
    free(unpacked_data);

    ck_assert_int_eq(rc, 9);
}
END_TEST


Suite *rle_suite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("WGDOS");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_decompress);
    suite_add_tcase(s, tc_core);

    return s;
}


int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = rle_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
