#include "Aether/Aether.hpp"

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

    Aether::Window m_window;

   public:
    static void run();
    static inline void setScreen(Aether::Screen& screen) { getInstance().m_window.showScreen(&screen); }
    static inline void exitApp() { getInstance().m_window.exit(); }
};

}  // namespace witch
