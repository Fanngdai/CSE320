#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "hw1.h"

#define ERR_RETURN_SUCCESS "Program exited with %d instead of EXIT_SUCCESS"
#define ERR_RETURN_FAILURE "Program exited with %d instead of EXIT_FAILURE"
#define CMP_MISMATCH "Output did not match the contents of the reference file"
#define CMP_PROBLEM "The test had a problem running 'cmp' -- requires investigation!"

//-u -f 2
Test(hw1_tests_suite, speed_up_test_2channels2bytes_1) {
    char *cmd1 = "bin/audible -u -f 2 < tests/rsrc/sample.au > out.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out.au tests/rsrc/sample_u_f2.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}


//-u
Test(hw1_tests_suite, speed_up_test_2channels2bytes_2) {
    char *cmd1 = "bin/audible -u < tests/rsrc/sample.au > out4.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out4.au tests/rsrc/sample_u.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-u -f 3
Test(hw1_tests_suite, speed_up_test_2channels2bytes_3) {
    char *cmd1 = "bin/audible -u -f 3 < tests/rsrc/sample.au > out18.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out18.au tests/rsrc/sample_u_f3.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-u -f 1024
Test(hw1_tests_suite, speed_up_test_triangle_4) {
    char *cmd1 = "bin/audible -u -f 1024 < tests/rsrc/triangle_1kHz_2ch_8bit.au > out29.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out29.au tests/rsrc/triangle_1kHz_2ch_8bit_u_f1024.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-u -f 36
Test(hw1_tests_suite, speed_up_test_triangle_5) {
    char *cmd1 = "bin/audible -u -f 36 < tests/rsrc/triangle_1kHz_2ch_8bit.au > out26.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out26.au tests/rsrc/triangle_1kHz_2ch_8bit_u_f36.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels3bytes
//-u -f 500
Test(hw1_tests_suite, speed_up_test_2channels3bytes_6) {
    char *cmd1 = "bin/audible -u -f 500 < tests/rsrc/sample_3byte.au > out30.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out30.au tests/rsrc/sample_3byte_u_f500.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels4bytes
//-u -f 100
Test(hw1_tests_suite, speed_up_test_2channels4bytes_7) {
    char *cmd1 = "bin/audible -u -f 100 < tests/rsrc/sample_4byte.au > out31.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out31.au tests/rsrc/sample_4byte_u_f100.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}


//-d -f 2
Test(hw1_tests_suite, slow_down_test_2channels2bytes_1) {
    char *cmd1 = "bin/audible -d -f 2 < tests/rsrc/sample.au > out1.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out1.au tests/rsrc/sample_d_f2.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d
Test(hw1_tests_suite, slow_down_test_2channels2bytes_2) {
    char *cmd1 = "bin/audible -d < tests/rsrc/sample.au > out5.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out5.au tests/rsrc/sample_d.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d -f 3
Test(hw1_tests_suite, slow_down_test_2channels2bytes_3) {
    char *cmd1 = "bin/audible -d -f 3 < tests/rsrc/sample.au > out6.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out6.au tests/rsrc/sample_d_f3.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}


//triangle
//-d -f 10
Test(hw1_tests_suite, slow_down_test_triangle_4) {
    char *cmd1 = "bin/audible -d -f 10 < tests/rsrc/triangle_1kHz_2ch_8bit.au > out22.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out22.au tests/rsrc/triangle_1kHz_2ch_8bit_d_f10.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels3bytes
//-d -f 7
Test(hw1_tests_suite, slow_down_test_2channels3bytes_5) {
    char *cmd1 = "bin/audible -d -f 7 < tests/rsrc/sample_3byte.au > out32.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out32.au tests/rsrc/sample_3byte_d_f7.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels4bytes
//-d -f 9
Test(hw1_tests_suite, slow_down_test_2channels4bytes_6) {
    char *cmd1 = "bin/audible -d -f 9 < tests/rsrc/sample_4byte.au > out33.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out33.au tests/rsrc/sample_4byte_d_f9.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}


//-c -k Dead6ee6
Test(hw1_tests_suite, crypt_test_2channels2bytes_1) {
    char *cmd1 = "bin/audible -c -k Dead6ee6 < tests/rsrc/sample.au > out11.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out11.au tests/rsrc/sample_c_Dead6ee6.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k AbCdef
Test(hw1_tests_suite, crypt_test_2channels2bytes_2) {
    char *cmd1 = "bin/audible -c -k AbCdef < tests/rsrc/sample.au > out12.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out12.au tests/rsrc/sample_c_AbCdef.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k DeadBeef
Test(hw1_tests_suite, crypt_test_2channels2bytes_3) {
    char *cmd1 = "bin/audible -c -k DeadBeef < tests/rsrc/sample.au > out13.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out13.au tests/rsrc/sample_c_DeadBeef.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-c -k 6546362A
Test(hw1_tests_suite, crypt_test_triangle_4) {
    char *cmd1 = "bin/audible -c -k 6546362A < tests/rsrc/triangle_1kHz_2ch_8bit.au > out27.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out27.au tests/rsrc/triangle_1kHz_2ch_8bit_c_6546362A.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels3bytes
//-c -k 123
Test(hw1_tests_suite, crypt_test_2channels3bytes_5) {
    char *cmd1 = "bin/audible -c -k 123 < tests/rsrc/sample_3byte.au > out34.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out34.au tests/rsrc/sample_3byte_c_123.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//2channels4bytes
//-c -k aaa
Test(hw1_tests_suite, crypt_test_2channels4bytes_6) {
    char *cmd1 = "bin/audible -c -k aaa < tests/rsrc/sample_4byte.au > out35.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out35.au tests/rsrc/sample_4byte_c_aaa.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

/**

//-u -f 4
Test(hw1_tests_suite, speed_up_test5) {
    char *cmd1 = "bin/audible -u -f 4 < tests/rsrc/sample.au > out19.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out19.au tests/rsrc/sample_u_f4.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d -f 4
Test(hw1_tests_suite, slow_down_test4) {
    char *cmd1 = "bin/audible -d -f 4 < tests/rsrc/sample.au > out7.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out7.au tests/rsrc/sample_d_f4.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k bdea5656
Test(hw1_tests_suite, crypt_test4) {
    char *cmd1 = "bin/audible -c -k bdea5656 < tests/rsrc/sample.au > out14.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out14.au tests/rsrc/sample_c_bdea5656.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

*/
