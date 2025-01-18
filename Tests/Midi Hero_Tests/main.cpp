#include "catch_amalgamated.hpp"

#define APPROVALS_CATCH_EXISTING_MAIN
#define APPROVALS_CATCH2_V3_AMALGAMATED
#include "ApprovalTests.v.10.13.0.hpp"

int main(int argc, char* argv[]) {
    // your setup ...
    auto directoryDisposer =
        ApprovalTests::Approvals::useApprovalsSubdirectory("approvals");

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}