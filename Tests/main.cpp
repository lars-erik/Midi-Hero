#include <catch2/catch_session.hpp>

#define APPROVALS_CATCH2_V3
#include <ApprovalTests/ApprovalTests.hpp>

#include "JuceHeader.h"

int main(int argc, char* argv[]) {

    // Setup JUCE message manager
    juce::MessageManager::getInstance();

    // Setup approval tests
    auto directoryDisposer =
        ApprovalTests::Approvals::useApprovalsSubdirectory("approvals");

    int result = Catch::Session().run(argc, argv);

    // Clean up JUCE
    juce::MessageManager::deleteInstance();

    return result;
}