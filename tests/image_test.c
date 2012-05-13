#include "configuration.h"
#include "image_test.h"
#include "image.h"
#include "config.h"
#include <stdio.h>

void test_image_arguments() {
    static char *archive_path = TESTDATAPATH "/image_fixture.tgz";
    static char *arguments[] = {
        "/path/to/execute",
        "--datapath=" TESTDATAPATH "/image_fixture.tgz"
    };

    event_manager_init();
    configuration_manager_init(2, arguments);
    CU_ASSERT_TRUE(configuration.data_path != NULL);
    CU_ASSERT_STRING_EQUAL(configuration.data_path, archive_path);
    graphic_manager_init();

    image_p image = image_create("./intro/logo.cnf");

    CU_ASSERT_PTR_NOT_NULL(image);
    CU_ASSERT_STRING_EQUAL("./intro/logo.png", image->name);
    CU_ASSERT_EQUAL(3, image->area_count);
    CU_ASSERT_STRING_EQUAL("tile1", image->area_list[0]->name);
    CU_ASSERT_DOUBLE_EQUAL(0, image->area_list[0]->rect_f[0], 0.00001);
    CU_ASSERT_DOUBLE_EQUAL(0, image->area_list[0]->rect_f[1], 0.00001);
    CU_ASSERT_DOUBLE_EQUAL(0.2, image->area_list[0]->rect_f[2], 0.00001);
    CU_ASSERT_DOUBLE_EQUAL(0.2, image->area_list[0]->rect_f[3], 0.00001);

    image_destroy(image);

    event_manager_shutdown();
    configuration_manager_shutdown();
    graphic_manager_shutdown();

    CU_ASSERT_TRUE(configuration.data_path == NULL);
}

CU_pSuite get_image_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("image test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "Image Test 1", test_image_arguments);

    return pSuite;
}
