
// How many milliseconds should the sleeper threads sleep
#define SLEEP_AMOUNT 150
#define MARGIN 50

typedef struct sleeper_thread_data {
    uint32_t target_core;
    uint32_t sleep_millis;
} sleeper_thread_data_t;


void init_sleeper_threads(int worker_threads, void (*func)(void));
void join_sleeper_threads(); 