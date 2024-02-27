#ifndef RANK_PRELUDE_DISPATCHER_SENDER_H
#define RANK_PRELUDE_DISPATCHER_SENDER_H

class Sender {
public:
    static Sender* get_instance() {
        static Sender instance;
        return &instance;
    }
    Sender* execute();
    Sender* stop();
    bool is_running();
private:
    Sender();
    bool _running = false;
};

#endif //RANK_PRELUDE_DISPATCHER_SENDER_H
