#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "hw1.h"


////////////////////////////////////////////
// ENCRYPT TESTS
////////////////////////////////////////////
Test(hw1_tests_suite, validargs_crypt_test_0) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-c", "-k", "5b5b5b5b", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 1;
    unsigned long opt = global_options;
    unsigned long flag = 0x100000005B5B5B5BL;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Speed down mode bit wasn't set. Got: %lx", opt);
}

// Test(hw1_tests_suite, validargs_crypt_test_1) {
//     int argc = 3;
//     char *argv[] = {"bin/audible", "-c", "-h", NULL};

//     int ret = validargs(argc, argv);
//     int exp_ret = 1;

//     cr_assert_eq(ret, exp_ret,"Program exited with %d instead of EXIT_FAILURE",ret);
// }

Test(hw1_tests_suite, validargs_crypt_test_2) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-c", "-k", "00000000","-h"};

    int return_code = validargs(argc, argv);
    int exp_ret = EXIT_FAILURE;

    cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);
}

Test(hw1_tests_suite, validargs_crypt_test_3) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-c", "-k", "000000ZZ", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;

    cr_assert_eq(ret, exp_ret,"Program exited with %d instead of 0",ret);
}

Test(hw1_tests_suite, validargs_invalid_key_test) {
    int argc = 4;
    char *argv1[] = {"bin/audible", "-c", "-k", "88BADF00d", NULL};
    int ret = validargs(argc, argv1);

    int exp_ret = 0;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 5;
    char *argv2[] = {"bin/audible", "-c", "-k", "8BADFO0D", "-p", NULL};
    ret = validargs(argc, argv2);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 5;
    char *argv3[] = {"bin/audible", "-c", "-p", "-k", "0F0gf09", NULL};
    ret = validargs(argc, argv3);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_invalid_option_order_test) {
    int argc = 5;
    char *argv1[] = {"bin/audible", "-d", "-f", "10", "-d", NULL};
    int ret = validargs(argc, argv1);

    // all calls to validargs should return 0
    int exp_ret = 0;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv2[] = {"bin/audible", "-k", "BEEF9EA9", "-c", NULL};
    ret = validargs(argc, argv2);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 5;
    char *argv3[] = {"bin/audible", "-p", "-c", "-k", "01234567", NULL};
    ret = validargs(argc, argv3);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv4[] = {"bin/audible", "-f", "7", "-u", NULL};
    ret = validargs(argc, argv4);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv5[] = {"bin/audible", "-u", "-p", "-p", NULL};
    ret = validargs(argc, argv5);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 6;
    char *argv6[] = {"bin/audible", "-c", "-k", "8BADF00D", "-f", "1", NULL};
    ret = validargs(argc, argv6);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 6;
    char *argv7[] = {"bin/audible", "-u", "-f", "1", "-f", "1024", NULL};
    ret = validargs(argc, argv7);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_crypt_no_key_option_with_p_option_test) {
    int argc = 3;
    char *argv[] = {"bin/audible", "-c", "-p", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x1000000000000000L) >> 60;
    cr_expect(flag, "Crypt mode bit wasn't set. Got: %lx", flag);
    flag = (opt & 0x0800000000000000L) >> 59;
    cr_expect(flag, "-p mode bit wasn't set. Got: %lx", flag);

    int key = opt & 0xFFFFFFFFL;
    int exp_key = 0x00000000;
    cr_expect_eq(key, exp_key, "Key doesn't have the correct value. Got: %lx | Expected: %lx", key, exp_key);
}

Test(hw1_tests_suite, validargs_crypt_test) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-c", "-k", "Deadbee", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x1000000000000000L) >> 60;
    cr_expect(flag, "Crypt mode bit wasn't set. Got: %lx", flag);

    int key = opt & 0xFFFFFFFFL;
    int exp_key = 0x0DEADBEE;
    cr_expect_eq(key, exp_key, "Key doesn't have the correct value. Got: %lx | Expected: %lx", key, exp_key);

    argv[3] = "De0d9eAf";
    ret = validargs(argc, argv);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    opt = global_options;
    flag = (opt & 0x1000000000000000L) >> 60;
    cr_expect(flag, "Crypt mode bit wasn't set. Got: %lx", flag);

    key = opt & 0xFFFFFFFFL;
    exp_key = 0xDE0D9EAF;
    cr_expect_eq(key, exp_key, "Key doesn't have the correct value. Got: %lx | Expected: %lx", key, exp_key);
}

////////////////////////////////////////////
// UPSAMPLE TESTS
////////////////////////////////////////////
Test(hw1_tests_suite, help_system_test_with_extra_args) {
    int argc = 5;
    char *argv[] = {"bin/audible", "-h", "-u", "-f", "2", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 1;

    // unsigned long opt = global_options;
    // unsigned long flag = 0x100000005B5B5B5BL;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
    // cr_assert_eq(opt & flag, flag, "Speed down mode bit wasn't set. Got: %lx", opt);

    // char *cmd = "bin/audible -h -u -f 2";
    // int return_code = WEXITSTATUS(system(cmd));
    // cr_assert_eq(return_code, EXIT_SUCCESS, "Program exited with %d instead of EXIT_SUCCESS", return_code);


}
Test(hw1_tests_suite, validargs_speedup_test_1) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-u", "-f", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    // char *cmd = "bin/audible -u -f -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speedup_test_2) {
    // char *cmd = "bin/audible -u -p -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);
    int argc = 4;
    char *argv[] = {"bin/audible", "-u", "-p", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speedup_test_3) {
    // char *cmd = "bin/audible -u -f -p -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 5;
    char *argv[] = {"bin/audible", "-u", "-f", "-p", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speedup_test_4) {
    // char *cmd = "bin/audible -u -f 3000";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 4;
    char *argv[] = {"bin/audible", "-u", "-f", "3000", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speedup_test) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-u", "-f", "2", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    // Use expect instead of assert because we want execution to continue and not abort
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x4000000000000000L) >> 62;
    cr_expect(flag, "Speedup mode bit wasn't set. Got: %lx", flag);

    unsigned long factor = (opt & 0x03FF000000000000L) >> 48;
    unsigned long exp_factor = 1;
    cr_expect_eq(factor, exp_factor, "Factor doesn't have the correct value. Got: %lx | Expected: %lx", factor, exp_factor);
}

Test(hw1_tests_suite, validargs_speedup_default_factor_test) {
    int argc = 2;
    char *argv[] = {"bin/audible", "-u", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x4000000000000000L) >> 62;
    cr_expect(flag, "Speedup mode bit wasn't set. Got: %lx", flag);

    unsigned long factor = (opt & 0x03FF000000000000L) >> 48;
    unsigned long exp_factor = 0;
    cr_expect_eq(factor, exp_factor, "Factor doesn't have the correct value. Got: %lx | Expected: %lx", factor, exp_factor);
}

Test(hw1_tests_suite, validargs_speedup_p_option_test) {
    int argc = 5;
    char *argv[] = {"bin/audible", "-u", "-p", "-f", "1", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x4000000000000000L) >> 62;
    cr_expect(flag, "Speedup mode bit wasn't set. Got: %lx", flag);
    flag = (opt & 0x0800000000000000L) >> 59;
    cr_expect(flag, "-p mode bit wasn't set. Got: %lx", flag);

    unsigned long factor = (opt & 0x03FC000000000000L) >> 48;
    unsigned long exp_factor = 0;
    cr_expect_eq(factor, exp_factor, "Factor doesn't have the correct value. Got: %lx | Expected: %lx", factor, exp_factor);
}

////////////////////////////////////////////
// DOWNSAMPLE TESTS
////////////////////////////////////////////
Test(hw1_tests_suite, validargs_speeddown_test_2) {
    // char *cmd = "bin/audible -d -f -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 4;
    char *argv[] = {"bin/audible", "-d", "-f", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speeddown_test_3) {
    // char *cmd = "bin/audible -d -p -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 4;
    char *argv[] = {"bin/audible", "-d", "-p", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_speeddown_test_4) {
    // char *cmd = "bin/audible -d -f -p -h";

    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 5;
    char *argv[] = {"bin/audible", "-d", "-f", "2", "-h", NULL};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}
Test(hw1_tests_suite, validargs_speeddown_test) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-d", "-f", "2", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 1;
    unsigned long opt = global_options;
    unsigned long flag = 0x2001000000000000L;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Speed down mode bit wasn't set. Got: %lx", opt);

}
Test(hw1_tests_suite, validargs_slow_down_max_factor_test) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-d", "-f", "1024", NULL};
    int ret = validargs(argc, argv);

    int exp_ret = 1;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    unsigned long opt = global_options;
    unsigned long flag = (opt & 0x2000000000000000L) >> 61;
    cr_expect(flag, "Slow down mode bit wasn't set. Got: %lx", flag);

    unsigned long factor = (opt & 0x03FF000000000000L) >> 48;
    unsigned long exp_factor = 1023;
    cr_expect_eq(factor, exp_factor, "Factor doesn't have the correct value. Got: %lx | Expected: %lx", factor, exp_factor);
}
Test(hw1_tests_suite, validargs_invalid_factor_test) {
    int argc = 5;
    char *argv1[] = {"bin/audible", "-d", "-f", "-p", "20", NULL};
    int ret = validargs(argc, argv1);

    int exp_ret = 0;
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    // I am not sure if no FACTOR specified is a valid test since the HW doc said if -f is provided then it will be followed
    // by an arg
    argc = 3;
    char *argv2[] = {"bin/audible", "-d", "-f", NULL};
    ret = validargs(argc, argv2);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv3[] = {"bin/audible", "-u", "-f", "000", NULL};
    ret = validargs(argc, argv3);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv4[] = {"bin/audible", "-d", "-f", "1025", NULL};
    ret = validargs(argc, argv4);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);

    argc = 4;
    char *argv5[] = {"bin/audible", "-u", "-f", "-2", NULL};
    ret = validargs(argc, argv5);
    cr_expect_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

////////////////////////////////////////////
// GENERIC TESTS
////////////////////////////////////////////
Test(hw1_tests_suite, help_system_test_2) {
    // char *cmd = "bin/audible";

    // system is a syscall defined in stdlib.h
    // it takes a shell command as a string and runs it
    // we use WEXITSTATUS to get the return code from the run
    // use 'man 3 system' to find out more
    // int return_code = WEXITSTATUS(system(cmd));

    // cr_assert_eq(return_code, EXIT_FAILURE,"Program exited with %d instead of EXIT_FAILURE",return_code);

    int argc = 0;
    char *argv[] = {"bin/audible"};

    int ret = validargs(argc, argv);
    int exp_ret = 0;
    
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(hw1_tests_suite, validargs_help_test) {
    int argc = 2;
    char *argv[] = {"bin/audible", "-h", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 1;
    unsigned long opt = global_options;
    unsigned long flag = 0x8000000000000000L;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Correct bit (0x800000000000) not set for -h. Got: %lx", opt);
}


/*
Test(hw1_tests_suite, help_system_test) {
    char *cmd = "bin/audible -h";
    // system is a syscall defined in stdlib.h
    // it takes a shell command as a string and runs it
    // we use WEXITSTATUS to get the return code from the run
    // use 'man 3 system' to find out more
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS, "Program exited with %d instead of EXIT_SUCCESS", return_code);
}
*/

// 
/* 
Test(hw1_tests_suite, validargs_crypt_test_3) {
    char *cmd = "bin/audible -c -k 000000000";

    int return_code = WEXITSTATUS(system(cmd));

    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with %d instead of EXIT_FAILURE",
		 return_code);
}
*/

/*
Test(hw1_tests_suite, help_system_test_3) {
    char *cmd = "bin/audible -u < ~";

    // system is a syscall defined in stdlib.h
    // it takes a shell command as a string and runs it
    // we use WEXITSTATUS to get the return code from the run
    // use 'man 3 system' to find out more
    int return_code = WEXITSTATUS(system(cmd));

    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with %d instead of EXIT_FAILURE",
		 return_code);
    printf("Hello 1\n"); 
}
*/
