#pragma once

#include "lve_window.hpp"

namespace lve {
    class FirstApp {
        public:
            void run();
        private:
            LveWindow lveWindow{800, 600, "Hello Vulkan!"};
    };
}