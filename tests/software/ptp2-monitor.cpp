//
// Created by Rui Lopes on 10/8/24.
//

#include <iostream>

#include <ptpmgmt/init.h>

using namespace ptpmgmt;

static Init object;
static Message& message = object.msg();
static Message message_unit;
static SockBase* socket_base;
static bool use_uds;

int main() {
    MsgParams parameters = message.getParams();
    parameters.boundaryHops = 0;
    use_uds = object.use_uds();

    if (!use_uds) {
        // TODO Handle this error.
    } else {
        message_unit.updateParams(parameters);
    }

    return 0;
}