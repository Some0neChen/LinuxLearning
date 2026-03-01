#include "Chore.h"
#include "HusbandService.h"
#include <unistd.h>

int main()
{
    HusbandService husbandService(5);
    std::cout << " 👸💖💋 老婆示意老公开始干活~" << std::endl;
    husbandService.orderHusbandWakeUp();
    for (int i = 0; i < 30; ++i) {
        Chore chore({i, static_cast<ChoreType>(rand() % CHORE_TYPE_MAX),
            chore_type_description_map_[rand() % CHORE_TYPE_MAX], 5 + rand() % 10});
        husbandService.addWifeOrder(chore);
    }
    usleep(2000000);
    std::cout << " 👸💖💋 老婆示意老公可以休息了~" << std::endl;
    husbandService.orderHusbandSleep();
    return 0;;
}