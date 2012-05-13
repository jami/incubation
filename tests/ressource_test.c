#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "ressource_test.h"
#include "ressource.h"
#include "config.h"

void test_archive_load() {
    archive_p archive = load_archive_from_file(TESTDATAPATH "/archive_fixture.tgz");
    CU_ASSERT_PTR_NOT_NULL(archive);
    
    ressource_p entry = ressource_from_archive(archive, "CMakeFiles/test_lib.dir/build.make");
    CU_ASSERT_PTR_NOT_NULL(entry);

    static char *txt_fixture1 = "# CMAKE generated file: DO NOT EDIT!";
    CU_ASSERT_NSTRING_EQUAL(entry->stream, txt_fixture1, strlen(txt_fixture1));

    entry = ressource_from_archive(archive, "?????????");
    CU_ASSERT_PTR_NULL(entry);

    archive_destroy(archive);
}

void test_ressource_load() {

}

CU_pSuite get_ressource_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("ressource test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "Load gzip archive file", test_archive_load);
    
    return pSuite;
}
