#include "Aether.hpp"

namespace witch {

class Application {
   private:
    Application();
    Application(const Application&) = delete;
    ~Application();
    static inline auto& getInstance() {
        static Application s_instance;
        return s_instance;
    }

    Aether::Display mDisplay;

   public:
    static void run();
    static inline void exit() { getInstance().mDisplay.exit(); }
};

}  // namespace witch
