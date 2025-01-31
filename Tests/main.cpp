#include <catch2/catch_session.hpp>

#define APPROVALS_CATCH2_V3
#include <ApprovalTests/ApprovalTests.hpp>

int main(int argc, char* argv[]) {
    // your setup ...
    auto directoryDisposer =
        ApprovalTests::Approvals::useApprovalsSubdirectory("approvals");

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}