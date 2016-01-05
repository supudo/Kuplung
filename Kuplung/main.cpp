#include "kuplung/Kuplung.hpp"

int main() {
    //FIXME: Remove for prod
    setbuf(stdout, NULL);

    Kuplung app;
    return app.run();
}
