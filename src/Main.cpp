#include <VulkanEngine.h>

int main(int argc, char* argv[])
{
    VulkanEngine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;
}
