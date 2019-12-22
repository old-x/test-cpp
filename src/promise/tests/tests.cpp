#include "test_promise.hpp"

int main() {
    test_empty_promise();
    test_completed_promise();
    test_failed_promise();
    test_completion();
    test_failure();
    test_cancellation();
    return 0;
}
