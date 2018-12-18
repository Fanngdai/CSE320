#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "hw1.h"

#define ERR_RETURN_SUCCESS "Program exited with %d instead of EXIT_SUCCESS"
#define ERR_RETURN_FAILURE "Program exited with %d instead of EXIT_FAILURE"
#define CMP_MISMATCH "Output did not match the contents of the reference file"
#define CMP_PROBLEM "The test had a problem running 'cmp' -- requires investigation!"

//-u -f 1 -p
Test(hw1_tests_suite, speed_up_test2) {
    char *cmd1 = "bin/audible -u -f 1 -p < tests/rsrc/sample.au > out3.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out3.au tests/rsrc/sample.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-u -f 7 -p
Test(hw1_tests_suite, speed_up_test6) {
    char *cmd1 = "bin/audible -u -f 7 -p < tests/rsrc/sample.au > out20.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out20.au tests/rsrc/sample_u_f7_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-u -f 20 -p
Test(hw1_tests_suite, speed_up_test7) {
    char *cmd1 = "bin/audible -u -f 20 -p < tests/rsrc/sample.au > out21.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out21.au tests/rsrc/sample_u_f20_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}


//triangle
//-u -p
Test(hw1_tests_suite, speed_up_test8) {
    char *cmd1 = "bin/audible -u -p < tests/rsrc/triangle_1kHz_2ch_8bit.au > out25.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out25.au tests/rsrc/triangle_1kHz_2ch_8bit.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d -f 1 -p
Test(hw1_tests_suite, slow_down_test5) {
    char *cmd1 = "bin/audible -d -f 1 -p < tests/rsrc/sample.au > out8.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out8.au tests/rsrc/sample.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d -f 4 -p
Test(hw1_tests_suite, slow_down_test6) {
    char *cmd1 = "bin/audible -d -f 4 -p < tests/rsrc/sample.au > out9.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out9.au tests/rsrc/sample_d_f4_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-d -f 5 -p
Test(hw1_tests_suite, slow_down_test7) {
    char *cmd1 = "bin/audible -d -f 5 -p < tests/rsrc/sample.au > out10.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out10.au tests/rsrc/sample_d_f5_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-d -f 23 -p
Test(hw1_tests_suite, slow_down_test9) {
    char *cmd1 = "bin/audible -d -f 23 -p < tests/rsrc/triangle_1kHz_2ch_8bit.au > out23.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out23.au tests/rsrc/triangle_1kHz_2ch_8bit_d_f23_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-d -p
Test(hw1_tests_suite, slow_down_test10) {
    char *cmd1 = "bin/audible -d -p < tests/rsrc/triangle_1kHz_2ch_8bit.au > out24.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out24.au tests/rsrc/triangle_1kHz_2ch_8bit.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k bdea5656 -p
Test(hw1_tests_suite, crypt_test5) {
    char *cmd1 = "bin/audible -c -k bdea5656 -p < tests/rsrc/sample.au > out15.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out15.au tests/rsrc/sample_c_bdea5656_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k 34567 -p
Test(hw1_tests_suite, crypt_test6) {
    char *cmd1 = "bin/audible -c -k 34567 -p < tests/rsrc/sample.au > out16.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out16.au tests/rsrc/sample_c_34567_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//-c -k ab34567 -p
Test(hw1_tests_suite, crypt_test7) {
    char *cmd1 = "bin/audible -c -k ab34567 -p < tests/rsrc/sample.au > out17.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out17.au tests/rsrc/sample_c_ab34567_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}

//triangle
//-c -k 6BBcf -p
Test(hw1_tests_suite, crypt_test9) {
    char *cmd1 = "bin/audible -c -k 6BBcf -p < tests/rsrc/triangle_1kHz_2ch_8bit.au > out28.au";
    int ret1 = WEXITSTATUS(system(cmd1));

    char *cmd2 = "cmp out28.au tests/rsrc/triangle_1kHz_2ch_8bit_c_6BBcf_p.au";
    int ret2 = WEXITSTATUS(system(cmd2));
    cr_assert_neq(ret2, 2, CMP_PROBLEM, ret2);

    cr_assert_eq(ret1, EXIT_SUCCESS, ERR_RETURN_SUCCESS, ret1);
    cr_assert_eq(ret2, EXIT_SUCCESS, CMP_MISMATCH, ret2);
}
