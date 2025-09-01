#include "main.hpp"

void clear_line() {
    std::cout << "\r\033[2K" << std::flush;
}

void hide_cursor() {
    std::cout << "\033[?25l" << std::flush;
}

void show_cursor() {
    std::cout << "\033[?25h" << std::flush;
}

void restore_cursor() {
    show_cursor();
}

void signal_handler(int signum) {
    restore_cursor();
    exit(signum);
}

void progress(std::string work, int progress, int total) {
    int barWidth = 50;
    float ratio = static_cast<float>(progress) / total;
    int pos = static_cast<int>(barWidth * ratio);

    std::cout << work << " [";
    for (int i = 0; i < barWidth; i++) {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }

    std::cout << "] " << int(ratio * 100.0) << "% (" << progress << "/" << total << ")\r";
    std::cout.flush();
}