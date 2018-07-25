#include <criterion/criterion.h>
#include "url.h"
#include "http.h"

Test(url_suite, parse_normal_url) {
    URL *url;
    char *str = "http://www.google.com";
    url = url_parse(str);
    cr_assert_neq(url, NULL);
    cr_assert_eq(url_port(url), 80);
    cr_assert_str_eq(url_method(url), "http");
    cr_assert_str_eq(url_hostname(url), "www.google.com");
    cr_assert_str_eq(url_path(url), "/");
}

Test(url_suite, url_no_slash) {
    URL *url;
    url = url_parse("http:www.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_no_method) {
    URL *url;
    url = url_parse("//www.google.com");
    cr_assert_neq(url, NULL);
    cr_assert_eq(url_port(url), 0);
    cr_assert_eq(url_method(url), NULL);
    cr_assert_eq(url_hostname(url), NULL);
    cr_assert_str_eq(url_path(url), "//www.google.com");
}

Test(http_suite, basic_test) {
    URL *url = url_parse("http://www.google.com");
    cr_assert_neq(url, NULL);
    HTTP *http = http_open(url_address(url), url_port(url));
    cr_assert_neq(http, NULL);
    http_request(http, url);
    http_response(http);
    int code;
    char *status = http_status(http, &code);
    cr_assert_str_eq(status, "HTTP/1.0 200 OK");
    cr_assert_eq(code, 200);
    http_close(http);
}

Test(http_suite, query_header_test) {
    URL *url = url_parse("http://bsd7.cs.stonybrook.edu/index.html");
    cr_assert_neq(url, NULL);
    HTTP *http = http_open(url_address(url), url_port(url));
    cr_assert_neq(http, NULL);
    http_request(http, url);
    http_response(http);
    int code;
    char *status = http_status(http, &code);
    cr_assert_str_eq(status, "HTTP/1.1 200 OK");
    cr_assert_eq(code, 200);
    cr_assert_str_eq(http_headers_lookup(http, "content-length"), "11510");
    cr_assert_str_eq(http_headers_lookup(http, "content-type"), "text/html");
    http_close(http);
}

Test(http_suite, not_found_test) {
    URL *url = url_parse("http://www.google.com/no_such_document.html");
    cr_assert_neq(url, NULL);
    HTTP *http = http_open(url_address(url), url_port(url));
    cr_assert_neq(http, NULL);
    http_request(http, url);
    http_response(http);
    int code;
    char *status = http_status(http, &code);
    cr_assert_str_eq(status, "HTTP/1.0 404 Not Found");
    cr_assert_eq(code, 404);
    http_close(http);
}

//############################################
// STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// DO NOT DELETE THESE COMMENTS
//############################################

Test(cli_suite, invalid_term_1) {
    char* cmd = "bin/snarf -o -o";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 255,"Program exited with %d instead of -1", return_code);
}

Test(cli_suite, invalid_term_2) {
    char* cmd = "bin/snarf -o -q";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 255,"Program exited with %d instead of -1", return_code);
}

Test(cli_suite, invalid_term_3) {
    char* cmd = "bin/snarf -q -o";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 255,"Program exited with %d instead of -1", return_code);
}

Test(cli_suite, invalid_term_4) {
    char* cmd = "bin/snarf -oo text.txt";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 255,"Program exited with %d instead of -1", return_code);
    URL *url;
    char *str = "http://www.facebook.com";
    url = url_parse(str);
    cr_assert_neq(url, NULL);
    cr_assert_eq(url_port(url), 80);
    cr_assert_str_eq(url_method(url), "http");
    cr_assert_str_eq(url_hostname(url), "www.facebook.com");
    cr_assert_str_eq(url_path(url), "/");
}

Test(cli_suite, invalid_term_5) {
    char* cmd = "bin/snarf -qq content-type";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 255,"Program exited with %d instead of -1", return_code);
}

Test(cli_suite, invalid_term_6) {
    char* cmd = "bin/snarf -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_7) {
    char* cmd = "bin/snarf -h -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_8) {
    char* cmd = "bin/snarf http://www.google.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_9) {
    char* cmd = "bin/snarf http:///www.google.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_10) {
    char* cmd = "bin/snarf //www.google.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_11) {
    char* cmd = "bin/snarf :www.google.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_12) {
    char* cmd = "bin/snarf http://www.google.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(cli_suite, invalid_term_13) {
    char* cmd = "bin/snarf http://www.google.com http://www.google.com http://www.gogole.com -h";
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, 0,"Program exited with %d instead of 0", return_code);
}

Test(url_suite, parse_facebook_url) {
    URL *url;
    char *str = "http://www.facebook.com";
    url = url_parse(str);
    cr_assert_neq(url, NULL);
    cr_assert_eq(url_port(url), 80);
    cr_assert_str_eq(url_method(url), "http");
    cr_assert_str_eq(url_hostname(url), "www.facebook.com");
    cr_assert_str_eq(url_path(url), "/");
}

Test(url_suite, parse_yahoo_url) {
    URL *url;
    char *str = "http://www.yahoo.com";
    url = url_parse(str);
    cr_assert_neq(url, NULL);
    cr_assert_eq(url_port(url), 80);
    cr_assert_str_eq(url_method(url), "http");
    cr_assert_str_eq(url_hostname(url), "www.yahoo.com");
    cr_assert_str_eq(url_path(url), "/");
}

Test(url_suite, url_no_slash_no_colon) {
    URL *url;
    url = url_parse("httpwww.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_one_slash) {
    URL *url;
    url = url_parse("http:/www.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_three_slash) {
    URL *url;
    url = url_parse("http:///www.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_no_http) {
    URL *url;
    url = url_parse("://www.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_no_http_no_colon) {
    URL *url;
    url = url_parse("//www.google.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_two_http) {
    URL *url;
    url = url_parse("http://http:/.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_three_http) {
    URL *url;
    url = url_parse("http://wwww.http://http://.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_wrong) {
    URL *url;
    url = url_parse("httphttp://www.http://.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_two_com_with_dot) {
    URL *url;
    url = url_parse("http://www.http://.com.com");
    cr_assert_neq(url, NULL);
}

Test(url_suite, url_two_com) {
    URL *url;
    url = url_parse("http://www.http://.comcom");
    cr_assert_neq(url, NULL);
}

Test(http_suite, basic_test_yahoo) {
    URL *url = url_parse("http://www.yahoo.com");
    cr_assert_neq(url, NULL);
    HTTP *http = http_open(url_address(url), url_port(url));
    cr_assert_neq(http, NULL);
    http_request(http, url);
    http_response(http);
    int code;
    char *status = http_status(http, &code);
    cr_assert_str_eq(status, "HTTP/1.0 200 OK");
    cr_assert_eq(code, 200);
    http_close(http);
}

Test(status_code_suite, _100) {
 URL *url = url_parse("http://httpstat.us/100");
 cr_assert_neq(url, NULL);
 HTTP *http = http_open(url_address(url), url_port(url));
 cr_assert_neq(http, NULL);

 http_request(http, url);
 http_response(http);

 int code;
 http_status(http, &code);

 cr_assert_eq(code, 100);
 http_close(http);
 }