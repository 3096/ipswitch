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

    Aether::Display m_Display;

   public:
    static void run();
    static inline void exit() { getInstance().m_Display.exit(); }
};

}  // namespace witch
