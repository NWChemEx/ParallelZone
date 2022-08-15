#define CATCH_CONFIG_RUNNER
#include "test_parallelzone.hpp"

int main(int argc, char* argv[]) {
    auto rt = parallelzone::runtime::RuntimeView(argc, argv);
    testing::PZEnvironment::pcomm_world = &rt;

    int res = Catch::Session().run(argc, argv);

    return res;
}
