#include <easylogging++.h>

INITIALIZE_EASYLOGGINGPP

void initLog(int argc, char **argv) {
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%logger] %msg [%fbase:%line]");
}

